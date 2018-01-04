
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
	int  iLogLevel:4;
	int  iShowTime:1;
	int  iAppend:1;
	char szLogPath[256];
	char szLogFile[256];
};

#define ON  1
#define OFF 0

#define SON "ON"
#define SOFF "OFF"

#define SHOW_TIME_STRING 3

int InitLogger(Logger *log, LOGTYPE type, const char pszLogPath[], const char pszLogFile[]);

int Log(Logger log, LOGTYPE type, const char *fmt, ...);

int SetLogAttr(Logger *log, int key, int value);

int CloseLogger(Logger *log);

void LogDumpHex(Logger log, LOGTYPE type, const char *pMem, unsigned int size,  const char *pTitle);

#endif /*__LOG_H__*/
