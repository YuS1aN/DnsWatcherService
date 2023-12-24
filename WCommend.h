#pragma once
#include <string>
#include <windows.h>

class WCommend
{
public:
	static wchar_t* char2wchar(const char* cchar);

	static wchar_t* char2wchar(const char* cchar, UINT CodePage);

	static int runCmdAndOutPutRedirect(HANDLE& handle, const std::wstring& cmd, bool wait = false);

	static void createFileHandle(const std::wstring& outPutFile, HANDLE& handle);
};