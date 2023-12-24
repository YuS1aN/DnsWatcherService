#include "WCommand.h"


wchar_t* WCommand::char2wchar(const char* cchar)
{
	return char2wchar(cchar, CP_ACP);
}

wchar_t* WCommand::char2wchar(const char* cchar, UINT CodePage)
{
	wchar_t* m_wchar;
	int len = MultiByteToWideChar(CodePage, 0, cchar, strlen(cchar), NULL, 0);
	m_wchar = new wchar_t[len + 1];
	MultiByteToWideChar(CodePage, 0, cchar, strlen(cchar), m_wchar, len);
	m_wchar[len] = '\0';
	return m_wchar;
}

int WCommand::runCmdAndOutPutRedirect(HANDLE& handle, const std::wstring& cmd, bool wait)
{
	if (!handle)
	{
		return -1;
	}

	STARTUPINFOW si;
	PROCESS_INFORMATION pi;

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

	CloseHandle(pi.hThread);
	CloseHandle(pi.hProcess);
	return 0;
}


void WCommand::createFileHandle(const std::wstring& outPutFile, HANDLE& handle)
{
	SECURITY_ATTRIBUTES sa{ sizeof(SECURITY_ATTRIBUTES), NULL, TRUE };
	handle = CreateFileW(outPutFile.c_str(),
		FILE_APPEND_DATA,
		FILE_SHARE_WRITE | FILE_SHARE_READ,
		&sa,
		CREATE_ALWAYS,
		FILE_ATTRIBUTE_NORMAL,
		NULL);
}