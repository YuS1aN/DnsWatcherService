#pragma once
#include <string>
#include <windows.h>

class WCommend
{
public:
	static wchar_t* char2wchar(const char* cchar);

	static int runCmdAndOutPutRedirect(const std::wstring& outPutFile, const std::wstring& cmd, bool wait = false);
};