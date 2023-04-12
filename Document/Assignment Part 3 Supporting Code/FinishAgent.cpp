#include "FinishAgent.h"

FinishAgent::FinishAgent(ThreadMap& t) : tMap(t) { }

void FinishAgent::proceed() { }

void FinishAgent::pause() {
    std::unique_lock<std::mutex> lk(tum);
    r.setPosition(tMap.getCompetitor());
}

Results FinishAgent::returnResults() { return r; }