// Test.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
//

#include "pch.h"
#include <windows.h>
#include <WinBase.h>
#include <iostream>
#include <string>
#include "IWebRequest.h"
#include "MXDllExportDefine.h"
#include <chrono>
#include <iomanip>
#include <sstream>
#include <iosfwd>
#include "MXSemaphore.h"

mxwebrequest::IWebRequest* wr = nullptr;

int webCount = 0;
mxtoolkit::MXSemaphore se(0);

class WRNotify : public mxwebrequest::IRespondNotify
{
public:
    virtual void OnCompleteRespond(uint32 nID, uint32 nCode, const char *pData, uint32 nSize) override
    {
        printf("Url :%d, data:%s.\n", nID, pData);

        webCount++;
        if (webCount <= 1)
        {
            mxwebrequest::Request rq;
            rq.request_type = mxwebrequest::REQUEST_TYPE_GET;
            rq.request_protocol = mxwebrequest::REQUEST_PROTOCOLTYPE_HTTPS;
            rq.respond_data_protocol = mxwebrequest::RESPOND_PROTOCOL_JSON;
            rq.request_host = (CHAR*)"https://v1.hitokoto.cn";

            unsigned int id = wr->AsynRequest(&rq);
            std::cout << "rquest ID:" << id << std::endl;
        }
        else
        {
            std::cout << "end.\n";
            se.Signal();
        }
    }

};

int main()
{
    std::cout << "Hello World!\n";
    
    mxtoolkit::mx_dll_export mx_dll_function;

    mxwebrequest::IWebRequest* webrequest = nullptr;

#ifdef _DEBUG
    HMODULE webrequest_dll = LoadLibraryA("MXWebRequestD.dll");
#else
    HMODULE webrequest_dll = LoadLibraryA("MXWebRequest.dll");
#endif
    if (webrequest_dll)
    {
        mx_dll_function.mx_dll_init = (mxtoolkit::MX_DLL_FUNCTION_TYPE(mx_dll_init))GetProcAddress(webrequest_dll, "mx_dll_init");
        mx_dll_function.mx_dll_uninit = (mxtoolkit::MX_DLL_FUNCTION_TYPE(mx_dll_uninit))GetProcAddress(webrequest_dll, "mx_dll_uninit");
        mx_dll_function.mx_dll_all_export = (mxtoolkit::MX_DLL_FUNCTION_TYPE(mx_dll_all_export))GetProcAddress(webrequest_dll, "mx_dll_all_export");
        mx_dll_function.mx_dll_get_interface = (mxtoolkit::MX_DLL_FUNCTION_TYPE(mx_dll_get_interface))GetProcAddress(webrequest_dll, "mx_dll_get_interface");
    }

    if (mx_dll_function.mx_dll_init)
        mx_dll_function.mx_dll_init();

    mxtoolkit::mx_dll_export_info* all_export = nullptr;
    if (mx_dll_function.mx_dll_all_export)
    {
        mx_dll_function.mx_dll_all_export(&all_export);
    }

    mxtoolkit::mx_export_interface_info info = *all_export->interface_info;

    if (mx_dll_function.mx_dll_get_interface)
    {
        mx_dll_function.mx_dll_get_interface(&info, (void**)&wr);
    }

    WRNotify notify;
    if (wr)
    {
        wr->Initialize(dynamic_cast<mxwebrequest::IRespondNotify*>(&notify));

        mxwebrequest::Request rq;
        rq.request_type = mxwebrequest::REQUEST_TYPE_GET;
        rq.request_protocol = mxwebrequest::REQUEST_PROTOCOLTYPE_HTTPS;
        rq.respond_data_protocol = mxwebrequest::RESPOND_PROTOCOL_JSON;
        rq.request_host = (CHAR*)"https://v1.hitokoto.cn";

        unsigned int id = wr->AsynRequest(&rq);
        std::cout << "rquest ID:" << id << std::endl;
    }


    se.Wait(-1);

    std::cout << "end 111.\n";
    wr->Uninstall();

    std::cout << "end2222.\n";
    mx_dll_function.mx_dll_uninit();

    std::cout << "end3333333333333.\n";
    return 0;
}

// 运行程序: Ctrl + F5 或调试 >“开始执行(不调试)”菜单
// 调试程序: F5 或调试 >“开始调试”菜单

// 入门提示: 
//   1. 使用解决方案资源管理器窗口添加/管理文件
//   2. 使用团队资源管理器窗口连接到源代码管理
//   3. 使用输出窗口查看生成输出和其他消息
//   4. 使用错误列表窗口查看错误
//   5. 转到“项目”>“添加新项”以创建新的代码文件，或转到“项目”>“添加现有项”以将现有代码文件添加到项目
//   6. 将来，若要再次打开此项目，请转到“文件”>“打开”>“项目”并选择 .sln 文件
