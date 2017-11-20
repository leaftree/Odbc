
#include "Log.h"

void LogDumpHex(const char *pTitle, const char *pMem, unsigned int size)
{
    int x, y;
    unsigned int X_line = 0;
    unsigned int Y_line = 0;
    unsigned int LINE_BYTE = 16;
    unsigned int TOTAL_SIZE = size;
    const char *pDumpMem = pMem;
    const char *pDumpTitle = pTitle;
    char szAsciiCode[16*3] = "";
    char chBtye;

    if(!pDumpMem || TOTAL_SIZE<1)
        return;

    X_line = LINE_BYTE;
    Y_line = roundup(TOTAL_SIZE, LINE_BYTE)/LINE_BYTE;

    if(pDumpTitle)
        fprintf(stdout, "%s\n", pDumpTitle);

    for(y=0; y<Y_line; y++)
    {
        memset(szAsciiCode, 0x00, sizeof(szAsciiCode));
        if(y==Y_line-1)
            X_line = TOTAL_SIZE%LINE_BYTE;

        fprintf(stdout, "%06X(%06d) ", y*LINE_BYTE, y*LINE_BYTE);

        for(x=0; x<X_line; x++)
        {
            chBtye = pDumpMem[((y*LINE_BYTE)+x)];
            fprintf(stdout, "%02X ", chBtye);
            sprintf(szAsciiCode+strlen(szAsciiCode), "%c", isprint(chBtye)?chBtye:'.');
        }

        if(X_line != LINE_BYTE)
        {
            for(; x<LINE_BYTE; x++)
            {
                fprintf(stdout, "   ");
                sprintf(szAsciiCode+strlen(szAsciiCode), "%c", '.');
            }
        }
        fprintf(stdout, "%s\n", szAsciiCode);
    }
    return;
}
