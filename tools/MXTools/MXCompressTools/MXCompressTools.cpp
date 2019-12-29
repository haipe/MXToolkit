// MXCompressTools.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
//

#include "pch.h"
#include <tchar.h>
#include <windows.h>
#include <stdio.h>
#include <string.h>
#include <iostream>
#include <fstream>
#include <string>
#include <vector>

#include "libxml/tree.h"

#include "MXStringKit.h"
#include "Win32FileCertUtil.h"

#include "OperateZip.h"

#include "AnonymousPip.h"

#pragma comment(lib,"zlib.lib")
#pragma comment(lib,"libxml.lib")

#define COMPRESS_DIR_NAME "_mx_compress_dir_"

#define MAX_PATH_LEN 1024

#ifdef UNICODE
typedef std::wstring TString;
#else
typedef std::string TString;
#endif

#define MAX_READ_BUFFER_SIZE 10240

bool CompressFile(const TString& src, const TString& dst)
{
    //读取源文件
    std::string sourceFilePath;
    mxtoolkit::WUtf8Convert<std::wstring, std::string>(src.c_str(), &sourceFilePath);

    FILE* sourceFile = ::fopen(sourceFilePath.c_str(), "rb");
    if (!sourceFile)
        return false;

    std::string destFilePath;
    mxtoolkit::WUtf8Convert<std::wstring, std::string>(dst.c_str(), &destFilePath);

    FILE* destFile = ::fopen(destFilePath.c_str(), "wb+");
    if (!destFile)
    {
        ::fclose(sourceFile);
        return false;
    }

    char readBuffer[MAX_READ_BUFFER_SIZE];
    char writeBuffer[MAX_READ_BUFFER_SIZE];
    int count = 0;
    while (!feof(sourceFile))
    {
        memset(readBuffer, 0, sizeof(readBuffer));
        int readSize = ::fread(readBuffer, sizeof(char), MAX_READ_BUFFER_SIZE - 1, sourceFile);
        if (readSize <= 0)
        {
            int n = feof(sourceFile);
            printf("%d,%d\n", count, n);
            printf("%s\n", strerror(errno));
            //结束
            break;
        }

        memset(writeBuffer, 0, sizeof(writeBuffer));
        uLongf writeSize = MAX_READ_BUFFER_SIZE;
        int compressResult = compress((Bytef*)writeBuffer, &writeSize, (const Bytef*)readBuffer, readSize);
        if (compressResult != Z_OK && writeSize <= 0)
            break;

        //写入文件
        ::fwrite(writeBuffer, sizeof(char), writeSize, destFile);

        //移动文件位置
        ::fseek(sourceFile, readSize, SEEK_CUR);

        count++;
        printf("---%d, size:%d.\n", count, count * 10240);
    }

    fclose(sourceFile);
    fclose(destFile);

    return true;
}



BOOL GetRootFiles(std::string findPath, std::vector<std::string>& rootFile)
{
    std::string filePath = findPath + "\\*.*";

    WIN32_FIND_DATAA findData;
    HANDLE hError = FindFirstFileA(filePath.c_str(), &findData);
    if (hError == INVALID_HANDLE_VALUE)
    {
        printf("搜索失败!");
        return 0;
    }

    while (::FindNextFileA(hError, &findData))
    {
        // 过虑.和..
        if (strcmp(findData.cFileName, ".") == 0 ||
            strcmp(findData.cFileName, "..") == 0 ||
            strcmp(findData.cFileName, COMPRESS_DIR_NAME) == 0)
            continue;

        // 构造完整路径

        std::string fullPath = findPath + findData.cFileName;
        // 输出本级的文件
        //printf("%s\n", fullPath.c_str());

        rootFile.emplace_back(fullPath);
    }

    return 0;
}

