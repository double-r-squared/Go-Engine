// MARK: Go Board Data Struct with Optimized Liberties and Captures + Google Benchmark
// Board Binary Representation: 728-bits total (91 bytes)
// Bits 0-360: Black stones
// Bits 361-721: White stones  
// Bit 726: Turn state (0=Black, 1=White)
// Bit 727: Game active

#include <iostream>
#include <array>
#include <cstdint>
#include <bitset>
#include <vector>
#include <queue>
#include <random>

// Comment out this line to run the game instead of benchmarks
#define RUN_BENCHMARKS

#ifdef RUN_BENCHMARKS
#include <benchmark/benchmark.h>
#endif

struct State {
    std::array<uint8_t, 91> data{};

    static std::array<std::vector<int>, 361> initNeighbors() {
        std::array<std::vector<int>, 361> neighbors;
        for (int idx = 0; idx < 361; ++idx) {
            int x = idx % 19;
            int y = idx / 19;
            if (x > 0)  neighbors[idx].push_back(idx - 1);
            if (x < 18) neighbors[idx].push_back(idx + 1);
            if (y > 0)  neighbors[idx].push_back(idx - 19);
            if (y < 18) neighbors[idx].push_back(idx + 19);
        }
        return neighbors;
    }
    
    static const std::array<std::vector<int>, 361> all_neighbors;

    bool getBlack(int idx) const {
        if (idx < 0 || idx >= 361) return false;
        int bitIndex = idx;
        return (data[bitIndex / 8] >> (bitIndex % 8)) & 1;
    }
    
    bool getWhite(int idx) const {
        if (idx < 0 || idx >= 361) return false;
        int bitIndex = 361 + idx;
        return (data[bitIndex / 8] >> (bitIndex % 8)) & 1;
    }
    
    bool isEmpty(int idx) const {
        return !getBlack(idx) && !getWhite(idx);
    }
    
    const std::vector<int>& getNeighbors(int idx) const {
        return all_neighbors[idx];
    }

    int countLiberties(int idx, bool isBlack) const {
        if (isEmpty(idx)) return 0;
        if ((isBlack && !getBlack(idx)) || (!isBlack && !getWhite(idx))) return 0;
        
        std::bitset<361> visited;
        std::bitset<361> liberties;
        std::queue<int> toVisit;
        
        toVisit.push(idx);
        visited.set(idx);
        
        while (!toVisit.empty()) {
            int current = toVisit.front();
            toVisit.pop();
            
            for (int neighbor : getNeighbors(current)) {
                if (isEmpty(neighbor)) {
                    liberties.set(neighbor);
                } else if (!visited.test(neighbor)) {
                    if ((isBlack && getBlack(neighbor)) || (!isBlack && getWhite(neighbor))) {
                        visited.set(neighbor);
                        toVisit.push(neighbor);
                    }
                }
            }
        }
        
        return liberties.count();
    }
    
    // Fast liberty check - returns true if group has at least one liberty
    bool hasLiberties(int idx, bool isBlack) const {
        if (isEmpty(idx)) return false;
        if ((isBlack && !getBlack(idx)) || (!isBlack && !getWhite(idx))) return false;
        
        std::bitset<361> visited;
        std::queue<int> toVisit;
        
        toVisit.push(idx);
        visited.set(idx);
        
        while (!toVisit.empty()) {
            int current = toVisit.front();
            toVisit.pop();
            
            for (int neighbor : getNeighbors(current)) {
                if (isEmpty(neighbor)) {
                    return true;  // Found a liberty - group is alive
                } else if (!visited.test(neighbor)) {
                    // Same color stone - add to group
                    if ((isBlack && getBlack(neighbor)) || (!isBlack && getWhite(neighbor))) {
                        visited.set(neighbor);
                        toVisit.push(neighbor);
                    }
                }
            }
        }
        
        return false;  // No liberties found - group is captured
    }

    std::vector<int> getGroup(int idx, bool isBlack) const {
        std::vector<int> group;
        if (isEmpty(idx)) return group;
        if ((isBlack && !getBlack(idx)) || (!isBlack && !getWhite(idx))) return group;
        
        std::bitset<361> visited;
        std::queue<int> toVisit;
        
        toVisit.push(idx);
        visited.set(idx);
        group.push_back(idx);
        
        while (!toVisit.empty()) {
            int current = toVisit.front();
            toVisit.pop();
            
            for (int neighbor : getNeighbors(current)) {
                if (!visited.test(neighbor) && !isEmpty(neighbor)) {
                    // Same color stone - add to group
                    if ((isBlack && getBlack(neighbor)) || (!isBlack && getWhite(neighbor))) {
                        visited.set(neighbor);
                        toVisit.push(neighbor);
                        group.push_back(neighbor);
                    }
                }
            }
        }
        
        return group;
    }
    
