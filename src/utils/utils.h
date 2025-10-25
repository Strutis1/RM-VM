#pragma once

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

// ------------------ Please dont break the code under ------------------ \\

#define DEFAULT_FNAME "default_log.log"


// write to STDOUT
char writeSTDOUT(const char* msg);

// write to FPTR 
char writeFPTR(const char* msg, const char* fname);

// write to default FPTR 
char writeDefaultFPTR(const char* msg);

// wrapper to write to STDOUT and FPTR
char writeBoth(const char* msg, const char* fname);

// the same as writeBoth() but with default fptr
// when writeBoth(...) is: wrapper to write to STDOUT and FPTR
char writeDefaultBoth(const char* msg);

extern char g_fname[17];
extern char (*logFuncPtr)(const char*, const char*);

char log(const char* msg);



// ------------------ Please dont break the code above ------------------ \\

static inline void sleep_ms(unsigned ms) {
    #if defined(__STDC_VERSION__) && __STDC_VERSION__ >= 201112L && __has_include(<threads.h>)
        #include <threads.h>
        struct timespec ts;
        ts.tv_sec = ms / 1000u;
        ts.tv_nsec = (long)(ms % 1000u) * 1000000L;
        thrd_sleep(&ts, NULL);
    #elif __has_include(<windows.h>)
        #include <windows.h>
        Sleep(ms);
    #elif __has_include(<unistd.h>)
        #include <unistd.h>
        usleep(ms * 1000u);
    #else
        #include <time.h>
        clock_t end = clock() + (clock_t)(ms * (CLOCKS_PER_SEC / 1000.0));
        while ((clock_t)clock() < end) {}
    #endif
}