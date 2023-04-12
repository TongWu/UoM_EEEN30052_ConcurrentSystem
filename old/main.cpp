// AssignmentPart1.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include "Competitor.h"
#include "ThreadMap.h"
//#include //other .h files
#include <string>
#include <cstdlib>
#include <map>

const int NO_TEAMS = 4;   // Number of teams
const int NO_MEMBERS = 4; //Number of members in each team

void run(Competitor &c)
{
    // Store the thread ID and competitor in a map
    // Read the thread ID
    // Read detail of the competitor

    // Delay for random period

    // Print message stating which competitor has just finished
}

int main()
{
    thread theThreads[NO_TEAMS][NO_MEMBERS];          // Declare two dimensional threads
    Competitor teamsAndMembers[NO_TEAMS][NO_MEMBERS]; // Declare two dimensional Competitor class objects
    // Define elements of teamsAndMembers
    string team_number[NO_TEAMS] = {"EEE", "MATH", "PHY", "CHEM"};
    string person_number[16] = {"Weilian Jiankang", "Pide Lv", "Wujigong",
                                "Laimu Mashi", "Wuliang Yin", "Daniel So",
                                "Wuqiang Yang", "Zhipeng WU", "1", "2", "3",
                                "4", "5", "6", "7", "8"};
    for (int i = 0; i < NO_TEAMS; i++)
    {
        for (int j = 0; j < NO_MEMBERS; j++)
        {
            teamsAndMembers[i][j].setPerson(person_number[j]); // Will only print first four name in person_number[]
            teamsAndMembers[i][j].setTeam(team_number[i]);
            // Print teamsAndMembers for test
            teamsAndMembers[i][j].printCompetitor();
        }
    }
    // Create threads
    // Two dimensional thread need to be created
    for (int i = 0; i < NO_TEAMS; i++)
    {
        for (int j = 0; j < NO_MEMBERS; j++)
        {
            // std::thread(&functionName, objectName, parameterNeedToBePassed)
            /* Using std::ref instead of directly enter object name, is in order to
             * not directly copy an argument to the function, but likely a pointer.
             * So that if the parameter value changes in the function, the value in
             * the main function still changed
             */
            theThreads[i][j] = std::thread(&run, std::ref(teamsAndMembers[i][j]));
        }
    }
    // Join threads
    for (int i = 0; i < NO_TEAMS; i++)
    {
        for (int j = 0; j < NO_MEMBERS; j++)
        {
            theThreads[i][j].join();
        }
    }

    // Default testing and running code from sketlon
    Competitor c("xyz", "abc"); /* Create a class object called "c",
                                                                    with team name "xyz" and person name "abc"*/
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

// for (int i = 0; i < 4; i++)
//        for (int j = 0; j < 4; j++)
//            theThreads[i][j] = std::thread(run, 42, std::ref(teamsAndMembers[i][j]));
