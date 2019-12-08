/*
	Cache Simulator (Starter Code) by Justin Goins
	Oregon State University
	Fall Term 2019
*/

#include "CacheController.h"
#include "CacheStuff.h"
#include <iostream>
#include <fstream>
#include <regex>
#include <cmath>

using namespace std;

CacheController::CacheController(std::vector<CacheInfo> ci, string tracefile) {
	// store the configuration info
	this->ci = ci;
	this->inputFile = tracefile;
	this->outputFile = this->inputFile + ".out";

	for(unsigned int i = 0; i < ci.size(); i++) {
		cout << "inside for: " << ci.size() << endl;
		// compute the other cache parameters
		this->ci[i].numByteOffsetBits = log2(ci[i].blockSize);
		this->ci[i].numSetIndexBits = log2(ci[i].numberSets);
		
		cout << "numByteOffsetBits of: " << i << " = " << this->ci[i].numByteOffsetBits << endl;
		
		cout << "numSetIndexBits of: " << i << " = " << this->ci[i].numSetIndexBits << endl;

		caches.push_back(Cache(ci[i]));
	}

	// initialize the counters
	this->globalCycles = 0;
	this->globalHits = 0;
	this->globalMisses = 0;
	this->globalEvictions = 0;
	
	this->globalReads = 0;
	this->globalWrites = 0;

	// create your cache structure
	// ...

	// manual test code to see if the cache is behaving properly
	// will need to be changed slightly to match the function prototype
	/*
	cacheAccess(false, 0);
	cacheAccess(false, 128);
	cacheAccess(false, 256);

	cacheAccess(false, 0);
	cacheAccess(false, 128);
	cacheAccess(false, 256);
	*/
}

/*
	Starts reading the tracefile and processing memory operations.
*/
void CacheController::runTracefile() {
	cout << "Input tracefile: " << inputFile << endl;
	cout << "Output file name: " << outputFile << endl;
	
	// process each input line
	string line;
	// define regular expressions that are used to locate commands
	regex commentPattern("==.*");
	regex instructionPattern("I .*");
	regex loadPattern(" (L )(.*)(,[[:digit:]]+)$");
	regex storePattern(" (S )(.*)(,[[:digit:]]+)$");
	regex modifyPattern(" (M )(.*)(,[[:digit:]]+)$");

	// open the output file
	ofstream outfile(outputFile);
	// open the output file
	ifstream infile(inputFile);
	// parse each line of the file and look for commands
	while (getline(infile, line)) {
		// these strings will be used in the file output
		string opString, activityString;
		smatch match; // will eventually hold the hexadecimal address string
		unsigned long int address;
		// create a struct to track cache responses
		CacheResponse response;
		response.hit = 0;

		// ignore comments
		if (std::regex_match(line, commentPattern) || std::regex_match(line, instructionPattern)) {
			// skip over comments and CPU instructions
			continue;
		} else if (std::regex_match(line, match, loadPattern)) {
			cout << "Found a load op!" << endl;
			istringstream hexStream(match.str(2));
			hexStream >> std::hex >> address;
			outfile << match.str(1) << match.str(2) << match.str(3);
			std::string printText;
			unsigned int totalCycles = 0; 
			for(unsigned int i = 0; i < ci.size(); i++) {
				if(!response.hit) {
					cacheAccess(this->ci[i], &response, false, address, i);
					totalCycles += response.cycles;
					if(i == ci.size()-1 && !response.hit)
						totalCycles += ci[i].memoryAccessCycles;
					// outfile << " " << response->cycles << " L" << i+1 << (response.hit ? " hit" : " miss") << (response.eviction ? " eviction" : "");
				} else {
					break;
				}
				printText += " L" + to_string(i+1) + (response.hit ? " hit" : " miss") + (response.eviction ? " eviction" : "");
			}
			outfile << " " << totalCycles << printText;
		} else if (std::regex_match(line, match, storePattern)) {
			cout << "Found a store op!" << endl;
			istringstream hexStream(match.str(2));
			hexStream >> std::hex >> address;
			outfile << match.str(1) << match.str(2) << match.str(3);
			std::string printText;
			unsigned int totalCycles = 0; 

			for(unsigned int i = 0; i < ci.size(); i++) {
				if(!response.hit) {
					cacheAccess(this->ci[i], &response, true, address, i);
					
					totalCycles += response.cycles;
					if(i == ci.size()-1){
						if(!response.hit) {
							totalCycles += (this->ci[i].memoryAccessCycles * 2);
						} else {
							totalCycles += this->ci[i].memoryAccessCycles;
						}
					} 
					// outfile << " " << response.cycles << " L" << i+1 << (response.hit ? " hit" : " miss") << (response.eviction ? " eviction" : "");
				} else {
					// update another cache level
					cacheAccess(this->ci[i], &response, true, address, i);
					totalCycles += response.cycles;
					totalCycles += this->ci[i].memoryAccessCycles;
					break;
				}
				printText += " L" + to_string(i+1) + (response.hit ? " hit" : " miss") + (response.eviction ? " eviction" : "");
			}
			outfile << " " << totalCycles << printText;
		} else if (std::regex_match(line, match, modifyPattern)) {
			cout << "Found a modify op!" << endl;
			istringstream hexStream(match.str(2));
			hexStream >> std::hex >> address;
			outfile << match.str(1) << match.str(2) << match.str(3);
			std::string printText;
			unsigned int totalCycles = 0; 

			// first process the read operation
			for(unsigned int i = 0; i < ci.size(); i++) {
				if(!response.hit) {
					cacheAccess(this->ci[i], &response, false, address, i);
					
					totalCycles += response.cycles;
					if(i == ci.size()-1 && !response.hit)
						totalCycles += ci[i].memoryAccessCycles;
					// outfile << " " << response.cycles << " L" << i+1 << (response.hit ? " hit" : " miss") << (response.eviction ? " eviction" : "") << endl;
				} else {
					break;
				}
				printText += " L" + to_string(i+1) + (response.hit ? " hit" : " miss") + (response.eviction ? " eviction" : "");
			}
			outfile << " " << totalCycles << printText << endl;

			outfile << match.str(1) << match.str(2) << match.str(3);
			// now process the write operation
			response.hit = 0;
			printText = "";
			totalCycles = 0; 
			for(unsigned int i = 0; i < ci.size(); i++) {
				if(!response.hit) {
					cacheAccess(this->ci[i], &response, true, address, i);

					totalCycles += response.cycles;

					if(i == ci.size()-1){
						if(!response.hit) {
							totalCycles += (this->ci[i].memoryAccessCycles * 2);
						} else {
							totalCycles += this->ci[i].memoryAccessCycles;
						}
					} 
					// outfile << " " << response.cycles << " L" << i+1 << (response.hit ? " hit" : " miss") << (response.eviction ? " eviction" : "");
				} else {
					// update another cache level
					cacheAccess(this->ci[i], &response, true, address, i);
					totalCycles += response.cycles;
					totalCycles += this->ci[i].memoryAccessCycles;
					break;
				}
				printText += " L" + to_string(i+1) + (response.hit ? " hit" : " miss") + (response.eviction ? " eviction" : "");
			}
			outfile << " " << totalCycles << printText;

		} else {
			throw runtime_error("Encountered unknown line format in tracefile.");
		}
		outfile << endl;
	}
	// add the final cache statistics
	outfile << "Hits: " << globalHits << " Misses: " << globalMisses << " Evictions: " << globalEvictions << endl;
	outfile << "Cycles: " << globalCycles << " Reads: " << globalReads << " Writes: " << globalWrites << endl;

	infile.close();
	outfile.close();
}