void MakeFileInfo(const std::string& path)
{
    //定义文档和节点指针
    xmlDocPtr doc = xmlNewDoc(BAD_CAST"1.0");

    xmlNodePtr root_node = xmlNewNode(NULL, BAD_CAST"root");
    //设置根节点
    xmlDocSetRootElement(doc, root_node);

    //在根节点中直接创建节点
    xmlNewTextChild(root_node, NULL, BAD_CAST "Version", BAD_CAST "时间戳");

    std::vector<std::string> output;
    GetRootFiles(path, output);

    for (auto item : output)
    {
        //创建一个节点，设置其内容和属性，然后加入根结点
        xmlNodePtr fileNode = xmlNewNode(NULL, BAD_CAST"File");
        xmlNewProp(fileNode, BAD_CAST"val", BAD_CAST "文件名");
        xmlNewProp(fileNode, BAD_CAST"compress", BAD_CAST "true");
        xmlNewProp(fileNode, BAD_CAST"version", BAD_CAST "true");

        xmlNewTextChild(fileNode, NULL, BAD_CAST "MD5", BAD_CAST "time");

        xmlAddChild(root_node, fileNode);
    }

    //存储xml文档
    int nRel = xmlSaveFile(std::string(path + "\\xml.xml").c_str(), doc);
    if (nRel != -1)
    {
        std::cout << "一个xml文档被创建,写入" << nRel << "个字节" << std::endl;
    }

    //释放文档内节点动态申请的内存
    xmlFreeDoc(doc);
}

int main(int argc, char* argv[])
{
    std::cout << "Hello World!\n";
            
    //DirectoryList(L"F:\\MX12306\\bin\\output");
    //printf("共%d个文件\n", g_fileCount);

    //CompressFile(L"F:\\node.dll", L"F:\\node.zz");

    while (true)
    {
        std::cout << "please input a dir or file :";

        std::string findDir;
        std::cin >> findDir;


        WIN32_FIND_DATAA findData;
        HANDLE hError = FindFirstFileA(findDir.c_str(), &findData);
        if (hError == INVALID_HANDLE_VALUE)
        {
            std::cout << "not a path.\n";
            continue;
        }

        std::cout << "y or n ?";
        char y;
        std::cin >> y;
        if(y != 'y')
            continue;

        findDir += "\\";
        std::vector<std::string> output;
        //EnumDirFiles("F:\\MX12306\\bin\\", "output", output);
        std::cout << "find path: " << findDir << std::endl;

        GetRootFiles(findDir, output);

        if (output.size() <= 0)
        {
            std::cout << "can not find file." << std::endl;
            return 0;
        }

        std::string destDir = findDir + "\\";
        destDir += COMPRESS_DIR_NAME;
        destDir += "\\";

        CreateDirectoryA(destDir.c_str(), NULL);

        //定义文档和节点指针
        xmlDocPtr doc = xmlNewDoc(BAD_CAST"1.0");

        xmlNodePtr root_node = xmlNewNode(NULL, BAD_CAST"root");
        //设置根节点
        xmlDocSetRootElement(doc, root_node);

        //在根节点中直接创建节点        
        xmlNewTextChild(root_node, NULL, BAD_CAST "Version", BAD_CAST "2019-12-27 18:00:00");

        //压缩 && 写入xml
        for (auto path : output)
        {
            std::string outName = path.substr(path.find_last_of("\\") + 1);
            std::string outPath = destDir + outName;
            OperateZip::Zip(path, outPath);
                        
            //创建一个节点，设置其内容和属性，然后加入根结点
            xmlNodePtr fileNode = xmlNewNode(NULL, BAD_CAST"File");
            xmlNewProp(fileNode, BAD_CAST"val", BAD_CAST outName.c_str());
            xmlNewProp(fileNode, BAD_CAST"compress", BAD_CAST "true");
            xmlNewProp(fileNode, BAD_CAST"version", BAD_CAST "2019-12-27 18:00:00");

            std::string md5 = mxtoolkit::GetMD5<std::string>(outPath);
            xmlNewTextChild(fileNode, NULL, BAD_CAST "MD5", BAD_CAST md5.c_str());

            xmlAddChild(root_node, fileNode);
        }

        //存储xml文档
        int nRel = xmlSaveFile(std::string(destDir + "xml.xml").c_str(), doc);
        if (nRel != -1)
        {
            std::cout << "一个xml文档被创建,写入" << nRel << "个字节" << std::endl;
        }

        //释放文档内节点动态申请的内存
        xmlFreeDoc(doc);
    }
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
