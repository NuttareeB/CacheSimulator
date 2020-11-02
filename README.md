# CacheSimulator

### Ability
* The simulator is configurable
* It is able to handle caches with varying capacities, block sizes, levels of associativity, replacement policies, and write policies
* The simulator will operate on trace files that indicate memory access properties.


### Output
* the number of cache misses, hits, and miss evictions (i.e. the number of block replacements)
* the total number of (simulated) clock cycles used during the situation. 
* how many read and write operations were requested by the CPU

### Assumption
* No actual data contents
* When a block is modified in a cache or in main memory, we always assume that the _entire_ block is read or written. No situation that only part of a block is updated.
* All memory accesses occur only within a single block at a time. (In other words, we don’t worry about the effects of a memory access overlapping two blocks, we just pretend the second block was not affected.)

### Example contents of a configuration file:
```
1 <-- this line will always contain a "1" for 472 students
230 <-- number of cycles required to write or read a block from main memory
8 <-- number of sets in cache (will be a non-negative power of 2)
16 <-- block size in bytes (will be a non-negative power of 2)
3 <-- level of associativity (number of blocks per set)
1 <-- replacement policy (will be 0 for random replacement, 1 for LRU)
1 <-- write policy (will be 0 for write-through, 1 for write-back)
13 <-- number of cycles required to read or write a block from the cache (consider this to be the access time per block)
```


