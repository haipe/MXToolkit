#include "win32/wmi_utils.h"

#include <iostream>


#include <windows.h>
#include <comdef.h>
#include <Wbemidl.h>

#include "base/string_convert.h"

#pragma comment(lib, "wbemuuid.lib")

_BEGIN_MX_KIT_NAME_SPACE_


namespace
{
    std::shared_ptr<WMIUtils::IniComtUtils> g_initUtils = nullptr;
}

WMIUtils::IniComtUtils::IniComtUtils()
{
    // Step 1: --------------------------------------------------
    // Initialize COM. ------------------------------------------

    hres = CoInitializeEx(0, COINIT_MULTITHREADED);
}

WMIUtils::IniComtUtils::~IniComtUtils()
{
    if (SUCCEEDED(hres))
    {
        CoUninitialize();
        hres = 0;
    }
}

WMIUtils::WMIUtils()
{
    if (g_initUtils == nullptr)
    {
        g_initUtils = std::make_shared<WMIUtils::IniComtUtils>();
    }

    m_initUtils = g_initUtils;
}


WMIUtils::~WMIUtils()
{
}

Result WMIUtils::Run(const char* wql)
{
    HRESULT hres = m_initUtils->hres; //定义COM调用的返回
    if (FAILED(hres))
    {
        std::cout << "Failed to initialize COM library. Error code = 0x"
            << std::hex << hres << std::endl;

        return 1;                  // Program has failed.
    }

    // Step 2: --------------------------------------------------
    // Set general COM security levels --------------------------
    // Note: If you are using Windows 2000, you need to specify -
    // the default authentication credentials for a user by using
    // a SOLE_AUTHENTICATION_LIST structure in the pAuthList ----
    // parameter of CoInitializeSecurity ------------------------
    //常规安全级别设置(不写系统会调用默认的)
    hres = CoInitializeSecurity(
        NULL,
        -1,                          // COM authentication
        NULL,                        // Authentication services
        NULL,                        // Reserved
        RPC_C_AUTHN_LEVEL_DEFAULT,   // Default authentication 
        RPC_C_IMP_LEVEL_IMPERSONATE, // Default Impersonation  
        NULL,                        // Authentication info
        EOAC_NONE,                   // Additional capabilities 
        NULL                         // Reserved
        );


    if (FAILED(hres))
    {
        std::cout << "Failed to initialize security. Error code = 0x"
            << std::hex << hres << std::endl;

        return 1;                    // Program has failed.
    }

    // Step 3: ---------------------------------------------------
    // Obtain the initial locator to WMI -------------------------
    //创建一个CLSID_WebmLocator对象.
    IWbemLocator *pLoc = NULL;
    hres = CoCreateInstance(
        CLSID_WbemLocator,
        0,
        CLSCTX_INPROC_SERVER,
        IID_IWbemLocator, (LPVOID *)&pLoc);

    if (FAILED(hres))
    {
        std::cout << "Failed to create IWbemLocator object."
            << " Err code = 0x"
            << std::hex << hres << std::endl;
        CoUninitialize();
        return 1;                 // Program has failed.
    }

    // Step 4: -----------------------------------------------------
    // Connect to WMI through the IWbemLocator::ConnectServer method
    //连接WMI
    IWbemServices *pSvc = NULL;

    // Connect to the root\cimv2 namespace with
    // the current user and obtain pointer pSvc
    // to make IWbemServices calls.
    hres = pLoc->ConnectServer(
        _bstr_t(L"ROOT\\CIMV2"), // Object path of WMI namespace
        NULL,                    // User name. NULL = current user
        NULL,                    // User password. NULL = current
        0,                       // Locale. NULL indicates current
        NULL,                    // Security flags.
        0,                       // Authority (e.g. Kerberos)
        0,                       // Context object 
        &pSvc                    // pointer to IWbemServices proxy
        );

    if (FAILED(hres))
    {
        std::cout << "Could not connect. Error code = 0x"
            << std::hex << hres << std::endl;
        pLoc->Release();
        CoUninitialize();
        return 1;                // Program has failed.
    }

    std::cout << "Connected to ROOT\\CIMV2 WMI namespace" << std::endl;


    // Step 5: --------------------------------------------------
    // Set security levels on the proxy -------------------------
    // 设置连接的安全级别
    hres = CoSetProxyBlanket(
        pSvc,                        // Indicates the proxy to set
        RPC_C_AUTHN_WINNT,           // RPC_C_AUTHN_xxx
        RPC_C_AUTHZ_NONE,            // RPC_C_AUTHZ_xxx
        NULL,                        // Server principal name 
        RPC_C_AUTHN_LEVEL_CALL,      // RPC_C_AUTHN_LEVEL_xxx 
        RPC_C_IMP_LEVEL_IMPERSONATE, // RPC_C_IMP_LEVEL_xxx
        NULL,                        // client identity
        EOAC_NONE                    // proxy capabilities 
        );

    if (FAILED(hres))
    {
        std::cout << "Could not set proxy blanket. Error code = 0x"
            << std::hex << hres << std::endl;
        pSvc->Release();
        pLoc->Release();
        CoUninitialize();
        return 1;               // Program has failed.
    }

    // Step 6: --------------------------------------------------
    // Use the IWbemServices pointer to make requests of WMI ----
    //执行你的代码
    //以SELECT开头,后面跟要查询的属性名(字段名)(可使用通配符*),再跟FROM,后面跟你要查询的类的名字
    // 下面是获取操作版本信息的方式
    // For example, get the name of the operating system
    IEnumWbemClassObject* pEnumerator = NULL;
    hres = pSvc->ExecQuery(
        bstr_t("WQL"),
        //        bstr_t("SELECT * FROM Win32_OperatingSystem"),
        bstr_t("SELECT CSDVersion FROM Win32_OperatingSystem"),
        WBEM_FLAG_FORWARD_ONLY | WBEM_FLAG_RETURN_IMMEDIATELY,
        NULL,
        &pEnumerator);

    if (FAILED(hres))
    {
        std::cout << "Query for operating system name failed."
            << " Error code = 0x"
            << std::hex << hres << std::endl;
        pSvc->Release();
        pLoc->Release();
        CoUninitialize();
        return 1;               // Program has failed.
    }

    // Step 7: -------------------------------------------------
    // Get the data from the query in step 6 -------------------
    //枚举查询
    IWbemClassObject *pclsObj = nullptr;
    ULONG uReturn = 0;
    std::cout << "ProcessID  :ProcessName" << std::endl;
    while (pEnumerator)
    {
        HRESULT hr = pEnumerator->Next(WBEM_INFINITE, 1,
            &pclsObj, &uReturn);

        if (0 == uReturn)
        {
            break;
        }

        VARIANT vtProp;
        //         // Get the value of the Name property
        //         hr = pclsObj->Get(L"Name", 0, &vtProp, 0, 0);
        //         CString str(vtProp.bstrVal);
        // 		//        cout << " OS Name : " << str.GetBuffer(NULL) << endl;
        //         cout << " Process+++: " << str.GetBuffer(NULL) << endl;
        //         //cout << " OS Name : " << vtProp.bstrVal << endl;

        // Get the value of the Name property
        hr = pclsObj->Get(L"ProcessID", 0, &vtProp, 0, 0);
        std::cout
            //<< setw(10)
            << (vtProp.uiVal) << std::ends;

        hr = pclsObj->Get(L"Name", 0, &vtProp, 0, 0);
        std::wstring str(vtProp.bstrVal);
        std::cout << ":" << str.c_str() << std::endl;

        VariantClear(&vtProp);

        pclsObj->Release();
        pclsObj = nullptr;
    }

    // Cleanup
    // ========
    // 不要忘记释放COM对象.
    pSvc->Release();
    pLoc->Release();
    pEnumerator->Release();
    //MSDN例子的错误
    if (pclsObj)
        pclsObj->Release();

    CoUninitialize();

    return 0;
}



_END_MX_KIT_NAME_SPACE_
