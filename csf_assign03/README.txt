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
-For cases where cache policy is no-write allocate and write through, FIFO and LRU have almost identical trace statistics. Every value from load hit/miss and store hit/miss, to the total number of cycles is about the same. This shows that FIFO and LRU appear to have the same efficiency for no-write allocate and write through. 
-For case where cache policy is set to write allocate and write through, FIFO and LRU perform similarly for most cases. However, if total number of sets is 512 or less, of 8 or less blocks, and block size of 16 bytes or less, LRU performs better. The cycle count in this case is close to identical, but when experimenting with different set size, block count, and block size, LRU has a smaller ratio of load misses and similar store hit/miss ratio. This helps to make the LRU a little more efficient than the FIFO since each load miss would cost 25n+1, where n is the block size. Meanwhile, hits only cost 1 cycle for write allocate, write through.
-Finally, cache policy with write-allocate and write-back is similar to result above where if total number of sets is 512 or less, of 8 or less blocks, and block size of 16 bytes or less, LRU has a smaller load miss ratio. However, unlike write allocate write through, the difference is more pronounced. The difference between the load misses of FIFO and LRU is just about 10%. For example, for base case with 256 sets, 4 way set associativity, and 16 byte block size, FIFO results in 4026 load misses for gcc.trace and 2311 load miss for swim.trace while LRU cache type results in 3399 load miss for gcc.trace and 1161 load miss for swim.trace. (4026-3399)/4026 x100% = 15.57% and (2311 - 1161)/2311 x 100% = 49.76% percentage differences for number of load miss in FIFO vs LRU. Once again, larger set numbers, set associativity, and block size would make miss ratio about the same due to less evictions and more hits due to larger set size. 
We experimented with different set size, set associativity, and block size to attempt to cover most potential edge cases. As seen above, overall, it appears LRU holds an advantage over FIFO, especially for write allocate write-back policy. Therefore, overall LRU cache has the best overall effectiveness because it allows blocks to be retained in cache longer if a hit occurs, increasing chance for program to result in another hit if block value appears again and decreasing overall miss rate vs FIFO which is much simpler and evicts first block in regardless of how recently it was accessed. 