/*
	Calculate the block index and tag for a specified address.
*/
AddressInfo CacheController::getAddressInfo(CacheInfo ci, unsigned long int address) {
	AddressInfo ai;
	ai.tag = address >> (ci.numByteOffsetBits+ci.numSetIndexBits);
	ai.setIndex = (address >> ci.numByteOffsetBits) & ((1 << ci.numSetIndexBits)-1);
	return ai;
}

/*
	This function allows us to read or write to the cache.
	The read or write is indicated by isWrite.
*/
void CacheController::cacheAccess(CacheInfo ci, CacheResponse* response, bool isWrite, unsigned long int address, unsigned int index) {
	// determine the index and tag
	AddressInfo ai = getAddressInfo(ci, address);

	cout << "\tSet index: " << ai.setIndex << ", tag: " << ai.tag << endl;
	
	//read or write data to cache
	caches[index].readCache(ci, ai, response, NULL, true, isWrite);

	// your code needs to update the global counters that track the number of hits, misses, and evictions
	if (response->hit) {
		cout << "Address " << std::hex << address << " was a hit." << endl;
		this->globalHits++;
	} else {
		cout << "Address " << std::hex << address << " was a miss." << endl;
		this->globalMisses++;

		if(response->eviction){
			this->globalEvictions++;
		}
	}

	if(isWrite) {
		this->globalWrites++;
	} else {
		this->globalReads++;
	}

	cout << "-----------------------------------------" << endl;

	updateCycles(ci, response, isWrite);
	return;
}

/*
	Compute the number of cycles used by a particular memory operation.
	This will depend on the cache write policy.
*/
void CacheController::updateCycles(CacheInfo ci, CacheResponse* response, bool isWrite) {
	// your code should calculate the proper number of cycles
	response->cycles = 0;

	if(response->hit) {
		// if(isWrite) {
		// 	response->cycles += ci.memoryAccessCycles + ci.cacheAccessCycles;
		// } else {
		// 	response->cycles += ci.cacheAccessCycles;
		// }

		response->cycles += ci.cacheAccessCycles;
	} else {
		// if(isWrite) {
		// 	response->cycles += (ci.memoryAccessCycles + ci.cacheAccessCycles) * 2;
		// }

		// if(!isWrite) {
		// 	response->cycles += ci.memoryAccessCycles + ci.cacheAccessCycles;
		// }

		if(isWrite) {
			response->cycles += ci.cacheAccessCycles * 2;
		} else {
			response->cycles += ci.cacheAccessCycles;
		}
	}

	this->globalCycles += response->cycles;
}
