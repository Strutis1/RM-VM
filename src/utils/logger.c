#include <stdio.h>
#include <string.h>
#include "utils.h"

// write to STDOUT
char writeSTDOUT(const char* msg) {
    size_t n = strlen(msg);
    return fwrite(msg, 1, n, stdout) == n ? 0 : 1;
}

// write to FPTR 
char writeFPTR(const char* msg, const char* fname) {
    FILE* fptr = fopen(fname, "ab");
    if (!fptr) {
        writeSTDOUT("[LOG] [ERROR] failed to open fptr\n");
        return 1;
    }

    size_t n = strlen(msg);
    if (fwrite(msg, 1, n, fptr) != n) {
        writeSTDOUT("[LOG] [ERROR] failed to fwrite\n");
        fclose(fptr);
        return 3;
    }

    if (fclose(fptr)) {
        writeSTDOUT("[LOG] [ERROR] failed to close fptr\n");
        return 4;
    }

    return 0;
}

// write to default FPTR 
char writeDefaultFPTR(const char* msg) {
    return writeFPTR(msg, DEFAULT_FNAME);
}

// wrapper to write to STDOUT and FPTR
char writeBoth(const char* msg, const char* fname) {
    char a = writeSTDOUT(msg);
    char b = writeFPTR(msg, fname);
    return a | b;
}


// the same as writeBoth() but with default fptr
// when writeBoth(...) is: wrapper to write to STDOUT and FPTR
char writeDefaultBoth(const char* msg) {
    char a = writeSTDOUT(msg);
    char b = writeFPTR(msg, DEFAULT_FNAME);
    return a | b;
}

static char w_stdout(const char* s, const char* f){ return writeSTDOUT(s); }
static char w_def_file(const char* s, const char* f){ return writeDefaultFPTR(s); }
static char w_def_both(const char* s, const char* f){ return writeDefaultBoth(s); }
static char w_file(const char* s, const char* f){ return writeFPTR(s,f); }
static char w_both(const char* s, const char* f){ return writeBoth(s,f); }

char g_fname[17] = DEFAULT_FNAME;
char (*logFuncPtr)(const char*, const char*) = w_stdout;

char logM(const char* msg) {
    return logFuncPtr ? logFuncPtr(msg, g_fname) : 0;
}

static int read_name(char *dst){
    if (scanf(" %16[.ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz]", dst) != 1) return 0;
    int c = getchar();
    int clean = 1;
    
    while(c!='\n'&&c!=EOF){
        clean=0;
        c=getchar();
    }
    if(!clean) return 0;
    size_t n = strlen(dst);
    return n > 0 && n <= 16;
}

void generateConfig() {
    printf("0) Write to stdout\n");
    printf("1) Write to fptr\n");
    printf("2) Write to default fptr\n");
    printf("3) Write to default fptr and stdout\n");
    printf("4) Write to custom fptr and stdout\n");
    int ans = getchar();
    int c = getchar();
    (void)c;
    if(ans < '0' || ans > '4') return;

    switch(ans){
        case '0':
            logFuncPtr = w_stdout;
            return;

        case '2':
            logFuncPtr = w_def_file;
            return;

        case '3':
            logFuncPtr = w_def_both;
            return;

        case '1':
            logFuncPtr = w_file;
            break;

        case '4':
            logFuncPtr = w_both;
            break;

    }

    if(!read_name(g_fname)){
        strncpy(g_fname, DEFAULT_FNAME, 16);
        g_fname[16] = '\0';
        logM("[ERROR] [logMain] Going with default logging file\n");
    }
}
