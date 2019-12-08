/*
	Cache Simulator (Starter Code) by Justin Goins
	Oregon State University
	Fall Term 2019
*/

#include "CacheSimulator.h"
#include "CacheStuff.h"
#include "CacheController.h"

#include <iostream>
#include <fstream>
#include <thread>

using namespace std;

/*
	This function creates the cache and starts the simulator.
	Accepts core ID number, configuration info, and the name of the tracefile to read.
*/
void initializeCache(int id, std::vector<CacheInfo> config, string tracefile) {
	CacheController singlecore = CacheController(config, tracefile);
	singlecore.runTracefile();
}

/*
	This function accepts a configuration file and a trace file on the command line.
	The code then initializes a cache simulator and reads the requested trace file(s).
*/
int main(int argc, char* argv[]) {
	if (argc < 3) {
		cerr << "You need at least two command line arguments. You should provide a configuration file and at least one trace file." << endl;
		return 1;
	}

	CacheInfo cacheInfo;

	// determine how many cache levels the system is using
	unsigned int numCacheLevels;

	// read the configuration file
	cout << "Reading config file: " << argv[1] << endl;
	ifstream infile(argv[1]);
	unsigned int tmp;
	infile >> numCacheLevels;
	
	std::vector<CacheInfo> config(numCacheLevels, cacheInfo);
	infile >> config[0].memoryAccessCycles;
	
	for(unsigned int i = 0; i < numCacheLevels; i++) {
		config[i].memoryAccessCycles = config[0].memoryAccessCycles;
		infile >> config[i].numberSets;
		infile >> config[i].blockSize;
		infile >> config[i].associativity;
		infile >> tmp;
		config[i].rp = static_cast<ReplacementPolicy>(tmp);
		infile >> tmp;
		config[i].wp = static_cast<WritePolicy>(tmp);
		infile >> config[i].cacheAccessCycles;
	}
	infile.close();
	
	// Examples of how you can access the configuration file information
	cout << "System has " << numCacheLevels << " cache(s)." << endl;
	for(unsigned int i = 0; i < numCacheLevels; i++) {
		cout << config[i].numberSets << " sets with " << config[i].blockSize << " bytes in each block. N = " << config[i].associativity << endl;
		
		if (config[i].rp == ReplacementPolicy::Random)
			cout << "Using random replacement protocol" << endl;
		else
			cout << "Using LRU protocol" << endl;
		
		if (config[i].wp == WritePolicy::WriteThrough)
			cout << "Using write-through policy" << endl;
		else
			cout << "Using write-back policy" << endl;
	}
	// For multithreaded operation you can do something like the following...
	// Note that this just shows you how to launch a thread and doesn't address
	// the complexities of how the threads communicate.
	// string tracefile(argv[2]);
	// thread t = thread(initializeCache, 0, config, tracefile);
	// t.detach();

	// For singlethreaded operation, you can use this approach:
	string tracefile(argv[2]);
	initializeCache(0, config, tracefile);

	// for(int i = 0; i < numCacheLevels; i++) {
	// 	initializeCache(0, config[i], tracefile);
	// }

	return 0;
}
