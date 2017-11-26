
#ifndef __LOG_H__
#define __LOG_H__

#include "main.h"

typedef enum {
	MESSAGE, WARNING, ERROR, FATAR
} LOGTYPE;

typedef struct Logger Logger;

struct Logger
{
	int  iLogFd;
	int  iLogLevel;
	char szLogPath[256];
	char szLogFile[256];
};

int InitLogger(Logger *log, LOGTYPE type, const char pszLogPath[], const char pszLogFile[]);

int Log(Logger log, LOGTYPE type, const char *fmt, ...);

int CloseLogger(Logger *log);

void LogDumpHex(Logger log, LOGTYPE type, const char *pMem, unsigned int size,  const char *pTitle);

#endif /*__LOG_H__*/
