// AssignmentPart1.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

// TODO: EZAgent function
//  当前线程结束之后唤醒下一个线程，theThread[i][j+1]

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
#include "Results.h"

const int NO_TEAMS = 4;         // Number of teams
const int NO_MEMBERS = 4;       //Number of members in each team
const int NO_MEMBER_EXCHANGES = 3; // Number of baton exchanges per team
bool ready = false;
// int count = 0;
bool canContinue = false;
bool isExchange = false;
double team_time[NO_TEAMS] = {0};
double winner[2];
std::thread::id temp[4];

// int sleep_time = 1;

std::mt19937 gen(time(0)); //Standard Mersenne_twister_engine seeded with time(0)
ThreadMap tm1;
// std::mutex main_mu;
std::mutex print_mu;
std::mutex insert_mu;
std::condition_variable cv;
std::mutex Team[NO_TEAMS];

int sleepRandomPeriod();
double sleepRandomPeriod_Real();
void printCompetitor(Competitor& c, int sleep_time, int o);
void run(Competitor &c, SyncAgent& s1, SyncAgent& s2, SyncAgent& finish, int o, int p);
void done();
void isDone();
void find_winner();

void done(int team_number) {
	std::unique_lock<std::mutex> done_locker(Team[team_number]);
	canContinue = true;
	cv.notify_one();
}
void isDone(int team_number) {
	std::unique_lock<std::mutex> done_locker(Team[team_number]);
	while (!canContinue)
		cv.wait(done_locker);
	canContinue = false;
}

/*
 * class StartAgent:
 * 1. Block all the threads
 * 2. Let all threads move into their allocated positions
 * 3. Must wait for all threads to be in position
 * 4. Start race
 *
 * Hint: race starts immediately the last thread arrives in position
 * Enhance hint: there is a random time delay between the last thread
 *               getting into position and the race starting
 */
class StartAgent : public SyncAgent {
public:
	StartAgent() = default;
	void pause() override {
		// Insert code to implement pausing of ALL athlete threads
		// Add a unique_lock to pause all the threads
		// Let function run in main do the conditional variable "wait" function to wait StartAgent_mu mutex to unlock
		// Same as isDone
		std::unique_lock<std::mutex> StartAgent_Locker(StartAgent_mu);
		while (!ready)
			StartAgent_cv.wait(StartAgent_Locker);
	}
	void proceed() override {
		// Same as Done
		// Insert code to implement releasing of all athlete threads
		std::unique_lock<std::mutex> StartAgent_Locker(StartAgent_mu);
		ready = true;
		StartAgent_cv.notify_all();
	}

private:
	// Insert any necessary data members including variables, mutexes, locks, conditional variables
	std::mutex StartAgent_mu;
	std::condition_variable StartAgent_cv;
};

/*
 * Each thread becomes blocked in the appropriate
 * SyncAgent Object before the race starts.
 * 1. One StartAgent blocks theThreads[0][j], j = 0, 1, 2, 3.
 * 2. One EZAgent for each team at exchange zone.
 *   2.1. So totally 12 EZAgent objects are required. (3 exchange zone * 4 teams)
 *   2.2. exchange[m][n] blocks theThread[m][n+1], which is released by theThread[m][n]
 */
class EZAgent : public SyncAgent{
public:
	EZAgent() = default;
	void pause() override {
		// To block a single thread
		std::unique_lock<std::mutex> ExchangeAgent_Locker(ExchangeAgent_mu);
		while (!isExchange)
			ExchangeAgent_cv.wait(ExchangeAgent_Locker);
		isExchange = false;
	}
	void proceed() override {
		// To release a single thread
		std::unique_lock<std::mutex> ExchangeAgent_Locker(ExchangeAgent_mu);
		isExchange = true;
		ExchangeAgent_cv.notify_all();
	}
private:
	std::mutex ExchangeAgent_mu;
	std::condition_variable ExchangeAgent_cv;

};

EZAgent exchanges[NO_TEAMS][NO_MEMBER_EXCHANGES];   // Declare two-dimensional EZAgent class objects

int sleepRandomPeriod()  {
	std::uniform_int_distribution<> dis(9000, 15000); //generate a random integer between 1-10.
	int n = dis(gen);
	return n;
}

double sleepRandomPeriod_Real()  {
	std::uniform_real_distribution<> dis(9000, 15000);
	//generate a random integer between 9-15 seconds, this in format milliseconds
	double n = dis(gen);
	return n;
}

