#ifndef __TRACKER_TRACE_H__
#define __TRACKER_TRACE_H__

#include <stdio.h>
#include "Time.h"

class __CFunctionTracker__;

#ifndef __SW_VERSION__
#define __SW_VERSION__ "Unknown SW"
#endif

#if defined(_DEBUG)

#define ASSERT(expr, ...) \
    ((expr) ? (void(0)) : Assert(#expr, __FILE__, __LINE__, ##__VA_ARGS__, ""))
#define ASSERT_IF(if_expr, expr, ...) \
    ((if_expr) ? ((expr) ? (void(0)) : Assert(#expr, __FILE__, __LINE__, ##__VA_ARGS__, "")) : (void(0)))

#define TRACK_FUNCTION_LIFE_CYCLE __CFunctionTracker__ __tracker__(__FUNCTION__)
#define OUTPUT_DEBUG_TRACE(format, ...) do { \
        TimeSpec __now__; \
        GetProcessElapseTime(&__now__); \
        OutputTrace("[%ld:%7ld] [DEBUG] [%s:%d]  " format, \
                    __now__.Second, __now__.MicroSecond, __FILE__, __LINE__, ##__VA_ARGS__); \
    } while (0)
    
#else   // _DEBUG
#define ASSERT(expr, ...)
#define ASSERT_IF(if_expr, expr, ...)
#define TRACK_FUNCTION_LIFE_CYCLE
#define OUTPUT_DEBUG_TRACE(format, ...)
#endif  // end __DEBUG__

#define OUTPUT_NOTICE_TRACE(format, ...) do { \
        TimeSpec __now__; \
        GetProcessElapseTime(&__now__); \
        OutputTrace("[%ld:%7ld] [NOTICE] [%s:%d]  " format, \
                    __now__.Second, __now__.MicroSecond, __FILE__, __LINE__, ##__VA_ARGS__); \
    } while (0)

#define OUTPUT_WARNING_TRACE(format, ...) do { \
        TimeSpec __now__; \
        GetProcessElapseTime(&__now__); \
        OutputTrace("[%ld:%7ld] [WARNING] [%s:%d]  " format, \
                    __now__.Second, __now__.MicroSecond, __FILE__, __LINE__, ##__VA_ARGS__); \
    } while (0)

#define OUTPUT_ERROR_TRACE(format, ...) do { \
        TimeSpec __now__; \
        GetProcessElapseTime(&__now__); \
        OutputTrace("[%ld:%7ld] [ERROR] [%s:%d]  " format, \
                    __now__.Second, __now__.MicroSecond, __FILE__, __LINE__, ##__VA_ARGS__); \
    } while (0)

#define OUTPUT_RAW_TRACE(format, ...) OutputTrace(format, ##__VA_ARGS__)

void Assert(const char* pExpression, const char* pFilename, int lineno, ...);
void OutputTrace(const char* format, ...);
void OutputStringTrace(const char* pStr, size_t len = 0);
void OutputCallStack();

void DumpBytes(void* pBytes, size_t len);
void DumpString(char* pStr, size_t len);

class __CFunctionTracker__
{
public:
    __CFunctionTracker__(const char* pFuncName) : m_pFuncName(pFuncName)
    {
        TimeSpec now;
        GetProcessElapseTime(&now);
        OutputTrace("[%ld:%7ld] [FUNC] %s <ENTER>\n", now.Second, now.MicroSecond, pFuncName);
    }
    ~__CFunctionTracker__()
    {
        TimeSpec now;
        GetProcessElapseTime(&now);
        OutputTrace("[%ld:%7ld] [FUNC] %s <EXIT>\n", now.Second, now.MicroSecond, m_pFuncName);
    }

private:
    const char* m_pFuncName;
};

#endif
