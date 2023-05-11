#pragma once
#include <string>
#include <processthreadsapi.h>
#include <fileapi.h>
#include <WinBase.h>
#include <WinUser.h>
#include <stringapiset.h>

class WCommend
{
public:
	static wchar_t* char2wchar(const char* cchar);

	static int runCmdAndOutPutRedirect(const std::wstring& outPutFile, const std::wstring& cmd, bool wait = false);
};