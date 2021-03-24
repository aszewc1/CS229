Alexandra Szewc		aszewc1
- Made Makefile
- Created README
- Wrote CL error handling in main
- Began implementation of Cache functionality in source code
- Added read from input
- Implemented Cache functionality and main
- Troubleshooting/commenting code

Victor Wen		vwen2
- Wrote cache struct
- Written assignment
- Commenting code
- Testing and validity testing


PART B: Best Cache

- For cases where cache policy is no-write allocate and write through, FIFO and LRU have almost identical trace statistics. However, if total number of sets is 512 or less, of 8 or 

less blocks, and block size of 16 bytes or less, LRU performs better, with slightly less load misses. This holds true for all three cases.

- For case where cache policy is set to write allocate and write through, FIFO and LRU perform similarly for most cases. The cycle count in this case is close to identical, but when

experimenting with different set size, block count, and block size, LRU has a smaller ratio of load misses and similar store hit/miss ratio. This helps to make the LRU a little more

efficient than the FIFO since each load miss would cost 25n+1, where n is the block size. Meanwhile, hits only cost 1 cycle for write allocate, write through.

- Finally, cache policy with write-allocate and write-back is similar to result above where if total number of sets is 512 or less, of 8 or less blocks, and block size of 16 bytes or

less, LRU has a smaller load miss ratio but also a smaller cycle count. However, the compared to the other two cache policies, the difference is more pronounced. The difference

between the load misses of FIFO and LRU is just about 10% or more. For example, for base case with 256 sets, 4 way set associativity, and 16 byte block size, FIFO results in 4026

load misses for gcc.trace and 2311 load miss for swim.trace while LRU cache type results in 3399 load miss for gcc.trace and 1161 load miss for swim.trace. (4026-3399)/4026 x100% =

15.57% and (2311 - 1161)/2311 x 100% = 49.76% percentage differences for number of load miss in FIFO vs LRU. Once again, larger set numbers, set associativity, and block size would

make miss ratio about the same due to less evictions and more hits due to larger set size. Higher miss ratios will also incur more penalties resulting in more cycles used when a

cache miss occurs, so in real-life programs, where traces could be much larger than the traces given to us in class, LRU will result in a significantly less cycles due to much lower 

load miss rates and thus less cycles needed to account for miss penalty compared to FIFO.

We experimented with different set size, set associativity, and block size to attempt to cover most potential edge cases. As seen above, overall, it appears LRU holds an advantage 

over FIFO, especially for write allocate write-back policy. Therefore, overall LRU cache has the best overall effectiveness because it allows blocks to be retained in cache longer if

a hit occurs, increasing chance for program to result in another hit if block value appears again and decreasing overall load miss rate vs FIFO which is much simpler and evicts first

block in regardless of how recently it was accessed. In addition, the larger the cache trace, the more important set size, block number, and block size are. Otherwise, the

performance is about the same for LRU and FIFO.


Experiment results (setNum BlockNum BlockSize) for load miss: 
Note all other values are about identical.
Gcc.trace:
No-write-allocate + write-through:
512 8 16
LRU: 6064 load miss
FIFO: 6211 load miss
512 16 16
LRU: 5993 load miss
FIFO: 6003 load miss
512 8 32
LRU: 3802 load miss
FIFO: 3838 load miss
256 4 16
LRU: 6584 load miss
FIFO: 7180 load miss
Write-allocate + write-through:
512 8 16
LRU: 2660 load miss
FIFO: 2843 load miss
512 16 16
LRU: 2439 load miss
FIFO: 2567 load miss
512 8 32
LRU: 1602 load miss
FIFO: 1709 load miss
256 4 16
LRU: 3399 load miss
FIFO:  4026 load miss
Write-allocate + write-back (same as WA + WT):
512 8 16
LRU: 2660 load miss
FIFO: 2843 load miss
512 16 16
LRU: 2439 load miss
FIFO: 2567 load miss
512 8 32
LRU: 1602 load miss
FIFO: 1709 load miss
256 4 16
LRU: 3399 load miss
FIFO:  4026 load miss

swim.trace:
No-write-allocate + write-through:
512 8 16
LRU:  2530 load miss
FIFO: 2552 load miss
512 16 16
LRU:  2520 load miss
FIFO:  2521 load miss
512 8 32
LRU: 2026 load miss
FIFO: 2027 load miss
256 4 16
LRU: 2596 load miss
FIFO: 2700 load miss
Write-allocate + write-through:
512 8 16
LRU: 1018 load miss
FIFO: 1095 load miss
512 16 16
LRU: 927 load miss
FIFO: 946 load miss
512 8 32
LRU: 609 load miss
FIFO:  659 load miss
256 4 16
LRU: 1161 load miss
FIFO:  2311 load miss
Write-allocate + write-back (same as WA + WT):
512 8 16
LRU: 1018 load miss
FIFO: 1095 load miss
512 16 16
LRU: 927 load miss
FIFO: 946 load miss
512 8 32
LRU: 609 load miss
FIFO:  659 load miss
256 4 16
LRU: 1161 load miss
FIFO:  2311 load miss



