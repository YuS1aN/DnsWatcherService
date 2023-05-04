#ifndef WIN_SERVICE_CONTROL_H
#define WIN_SERVICE_CONTROL_H

#pragma once

#include <winsock2.h>
#include <windows.h>
#include <tchar.h>
#include <strsafe.h>

#pragma comment(lib, "advapi32.lib")

#define SERVICE_NAME TEXT("DnsWatcherService")
#define SERVICE_DESC "Change my v6 dns."

extern SERVICE_STATUS          gSvcStatus;
extern SERVICE_STATUS_HANDLE   gSvcStatusHandle;
extern HANDLE                  ghSvcStopEvent;

class WinServiceControl {
public:

	static void installService();

	static void deleteService();

	static void WINAPI svcMain(DWORD dwArgc, LPTSTR* lpszArgv);

	static void svcReportEvent(LPCTSTR szFunction);

private:

	static void initService(DWORD dwArgc, LPTSTR* lpszArgv);

	static void reportSvcStatus(DWORD dwCurrentState, DWORD dwWin32ExitCode, DWORD dwWaitHint);

	static void svcCtrlHandler(DWORD dwCtrl);
};

#endif //WIN_SERVICE_CONTROL_H