    // Remove stones from board (used for captures)
    void removeStones(const std::vector<int>& positions, bool isBlack) {
        for (int pos : positions) {
            if (isBlack) {
                int bitIndex = pos;
                data[bitIndex / 8] &= ~(1 << (bitIndex % 8));
            } else {
                int bitIndex = 361 + pos;
                data[bitIndex / 8] &= ~(1 << (bitIndex % 8));
            }
        }
    }
    
    // Check for captures after placing a stone
    int checkAndProcessCaptures(int placedStone, bool placedIsBlack) {
        int capturedCount = 0;
        std::bitset<361> checked;
        
        for (int neighbor : getNeighbors(placedStone)) {
            if (!isEmpty(neighbor) && !checked.test(neighbor)) {
                bool neighborIsBlack = getBlack(neighbor);
                if (neighborIsBlack != placedIsBlack) {
                    if (!hasLiberties(neighbor, neighborIsBlack)) {
                        std::vector<int> capturedGroup = getGroup(neighbor, neighborIsBlack);
                        for (int g : capturedGroup) checked.set(g);
                        capturedCount += capturedGroup.size();
                        removeStones(capturedGroup, neighborIsBlack);
                        
                        std::cout << "C" << capturedGroup.size() << (neighborIsBlack ? "B" : "W") << "\n";
                    }
                }
            }
        }
        
        return capturedCount;
    }
    
    // Check if a move would be suicide (illegal in most Go rules)
    bool isSuicideMove(int idx, bool isBlack) const {
        // Temporarily place the stone (conceptually)
        State tempState = *this;
        
        if (isBlack) {
            int bitIndex = idx;
            tempState.data[bitIndex / 8] |= (1 << (bitIndex % 8));
        } else {
            int bitIndex = 361 + idx;
            tempState.data[bitIndex / 8] |= (1 << (bitIndex % 8));
        }
        
        // Check if this move captures opponent stones
        bool capturesSomething = false;
        for (int neighbor : getNeighbors(idx)) {
            if (!tempState.isEmpty(neighbor)) {
                bool neighborIsBlack = tempState.getBlack(neighbor);
                if (neighborIsBlack != isBlack) {
                    if (!tempState.hasLiberties(neighbor, neighborIsBlack)) {
                        capturesSomething = true;
                        break;
                    }
                }
            }
        }
        
        if (capturesSomething) return false; // Not suicide
        
        // Check if our own group would have liberties
        return !tempState.hasLiberties(idx, isBlack);
    }

    void setBlack(int idx, bool value) {
        if (value) {
            if (getWhite(idx) || getBlack(idx)) {
#ifndef RUN_BENCHMARKS
                std::cout << "E1\n"; // ERROR: Stone already placed
#endif
                return;
            }
            
            // Check for suicide move
            if (isSuicideMove(idx, true)) {
#ifndef RUN_BENCHMARKS
                std::cout << "E2\n"; // ERROR: Suicide move
#endif
                return;
            }
            
            // Place the stone
            int bitIndex = idx;
            data[bitIndex / 8] |= (1 << (bitIndex % 8));
            
            // Check for captures
            int captured = checkAndProcessCaptures(idx, true);
            
            // Switch turns
            setTurnState(1);
        } else {
            // Remove stone (undo)
            int bitIndex = idx;
            data[bitIndex / 8] &= ~(1 << (bitIndex % 8));
        }
    }
    
    void setWhite(int idx, bool value) {
        if (value) {
            if (getWhite(idx) || getBlack(idx)) {
#ifndef RUN_BENCHMARKS
                std::cout << "E1\n"; // ERROR: Stone already placed
#endif
                return;
            }
            
            // Check for suicide move
            if (isSuicideMove(idx, false)) {
#ifndef RUN_BENCHMARKS
                std::cout << "E2\n"; // ERROR: Suicide move
#endif
                return;
            }
            
            // Place the stone
            int bitIndex = 361 + idx;
            data[bitIndex / 8] |= (1 << (bitIndex % 8));
            
            // Check for captures
            int captured = checkAndProcessCaptures(idx, false);
            
            // Switch turns
            setTurnState(0);
        } else {
            // Remove stone (undo)
            int bitIndex = 361 + idx;
            data[bitIndex / 8] &= ~(1 << (bitIndex % 8));
        }
    }

    bool getTurnState() const {
        return (data[90] >> (726 % 8)) & 1;  // 726 / 8 = 90 (since 91 elements, 0-90)
    }
    
    void setTurnState(bool value) {
        if (value) data[90] |= (1 << (726 % 8));
        else       data[90] &= ~(1 << (726 % 8));
    }
    
