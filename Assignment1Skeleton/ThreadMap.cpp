#include "ThreadMap.h"

ThreadMap::ThreadMap() {};

// ThreadMap will compile but won't do anyhing useful until code is added to insertThreadPair and printMapContents
void ThreadMap::insertThreadPair(Competitor c) {
    // create a threadID, Competitor pair using a call to std::make_pair 
    // store the pair in the map using the map insert member function

}

Competitor ThreadMap::getThreadId() {
	std::map <std::thread::id, Competitor>::iterator it = threadComp.find(std::this_thread::get_id());
	if (it == threadComp.end())
		return Competitor::makeNull();						//	alternatively	return *(new Competitor(" ", " "));
	else 
		return it->second;									// i.e. the second item in the pair
}

void ThreadMap::printMapContents() {
	std::cout << "MAP CONTENTS:" << std::endl;
	// you need to write the rest!
	cout << "END MAP CONTENTS" << endl;
}

int ThreadMap::ThreadMapSize() { return threadComp.size(); }
