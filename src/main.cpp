//
// Created by IntelliJ IDEA.
// User: AppleTree
// Date: 16/9/26
// Time: 上午8:02
// To change this template use File | Settings | File Templates.
//

#include "proxy.h"

USING_NS_HIVE;

int main(int argc, char *argv[])
{
	// 使用默认的信号处理
	defaultSignalHandle();

	MainWorker::createInstance();
	MainWorker::getInstance()->initialize(1, 2, 2);
	MainWorker::getInstance()->update();
//MAIN_EXIT:
//	sleep(3);
	MainWorker::destroyInstance();

	return 0;
}