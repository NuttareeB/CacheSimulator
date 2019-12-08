/*
	Cache Simulator (Starter Code) by Justin Goins
	Oregon State University
	Fall Term 2019
*/

#ifndef _CACHECONTROLLER_H_
#define _CACHECONTROLLER_H_

#include "CacheStuff.h"
#include "Cache.h"
#include <string>

class CacheController {
	private:
		unsigned int globalCycles;
		unsigned int globalHits;
		unsigned int globalMisses;
		unsigned int globalEvictions;

		unsigned int globalReads;
		unsigned int globalWrites;

		std::string inputFile, outputFile;

		std::vector<CacheInfo> ci;

		std::vector<Cache> caches;

		// function to allow read or write access to the cache
		void cacheAccess(CacheInfo, CacheResponse*, bool, unsigned long int);
		// function that can compute the index and tag matching a specific address
		AddressInfo getAddressInfo(CacheInfo, unsigned long int);
		// compute the number of clock cycles used to complete a memory access
		void updateCycles(CacheInfo, CacheResponse*, bool);

	public:
		CacheController(std::vector<CacheInfo>, std::string);
		void runTracefile();
};

#endif //CACHECONTROLLER
