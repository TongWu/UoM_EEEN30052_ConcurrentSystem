#pragma once

#include <iostream>
#include <mutex>
#include "SyncAgent.h"
#include "ThreadMap.h"
#include "Competitor.h"
#include "Results.h"

class FinishAgent : public SyncAgent {
private:
    Results r;
    std::mutex tum;
    ThreadMap& tMap;
public:
    FinishAgent (ThreadMap& t);
    void proceed();
    void pause();
    Results returnResults();
};