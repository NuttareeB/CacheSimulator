# CacheSimulator
The infomation is the instruction of the project from Porfessor Justin Goin.

### Ability
* The simulator is configurable
* It is able to handle caches with varying capacities, block sizes, levels of associativity, replacement policies, and write policies
* The simulator will operate on trace files that indicate memory access properties.


### Expectation
* the number of cache misses, hits, and miss evictions (i.e. the number of block replacements)
* the total number of (simulated) clock cycles used during the situation. 
* how many read and write operations were requested by the CPU

### Assumption
* No actual data contents
* When a block is modified in a cache or in main memory, we always assume that the _entire_ block is read or written. No situation that only part of a block is updated.
* All memory accesses occur only within a single block at a time. (In other words, we don’t worry about the effects of a memory access overlapping two blocks, we just pretend the second block was not affected.)

### Input (How to call)
```
make CC=g++-9
```
```
bin/cache_sim ./resources/testconfig ./resources/simpletracefile
```

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

Here is another example configuration file specifying a direct-mapped cache with 64 entries, a 32 byte block size, associativity level of 1 (direct-mapped), least recently used (LRU) replacement policy, write-through operation, 26 cycles to read or write data to the cache, and 1402 cycles to read or write data to the main memory.
```
1
1402
64
32
1
1
0
26
```

This trace file (output) will follow the format used by Valgrind (a memory debugging tool). The file consists of comments and memory access information. Any line beginning with the ‘=’ character should be treated as a comment and ignored.
```
==This is a comment and can safely be ignored.
==An example snippet of a Valgrind trace file
I  04010173,3
I  04010176,6
 S 04222cac,1
I  0401017c,7
 L 04222caf,8
I  04010186,6
I  040101fd,7
 L 1ffefffd78,8
 M 04222ca8,4
I  04010204,4
```

Memory access entries will use the following format in the trace file:
```
[space]operation address,size
```
* Lines beginning with an ‘I’ character represent an instruction load. For this assignment, you can ignore instruction read requests and assume that they are handled by a separate instruction cache.
* Lines with a space followed by an ‘S’ indicate a data store operation. This means that data needs to be written from the CPU into the cache or main memory (possibly both) depending on the write policy.
* Lines with a space followed by an ‘L’ indicate a data load operation. Data is loaded from the cache into the CPU.
* Lines with a space followed by an ‘M’ indicate a data modify operation (which implies a special case of a data load, followed immediately by a data store).

The address is a 64 bit hexadecimal number representing the address of the first byte that is being requested. Note that leading 0's are not necessarily shown in the file. The size of the memory operation is indicated in bytes (as a decimal number). In this project you will use a simplification and ignore the size of the request (essentially treating each operation as if it only affects 1 byte).

### Output

