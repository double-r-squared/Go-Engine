# Go++ Engine

Go++ Engine is a blazingly fast rule based engine for the game go in C++ implementing all the rules of Go including captures, suicides, and more.
Here is the general archetecture for the array representing the game:
- Board Binary Representation: 728-bits total (91 bytes)
- Bits 0-360: Black stones
- Bits 361-721: White stones
- Bits 722-725: 3 bits of paddign for aligment
- Bit 726: Turn state (0=Black, 1=White)
- Bit 727: Game active
 
## Go++ sim benchmarks:
Running on MacBook Pro 10 

Specs: Apple M1 Chip, 8gb RAM

Running ./go_benchmark

Run on (8 X 24 MHz CPU s)

CPU Caches:

  L1 Data 64 KiB
  
  L1 Instruction 128 KiB
  
  L2 Unified 4096 KiB (x8)
  
Load Average: 1.23, 2.16, 2.84


| Benchmark              | Time       | CPU       | Iterations  | Extra Counters |
|-------------------------|-----------:|----------:|------------:|----------------|
| BM_StonePlacement       | <pre>56.5 ns</pre>   | <pre>55.6 ns</pre>   | <pre>12982677</pre>   | items_per_second=17.9716M/s |
| BM_StoneRetrieval       | <pre>0.316 ns</pre>  | <pre>0.316 ns</pre>  | <pre>2208257622</pre> | items_per_second=3.16921G/s |
| BM_StonesByPosition/0   | <pre>49.5 ns</pre>   | <pre>49.5 ns</pre>   | <pre>14177158</pre>   | items_per_second=20.204M/s |
| BM_StonesByPosition/18  | <pre>49.4 ns</pre>   | <pre>49.4 ns</pre>   | <pre>14183506</pre>   | items_per_second=20.2516M/s |
| BM_StonesByPosition/180 | <pre>54.6 ns</pre>   | <pre>54.1 ns</pre>   | <pre>13274672</pre>   | items_per_second=18.4902M/s |
| BM_StonesByPosition/342 | <pre>49.8 ns</pre>   | <pre>49.8 ns</pre>   | <pre>14131479</pre>   | items_per_second=20.0886M/s |
| BM_StonesByPosition/360 | <pre>54.3 ns</pre>   | <pre>51.8 ns</pre>   | <pre>13778985</pre>   | items_per_second=19.3011M/s |
| BM_LibertyCount         | <pre>87.4 ns</pre>   | <pre>76.5 ns</pre>   | <pre>10288066</pre>   | items_per_second=13.0756M/s |
| BM_HasLiberties         | <pre>54.9 ns</pre>   | <pre>46.9 ns</pre>   | <pre>13261846</pre>   | items_per_second=21.3375M/s |
| BM_CaptureDetection     | <pre>56.1 ns</pre>   | <pre>53.1 ns</pre>   | <pre>12294506</pre>   | items_per_second=18.8252M/s |
| BM_SuicideDetection     | <pre>246 ns</pre>    | <pre>245 ns</pre>    | <pre>2849188</pre>    | items_per_second=4.08053M/s |
| BM_GetNeighbors/0       | <pre>0.316 ns</pre>  | <pre>0.316 ns</pre>  | <pre>2207867578</pre> | items_per_second=3.16792G/s |
| BM_GetNeighbors/18      | <pre>0.335 ns</pre>  | <pre>0.321 ns</pre>  | <pre>2211257194</pre> | items_per_second=3.11716G/s |
| BM_GetNeighbors/180     | <pre>0.347 ns</pre>  | <pre>0.323 ns</pre>  | <pre>2110499736</pre> | items_per_second=3.09566G/s |
| BM_GetNeighbors/342     | <pre>0.375 ns</pre>  | <pre>0.329 ns</pre>  | <pre>2180359323</pre> | items_per_second=3.03512G/s |
| BM_GetNeighbors/360     | <pre>0.347 ns</pre>  | <pre>0.324 ns</pre>  | <pre>2198692720</pre> | items_per_second=3.08521G/s |
| BM_GetGroup             | <pre>262 ns</pre>    | <pre>253 ns</pre>    | <pre>2839446</pre>    | items_per_second=3.94879M/s |
| BM_FullMoveSequence     | <pre>445 ns</pre>    | <pre>416 ns</pre>    | <pre>1717673</pre>    | items_per_second=9.62352M/s |
| BM_StateCreation        | <pre>1.19 ns</pre>   | <pre>1.15 ns</pre>   | <pre>592798347</pre>  | bytes_per_second=73.4803Gi/s, items_per_second=867.02M/s |
| BM_StateCopy            | <pre>2.54 ns</pre>   | <pre>2.53 ns</pre>   | <pre>277878123</pre>  | bytes_per_second=33.4379Gi/s, items_per_second=394.546M/s |
| BM_GameSimulation/10    | <pre>1684 ns</pre>   | <pre>1680 ns</pre>   | <pre>415349</pre>     | items_per_second=5.95387M/s |
| BM_GameSimulation/50    | <pre>5320 ns</pre>   | <pre>5306 ns</pre>   | <pre>131022</pre>     | items_per_second=9.4236M/s |
| BM_GameSimulation/100   | <pre>13087 ns</pre>  | <pre>12062 ns</pre>  | <pre>59913</pre>      | items_per_second=8.2906M/s |
| BM_WithCustomCounters   | <pre>0.640 ns</pre>  | <pre>0.637 ns</pre>  | <pre>1093801272</pre> | MemoryUsage=91, StonesPerSecond=517.892/s, StonesPlaced=361 |
