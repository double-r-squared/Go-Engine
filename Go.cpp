// MARK: Go Board Data Struct with Optimized Liberties and Captures
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

struct State {
    std::array<uint8_t, 91> data{};

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
    
    std::vector<int> getNeighbors(int idx) const {
        std::vector<int> neighbors;
        int x = idx % 19;
        int y = idx / 19;
        
        if (x > 0)  neighbors.push_back(idx - 1);    // Left
        if (x < 18) neighbors.push_back(idx + 1);    // Right
        if (y > 0)  neighbors.push_back(idx - 19);   // Up
        if (y < 18) neighbors.push_back(idx + 19);   // Down
        
        return neighbors;
    }

    int countLiberties(int idx, bool isBlack) const {
        if (isEmpty(idx)) return 0;
        if ((isBlack && !getBlack(idx)) || (!isBlack && !getWhite(idx))) return 0;
        
        std::vector<bool> visited(361, false);
        std::vector<bool> liberties(361, false);
        std::queue<int> toVisit;
        
        toVisit.push(idx);
        visited[idx] = true;
        
        while (!toVisit.empty()) {
            int current = toVisit.front();
            toVisit.pop();
            
            for (int neighbor : getNeighbors(current)) {
                if (isEmpty(neighbor)) {
                    liberties[neighbor] = true;  // Found a liberty
                } else if (!visited[neighbor]) {
                    // Same color stone - add to group
                    if ((isBlack && getBlack(neighbor)) || (!isBlack && getWhite(neighbor))) {
                        visited[neighbor] = true;
                        toVisit.push(neighbor);
                    }
                }
            }
        }
        
        // Count unique liberties
        int libertyCount = 0;
        for (bool hasLiberty : liberties) {
            if (hasLiberty) libertyCount++;
        }
        
        return libertyCount;
    }
    
