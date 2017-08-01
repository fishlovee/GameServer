#include "StdAfx.h"
#include "FangKaHttpUnits.h"
#include "restsdk/json/writer.h"
#include "restsdk/json/stringbuffer.h"
#include "restsdk/json/document.h"

#include "restsdk/include/cpprest/http_client.h"
#include "restsdk/include/cpprest/json.h"
#include <iostream>

using namespace web;
using namespace web::http;
using namespace web::http::client;

#ifndef _DEBUG
#pragma comment (lib,"restsdk/Release/cpprest110_1_4.lib")
#else
#pragma comment (lib,"restsdk/Debug/cpprest110d_1_4.lib")
#endif

//服务单元
namespace FangKaHttpUnits 
{
	static utility::string_t s_kUrl = L"http://120.76.118.20:8080/api/";
	static utility::string_t s_kAppID = L"701";
	static utility::string_t s_kAppKey = L"1669h01mes";
	static utility::string_t s_kAppChannel = L"79969";

	std::string WChar2Ansi(LPCWSTR pwszSrc)
	{
		int nLen = WideCharToMultiByte(CP_ACP, 0, pwszSrc, -1, NULL, 0, NULL, NULL);

		if (nLen<= 0) return std::string("");

		char* pszDst = new char[nLen];
		if (NULL == pszDst) return std::string("");

		WideCharToMultiByte(CP_ACP, 0, pwszSrc, -1, pszDst, nLen, NULL, NULL);
		pszDst[nLen -1] = 0;

		std::string strTemp(pszDst);
		delete [] pszDst;

		return strTemp;
	}

	int getUserCard(int iUserID)
	{
		//http://120.76.118.20:8080/api/queryweixin?appid=701&appkey=1669h01mes&channel=79969&u_weixin=testweixin1
		utility::string_t kUrl = 
			s_kUrl+std::wstring(L"queryweixin?")+
			std::wstring(L"appid=")+s_kAppID+
			std::wstring(L"&appkey=")+s_kAppKey+
			std::wstring(L"&channel=")+s_kAppChannel+
			std::wstring(L"&u_weixin=")+toString(iUserID);

		http_client client(kUrl);
		http_response response = client.request(methods::GET).get();
		if(response.status_code() == status_codes::OK)
		{
			if(response.status_code() == status_codes::OK)
			{
				const utility::string_t& v = response.extract_string().get();
				return atoi((const char*)v.c_str());
			}

			return 0;
		}
		return 0;
	}
	int UseCard(int iUserID,int iCout,std::string kHttpChannel)
	{
		//http://120.76.118.20:8080/api/pay/card?appid=701&appkey=1669h01mes&u_id=&wx_id=testweixin1&use_card=1
		utility::string_t kUrl = 
			s_kUrl+std::wstring(L"pay/card?")+
			std::wstring(L"appid=")+s_kAppID+
			std::wstring(L"&appkey=")+s_kAppKey+
			std::wstring(L"&wx_id=")+toString(iUserID)+
			std::wstring(L"&use_card=")+toString(iCout);

		http_client client(kUrl);
		http_response kResout = client.request(methods::GET).get();
		if(kResout.status_code() == status_codes::OK)
		{
			try
			{
				std::wstring obj1 = kResout.extract_string().get();
				rapidjson::Document doc;
				doc.Parse<0>(WChar2Ansi(obj1.c_str()).c_str());
				if (doc.GetParseError())
				{
					return -1;
				}
				std::string card_num = getDataValueStr(&doc,"card_num");
				std::string use_card = getDataValueStr(&doc,"use_card");
				return parseInt(card_num);
			}
			catch (const http_exception& e)
			{
				std::cout << e.what() << std::endl;
			}
		}
		return -1;
	}
};
