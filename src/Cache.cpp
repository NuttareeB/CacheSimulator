#include "Cache.h"
#include <stdlib.h>
#include <iostream>

using namespace std;

Cache::Cache(CacheInfo cacheInfo) {
    Block block;

    block.tag = 0;
    block.dirtyBit = 0;
    block.validBit = 0;

    std::vector<std::vector<Block>> blocks(cacheInfo.numberSets, std::vector<Block> (cacheInfo.associativity, block));
    this->blocks = blocks;
    
 cout << "tag==" << blocks[0][3].tag << endl;

 cout << "initial size: ==" << blocks.size() << endl;

};

void Cache::readCache(CacheInfo cacheInfo, AddressInfo ai, CacheResponse* response) {
    response->hit = 0;
    response->eviction = 0;

    //read
    for(unsigned int i = 0; i < cacheInfo.associativity; i++) {

        //can find the value
        if(this->blocks[ai.setIndex][i].tag == ai.tag && blocks[ai.setIndex][i].validBit == 1){
            response->hit = 1;
            response->eviction = 0;
            //move vector to the back and delete from the exiting place to support LRU
            this->blocks[ai.setIndex].push_back(*(this->blocks[ai.setIndex].begin() + i));
            this->blocks[ai.setIndex].erase(this->blocks[ai.setIndex].begin() + i);
        }
    }

    //cannot find value from cache
    if (response->hit == 0) {
        for(unsigned int i = 0; i < cacheInfo.associativity; i++) {

            // in case we can find the empty block
            if(this->blocks[ai.setIndex][i].validBit == 0) {
                this->blocks[ai.setIndex][i].tag = ai.tag;
                this->blocks[ai.setIndex][i].validBit = 1;
                
                //move vector to the back and delete from the exiting place to support LRU
                this->blocks[ai.setIndex].push_back(*(this->blocks[ai.setIndex].begin() + i));
                this->blocks[ai.setIndex].erase(this->blocks[ai.setIndex].begin() + i);

                return;
            }
        }

        response->eviction = 1;
        
        // when all block of that particular index are full
        if(cacheInfo.rp == ReplacementPolicy::Random) {
            //write in random mode
            int ranVal = rand() % cacheInfo.associativity;

            // if(cacheInfo.wp == WritePolicy::WriteBack) {
            //     //do the eviction
            //     response->eviction = 1;

            //     //set the dirty bit to 1
            //     if(ISWRITE && this->blocks[ai.setIndex][ranVal].dirtyBit == 0) {
            //         this->blocks[ai.setIndex][ranVal].dirtyBit = 1;
            //     }
            // }

            this->blocks[ai.setIndex][ranVal].tag = ai.tag;
        } else {
            //write in LRU
            // if(cacheInfo.wp == WritePolicy::WriteBack) {
            //     //set the dirty bit to 1
            //     if(this->blocks[ai.setIndex][0].dirtyBit == 1) {
            //         //do the eviction
            //         response->eviction = 1;
            //     }
            // }

            this->blocks[ai.setIndex][0].tag = ai.tag;

            this->blocks[ai.setIndex].push_back(this->blocks[ai.setIndex].front());
            this->blocks[ai.setIndex].erase(this->blocks[ai.setIndex].begin());
        }
    }
}