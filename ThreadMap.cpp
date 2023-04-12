#include "ThreadMap.h"

ThreadMap::ThreadMap() {}
// ThreadMap will compile but won't do anyhing useful until code is added to insertThreadPair and printMapContents
void ThreadMap::insertThreadPair(Competitor c) {
    // create a threadID, Competitor pair using a call to std::make_pair
    // store the pair in the map using the map insert member function
	std::unique_lock<std::mutex> insert_locker(mu);
	threadComp.insert(std::make_pair(this_thread::get_id(), c));
}

Competitor ThreadMap::getCompetitor () {
    std::map <std::thread::id, Competitor>::iterator it = threadComp.find(std::this_thread::get_id());
    if (it == threadComp.end())
        return Competitor::makeNull();						//	alternatively	return *(new Competitor(" ", " "));
    else
        return it->second;									// i.e. the second item in the pair
}

void ThreadMap::printMapContents() {
    std::cout << "MAP CONTENTS:" << std::endl;
    // you need to write the rest!
	std::map<std::thread::id, Competitor>::iterator it = threadComp.begin();
	for(;it!=threadComp.end(); ++it) {
		std::cout << it->first << "::";
		it->second.printCompetitor();
	}
    cout << "END MAP CONTENTS" << endl;
}

int ThreadMap::ThreadMapSize() { return threadComp.size(); }

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

float** ThreadMap::getRaceTime() {
	std::map <std::thread::id, Competitor>::iterator it = threadComp.begin();
	float **FinishCompetitorRT = {nullptr};
	FinishCompetitorRT = new float*[4];
	for(int i=0; i<4; ++i)
		FinishCompetitorRT[i] = new float[4];
	int i = 0;
	int j = 0;
	for(; it!=threadComp.end(); it++) {
		if(j==4) {
			++i;
			j = 0;
		}
		FinishCompetitorRT[i][j] = float(it->second.getRaceTime()/1000.0);
		++j;
	}
	return FinishCompetitorRT;
}
