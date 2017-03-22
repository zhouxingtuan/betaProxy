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

void setLogLevel(int level){
	g_logLevel = level;
}
int getLogLevel(void) const{
	return g_logLevel;
}

NS_HIVE_END
