//
// Created by IntelliJ IDEA.
// User: AppleTree
// Date: 17/3/22
// Time: 上午8:39
// To change this template use File | Settings | File Templates.
//

#ifndef __hive__log__
#define __hive__log__

#include "common.h"
#include "timer.h"

NS_HIVE_BEGIN

#define LOG_LEVEL_DEBUG 0
#define LOG_LEVEL_RELEASE 1
#define LOG_LEVEL_ERROR 2

extern void setLogLevel(int level);
extern int getLogLevel(void);
extern const char* getTimeString(void);

#define LOG_DEBUG(fmt, args...)\
do{\
	if( getLogLevel() > LOG_LEVEL_DEBUG ){\
		break;\
	}\
	fprintf(stderr, "[DEBUG][%s][%s:%d %s()]:" fmt "\n", getTimeString(), __FILE__, __LINE__, __FUNCTION__, ##args);\
}while(0);

#define LOG_RELEASE(fmt, args...)\
do{\
	if( getLogLevel() > LOG_LEVEL_RELEASE ){\
		break;\
	}\
	fprintf(stderr, "[RELEASE][%s][%s:%d %s()]" fmt "\n", getTimeString(), __FILE__, __LINE__, __FUNCTION__, ##args);\
}while(0);

#define LOG_ERROR(fmt, ...)\
do{\
	if( getLogLevel() > LOG_LEVEL_ERROR ){\
		break;\
	}\
	fprintf(stderr, "[ERROR][%s][%s:%d %s()]:" fmt "\n", getTimeString(), __FILE__, __LINE__, __FUNCTION__, ##__VA_ARGS__);\
}while(0);

NS_HIVE_END

#endif
