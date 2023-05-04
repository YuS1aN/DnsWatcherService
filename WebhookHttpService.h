#ifndef WEBHOOK_HTTP_SERVICE_H
#define WEBHOOK_HTTP_SERVICE_H

#pragma once
#include "mongoose.h"
#include <string>

static const char* s_listening_address = "http://0.0.0.0:11451";
static const char* s_interface_name = "以太网";

class WebhookHttpService
{
public:
	bool start_service();

private:
	static void event_handler(struct mg_connection* c, int ev, void* ev_data, void* fn_data);

	static void modify_ipv6_dns(const std::string interface_name, const std::string addr);
};

#endif //WEBHOOK_HTTP_SERVICE_H