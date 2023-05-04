// DnsWatcherService.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
//
#include <iostream>
#include "WinServiceControl.h"
#include "WebhookHttpService.h"

int main(int argc, char* argv[])
{
	if (argc > 1 && strcmp(argv[1], "--install") == 0)
	{
		WinServiceControl::installService();
		return 0;
	}
	if (argc > 1 && strcmp(argv[1], "--uninstall") == 0)
	{
		WinServiceControl::deleteService();
		return 0;
	}

	WebhookHttpService service;
	service.start_service();
	return 0;
}