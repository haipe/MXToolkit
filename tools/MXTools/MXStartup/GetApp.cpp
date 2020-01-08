#include "stdafx.h"
#include "GetApp.h"

#include "Resource.h"

#include "MXLibxml.h"
#include "Win32FileCertUtil.h"
#include "Win32Process.h"
#include "Win32PathUtil.h"

std::vector<std::string> GetApp::hosts_ = {"",""};


GetApp::~GetApp()
{
    if (getAppThread_.native_handle())
        getAppThread_.join();
}

bool GetApp::Start(HWND mainWnd)
{
    mainWnd_ = mainWnd;
    getAppThread_ = std::thread(std::bind(&GetApp::GetAppProcessor,this));

    return getAppThread_.native_handle() != 0;
}

void GetApp::GetAppProcessor()
{
    //判断是否有 startup.xml，没有就解压 =======================================================================
    std::string startupXml = (mxtoolkit::Win32App<std::string>::CurrentDirectory() + "\\startup.xml");
    if (!mxtoolkit::FileExist(startupXml))
    {
        ::PostMessage(mainWnd_, MSG_GET_APP, MT_UNZIP_STARTUP_XML, 0);

        //从资源里面解压
        void* xmlBuffer;
        int xmlSize = 0;
        if (mxtoolkit::LoadResource(L"XML", IDR_XML_STARTUP, &xmlBuffer, &xmlSize))
        {
            std::fstream f = std::fstream(startupXml.c_str(), std::ios::binary | std::ios::out);
            f.write((const char*)xmlBuffer, xmlSize);
            f.close();
        }
    }

    //加载startup.xml 获取要启动的应用信息=======================================================================
    StartupAppInfo appInfo;
    do
    {
        /*****************打开xml文档********************/
        //必须加上，防止程序把元素前后的空白文本符号当作一个node
        xmlKeepBlanksDefault(0);
        xmlDocPtr xmlDoc = xmlReadFile(startupXml.c_str(), "UTF-8", XML_PARSE_RECOVER);
        if (xmlDoc == NULL)
        {
            printf("error:can't open :%s!\n", startupXml.c_str());
            break;
        }

        /*****************获取xml文档对象的根节对象********************/
        xmlNodePtr rootNode = NULL;
        rootNode = xmlDocGetRootElement(xmlDoc);
        if (rootNode == NULL)
        {
            printf("error: file is empty!\n");
            break;
        }

        xmlChar *xpath = BAD_CAST("//Host"); //xpath语句
        mxtoolkit::LoadResult(xmlDoc, xpath, [&](xmlNodePtr node)
        {
            std::string host = (const char*)XML_GET_CONTENT(node->xmlChildrenNode);
            appInfo.appHosts.emplace_back(host);
            return true;//找多个
        });

        xpath = BAD_CAST("//App"); //xpath语句
        mxtoolkit::LoadResult(xmlDoc, xpath, [&](xmlNodePtr node)
        {
            appInfo.appName = (const char*)XML_GET_CONTENT(node->xmlChildrenNode);

            xmlChar* attrValue = NULL;
            attrValue = xmlGetProp(node, BAD_CAST("file"));
            if (attrValue)
            {
                appInfo.appFile = (const char*)attrValue;
                xmlFree(attrValue);
            }

            attrValue = xmlGetProp(node, BAD_CAST("version"));
            if (attrValue)
            {
                appInfo.appVersion = (const char*)attrValue;
                xmlFree(attrValue);
            }
            return false;//只找一个
        });

        /*****************释放资源********************/
        xmlFreeDoc(xmlDoc);
        xmlCleanupParser();
        xmlMemoryDump();
    } while (0);

    //判断应用是否存在，如果存在，则直接启动 =======================================================================
    std::string appPath = mxtoolkit::Win32App<std::string>::CurrentDirectory() + appInfo.appName;
    appPath += "\\";
    appPath += appInfo.appVersion;
    appPath += "\\";
    appPath += appInfo.appFile;
    bool isAppExist = mxtoolkit::FileExist(appPath);
    if (isAppExist)
    {
        int ret = mxtoolkit::CreateProcess(appPath, "", 1000);//5秒
        //if (ret == 0)
        ::PostMessage(mainWnd_, MSG_GET_APP, MT_RUN_APP, 0);
        return;
    }

    //判断 MXFile.exe 文件是否存在，一样的 =======================================================================
    bool needUnzip = false;
    std::string mxFilePath = mxtoolkit::Win32App<std::string>::CurrentDirectory() + "MXFile.exe";
    unsigned int fileSize = 0;
    bool mxFileExist = mxtoolkit::FileExist(mxFilePath, &fileSize);
    unsigned int resourceFileSize = 0;
    void* resourceFileBuffer = nullptr;
    //解压成功，并且文件大小不一样，则解压覆盖
    if (mxtoolkit::LoadResource(L"E", IDR_E_FILEMANAGER, &resourceFileBuffer, &resourceFileSize) && resourceFileSize != fileSize)
    {
        ::PostMessage(mainWnd_, MSG_GET_APP, MT_UNZIP_MXFILE_EXE, 0);
        std::fstream newFile(mxFilePath.c_str(), std::ios::binary | std::ios::out);
        if (newFile && newFile.is_open())
        {
            newFile.write((const char*)resourceFileBuffer, resourceFileSize);
            newFile.close();
        }
    }

    //下载app.xml 文件 =======================================================================
    std::string appXmlPath = mxtoolkit::Win32App<std::string>::CurrentDirectory() + appInfo.appName + "\\app.xml";
    std::string mxFileParam;// = "-d true";
    mxFileParam += (" -u " + appInfo.appHosts[0] + appInfo.appName + "/app.xml");
    mxFileParam += (" -l " + appXmlPath);

#ifdef _DEBUG
    if (!mxtoolkit::FileExist(appXmlPath))  //Debug的时候 ，不存在才下载
#endif
    {
        ::PostMessage(mainWnd_, MSG_GET_APP, MT_DOWNLOAD_APP_XML, 0);
        mxtoolkit::CreateProcess(mxFilePath, mxFileParam);
    }
    //解析app.xml下载所有文件
    AppRemoteFileList appRemoteFileList;
    do
    {
        xmlKeepBlanksDefault(0);
        xmlDocPtr xmlDoc = xmlReadFile(appXmlPath.c_str(), "UTF-8", XML_PARSE_RECOVER);
        if (xmlDoc == NULL)
        {
            printf("error:can't open :%s!\n", startupXml.c_str());
            break;
        }

        /*****************获取xml文档对象的根节对象********************/
        xmlNodePtr rootNode = NULL;
        rootNode = xmlDocGetRootElement(xmlDoc);
        if (rootNode == NULL)
        {
            printf("error: file is empty!\n");
            break;
        }

        //xmlChar *xpath = BAD_CAST("//App[@name=\"MXWasai.exe\" and @version=\"1.0.0.0\"]//File[text()='node.dll']"); //xpath语句    and File[text()='node.dll']
        xmlChar *xpath = BAD_CAST("//App"); //xpath语句    and File[text()='node.dll']
        mxtoolkit::LoadResult(xmlDoc, xpath, [&](xmlNodePtr appNode)
        {
            AppRemoteFile appRemoteFile;

            xmlChar* attrValue = NULL;
            attrValue = xmlGetProp(appNode, BAD_CAST("version"));
            if (attrValue)
            {
                appRemoteFile.appVersion = (const char*)attrValue;
                xmlFree(attrValue);
            }

            attrValue = xmlGetProp(appNode, BAD_CAST("name"));
            if (attrValue)
            {
                appRemoteFile.appName = (const char*)attrValue;
                xmlFree(attrValue);
            }

            xmlNodePtr fileNode = appNode->xmlChildrenNode;
            while (fileNode)
            {
                if (xmlStrcmp(fileNode->name, (const xmlChar *)"File") == 0)
                {
                    AppFile file;
                    const char* content = (const char*)XML_GET_CONTENT(fileNode->xmlChildrenNode);
                    if (content)
                        file.fileName = content;

                    attrValue = xmlGetProp(fileNode, BAD_CAST("library"));
                    if (attrValue)
                    {
                        file.library = (const char*)attrValue;
                        xmlFree(attrValue);
                    }

                    attrValue = xmlGetProp(fileNode, BAD_CAST("version"));
                    if (attrValue)
                    {
                        file.libraryVersion = (const char*)attrValue;
                        xmlFree(attrValue);
                    }

                    attrValue = xmlGetProp(fileNode, BAD_CAST("md5"));
                    if (attrValue)
                    {
                        file.fileMD5 = (const char*)attrValue;
                        xmlFree(attrValue);
                    }

                    attrValue = xmlGetProp(fileNode, BAD_CAST("compress"));
                    if (attrValue)
                    {
                        file.compress = strcmp("true", (const char*)attrValue) == 0;
                        xmlFree(attrValue);
                    }

                    attrValue = xmlGetProp(fileNode, BAD_CAST("issuance"));
                    if (attrValue)
                    {
                        if (strcmp("beta", (const char*)attrValue) == 0)
                            file.issuance = AppFile::beta;
                        else if (strcmp("trial", (const char*)attrValue) == 0)
                            file.issuance = AppFile::trial;
                        else if (strcmp("release", (const char*)attrValue) == 0)
                            file.issuance = AppFile::release;
                        else if (strcmp("registered", (const char*)attrValue) == 0)
                            file.issuance = AppFile::registered;
                        else// (strcmp("alpha", (const char*)attrValue) == 0)
                            file.issuance = AppFile::alpha;

                        xmlFree(attrValue);
                    }

                    appRemoteFile.files.emplace_back(file);
                }

                fileNode = fileNode->next;
            }

            appRemoteFileList.emplace_back(appRemoteFile);
            return true;
        });


        /*****************释放资源********************/
        xmlFreeDoc(xmlDoc);
        xmlCleanupParser();
        xmlMemoryDump();
    } while (0);

    // 下载最后一个
    while (!appRemoteFileList.empty())
    {
        AppRemoteFile& appRemoteFile = appRemoteFileList[appRemoteFileList.size() - 1];

        std::string baseDir = mxtoolkit::Win32App<std::string>::CurrentDirectory() + appInfo.appName + "\\" + appRemoteFile.appVersion + "\\";

        std::string appDownloadPath = baseDir + appInfo.appFile;

        std::string fileParam;
        //fileParam += "-d true";
        //fileParam += " -c true";
        fileParam += (" -u " + appInfo.appHosts[0] + appInfo.appName + "/" + appInfo.appVersion + "/" + appInfo.appFile);
        //fileParam += (" -m " + item.fileMD5);
        fileParam += (" -l " + appDownloadPath);

        ::PostMessage(mainWnd_, MSG_GET_APP, MT_DOWNLOAD_APP_EXE, 0);
        mxtoolkit::CreateProcess(mxFilePath, fileParam);

        for (auto item : appRemoteFile.files)
        {
            std::string * name = new std::string(item.fileName);
            ::PostMessage(mainWnd_, MSG_GET_APP, MT_DOWNLOAD_APP_LIBRARY, (LPARAM)name);
            std::string fileDownloadPath = baseDir + item.fileName;

            fileParam.clear();
            //fileParam += "-d true";
            if(item.compress)
                fileParam += " -c true";

            fileParam += (" -u " + appInfo.appHosts[0] + item.library + "/" + item.libraryVersion + "/" + item.fileName);
            //fileParam += (" -m " + item.fileMD5);
            fileParam += (" -l " + fileDownloadPath);

            mxtoolkit::CreateProcess(mxFilePath, fileParam);
        }

        ::PostMessage(mainWnd_, MSG_GET_APP, MT_RUN_APP, 0);
        mxtoolkit::CreateProcess(appDownloadPath, "", 0);
        break;
    }
}

