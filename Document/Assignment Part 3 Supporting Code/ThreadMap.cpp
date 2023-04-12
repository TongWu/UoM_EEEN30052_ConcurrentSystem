#include "ThreadMap.h"

ThreadMap::ThreadMap() {};

void ThreadMap::insertThreadPair(Competitor c) {
	std::unique_lock<std::mutex> map_locker(mu);
	threadComp.insert(std::make_pair(std::this_thread::get_id(), c)); //insert the threadID and idx into the map
}

Competitor ThreadMap::getCompetitor() {
	std::map <std::thread::id, Competitor>::iterator it = threadComp.find(std::this_thread::get_id());
	if (it == threadComp.end())
		return Competitor::makeNull();						//	alternatively	return *(new Competitor(" ", " "));
	else 
		return it->second;									// i.e. the second item in the pair
}

void ThreadMap::printMapContents() {
	std::cout << "MAP CONTENTS:" << std::endl;
	std::map <std::thread::id, Competitor>::iterator it = threadComp.begin();
	while (it != threadComp.end()) {
		std::cout << it->first << " :: ";
		it->second.printCompetitor();
		it++;
	}
	cout << "END MAP CONTENTS" << endl;
}

void ThreadMap::initContentIt(){
	contentIt = threadComp.begin();
}

Competitor ThreadMap::getNextMappedVal() {
	Competitor c;
	if (contentIt != threadComp.end())
		c = contentIt->second;
	else
		c = contentIt->second.makeNull();
	contentIt++;
	return c;
}

int ThreadMap::ThreadMapSize() { return threadComp.size(); }