void printCompetitor(Competitor& c, int sleep_time, int o) {
	/*std::unique_lock<std::mutex> printLocker(print_mu);
	c.printCompetitor();
	std::cout << "Run for " <<  sleep_time << endl;*/
	std::unique_lock<std::mutex> PrintLocker(print_mu);
	c.printCompetitor();
	team_time[o] += sleep_time;
	std::cout << "Run for: " << float(sleep_time/1000.0) << " seconds" << endl;
	std::cout << "Team " << o+1 << " runs total " << team_time[o]/1000 << " seconds\n" << endl;
	// sleep_time++;
}

void insertPair(Competitor&c) {
	// Add a unique_lock to prevent other threads
	std::unique_lock<std::mutex> insertLocker(insert_mu);
	// Store the thread ID and competitor in the map
	tm1.insertThreadPair(c);
}

void find_winner() {
	winner[0] = team_time[0];
	winner[1] = 0;
	for (int q=1; q<NO_TEAMS; q++) {
		if(winner[0] > team_time[q]) {
			winner[0] = team_time[q];
			winner[1] = q;
		}
	}
}

void run(Competitor &c, SyncAgent& s1, SyncAgent& s2, SyncAgent& finish, int o, int p)
{
	// Store the thread ID and competitor in a map
	// tm1.insertThreadPair(c);
	insertPair(c);
	//int sleep_time = sleepRandomPeriod();
	int sleep_time = sleepRandomPeriod();
	/*
	 * Pause each thread here
	 * Whatever the s1 point to, pause it
	 */
	s1.pause();
	// Delay for random period
	std::this_thread::sleep_for(std::chrono::milliseconds(sleep_time));
	if(p < NO_MEMBER_EXCHANGES)
		s2.proceed();
	/*
	// std::unique_lock<std::mutex> printLocker(print_mu);
	print_mu.lock();
	// printCompetitor(c, sleep_time);
	// Print message stating which competitor has just finished
	c.printCompetitor();
	// And print message stating the sprint time
	team_time[o] += sleep_time;
	std::cout << "Run for " << sleep_time << endl;

	//s.proceed();
	// TODO:
	//  起跑线程在结束后唤醒第一接力线程
	// exchanges[i][1].proceed()
	// s.proceed();
	//if(p < NO_MEMBER_EXCHANGES)                              // If competitors start on the start point finished
	//	exchanges[o][p].proceed();          // Call the next global exchange object

	//proceed the next object

	cout << o << ", " << p << endl;
	print_mu.unlock();*/
	printCompetitor(c, sleep_time, o);

	// Part 3
	// Competitor get time
	c.setRaceTime(sleep_time);
	// Let map insert the object again to store the race time
	insertPair(c);

	if(p == 3) {// If the last competitor has been finished
		finish.pause();
		temp[o] = this_thread::get_id();
	}

}

