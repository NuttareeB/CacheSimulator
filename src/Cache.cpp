#include "Cache.h"
#include <stdlib.h>
#include <iostream>

using namespace std;

Cache::Cache() {
}
Cache::Cache(CacheInfo cacheInfo) {
    Block block;

    block.tag = 0;
    block.dirtyBit = 0;
    block.validBit = 0;

    std::vector<std::vector<Block>> blocks(cacheInfo.numberSets, std::vector<Block> (cacheInfo.associativity, block));
    this->blocks = blocks;

 cout << "tag==" << blocks[0][2].tag << endl;

 cout << "initial size: ==" << blocks.size() << endl;

};

CacheUpdateResponse Cache::readCache(CacheInfo cacheInfo, AddressInfo ai, CacheResponse* response, Block* evictedBlock, bool isL1Cache, bool isWrite) {
    CacheUpdateResponse cacheUpdateResponse;
    Block block;
    Block tmpFoundBlock;
    bool isCurrAddEvictedBlockFound = false;
    
    response->hit = 0;
    response->eviction = 0;

    block.tag = ai.tag;
    block.index = ai.setIndex;
    block.validBit = 1;
    
    //read
    for(unsigned int i = 0; i < cacheInfo.associativity; i++) {

        //can find the value
        if(this->blocks[block.index][i].tag == block.tag && blocks[block.index][i].validBit == 1){
            response->hit = 1;
            response->eviction = 0;

            //if we need to evict the block, keep the found block first to avoid replace to that particular block
            if(evictedBlock) {
                //update tmp file
                tmpFoundBlock = this->blocks[block.index][i];
            }

            //move vector to the back and delete from the exiting place to support LRU
            this->blocks[block.index].push_back(*(this->blocks[block.index].begin() + i));
            this->blocks[block.index].erase(this->blocks[block.index].begin() + i);
        }
    }

    //cannot find value from cache
    if(isL1Cache) {
        if (response->hit == 0) {
            cacheUpdateResponse.evictedBlock = updateCache(cacheInfo, response, tmpFoundBlock, block, isWrite);
        }
    } else {
        if (response->hit == 0) {
            if(isWrite) {
                cacheUpdateResponse.evictedBlock = updateCache(cacheInfo, response, tmpFoundBlock, block, isWrite);
            }
        }

        //if we have evicted block and it is a write back, cache need to be updated
        if(cacheInfo.wp == WritePolicy::WriteBack && evictedBlock) {
            //find the match evicted block first in this cache
            for(unsigned int i = 0; i < cacheInfo.associativity; i++) {
                //found the match evicted block, we do not do anything since we don't care the value
                if(this->blocks[block.index][i].tag == evictedBlock->tag && blocks[block.index][i].validBit == 1) {
                    isCurrAddEvictedBlockFound = true;
                    break;
                }
            }

            if(!isCurrAddEvictedBlockFound) {
                cacheUpdateResponse.evictedBlock = updateCache(cacheInfo, response, tmpFoundBlock, *evictedBlock, isWrite);
            }
        }
    }

    return cacheUpdateResponse;
}

Block Cache::updateCache(CacheInfo cacheInfo, CacheResponse* response, Block tmpFoundBlock, Block block, bool isWrite){
    Block evictedBlock;

    for(unsigned int i = 0; i < cacheInfo.associativity; i++) {

        // in case we can find the empty block
        if(this->blocks[block.index][i].validBit == 0) {
            this->blocks[block.index][i] = block;
            
            //move vector to the back and delete from the exiting place to support LRU
            this->blocks[block.index].push_back(*(this->blocks[block.index].begin() + i));
            this->blocks[block.index].erase(this->blocks[block.index].begin() + i);

            return evictedBlock;
        }
    }

    response->eviction = 1;
    
    // when all block of that particular index are full, we need to evict a block
    // eviction begin
    if(cacheInfo.rp == ReplacementPolicy::Random) {

        //write in random mode
        int ranVal;

        // if(cacheInfo.wp == WritePolicy::WriteBack) {
        //     //do the eviction
        //     response->eviction = 1;

        //     //set the dirty bit to 1
        //     if(ISWRITE && this->blocks[ai.setIndex][ranVal].dirtyBit == 0) {
        //         this->blocks[ai.setIndex][ranVal].dirtyBit = 1;
        //     }
        // }

        do {
            ranVal = rand() % cacheInfo.associativity;
        } while (this->blocks[block.index][ranVal].tag == tmpFoundBlock.tag);

        //save old value of the block before update new value
        evictedBlock = this->blocks[block.index][ranVal];

        // if(!isWrite) {
        //     this->blocks[block.index][ranVal].validBit = 0;
        // }
        // else {
            this->blocks[block.index][ranVal].tag = block.tag;
        // }
        

    } else {
        //write in LRU
        // if(cacheInfo.wp == WritePolicy::WriteBack) {
        //     //set the dirty bit to 1
        //     if(this->blocks[ai.setIndex][0].dirtyBit == 1) {
        //         //do the eviction
        //         response->eviction = 1;
        //     }
        // }

        evictedBlock = this->blocks[block.index][0];

        // if(!isWrite) {
        //     this->blocks[block.index][0].validBit = 0;
        // } else{
            this->blocks[block.index][0].tag = block.tag;

            this->blocks[block.index].push_back(this->blocks[block.index].front());
            this->blocks[block.index].erase(this->blocks[block.index].begin());
        // }   
    }

    return evictedBlock;
}