    bool getGameActive() const {
        return (data[90] >> (727 % 8)) & 1;
    }
    
    void setGameActive(bool value) {
        if (value) data[90] |= (1 << (727 % 8));
        else       data[90] &= ~(1 << (727 % 8));
    }

    void printBinary() const {
        for (int i = 0; i < data.size(); i++) {
            std::bitset<8> b(data[i]);
            std::cout << b << " ";
            if ((i + 1) % 8 == 0) std::cout << "\n";
        }
        std::cout << std::endl;
    }

    int bIn() {
        int x, y;
        char comma;
        std::cout << "xy: ";
        if (std::cin >> x >> comma >> y && comma == ',') {
            // Silent - no index output
        } else {
            std::cout << "E3\n"; // ERROR: Invalid input format
        }
        return x + (19 * y);
    } 
    
    int wIn() {
        int x, y;
        char comma;
        std::cout << "xy: ";
        if (std::cin >> x >> comma >> y && comma == ',') {
            // Silent - no index output
        } else {
            std::cout << "E3\n"; // ERROR: Invalid input format
        }
        return x + (19 * y);
    }
};

const std::array<std::vector<int>, 361> State::all_neighbors = State::initNeighbors();

#ifdef RUN_BENCHMARKS

// MARK: --- Google Benchmark Code ---

// Benchmark basic stone operations
static void BM_StonePlacement(benchmark::State& state) {
    State go_state;
    int position = 180; // Center of board
    
    for (auto _ : state) {
        go_state.setBlack(position, true);
        go_state.setBlack(position, false); // Reset for next iteration
    }
    
    state.SetItemsProcessed(state.iterations());
}
BENCHMARK(BM_StonePlacement);

static void BM_StoneRetrieval(benchmark::State& state) {
    State go_state;
    go_state.setBlack(180, true);
    
    for (auto _ : state) {
        bool stone = go_state.getBlack(180);
        benchmark::DoNotOptimize(stone);
    }
    
    state.SetItemsProcessed(state.iterations());
}
BENCHMARK(BM_StoneRetrieval);

// Benchmark different board positions
static void BM_StonesByPosition(benchmark::State& state) {
    State go_state;
    int position = state.range(0);
    
    for (auto _ : state) {
        if (go_state.isEmpty(position)) {
            go_state.setBlack(position, true);
            go_state.setBlack(position, false);
        }
    }
    
    state.SetItemsProcessed(state.iterations());
}
BENCHMARK(BM_StonesByPosition)->Arg(0)->Arg(18)->Arg(180)->Arg(342)->Arg(360); // Corners, center, edges

// Benchmark liberty counting
static void BM_LibertyCount(benchmark::State& state) {
    State go_state;
    
    // Set up a test pattern with varying liberty counts
    go_state.setBlack(180, true); // Center - should have 4 liberties initially
    go_state.setBlack(181, true); // Adjacent
    go_state.setBlack(199, true); // Adjacent
    
    for (auto _ : state) {
        int liberties = go_state.countLiberties(180, true);
        benchmark::DoNotOptimize(liberties);
    }
    
    state.SetItemsProcessed(state.iterations());
}
BENCHMARK(BM_LibertyCount);

// Benchmark fast liberty check
static void BM_HasLiberties(benchmark::State& state) {
    State go_state;
    
    // Set up a test pattern
    go_state.setBlack(180, true);
    go_state.setBlack(181, true);
    
    for (auto _ : state) {
        bool hasLibs = go_state.hasLiberties(180, true);
        benchmark::DoNotOptimize(hasLibs);
    }
    
    state.SetItemsProcessed(state.iterations());
}
BENCHMARK(BM_HasLiberties);

// Benchmark capture detection
static void BM_CaptureDetection(benchmark::State& state) {
    State go_state;
    
    // Set up a capture scenario - white stone surrounded
    go_state.setWhite(180, true); // Center stone
    go_state.setBlack(179, true); // Left
    go_state.setBlack(181, true); // Right
    go_state.setBlack(161, true); // Up
    // Leave bottom (199) open for the capturing move
    
    for (auto _ : state) {
        int captured = go_state.checkAndProcessCaptures(199, true);
        benchmark::DoNotOptimize(captured);
        
        // Reset the captured stone for next iteration
        if (captured > 0) {
            go_state.setWhite(180, true);
        }
    }
    
    state.SetItemsProcessed(state.iterations());
}
BENCHMARK(BM_CaptureDetection);

