# Go-Engine
 
GoInCpp sim benchmarks:
Running on MacBook Pro 10, Apple M1 Chip
Running ./go_benchmark
Run on (8 X 24 MHz CPU s)
CPU Caches:
  L1 Data 64 KiB
  L1 Instruction 128 KiB
  L2 Unified 4096 KiB (x8)
Load Average: 1.23, 2.16, 2.84

----------------------------------------------------------------------------------
Benchmark                        Time             CPU   Iterations UserCounters...
----------------------------------------------------------------------------------
BM_StonePlacement             56.5 ns         55.6 ns     12982677 items_per_second=17.9716M/s
BM_StoneRetrieval            0.316 ns        0.316 ns   2208257622 items_per_second=3.16921G/s
BM_StonesByPosition/0         49.5 ns         49.5 ns     14177158 items_per_second=20.204M/s
BM_StonesByPosition/18        49.4 ns         49.4 ns     14183506 items_per_second=20.2516M/s
BM_StonesByPosition/180       54.6 ns         54.1 ns     13274672 items_per_second=18.4902M/s
BM_StonesByPosition/342       49.8 ns         49.8 ns     14131479 items_per_second=20.0886M/s
BM_StonesByPosition/360       54.3 ns         51.8 ns     13778985 items_per_second=19.3011M/s
BM_LibertyCount               87.4 ns         76.5 ns     10288066 items_per_second=13.0756M/s
BM_HasLiberties               54.9 ns         46.9 ns     13261846 items_per_second=21.3375M/s
BM_CaptureDetection           56.1 ns         53.1 ns     12294506 items_per_second=18.8252M/s
BM_SuicideDetection            246 ns          245 ns      2849188 items_per_second=4.08053M/s
BM_GetNeighbors/0            0.316 ns        0.316 ns   2207867578 items_per_second=3.16792G/s
BM_GetNeighbors/18           0.335 ns        0.321 ns   2211257194 items_per_second=3.11716G/s
BM_GetNeighbors/180          0.347 ns        0.323 ns   2110499736 items_per_second=3.09566G/s
BM_GetNeighbors/342          0.375 ns        0.329 ns   2180359323 items_per_second=3.03512G/s
BM_GetNeighbors/360          0.347 ns        0.324 ns   2198692720 items_per_second=3.08521G/s
BM_GetGroup                    262 ns          253 ns      2839446 items_per_second=3.94879M/s
BM_FullMoveSequence            445 ns          416 ns      1717673 items_per_second=9.62352M/s
BM_StateCreation              1.19 ns         1.15 ns    592798347 bytes_per_second=73.4803Gi/s items_per_second=867.02M/s
BM_StateCopy                  2.54 ns         2.53 ns    277878123 bytes_per_second=33.4379Gi/s items_per_second=394.546M/s
BM_GameSimulation/10          1684 ns         1680 ns       415349 items_per_second=5.95387M/s
BM_GameSimulation/50          5320 ns         5306 ns       131022 items_per_second=9.4236M/s
BM_GameSimulation/100        13087 ns        12062 ns        59913 items_per_second=8.2906M/s
BM_WithCustomCounters        0.640 ns        0.637 ns   1093801272 MemoryUsage=91 StonesPerSecond=517.892/s StonesPlaced=361# Go-Engine
