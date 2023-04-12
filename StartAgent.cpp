//
// Created by TedWu on 2021/11/22.
//
#include "SyncAgent.h"

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
	void pause() {
		// Insert code to implement pausing of ALL athlete threads
	}
	void proceed() {
		// Insert code to implement releasing of all athlete threads
	}

private:
	// Insert any necessary data members including variables, mutexes, locks, conditional variables
};
