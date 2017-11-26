
#ifndef __MAIN_H__
#define __MAIN_H__

#include <time.h>
#include <ctype.h>
#include <stdio.h>
#include <fcntl.h>
#include <stdarg.h>
#include <malloc.h>
#include <string.h>
#include <unistd.h>

#include <sys/param.h>

#include "Log.h"
#include "List.h"
#include "DB_Api.h"

void __tracer(const char *file, int line, const char *func, const char *str);
void __tracerNumber(const char *file, int line, const char *func, const int v);

#endif /*__MAIN_H__*/
