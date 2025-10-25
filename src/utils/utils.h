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