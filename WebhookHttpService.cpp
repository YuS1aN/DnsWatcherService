#include "WebhookHttpService.h"
#include "WCommand.h"

#include <vector>

static void log_wprintf(const wchar_t* _format, ...) {
	va_list va;
	va_start(va, _format);
	std::FILE* fp = fopen(s_log_path, "a");
	vfwprintf(fp, _format, va);
	fclose(fp);
	va_end(va);
}

bool WebhookHttpService::start_service()
{
	struct mg_mgr mgr;
	mg_log_set(MG_LL_INFO);
	mg_mgr_init(&mgr);
	struct mg_connection* c;

	if ((c = mg_http_listen(&mgr, s_listening_address, event_handler, &mgr)) == NULL) {
		MG_ERROR(("Cannot listen on %s. Use http://ADDR:PORT or :PORT", s_listening_address));
		log_wprintf(L"Cannot listen address.");
		exit(EXIT_FAILURE);
	}

	MG_INFO(("Mongoose version : v%s", MG_VERSION));
	MG_INFO(("Listening on     : %s", s_listening_address));
	wchar_t* w_address = WCommand::char2wchar(s_listening_address);
	log_wprintf(L"Listening on     : %s.\r\n", w_address);
	delete[] w_address;

	for (;;)
	{
		mg_mgr_poll(&mgr, 1000);
	}

	mg_mgr_free(&mgr);
	return true;
}

void WebhookHttpService::event_handler(struct mg_connection* c, int ev, void* ev_data, void* fn_data)
{
	if (ev == MG_EV_HTTP_MSG) {
		struct mg_http_message* hm = (struct mg_http_message*)ev_data;
		if (mg_http_match_uri(hm, "/webhook")) {
			wchar_t* w_request = WCommand::char2wchar(hm->body.ptr);
			log_wprintf(L"Receive request:\r\n%s\r\n", w_request);
			delete[] w_request;

			char* ipv6_result = mg_json_get_str(hm->body, "$.ipv6Result");
			if (ipv6_result == NULL)
			{
				mg_http_reply(c, 200, NULL, "Fail.\n");
				return;
			}
			wchar_t* w_ipv6_result = WCommand::char2wchar(ipv6_result, CP_UTF8);
			free(ipv6_result);
			if (wcscmp(w_ipv6_result, L"成功") != 0 && wcscmp(w_ipv6_result, L"未改变") != 0)
			{
				delete[] w_ipv6_result;
				mg_http_reply(c, 200, NULL, "Fail.\n");
				return;
			}
			delete[] w_ipv6_result;
			uint32_t ip = c->rem.ip;
			int ip_arr[4]{};
			for (int i = 0; i < 4; i++)
			{
				ip_arr[i] = ip & 0xFF;
				ip >>= 8;
			}
			if (ip_arr[0] != 192 || ip_arr[1] != 168 || ip_arr[2] != 33)
			{
				mg_http_reply(c, 404, NULL, "\n");
				return;
			}
			char* ipv6_address = mg_json_get_str(hm->body, "$.ipv6Address");
			wchar_t* w_ipv6_address = WCommand::char2wchar(ipv6_address);
			free(ipv6_address);
			std::wstring local_dns;
			get_ipv6_dns(s_interface_name, local_dns);
			if (local_dns != w_ipv6_address)
			{
				modify_ipv6_dns(s_interface_name, w_ipv6_address);
			}
			delete[] w_ipv6_address;
			mg_http_reply(c, 200, NULL, "Success.\n");
			return;
		}
		// For all other URIs,
		mg_http_reply(c, 404, NULL, "\n");
	}
}

void WebhookHttpService::modify_ipv6_dns(const std::wstring interface_name, const std::wstring addr)
{
	std::wstring cmd = L"netsh interface ipv6 set dnsservers \"" + interface_name + L"\" static " + addr;
	const wchar_t* cs = cmd.c_str();

	log_wprintf(L"cmd: %s\r\n", cs);
	HANDLE handle;
	WCommand::createFileHandle(s_w_log_path, handle);
	log_wprintf(L"ret: %d\r\n", WCommand::runCmdAndOutPutRedirect(handle, cs, true));

	//备用DNS配置
	cmd = L"netsh interface ipv6 add dnsservers \"" + interface_name + L"\" " + s_w_dns2;

	cs = cmd.c_str();
	log_wprintf(L"cmd: %s\r\n", cs);
	log_wprintf(L"ret: %d\r\n", WCommand::runCmdAndOutPutRedirect(handle, cs, true));

	CloseHandle(handle);
}

static void splitString(std::string str, char separator, std::vector<std::string>& output)
{
	size_t start = 0, end = 0;
	for (size_t i = 0; i <= str.size(); i++)
	{
		if (str[i] == separator || i == str.size())
		{
			end = i;
			std::string temp;
			temp.append(str, start, end - start);
			output.push_back(temp);
			start = end + 1;
		}
	}
}

static int getCharCount(std::string str, char c)
{
	size_t index = -1;
	int count = 0;
	while ((index = str.find(c, index + 1)) != std::string::npos)
	{
		count++;
	}
	return count;
}

static void getIpSubstring(std::string& line, std::string& output)
{
	size_t begin = line.find_last_of(' ');
	if (!begin || getCharCount(line, ':') <= 1) return;
	output = line.substr(begin + 1, line.length() - begin - 2);
}

void WebhookHttpService::get_ipv6_dns(const std::wstring interface_name, std::wstring& output)
{
	std::wstring cmd = L"netsh interface ipv6 show dnsservers \"" + interface_name + L"\"";
	HANDLE hRead{}, hWrite{};
	SECURITY_ATTRIBUTES sa{ sizeof(SECURITY_ATTRIBUTES), NULL, TRUE };
	if (!CreatePipe(&hRead, &hWrite, &sa, 0))
	{
		log_wprintf(L"Error on CreatePipe().\r\n");
	}

	WCommand::runCmdAndOutPutRedirect(hWrite, cmd.c_str(), true);
	CloseHandle(hWrite);

	std::string res;
	char buffer[1025]{ 0 };
	DWORD count = 0;
	while (true)
	{
		memset(buffer, 0, 1025);
		if (!ReadFile(hRead, buffer, 1024, &count, NULL)) break;
		res += buffer;
		Sleep(100);
	}
	CloseHandle(hRead);

	std::vector<std::string> split;
	splitString(res, '\n', split);

	for (std::string s : split)
	{
		std::string ip;
		getIpSubstring(s, ip);
		if (!ip.empty() && ip != s_dns2)
		{
			wchar_t* w_ip = WCommand::char2wchar(ip.c_str());
			output = w_ip;
			delete[] w_ip;
			break;
		}
	}
}
