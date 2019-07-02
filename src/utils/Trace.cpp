#include "Trace.h"
#include <cstdio>
#include <cstdint>
#include <cctype>
#include "Time.h"

using namespace std;

void Assert(const char* pExpression, const char* pFilename, int lineno, ...)
{
    TimeSpec __now__;
    GetProcessElapseTime(&__now__);
    OutputTrace(
        "[%ld:%7ld] [ASSERTION FAILED] [%s:%d] : %s\n",
        __now__.Second,
        __now__.MicroSecond,
        pFilename,
        lineno,
        pExpression);

    va_list args;
    va_start(args, lineno);
    const char* pFormat = va_arg(args, const char*);
    if (pFormat && *pFormat != '\0') {
        vprintf(pFormat, args);
    }
    va_end(args);
    OutputTrace("Version: %s\n", __SW_VERSION__);
    OutputCallStack();
    _exit(-1);
}

void OutputTrace(const char* format, ...)
{
    va_list ap;

    va_start(ap, format);
    vprintf(format, ap);
    va_end(ap);
    fflush(stdout);
}

void OutputStringTrace(const char* pStr, uint64_t len /* = 0 */)
{
    if (len) {
        for (size_t i = 0; i < len; ++i) {
            if (pStr[i] != '\0') {
                putc(pStr[i], stdout);
            }
        }
        putc('\n', stdout);
        fflush(stdout);
    } else {
        OutputTrace("%s\n", pStr);
    }
}

void OutputCallStack()
{
}

void DumpBytes(void* pData, uint64_t len)
{
    if (len == 0) {
        return;
    }

    size_t colum = 0;
	char* pBytes = reinterpret_cast<char*>(pData);
    printf("\n----------------- Data length: %llu -----------------\n", len);
    for (size_t i = 0; i < len; ++i) {
        if (isprint(pBytes[i])) {
            putchar(pBytes[i]);
        } else {
            printf("[%02x]", pBytes[i]);
        }
        if (++colum == 80) {
            putchar('\n');
            colum = 0;
        }
    }
    printf("\n---------------------------------------------------\n");
    fflush(stdout);
}

void DumpString(char* pStr, uint64_t len)
{
    if (len == 0) {
        return;
    }
    printf("\n----------------- Data length: %llu -----------------\n", len);
    for (uint64_t i = 0; i < len; ++i) {
        char ch = pStr[i];
        if (ch == '\r') {
            putchar('\\');
            putchar('r');
        } else if (ch == '\n') {
            putchar('\\');
            putchar('n');
            putchar(ch);
        } else {
            putchar(ch);
        }
    }
    printf("\n----------------- Data length: %llu -----------------\n", len);
    fflush(stdout);
}
