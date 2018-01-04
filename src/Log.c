
#include "Log.h"

int InitLogger(Logger *log, LOGTYPE type, const char pszLogPath[], const char pszLogFile[])
{
	char szLogFileName[512] = "";

	if(!log)
		return -1;

	log->iLogLevel = type;

	if(pszLogPath && *pszLogPath)
	{
		if(pszLogFile && *pszLogFile)
		{
			sprintf(log->szLogPath, "%s", pszLogPath);
			sprintf(log->szLogFile, "%s", pszLogFile);

			sprintf(szLogFileName, "%s/%s", pszLogPath, pszLogFile);
		}
	}
	else if(pszLogFile && *pszLogFile)
	{
		sprintf(log->szLogFile, "%s", pszLogFile);

		sprintf(szLogFileName, "%s", pszLogFile);
	}

	if(*szLogFileName)
	{
		log->iLogFd = open(szLogFileName, O_RDWR|O_CREAT|O_TRUNC, 0644);
		if(log->iLogFd<1)
			return -1;
	}
	else
	{
		log->iLogFd = STDOUT_FILENO;
	}

	return 0;
}

#include <sys/timeb.h>
static int getTimeString(char *buf)
{
	time_t t;
	struct tm *tmp,tmRes;
	struct  timeb stTimeB;
	char timeString[64] = "";
	int buflen = 0;
	memset((char *)&stTimeB,'\0',sizeof(struct  timeb));
	memset((char *)&tmRes,'\0',sizeof(struct tm));

	ftime(&stTimeB);
	t =stTimeB.time;
	tmp=localtime_r(&t,&tmRes);

	if (tmp == NULL)
		return 0;

	if ((buflen=strftime(timeString, sizeof(timeString), "%Y-%m-%d %H:%M:%S", tmp)) == 0)
		return 0;

	memcpy(buf, timeString, buflen);
	buflen+=sprintf(buf+buflen, ".%d", stTimeB.millitm);

	return buflen;
}

int Log(Logger log, LOGTYPE type, const char *fmt, ...)
{
	int  nLogBufferLength = 0;
	char szLogBuffer[1024*8] = "";

	va_list ap;

	if(log.iShowTime)
	{
		nLogBufferLength = getTimeString(szLogBuffer);
		nLogBufferLength+= sprintf(szLogBuffer+nLogBufferLength, " ");
	}

	va_start(ap, fmt);
	nLogBufferLength += vsnprintf(szLogBuffer+nLogBufferLength, 1024*8, fmt, ap);
	va_end(ap);

	write(log.iLogFd, szLogBuffer, nLogBufferLength);

	return 0;
}

int SetLogAttr(Logger *log, int key, int value)
{
	return_val_if_fail(log!=NULL, -1);

	switch(key)
	{
		case SHOW_TIME_STRING:
			log->iShowTime = value;
			break;
		default:
			return 1;
	}
	return 0;
}

int CloseLogger(Logger *log)
{
	if(!log)
		return -1;

	if(log->iLogFd < 2)
		return -1;

	return close(log->iLogFd);
}

void LogDumpHex(Logger log, LOGTYPE type, const char *pMem, unsigned int size, const char *pTitle)
{
    int x, y;
    unsigned int X_line = 0;
    unsigned int Y_line = 0;
    unsigned int LINE_BYTE = 16;
    unsigned int TOTAL_SIZE = size;
    const char *pDumpMem = pMem;
    const char *pDumpTitle = pTitle;
    char szAsciiCode[24] = "";
    unsigned char chBtye;

    if(!pDumpMem || TOTAL_SIZE<1)
        return;

    X_line = LINE_BYTE;
    Y_line = roundup(TOTAL_SIZE, LINE_BYTE)/LINE_BYTE;

    if(pDumpTitle)
        Log(log, type, "%s\n", pDumpTitle);

    for(y=0; y<Y_line; y++)
    {
        memset(szAsciiCode, 0x00, sizeof(szAsciiCode));
				if(y==Y_line-1)
					X_line = TOTAL_SIZE%LINE_BYTE==0?LINE_BYTE:TOTAL_SIZE%LINE_BYTE;

        Log(log, type, "%06X(%04d): ", y*LINE_BYTE, y);

        for(x=0; x<X_line; x++)
        {
            chBtye = pDumpMem[((y*LINE_BYTE)+x)];
            Log(log, type, "%02X ", chBtye);
            sprintf(szAsciiCode+strlen(szAsciiCode), "%c", isprint(chBtye)?chBtye:'.');
        }

        if(X_line - LINE_BYTE)
        {
            for(; x<LINE_BYTE; x++)
            {
                Log(log, type, "   ");
                sprintf(szAsciiCode+strlen(szAsciiCode), "%c", '.');
            }
        }
        Log(log, type, "%s\n", szAsciiCode);
    }
    return;
}
