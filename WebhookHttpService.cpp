#include "WebhookHttpService.h"

static const char* s_listening_address = "http://0.0.0.0:11451";

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
			if (ipv6_result != nullptr && !strcmp(ipv6_result, "³É¹¦"))
			{
				mg_http_reply(c, 200, NULL, "Fail.\n");
				return;
			}
			char* ipv6_address = mg_json_get_str(hm->body, "$.ipv6Address");
			modify_ipv6_dns("ÒÔÌ«Íø", ipv6_address);
			mg_http_reply(c, 200, NULL, "Success.\n");
			return;
		}
		// For all other URIs,
		mg_http_reply(c, 404, NULL, "\n");
	}
}

void WebhookHttpService::modify_ipv6_dns(const std::string interface_name, const std::string addr)
{
	std::string cmd = "netsh interface ipv6 set dnsservers \"" + interface_name + "\" static " + addr + " >> log.txt";
	std::system(cmd.c_str());
	cmd = "netsh interface ipv6 add dnsservers \"" + interface_name + "\" 240e:1c:200::1 >> log.txt";
	std::system(cmd.c_str());
}
