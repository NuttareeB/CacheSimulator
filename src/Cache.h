#ifndef _CACHE_H
#define _CACHE_H

#include "CacheStuff.h"
#include <vector>

class Cache {
    private:
        std::vector<std::vector<Block>> blocks;
        ReplacementPolicy rp;
	    WritePolicy wp;
    public:
        Cache(CacheInfo);
        Cache();
        CacheUpdateResponse readCache(CacheInfo, AddressInfo, CacheResponse*, Block*, bool, bool);
        Block updateCache(CacheInfo, CacheResponse*, Block, Block, bool);
};

#endif //CACHEH