#include "Time.h"
#include <cstdio>

using namespace std;

TimeSpec* GetProcessStartTime()
{
    static bool s_bInited = false;
    static TimeSpec s_SystemStartTime;

    if (!s_bInited) {
		LARGE_INTEGER count;
		if (QueryPerformanceCounter(&count) != TRUE) {
			fprintf(stderr, "Fatal Error: failed to get clock count.\n");
			exit(-1);
		}
        s_bInited = true;
		Ticket2TimeSpec(&count, &s_SystemStartTime);
    }
    return &s_SystemStartTime;
}

void GetProcessElapseTime(TimeSpec* pNow)
{
    static TimeSpec* s_pSystemStartTime = GetProcessStartTime();

	LARGE_INTEGER count;
	pNow->Second = 0;
	pNow->MicroSecond = 0;
	if (QueryPerformanceCounter(&count) == TRUE) {
		Ticket2TimeSpec(&count, pNow);
		pNow->Second -= s_pSystemStartTime->Second;
		pNow->MicroSecond -= s_pSystemStartTime->MicroSecond;
		if (pNow->MicroSecond < 0) {
			pNow->MicroSecond += 1000000;
			--pNow->Second;
		}
	}
}

void Ticket2TimeSpec(LARGE_INTEGER* pTicket, TimeSpec* pTS)
{
	pTS->Second = pTicket->QuadPart / 1000000;
	pTS->MicroSecond = pTicket->QuadPart - (pTS->Second * 1000000);
}