    // Fast liberty check - returns true if group has at least one liberty
    bool hasLiberties(int idx, bool isBlack) const {
        if (isEmpty(idx)) return false;
        if ((isBlack && !getBlack(idx)) || (!isBlack && !getWhite(idx))) return false;
        
        std::vector<bool> visited(361, false);
        std::queue<int> toVisit;
        
        toVisit.push(idx);
        visited[idx] = true;
        
        while (!toVisit.empty()) {
            int current = toVisit.front();
            toVisit.pop();
            
            for (int neighbor : getNeighbors(current)) {
                if (isEmpty(neighbor)) {
                    return true;  // Found a liberty - group is alive
                } else if (!visited[neighbor]) {
                    // Same color stone - add to group
                    if ((isBlack && getBlack(neighbor)) || (!isBlack && getWhite(neighbor))) {
                        visited[neighbor] = true;
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
        
        std::vector<bool> visited(361, false);
        std::queue<int> toVisit;
        
        toVisit.push(idx);
        visited[idx] = true;
        group.push_back(idx);
        
        while (!toVisit.empty()) {
            int current = toVisit.front();
            toVisit.pop();
            
            for (int neighbor : getNeighbors(current)) {
                if (!visited[neighbor] && !isEmpty(neighbor)) {
                    // Same color stone - add to group
                    if ((isBlack && getBlack(neighbor)) || (!isBlack && getWhite(neighbor))) {
                        visited[neighbor] = true;
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
        
        // Check all opponent neighbors for captures
        for (int neighbor : getNeighbors(placedStone)) {
            if (!isEmpty(neighbor)) {
                bool neighborIsBlack = getBlack(neighbor);
                // If it's an opponent stone
                if (neighborIsBlack != placedIsBlack) {
                    // Check if this opponent group has no liberties
                    if (!hasLiberties(neighbor, neighborIsBlack)) {
                        std::vector<int> capturedGroup = getGroup(neighbor, neighborIsBlack);
                        capturedCount += capturedGroup.size();
                        removeStones(capturedGroup, neighborIsBlack);
                        
                        std::cout << "Captured " << capturedGroup.size() 
                                  << (neighborIsBlack ? " black" : " white") << " stones!\n";
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
        for (int neighbor : getNeighbors(idx)) {
            if (!tempState.isEmpty(neighbor)) {
                bool neighborIsBlack = tempState.getBlack(neighbor);
                if (neighborIsBlack != isBlack) {
                    if (!tempState.hasLiberties(neighbor, neighborIsBlack)) {
                        return false; // Not suicide - we capture something
                    }
                }
            }
        }
        
        // Check if our own group would have liberties
        return !tempState.hasLiberties(idx, isBlack);
    }

    void setBlack(int idx, bool value) {
        if (value) {
            if (getWhite(idx) || getBlack(idx)) {
                std::cout << "ERROR: A stone is already here, choose a different position\n";
                return;
            }
            
            // Check for suicide move
            if (isSuicideMove(idx, true)) {
                std::cout << "ERROR: Suicide move is not allowed\n";
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
                std::cout << "ERROR: A stone is already here, choose a different position\n";
                return;
            }
            
            // Check for suicide move
            if (isSuicideMove(idx, false)) {
                std::cout << "ERROR: Suicide move is not allowed\n";
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
        return (data[726 / 8] >> (726 % 8)) & 1;
    }
    
    void setTurnState(bool value) {
        if (value) data[726 / 8] |= (1 << (726 % 8));
        else       data[726 / 8] &= ~(1 << (726 % 8));
    }
    
    bool getGameActive() const {
        return (data[727 / 8] >> (727 % 8)) & 1;
    }
    
    void setGameActive(bool value) {
        if (value) data[727 / 8] |= (1 << (727 % 8));
        else       data[727 / 8] &= ~(1 << (727 % 8));
    }

    void printBinary() const {
        for (int i = 0; i < data.size(); i++) {
            std::bitset<8> b(data[i]);
            std::cout << b << " ";
            if ((i + 1) % 8 == 0) std::cout << "\n";
        }
        std::cout << std::endl;
    }
 
    void prettyPrint() const {
        std::cout << "   ";
        for (int x = 0; x < 19; x++) {
            std::cout << (x > 9 ?  x-10 : x) << " ";
        }
        std::cout << "\n";
        
        for (int y = 0; y < 19; y++) {
            std::cout << (y < 10 ? " " : "") << y << " ";
            for (int x = 0; x < 19; x++) {
                int idx = x + 19 * y;
                if (getBlack(idx)) std::cout << "● ";
                else if (getWhite(idx)) std::cout << "○ ";
                else std::cout << "◦ ";
            }
            std::cout << '\n';
        }
    }

    int bIn() {
        int x, y;
        char comma;
        std::cout << "Enter Coords (x,y): ";
        if (std::cin >> x >> comma >> y && comma == ',') {
            std::cout << "Index: " << x + 19 * y << std::endl;
        } else {
            std::cout << "Invalid input format!" << std::endl;
        }
        return x + (19 * y);
    } 
    
    int wIn() {
        int x, y;
        char comma;
        std::cout << "Enter Coords (x,y): ";
        if (std::cin >> x >> comma >> y && comma == ',') {
            std::cout << "Index: " << x + 19 * y << std::endl;
        } else {
            std::cout << "Invalid input format!" << std::endl;
        }
        return x + (19 * y);
    }
};

int main() {
    State s;
    s.setGameActive(1);
    
    std::cout << "\nsizeof(State) = " << sizeof(State) << " bytes\n";

    while (s.getGameActive()) {
        std::cout << "\n";
        if (s.getTurnState()) {
            s.prettyPrint();
            std::cout << "\nWhite Turn ---- \n";
            s.setWhite(s.wIn(), true);
        } else {
            s.prettyPrint();
            std::cout << "\nBlack Turn ---- \n";
            s.setBlack(s.bIn(), true);
        }
    } 
    return 0;
}
