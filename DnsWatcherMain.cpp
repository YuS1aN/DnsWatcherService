// DnsWatcherService.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
//
#include <iostream>
#include <locale.h> 
#include "WinServiceControl.h"
#include "WebhookHttpService.h"

int main(int argc, char* argv[])
{
	_wsetlocale(LC_ALL, L"chs");

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
	if (argc > 1 && strcmp(argv[1], "--run") == 0)
	{
		WebhookHttpService service;
		service.start_service();
		return 0;
	}

	SERVICE_TABLE_ENTRY DispatchTable[] =
	{
		{ LPWSTR(SERVICE_NAME), (LPSERVICE_MAIN_FUNCTION)WinServiceControl::svcMain},
		{ NULL, NULL }
	};

	// This call returns when the service has stopped. 
	// The process should simply terminate when the call returns.

	if (!StartServiceCtrlDispatcher(DispatchTable))
	{
		WinServiceControl::svcReportEvent(TEXT("StartServiceCtrlDispatcher"));
	}
	return 0;
}