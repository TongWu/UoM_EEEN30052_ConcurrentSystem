// AssignmentPart1.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include "Competitor.h"
#include "ThreadMap.h"
//#include //other .h files
#include <string>
#include <random>
#include <ctime>
#include <mutex>
#include <condition_variable>
#include "SyncAgent.h"
#include "FinishAgent.h"

const int NO_TEAMS = 4;         // Number of teams
const int NO_MEMBERS = 4;       //Number of members in each team
const int NO_MEMBER_EXCHANGES = 3; // Number of baton exchanges per team

std::mt19937 gen(time(0)); //Standard Mersenne_twister_engine seeded with time(0)
ThreadMap tm1;
std::mutex print_mu;

int sleepRandomPeriod();
void printCompetitor(Competitor& c, int sleep_time);
void run(Competitor &c, SyncAgent& s1, SyncAgent& s2, SyncAgent& finish, int p);

class StartAgent : public SyncAgent {
public:
	StartAgent() {};
	void pause() {
		// Insert code to implement pausing of ALL athlete threads
		// Add a unique_lock to pause all the threads
		// Let function run in main do the conditional variable "wait" function to wait StartAgent_mu mutex to unlock
		// Same as isDone
		std::unique_lock<std::mutex> StartAgent_Locker(StartAgent_mu);
		while (!ready)
			StartAgent_cv.wait(StartAgent_Locker);
	}
	void proceed() {
		// Same as Done
		// Insert code to implement releasing of all athlete threads
		std::unique_lock<std::mutex> StartAgent_Locker(StartAgent_mu);
		ready = true;
		StartAgent_cv.notify_all();
	}
private:
	// Insert any necessary data members including variables, mutexes, locks, conditional variables
	bool ready = false;
	std::mutex StartAgent_mu;
	std::condition_variable StartAgent_cv;
};

class EZAgent : public SyncAgent{
public:
	EZAgent() {};
	void pause() {
		// To block a single thread
		std::unique_lock<std::mutex> ExchangeAgent_Locker(ExchangeAgent_mu);
		while (!isExchange)
			ExchangeAgent_cv.wait(ExchangeAgent_Locker);
		isExchange = false;
	}
	void proceed() {
		// To release a single thread
		std::unique_lock<std::mutex> ExchangeAgent_Locker(ExchangeAgent_mu);
		isExchange = true;
		ExchangeAgent_cv.notify_all();
	}
private:
	std::mutex ExchangeAgent_mu;
	std::condition_variable ExchangeAgent_cv;
	bool isExchange = false;
};

int sleepRandomPeriod()  {
	std::uniform_int_distribution<> dis(9000, 15000); //generate a random integer between 1-10.
	int n = dis(gen);
	return n;
}

void printCompetitor(Competitor& c, int sleep_time) {
	std::unique_lock<std::mutex> PrintLocker(print_mu);
	c.printCompetitor();
	std::cout << "Run for: " << float(sleep_time/1000.0) << " seconds" << endl;
}

void run(Competitor &c, SyncAgent& s1, SyncAgent& s2, SyncAgent& finish, int p)
{
	int sleep_time = sleepRandomPeriod();
	s1.pause();
	// Delay for random period
	std::this_thread::sleep_for(std::chrono::milliseconds(sleep_time));
	if(p < NO_MEMBER_EXCHANGES)
		s2.proceed();
	printCompetitor(c, sleep_time);
	c.setRaceTime(sleep_time);
	// Let map insert the object again to store the race time
	tm1.insertThreadPair(c);
	if(p == 3) // If the last competitor has been finished
		finish.pause();
}

