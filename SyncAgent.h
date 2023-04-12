#pragma once
#include <string>
using namespace std;
class SyncAgent {
public:
	SyncAgent() = default;
	virtual void pause() = 0;
	virtual void proceed() = 0;
};
