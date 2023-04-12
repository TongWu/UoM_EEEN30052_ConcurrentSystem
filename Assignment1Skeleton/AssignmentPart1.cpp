// AssignmentPart1.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include "Competitor.h"
#include "ThreadMap.h"

int main() {
    Competitor c("xyz", "abc");
    ThreadMap tm;
    c.printCompetitor();
    tm.insertThreadPair(c);
    tm.printMapContents();

    std::cout << "Hello World!\n";
}

//#include <iostream>
//#include <string>
//#include //other .h files
//
//const int NO_TEAMS = 4; 	// number of teams in the race
//const int NO_MEMBERS = 4;	// number of athletes in the team
//
//void run(Competitor& c) {
//    // store thread id and competitor in a map
//    // delay for random period
//    // print message stating which competitor has just finished
//}
//
//int main() {
//    thread theThreads[NO_TEAMS][NO_MEMBERS];
//    Competitor teamsAndMembers[NO_TEAMS][NO_MEMBERS];
//
//    // define elements of teamsAndMembers
//    // create threads (elements of theThreads)
//    // join threads
//
//}