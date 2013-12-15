#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include "_log.h"

#define BUFFER_SIZE (1024 * 1)
static char db_buffer[BUFFER_SIZE];

#ifdef WIN32

#	define vsnprintf _vsnprintf

#endif //#ifdef WIN32

#define db_print(__tag, __p, __format) \
do { \
	int n = 0; \
	va_list args; \
	va_start(args, __format); \
	vsnprintf(db_buffer, BUFFER_SIZE - 1, __format, args); \
	db_buffer[BUFFER_SIZE - 1] = 0; \
	__p(__tag);\
	n = __p(db_buffer); \
	va_end(args); \
	return n; \
} while (0)

int log_d(const char * format, ...)
{
	db_print("debug: ", printf, format);
//	return 0;
}

int log_i(const char * format, ...)
{
	db_print("info : ", printf, format);
}

int log_w(const char * format, ...)
{
	db_print("warn : ", printf, format);
}

int log_e(const char * format, ...)
{
	db_print("error: ", printf, format);
}

int log_v(const char * format, ...)
{
	db_print("verbose: ", printf, format);
}


//////////////////////////////////////////////////////////////////////////
//≤‚ ‘≥Ã–Ú
void testLog(void)
{
	LOGD("ddddddd = %d\n", __LINE__);
	LOGI("iiiiiii = %d\n", __LINE__);
	LOGW("wwwwwww = %d\n", __LINE__);
	LOGE("eeeeeee = %d\n", __LINE__);
	LOGV("vvvvvvv = %d\n", __LINE__);
}