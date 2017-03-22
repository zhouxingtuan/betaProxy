//
// Created by IntelliJ IDEA.
// User: AppleTree
// Date: 17/3/22
// Time: 上午8:39
// To change this template use File | Settings | File Templates.
//

#include "log.h"

NS_HIVE_BEGIN

static int g_logLevel = 0;
static int g_currentTime = 0;
static char g_timeString[256] = {0};

void setLogLevel(int level){
	g_logLevel = level;
}
int getLogLevel(void){
	return g_logLevel;
}
const char* getTimeString(void){
	time_t t = time(NULL);
	if(g_currentTime == t){
		return g_timeString;
	}
	struct tm* ptm = localtime(&t);
	sprintf(g_timeString, "%4d-%02d-%02d %02d:%02d:%02d",
		ptm->tm_year + 1900, ptm->tm_mon + 1, ptm->tm_mday, ptm->tm_hour, ptm->tm_min, ptm->tm_sec);
	return g_timeString;
}

NS_HIVE_END
