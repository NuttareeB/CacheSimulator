#ifndef _CACHE_H
#define _CACHE_H

#include "CacheStuff.h"
#include <vector>

class Cache {
    private:
        struct Block {
            unsigned long int tag;
            unsigned int dirtyBit;
            unsigned int validBit;
        };

        std::vector<std::vector<Block>> blocks;
        ReplacementPolicy rp;
	    WritePolicy wp;
    public:
        Cache(CacheInfo);
        void readCache(CacheInfo, AddressInfo, CacheResponse*);
};

#endif //CACHEH