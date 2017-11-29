
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

#define xFree(var) var=(var==NULL?NULL:(free(var)), NULL)

#endif /*__MAIN_H__*/
