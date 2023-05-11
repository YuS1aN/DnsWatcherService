#include "WCommend.h"


wchar_t* WCommend::char2wchar(const char* cchar)
{
	wchar_t* m_wchar;
	int len = MultiByteToWideChar(CP_ACP, 0, cchar, strlen(cchar), NULL, 0);
	m_wchar = new wchar_t[len + 1];
	MultiByteToWideChar(CP_ACP, 0, cchar, strlen(cchar), m_wchar, len);
	m_wchar[len] = '\0';
	return m_wchar;
}

int WCommend::runCmdAndOutPutRedirect(const std::wstring& outPutFile, const std::wstring& cmd, bool wait)
{
	STARTUPINFOW si;
	PROCESS_INFORMATION pi;

	SECURITY_ATTRIBUTES sa;
	sa.nLength = sizeof(sa);
	sa.lpSecurityDescriptor = NULL;
	sa.bInheritHandle = TRUE;

	HANDLE handle = CreateFileW(outPutFile.c_str(),
		FILE_APPEND_DATA,
		FILE_SHARE_WRITE | FILE_SHARE_READ,
		&sa,
		CREATE_ALWAYS,
		FILE_ATTRIBUTE_NORMAL,
		NULL);
	if (!handle)
	{
		return -1;
	}

	memset(&si, 0, sizeof(STARTUPINFOW));
	memset(&pi, 0, sizeof(PROCESS_INFORMATION));

	si.cb = sizeof(STARTUPINFOW);
	si.dwFlags |= STARTF_USESTDHANDLES;
	si.wShowWindow = SW_HIDE;
	si.hStdInput = NULL;
	si.hStdError = handle;
	si.hStdOutput = handle;

	if (!CreateProcessW(NULL,
		(LPWSTR)cmd.c_str(),
		NULL,
		NULL,
		TRUE,
		CREATE_NO_WINDOW,
		NULL,
		NULL,
		&si,
		&pi))
	{
		return -2;
	}

	if (wait)
	{
		WaitForSingleObject(pi.hProcess, INFINITE);
	}

	CloseHandle(handle);
	CloseHandle(pi.hThread);
	CloseHandle(pi.hProcess);
	return 0;
}
