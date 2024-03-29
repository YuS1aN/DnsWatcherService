﻿#ifndef WEBHOOK_HTTP_SERVICE_H
#define WEBHOOK_HTTP_SERVICE_H

#pragma once
#include "mongoose.h"
#include <string>

static const char* s_listening_address = "http://0.0.0.0:11451";
static const wchar_t* s_interface_name = L"以太网";
static const char* s_log_path = "DnsLog.txt";
static const wchar_t* s_w_log_path = L"DnsLog.txt";
static std::string s_dns2 = "2001:4860:4860::8888";
static std::wstring s_w_dns2 = L"2001:4860:4860::8888";
static const int s_request_ip[4] = { 0,0,0,0 }; //TODO Only requests from this IP are allowed. eg. 192.168.1.XXX -> { 192,168,1,0 }

class WebhookHttpService
{
public:
	bool start_service();

	static void get_ipv6_dns(const std::wstring interface_name, std::wstring& output);

private:
	static void event_handler(struct mg_connection* c, int ev, void* ev_data, void* fn_data);

	static void modify_ipv6_dns(const std::wstring interface_name, const std::wstring addr);

	//static void get_ipv6_dns(const std::wstring interface_name);
};

#endif //WEBHOOK_HTTP_SERVICE_H