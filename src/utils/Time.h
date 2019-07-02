#ifndef __TRACKER_TIME_H__
#define __TRACKER_TIME_H__

#include <cstdint>
#include <Windows.h>

using namespace std;

struct TimeSpec {
	int64_t Second;
	int64_t MicroSecond;
};

TimeSpec* GetProcessStartTime();
void GetProcessElapseTime(TimeSpec* pNow);

void Ticket2TimeSpec(LARGE_INTEGER* pTicket, TimeSpec* pTS);

#endif