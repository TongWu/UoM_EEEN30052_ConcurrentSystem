//
// Created by TedWu on 2021/11/22.
//

#include "SyncAgent.h"
#include <iostream>

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
	void pause() {}
	void proceed() {}
private:
};
