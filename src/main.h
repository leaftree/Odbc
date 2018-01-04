
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

#define xFree(var) (var=(var==NULL?NULL:(free(var)), NULL))

#define return_val_if_fail_warning(a, b, c) printf("%s\n", c)

#define return_val_if_fail(expr, val) do {\
	if(expr) { } else { \
	return_val_if_fail_warning("≤‚ ‘", "≤‚ ‘", #expr); \
	return(val); }}while(0)\

#define min(x,y) ({ \
  typeof(x) _x = (x); \
  typeof(y) _y = (y); \
  _x < _y ? _x : _y; })

#define max(x,y) ({ \
  typeof(x) _x = (x); \
  typeof(y) _y = (y); \
  _x > _y ? _x : _y; })

#endif /*__MAIN_H__*/