Your simulator will write output to a text file. The output filename will be derived from the trace filename with “.out” appended to the original filename. E.g. if your program was called using the invocation “cache_sim ./dm_config ./memtrace” then the output file would be written to “./memtrace.out”
(S)tore, (L)oad, and (M)odify operations will each be printed to the output file (in the exact order that they were read from the Valgrind trace file). Lines beginning with “I” should not appear in the output since they do not affect the operation of your simulator.
Each line will have a copy of the original trace file instruction. There will then be a space, followed by the number of cycles used to complete the operation. Lastly, each line will have one or more statements indicating the impact on the cache. This could be one or more of the following: miss, hit, or eviction.
Note that an eviction is what happens when a cache block needs to be removed in order to make space in the cache for another block. It is simply a way of indicating that a block was replaced. In our simulation, an eviction means that the next instruction cannot be executed until after the existing cache block is written to main memory. An eviction is an expensive cache operation.
It is possible that a single memory access has multiple impacts on the cache. For example, if a particular cache index is already full, a (M)odify operation might miss the cache, evict an existing block, and then hit the cache when the result is written to the cache.
The general format of each output line (for 472 students) is as follows (and will contain one or more cache impacts):
```
operation address,size <number_of_cycles> L1 <cache_impact1> <cache_impact2> <...>
```
The final lines of the output file are special.  They will indicate the total number of hits, misses, and evictions. The last line will indicate the total number of simulated cycles that were necessary to simulate the trace file, as well as the total number of read and write operations that were directly requested by the CPU.
These lines should exactly match the following format (with values given in decimal):
```
L1 Cache: Hits:<hits> Misses:<misses> Evictions:<evictions>
Cycles:<number of total simulated cycles> Reads:<# of CPU read requests> Writes:<# of CPU write requests>
```
In order to illustrate the output file format let’s look at an example. Suppose we are simulating a direct-mapped cache operating in write-through mode. Note that the replacement policy does not have any effect on the operation of a direct-mapped cache. Assume that the configuration file told us that it takes 13 cycles to access the cache and 230 cycles to access main memory. Keep in mind that a hit during a load operation only accesses the cache while a miss must access both the cache and the main memory. For this scenario, assume that memory access is aligned to a single block and does not straddle multiple cache blocks.
In this example the cache is operating in write-through mode so a standalone (S)tore operation takes 243 cycles, even if it is a hit, because we always write the block into both the cache and into main memory. If this particular cache was operating in write-back mode, a (S)tore operation would take only 13 cycles if it was a hit (since the block would not be written into main memory until it was evicted).
The exact details of whether an access is a hit or a miss is entirely dependent on the specific cache design (block size, level of associativity, number of sets, etc). Your program will implement the code to see if each access is a hit, miss, eviction, or some combination.
Since the (M)odify operation involves a Load operation (immediately followed by a Store operation), it is recorded twice in the output file. The first instance represents the load operation and the next line will represent the store operation. See the example below...
```
==For this example we assume that addresses 04222cac, 04222caf, and 04222ca8 are all in the same block at index 2
==Assume that addresses 047ef249 and 047ef24d share a block that also falls at index 2.
==Since the cache is direct-mapped, only one of those blocks can be in the cache at a time.
==Fortunately, address 1ffefffd78 happens to fall in a different block index (in our hypothetical example).
==Side note: For this example a store takes 243 cycles (even if it was a hit) because of the write-through behavior.
==The output file for our hypothetical example:
S 04222cac,1 486 L1 miss <-- (243 cycles to fetch the block and write it to L1) + (243 cycles to update the cache & main memory)
L 04222caf,8 13 L1 hit
M 1ffefffd78,8 243 L1 miss <-- notice that this (M)odify has a miss for the load and a hit for the store
M 1ffefffd78,8 243 L1 hit <-- this line represents the Store of the modify operation
M 04222ca8,4 13 L1 hit <-- notice that this (M)odify has two hits (one for the load, one for the store)
M 04222ca8,4 243 L1 hit <-- this line represents the Store of the modify operation
S 047ef249,4 486 L1 miss eviction <-- 486 cycles for miss, no eviction penalty for write-through cache
L 04222caf,8 243 L1 miss eviction
M 047ef24d,2 243 L1 miss eviction <-- notice that this (M)odify initially misses, evicts the block, and then hits
M 047ef24d,2 243 L1 hit <-- this line represents the Store of the modify operation
L 1ffefffd78,8 13 L1 hit
M 047ef249,4 13 L1 hit
M 047ef249,4 243 L1 hit
L1 Cache: Hits:8 Misses:5 Evictions:3
Cycles:2725 Reads:7 Writes:6 <-- total sum of simulated cycles (from above), as well as the number of reads and writes
```

## L2/L3 Cache Implementation
Implement your cache simulator so that it can support up to 3 layers of cache. You can imagine that these caches are connected in a sequence. The CPU will first request information from the L1 cache. If the data is not available, the request will be forwarded to the L2 cache. If the L2 cache cannot fulfill the request, it will be passed to the L3 cache. If the L3 cache cannot fulfill the request, it will be fulfilled by main memory.
There are specific implementation requirements (see the Code Implementation section).
It is important that the properties of each cache are read from the provided configuration file. As an example, it is possible to have a direct-mapped L1 cache that operates in cohort with an associative L2 cache. All of these details will be read from the configuration file. As with any programming project, you should be sure to test your code across a wide variety of scenarios to minimize the probability of an undiscovered bug.
Cache Operation
When multiple layers of cache are implemented, the L1 cache will no longer directly access main memory. Instead, the L1 cache will interact with the L2 cache. During the design process, you need to consider the various interactions that can occur. For example, if you are working with three write-through caches, than a single write request from the CPU will update the contents of L1, L2, L3, and main memory!
```
++++++++++++        ++++++++++++        ++++++++++++        ++++++++++++        +++++++++++++++
|          |        |          |        |          |        |          |        |             |
|   CPU    | <----> | L1 Cache | <----> | L2 Cache | <----> | L3 Cache | <----> | Main Memory |
|          |        |          |        |          |        |          |        |             |
++++++++++++        ++++++++++++        ++++++++++++        ++++++++++++        +++++++++++++++
```
Note that your program should still handle a configuration file that specifies an L1 cache (without any L2 or L3 present). In other words, you can think of your project as a more advanced version of the 472 implementation.
