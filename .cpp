#pragma hdrstop
#include <iostream>
using namespace std;
#include <wbemcli.h>
#pragma comment(lib, "wbemUuid2.lib")
#pragma argsused
void PrintWMIError(HRESULT hr);

HRESULT modifyDNS() {
	HRESULT hr;
	hr = CoInitializeEx(0, COINIT_MULTITHREADED);
	hr = CoInitializeSecurity(NULL, -1, NULL, NULL,
		RPC_C_AUTHN_LEVEL_PKT_PRIVACY, RPC_C_IMP_LEVEL_IMPERSONATE, NULL,
		EOAC_SECURE_REFS, NULL);


	IWbemLocator* pLocator = NULL;
	IWbemServices* pNamespace = NULL;
	IWbemClassObject* pClass = NULL;
	IWbemClassObject* pInputParamClass = NULL;
	IWbemClassObject* pInputParamInstance = NULL;
	IWbemClassObject* pOutInst = NULL;
	BSTR InstancePath = SysAllocString(
		L"Win32_NetworkAdapterConfiguration.index='7'"); //indexÎªÍø¿¨ºÅ
	BSTR Path = SysAllocString(L"root\\cimv2");
	BSTR ClassPath = SysAllocString(L"Win32_NetworkAdapterConfiguration");
	BSTR MethodName1 = SysAllocString(L"SetDNSServerSearchOrder");
	BSTR MethodName2 = SysAllocString(L"EnableDHCP");
	LPCWSTR MethodName1ArgName = L"DNSServerSearchOrder";
	BSTR dns1 = SysAllocString(L"10.0.0.22");
	BSTR dns2 = SysAllocString(L"10.0.0.23");


	long DnsIndex1[] = { 0 };
	long DnsIndex2[] = { 1 };


	SAFEARRAY* ip_list = SafeArrayCreateVector(VT_BSTR, 0, 2);
	SafeArrayPutElement(ip_list, DnsIndex1, dns1);
	SafeArrayPutElement(ip_list, DnsIndex2, dns2);
	VARIANT dns;
	dns.vt = VT_ARRAY | VT_BSTR;
	dns.parray = ip_list;


	hr = CoCreateInstance(CLSID_WbemLocator, 0, CLSCTX_INPROC_SERVER,
		IID_IWbemLocator, (void**)&pLocator);
	hr = pLocator->ConnectServer(Path, NULL, NULL, NULL, 0, NULL, NULL,
		&pNamespace);
	hr = CoSetProxyBlanket(pNamespace, RPC_C_AUTHN_WINNT, RPC_C_AUTHZ_NONE,
		NULL, RPC_C_AUTHN_LEVEL_CALL, RPC_C_IMP_LEVEL_IMPERSONATE, NULL,
		EOAC_NONE);
	if (!SUCCEEDED(hr)) {
		PrintWMIError(hr);
		return hr;
	}
	if (SUCCEEDED(hr))
		hr = pNamespace->GetObject(ClassPath, 0, NULL, &pClass, NULL);
	if (!SUCCEEDED(hr)) {
		PrintWMIError(hr);
		return hr;
	}
	if (SUCCEEDED(hr))
		hr = pClass->GetMethod(MethodName1, 0, &pInputParamClass, NULL);
	if (!SUCCEEDED(hr)) {
		PrintWMIError(hr);
		return hr;
	}
	if (SUCCEEDED(hr))
		hr = pInputParamClass->SpawnInstance(0, &pInputParamInstance);
	if (!SUCCEEDED(hr)) {
		PrintWMIError(hr);
		return hr;
	}
	if (SUCCEEDED(hr)) {
		hr = pInputParamInstance->Put(MethodName1ArgName, 0, &dns, 0);
	}


	if (!SUCCEEDED(hr)) {
		PrintWMIError(hr);
		return hr;
	}
	if (SUCCEEDED(hr))
		hr = pNamespace->ExecMethod(InstancePath, MethodName1, 0, NULL,
			pInputParamInstance, &pOutInst, NULL);
	if (!SUCCEEDED(hr)) {
		PrintWMIError(hr);
		return hr;
	}
	if (SUCCEEDED(hr))
		hr = pNamespace->ExecMethod(InstancePath, MethodName2, 0, NULL, NULL,
			&pOutInst, NULL);
	if (!SUCCEEDED(hr)) {
		PrintWMIError(hr);
		return hr;
	}
	SysFreeString(InstancePath);
	SysFreeString(Path);
	SysFreeString(ClassPath);
	SysFreeString(MethodName1);
	SysFreeString(MethodName2);
	if (pOutInst)
		pOutInst->Release();
	if (pInputParamInstance)
		pInputParamInstance->Release();
	if (pInputParamClass)
		pInputParamClass->Release();
	if (pClass)
		pClass->Release();
	if (pNamespace)
		pNamespace->Release();
	if (pLocator)
		pLocator->Release();
	CoUninitialize();
	return hr;
}

void PrintWMIError(HRESULT hr) {
	IWbemStatusCodeText* pStatus = NULL;
	HRESULT hres = CoCreateInstance(CLSID_WbemStatusCodeText, 0,
		CLSCTX_INPROC_SERVER, IID_IWbemStatusCodeText, (LPVOID*)&pStatus);
	if (S_OK == hres) {
		BSTR bstrError;
		hres = pStatus->GetErrorCodeText(hr, 0, 0, &bstrError);
		if (S_OK != hres)
			bstrError = SysAllocString(L"Get     last     error     failed");
		BSTR bstrTError = bstrError;
		wchar_t* lpszError = bstrTError;
		printf("%s\n", lpszError);
		pStatus->Release();
		SysFreeString(bstrError);
	}
}