int main()
{
	int i = 0;
	int j = 0;
	double *winner;
	StartAgent startAgent;
	FinishAgent Finish(tm1);
	Results result;
    thread theThreads[NO_TEAMS][NO_MEMBERS];          // Declare two-dimensional threads
    Competitor teamsAndMembers[NO_TEAMS][NO_MEMBERS]; // Declare two-dimensional Competitor class objects

    // Define elements of teamsAndMembers
    string team_number[NO_TEAMS] = {"Soft", "Hard", "TUTU", "Sheep Village"};
    string person_number[4][4] = {
			{    "Weilian"     ,  "Pide Lv"  ,   "Wujigong"  ,"Laimu Mashi"},   // Team Soft
			{  "Yin Wuliang"   ,  "Daniel So" , "Yang Wuqiang","Wu Zhipeng" },   // Team Hard
			{    "Hu Tutu"     , "Hu Yingjun" , "Zhang Meili" ,"Grandpa Niu"},   // Team TUTU
			{   "Xi Yangyang"  ,"Mei Yangyang", "Lan Yangyang","Fei Yangyang"}   // Team Sheep Village
		    //      EZ 0              EZ 1          EZ2             EZ 3
	};
	// Load data for team and person
    for (i = 0; i < NO_TEAMS; i++)
    {
        for (j = 0; j < NO_MEMBERS; j++)
        {
            teamsAndMembers[i][j].setPerson(person_number[i][j]);
            teamsAndMembers[i][j].setTeam(team_number[i]);
            // Print teamsAndMembers for test
            teamsAndMembers[i][j].printCompetitor();
        }
    }
    // Create threads
    // Two dimensional thread need to be created
    for (i = 0; i < NO_TEAMS;)
    {
        for (j = 0; j < NO_MEMBERS;)
        {
            // std::thread(&functionName, objectName, parameterNeedToBePassed)
            /* Using std::ref instead of directly enter object name, is in order to
             * not directly copy an argument to the function, but likely a pointer.
             * So that if the parameter value changes in the function, the value in
             * the main function still changed
             */
			if(j==0)
			// If the competitor should be located at starting point, pass startAgent to run function
			theThreads[i][j] = std::thread(&run, std::ref(teamsAndMembers[i][j]), std::ref(startAgent), std::ref(exchanges[i][j]), std::ref(Finish), i, j);
			else
			// Other competitors who should be located at exchange zones, pass exchange to run function
			theThreads[i][j] = std::thread(&run, std::ref(teamsAndMembers[i][j]), std::ref(exchanges[i][j-1]), std::ref(exchanges[i][j]), std::ref(Finish), i, j);
			/*
			 * 1. 创建了一个线程，该线程对应的线程名字是theThreads，由于这个线程是一个二位数组线程，所以你还需要确定他在哪一行，哪一列
			 * 2. 所以这个线程在theThread中的第i行，第j列
			 * 3. 这个线程承载了/调用了run函数
			 * 4. run函数需要输入Competitor &c，代表输入c的地址，c是Competitor类的一个对象
			 * 5. 在run（Competitor &c）里，如果你需要调用run函数的时候，你不需要将你要输入的对象名称变为c，这里的c只是在run函数里的名称，相当于代号
			 * 6. std::ref是指引用的意思，用处在于将该对象在其他函数中做出的改动带回到main function中
			 */
			j++;
        }
		i++;
    }
	std::cout<< "----------------Track Resurfaced----------------"<<endl;
	// Do something here
	std::this_thread::sleep_for(std::chrono::seconds(0));

	// Starting gun shot!
	startAgent.proceed();
	// Debug use
	// cout << "startAgent OK" << endl;

	// Join all threads
    for (auto & theThread : theThreads)
	{
		for (auto & j : theThread)
		{
			j.join();
		}
	}
/*	theThreads[0][0].join();
	theThreads[0][1].join();
	theThreads[0][2].join();
	theThreads[0][3].join();
	theThreads[1][0].join();
	theThreads[1][1].join();
	theThreads[1][2].join();
	theThreads[1][3].join();
	theThreads[2][0].join();
	theThreads[2][1].join();
	theThreads[2][2].join();
	cout << "threads[2][2] OK" << endl;
	theThreads[2][3].join();
	theThreads[3][0].join();
	theThreads[3][1].join();
	theThreads[3][2].join();
	cout << "threads[3][2] OK" << endl;
	theThreads[3][3].join();
	cout << "threads[3][3] OK" << endl;*/
	//std::this_thread::sleep_for(std::chrono::seconds(20));

	/*	Default testing and running code from skeleton
	Competitor c("xyz", "abc");
	Create a class object called "c",with team name "xyz" and person name "abc"

	c.printCompetitor();
    tm1.insertThreadPair(c);*/

	// Print Map contents
    tm1.printMapContents();

    std::cout << "Fuck you, world!\n";

	// End of Game
	std::cout << "----------------Game Ended----------------" << endl;
/*	find_winner();
	int winner_team = winner[1];
	double winner_time = winner[0];
	cout << "The winner group is " << team_number[winner_team] << ", which runs for totally " <<  winner_time << endl ;
 */

	// Part 3
	// return a object
	result = Finish.returnResults();
	// Calculate the fastest team
	double MemberResult[NO_TEAMS][NO_MEMBERS] = {0};
	double TeamResult[NO_TEAMS][2] = {0};
	for(i=0; i<NO_TEAMS; ++i) {
		for (j = 0; j < NO_MEMBERS; ++j) {
			MemberResult[i][j] = teamsAndMembers[i][j].getRaceTime();
			TeamResult[i][0] += MemberResult[i][j];
		}
		TeamResult[i][1] = i+1;
	}
	for(i=0; i<NO_TEAMS; ++i) {
		result.setPosition(tm1.getCompetitor(temp[i]));
	}
	result.printResults();

	for(i=0; i<NO_TEAMS; ++i) {
		std::cout << "\nTeam " << teamsAndMembers[i][0].getTeam() << endl;
		std::cout << "Competitor\t\tTime" << endl;
		for(j=0; j<NO_MEMBERS; j++) {
			std::cout << teamsAndMembers[i][j].getPerson() << "  \t\t" << float(teamsAndMembers[i][j].getRaceTime()/1000.0) << endl;
		}

	}
/*	for(i=0; i<NO_TEAMS; i++)
		for(j=0; j<NO_MEMBERS; j++)
			result.setPosition(teamsAndMembers[i][j]);*/
}
