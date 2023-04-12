#pragma once
#include <iostream>
#include <thread>
#include <map>
#include <string>
#include <mutex>
#include "Competitor.h"
class ThreadMap {
private:
    std::map <std::thread::id, Competitor> threadComp;  //map of thread ids v. Competitors
	std::map <std::thread::id, Competitor>::iterator contentIt;
	std::mutex mu;
public:
    ThreadMap();
    void insertThreadPair(Competitor c);
	Competitor getCompetitor();
    void printMapContents();
    int ThreadMapSize();
	float **getRaceTime();
	void initContentIt();
	Competitor getNextMappedVal();
};