// Benchmark suicide move detection
static void BM_SuicideDetection(benchmark::State& state) {
    State go_state;
    
    // Set up scenario where placing a stone would be suicide
    go_state.setWhite(179, true); // Left
    go_state.setWhite(181, true); // Right
    go_state.setWhite(161, true); // Up
    go_state.setWhite(199, true); // Down
    // Center (180) would be suicide for black
    
    for (auto _ : state) {
        bool isSuicide = go_state.isSuicideMove(180, true);
        benchmark::DoNotOptimize(isSuicide);
    }
    
    state.SetItemsProcessed(state.iterations());
}
BENCHMARK(BM_SuicideDetection);

// Benchmark neighbor calculation
static void BM_GetNeighbors(benchmark::State& state) {
    State go_state;
    int position = state.range(0);
    
    for (auto _ : state) {
        const std::vector<int>& neighbors = go_state.getNeighbors(position);
        benchmark::DoNotOptimize(neighbors);
    }
    
    state.SetItemsProcessed(state.iterations());
}
BENCHMARK(BM_GetNeighbors)->Arg(0)->Arg(18)->Arg(180)->Arg(342)->Arg(360); // Different positions

// Benchmark group finding
static void BM_GetGroup(benchmark::State& state) {
    State go_state;
    
    // Create a group of connected stones
    std::vector<int> positions = {180, 181, 182, 199, 200, 201};
    for (int pos : positions) {
        go_state.setBlack(pos, true);
    }
    
    for (auto _ : state) {
        std::vector<int> group = go_state.getGroup(180, true);
        benchmark::DoNotOptimize(group);
    }
    
    state.SetItemsProcessed(state.iterations());
}
BENCHMARK(BM_GetGroup);

// Benchmark full move sequence
static void BM_FullMoveSequence(benchmark::State& state) {
    for (auto _ : state) {
        State go_state;
        
        // Play a sequence of moves
        go_state.setBlack(180, true);  // Center
        go_state.setWhite(181, true);  // Adjacent
        go_state.setBlack(199, true);  // Adjacent to center
        go_state.setWhite(200, true);  // Adjacent to previous white
        
        benchmark::DoNotOptimize(go_state);
    }
    
    state.SetItemsProcessed(state.iterations() * 4); // 4 moves per iteration
}
BENCHMARK(BM_FullMoveSequence);

// Memory and state management benchmarks
static void BM_StateCreation(benchmark::State& state) {
    for (auto _ : state) {
        State go_state;
        benchmark::DoNotOptimize(go_state);
    }
    
    state.SetItemsProcessed(state.iterations());
    state.SetBytesProcessed(state.iterations() * sizeof(State));
}
BENCHMARK(BM_StateCreation);

static void BM_StateCopy(benchmark::State& state) {
    State original;
    original.setBlack(180, true);
    original.setWhite(181, true);
    
    for (auto _ : state) {
        State copy = original;
        benchmark::DoNotOptimize(copy);
    }
    
    state.SetItemsProcessed(state.iterations());
    state.SetBytesProcessed(state.iterations() * sizeof(State));
}
BENCHMARK(BM_StateCopy);

// Benchmark game simulation
static void BM_GameSimulation(benchmark::State& state) {
    for (auto _ : state) {
        State go_state;
        std::mt19937 gen(42); // Fixed seed for reproducibility
        std::uniform_int_distribution<> dist(0, 360);
        
        int moves = 0;
        int max_moves = state.range(0);
        
        while (moves < max_moves) {
            int pos = dist(gen);
            if (go_state.isEmpty(pos)) {
                if (moves % 2 == 0) {
                    go_state.setBlack(pos, true);
                } else {
                    go_state.setWhite(pos, true);
                }
                moves++;
            }
        }
        
        benchmark::DoNotOptimize(go_state);
    }
    
    state.SetItemsProcessed(state.iterations() * state.range(0));
}
BENCHMARK(BM_GameSimulation)->Arg(10)->Arg(50)->Arg(100);

// Custom counter example
static void BM_WithCustomCounters(benchmark::State& state) {
    State go_state;
    size_t stones_placed = 0;
    size_t captures = 0;
    
    for (auto _ : state) {
        int pos = stones_placed % 361;
        if (go_state.isEmpty(pos)) {
            go_state.setBlack(pos, true);
            stones_placed++;
        }
    }
    
    state.counters["StonesPlaced"] = stones_placed;
    state.counters["StonesPerSecond"] = benchmark::Counter(
        stones_placed, benchmark::Counter::kIsRate);
    state.counters["MemoryUsage"] = sizeof(State);
}
BENCHMARK(BM_WithCustomCounters);

// Main function for benchmarks
BENCHMARK_MAIN();

#else

// MARK: --- Game Main Function ---
int main() {
    State s;
    s.setGameActive(1);
    
    while (s.getGameActive()) {
        std::cout << "\n";
        if (s.getTurnState()) {
            s.setWhite(s.wIn(), true);
        } else {
            s.setBlack(s.bIn(), true);
        }
    } 
    return 0;
}

#endif