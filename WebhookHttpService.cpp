#include "WebhookHttpService.h"
#include "WCommend.h"

bool WebhookHttpService::start_service()
{
	struct mg_mgr mgr;
	mg_log_set(MG_LL_INFO);
	mg_mgr_init(&mgr);
	struct mg_connection* c;

	if ((c = mg_http_listen(&mgr, s_listening_address, event_handler, &mgr)) == NULL) {
		MG_ERROR(("Cannot listen on %s. Use http://ADDR:PORT or :PORT", s_listening_address));
		exit(EXIT_FAILURE);
	}
	MG_INFO(("Mongoose version : v%s", MG_VERSION));
	MG_INFO(("Listening on     : %s", s_listening_address));

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
			char* ipv6_result = mg_json_get_str(hm->body, "$.ipv6Result");
			MG_INFO(("ipv6_result:%s", ipv6_result));
			if (ipv6_result != nullptr && !strcmp(ipv6_result, "成功"))
			{
				mg_http_reply(c, 200, NULL, "Fail.\n");
				return;
			}
			char* ipv6_address = mg_json_get_str(hm->body, "$.ipv6Address");
			wchar_t* w_ipv6_address = WCommend::char2wchar(ipv6_address);
			modify_ipv6_dns(s_interface_name, w_ipv6_address);
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
	std::FILE* fp = fopen(s_log_path, "a");

	std::wstring cmd = L"netsh interface ipv6 set dnsservers \"" + interface_name + L"\" static " + addr;
	const wchar_t* cs = cmd.c_str();

	fwprintf(fp, L"cmd: %s\r\n", cs);
	fwprintf(fp, L"ret: %d\r\n", WCommend::runCmdAndOutPutRedirect(s_w_log_path, cs, true));

	//备用DNS配置
	//cmd = L"netsh interface ipv6 add dnsservers \"" + interface_name + L"\" 240e:1c:200::1";

	//cs = cmd.c_str();
	//fwprintf(fp, L"cmd: %s\r\n", cs);
	//fwprintf(fp, L"ret: %d\r\n", WCommend::runCmdAndOutPutRedirect(s_w_log_path, cs, true));

	fclose(fp);
}
