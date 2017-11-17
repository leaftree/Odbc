
#ifndef _DB_API_H_
#define _DB_API_H_ 1

#include <stdio.h>
#include <stdarg.h>

#include <sqlext.h>

#ifdef true
# undef true
# define true 0
#endif

#ifdef false
# undef false
# define false (!true)
#endif

#define DBOP_OK (SQL_SUCCESS)
#define DBOP_NO (SQL_ERROR)

#define DB_NO_DATA 100

#ifdef SUCC
# undef DBOP_OK
# define DBOP_OK (SUCC)
#endif

#ifdef FAIL
# undef DBOP_NO
# define DBOP_NO (FAIL)
#endif

#ifdef NOT_FOUND
# undef DB_NO_DATA
# define DB_NO_DATA (NOT_FOUND)
#endif

//int DBApi_Connect();
//int DBApi_Disconnect();

#endif /* _DB_API_H_ */