int main()
{
	StartAgent startAgent;
	EZAgent exchanges[NO_TEAMS][NO_MEMBER_EXCHANGES];   // Declare two-dimensional EZAgent class objects
	FinishAgent Finish(tm1);
	Results result;
    thread theThreads[NO_TEAMS][NO_MEMBERS];          // Declare two-dimensional threads
    Competitor teamsAndMembers[NO_TEAMS][NO_MEMBERS]; // Declare two-dimensional Competitor class objects
    // Define elements of teamsAndMembers
    string team[NO_TEAMS] = {"Soft", "Hard", "TUTU", "Sheep Village"};
    string person[4][4] = {
			{    "Weilian"     ,  "Pide Lv"  ,   "Wujigong"  , "Laimu Mashi" },   // Team Soft
			{  "Yin Wuliang"   ,  "Daniel So" , "Yang Wuqiang", "Wu Zhipeng" },   // Team Hard
			{    "Hu Tutu"     , "Hu Yingjun" , "Zhang Meili" , "Grandpa Niu"},   // Team TUTU
			{   "Xi Yangyang"  ,"Mei Yangyang", "Lan Yangyang","Fei Yangyang"}    // Team Sheep Village
		    //  Starting Point       EZ 1            EZ2            EZ 3
	};
	// Load data for team and person
    for (int i = 0; i < NO_TEAMS; i++)
    {
        for (int j = 0; j < NO_MEMBERS; j++)
        {
            teamsAndMembers[i][j].setPerson(person[i][j]);
            teamsAndMembers[i][j].setTeam(team[i]);
        }
    }
    // Create threads
    for (int i = 0; i < NO_TEAMS; ++i)
    {
        for (int j = 0; j < NO_MEMBERS; ++j)
        {
			if(j==0)
			// If the competitor should be located at starting point, pass startAgent to run function
			theThreads[i][j] = std::thread(&run, std::ref(teamsAndMembers[i][j]), std::ref(startAgent), std::ref(exchanges[i][j]), std::ref(Finish), j);
			else
			// Other competitors who should be located at exchange zones, pass exchange to run function
			theThreads[i][j] = std::thread(&run, std::ref(teamsAndMembers[i][j]), std::ref(exchanges[i][j-1]), std::ref(exchanges[i][j]), std::ref(Finish), j);
        }
    }
	// Start
	startAgent.proceed();
	// Join all threads
	for (auto & i : theThreads)
		for(auto & j : i)
			j.join();
	// Print Map contents
    tm1.printMapContents();
	// read the result
	/*
	 * Self created method is written below
	 */
	/*
	result = Finish.returnResults();
	float** CompetitorRaceTime = tm1.getRaceTime();
	// Calculate the race time
	double TeamResult[NO_TEAMS] = {0};
	// Calculate the race time(C++ Two-dimensional Array - Standard type)
	for(int i=0; i<NO_TEAMS; ++i)
		for (int j = 0; j < NO_MEMBERS; ++j) {
			TeamResult[i] += CompetitorRaceTime[i][j];
		}
	*/
	result = Finish.returnResults();
	float CompetitorRaceTime[NO_TEAMS][NO_MEMBERS] = {0};
	float TeamResult[NO_TEAMS] = {0};
	// Initialise the iterator
	tm1.initContentIt();
	for (int k=0; k<tm1.ThreadMapSize(); k++) {
			CompetitorRaceTime[k/4][k%4] = float(tm1.getNextMappedVal().getRaceTime()/1000.0);
			TeamResult[k/4] += CompetitorRaceTime[k/4][k%4];
	}

	// Print Race results
	std::cout << "%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%" << endl;
	std::cout << "Race Results" << endl;
	std::cout << "%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%" << endl;
	result.printResults();
	std::cout << "%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%" << endl;
	std::cout << "Total Team Times" << endl;
	std::cout << "%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%" << endl;
	for(int i=0; i<NO_TEAMS; ++i)
		std::cout << "Team " << teamsAndMembers[i][0].getTeam() << ": " << TeamResult[i] << " s" << endl;
	std::cout << "%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%" << endl;
	std::cout << "Individual Times" << endl;
	std::cout << "%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%" << endl;
	for(int i=0; i<NO_TEAMS; ++i) {
		std::cout << "Team " << teamsAndMembers[i][0].getTeam() << ":" << endl;
		for(int j=0; j<NO_MEMBERS; j++)
			std::cout << "Athlete " << teamsAndMembers[i][j].getPerson() << ": " << CompetitorRaceTime[i][j] << " s" << endl;
		std::cout << "%%%%%%%%%%%%%%%" << endl;
	}
	std::cout << "%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%" << endl;
}
