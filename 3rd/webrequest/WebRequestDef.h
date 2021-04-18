
#pragma once

#include <ObjBase.h>
#include "MXCommon.h"


using mxtoolkit::uint32;

namespace mxwebrequest
{

    enum 
    {
        DEFAULT_TIME_OUT = 10,  //
    };

	enum RequestType
	{
		REQUEST_TYPE_GET = 1,
		REQUEST_TYPE_POST,
		REQUEST_TYPE_DELETE,
		REQUEST_TYPE_PUT
	};

	enum RequestProtocolType
	{
		REQUEST_PROTOCOLTYPE_HTTP  = 1,
		REQUEST_PROTOCOLTYPE_HTTPS
	};

	enum RespondProtocol
	{
		RESPOND_PROTOCOL_JSON = 1,
		RESPOND_PROTOCOL_XML
	};

	typedef struct RequestHeaderItem
	{
		unsigned char len;
		CHAR* data;

		RequestHeaderItem()
		{
			len = 0;
			data = nullptr;
		}
	}tagRequestHeaderItem;

	typedef struct RequestHeader
	{
		unsigned char header_count;
		RequestHeaderItem *headers;

		RequestHeader()
		{
			header_count = 0;
			headers = nullptr;
		}
	}tagRequestHeader;

	enum ProxType
	{
		PROXY_TYPE_HTTP = 1,
		PROXY_TYPE_SOCK4,
		PROXY_TYPE_SOCK5
	};

	typedef struct Proxy
	{
		//proxy type
		ProxType proxy_type;

		//代理端口
		unsigned int proxy_port;

		//代理地址
		CHAR* proxy_host;

		//代理用户名
		CHAR* proxy_username;

		//代理用户名密码
		CHAR* proxy_password;

		Proxy()
		{
			proxy_type = PROXY_TYPE_HTTP;

			proxy_port = 80;

			proxy_host = nullptr;

			proxy_username = nullptr;

			proxy_password = nullptr;
		}
	}tagProxy;

	typedef struct Request
	{
		//request type : get post put
		RequestType         request_type;

		//request protocol : https http
		RequestProtocolType request_protocol;

		//respond data protocol : json xml
		RespondProtocol     respond_data_protocol;

		//代理
		Proxy *request_proxy;

		//request headers
		RequestHeader *request_headers;

		//request timeout
		int  timeout; // s

		//request host
		CHAR* request_host;

		//request param
		CHAR* request_param;

		Request()
		{
			request_type = REQUEST_TYPE_POST;
			request_protocol = REQUEST_PROTOCOLTYPE_HTTPS;
			respond_data_protocol = RESPOND_PROTOCOL_JSON;
			request_proxy = nullptr;
			request_headers = nullptr;
			timeout = DEFAULT_TIME_OUT;
			request_host = nullptr;
			request_param = nullptr;
		}
	}tagRequest;

	typedef struct Respond
	{
		unsigned int code;
		unsigned int buffer_size;
		CHAR* buffer;
	}tagRespond;


#define REQUEST_HEADER_RESPOND_NOTIFY      1
#define REQUEST_DATA_RESPOND_NOTIFY        2
#define REQUEST_COMPLETE_NOTIFY            3


	interface IRespondNotify
	{
        virtual void OnHeaderRespond(uint32 nID, const char *pData, uint32 nSize) {};
        virtual void OnDataRespond(uint32 nID, const char *pData, uint32 nSize) {};
        virtual void OnCompleteRespond(uint32 nID, uint32 nCode, const char *pData, uint32 nSize) {};
	};
}

