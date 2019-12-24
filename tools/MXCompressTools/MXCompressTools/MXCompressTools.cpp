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


#include "MXStringKit.h"

#include "zlib.h"
#include "zip.h"

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



void EnumDirFiles(const std::string& dirPrefix, const std::string& dirName, std::vector<std::string>& vFiles)
{
    if (dirPrefix.empty())
        return;

    std::string dirNameTmp = dirName;
    std::string dirPre = dirPrefix;

    if (dirNameTmp.find_last_of("\\") != dirNameTmp.length() - 1)
        dirNameTmp += "\\";
    if (dirNameTmp[0] == '\\')
        dirNameTmp = dirNameTmp.substr(1);
    if (dirPre.find_last_of("\\") != dirPre.length() - 1)
        dirPre += "\\";

    std::string path;

    path = dirPre + dirNameTmp;


    CHAR filePathName[MAX_PATH_LEN];
    strcpy(filePathName, path.c_str());
    strcat(filePathName, "\\*.*");

    WIN32_FIND_DATAA findData;
    HANDLE hError;
    hError = ::FindFirstFileA(filePathName, &findData);
    if (hError == INVALID_HANDLE_VALUE)
    {
        printf("搜索失败!");
        return;
    }

    while (::FindNextFileA(hError, &findData))
    {
        // 过虑.和..
        if (strcmp(findData.cFileName, ".") == 0 ||
            strcmp(findData.cFileName, "..") == 0 ||
            strcmp(findData.cFileName, COMPRESS_DIR_NAME) == 0)
            continue;

        std::string innerDir = dirNameTmp + findData.cFileName;
        // 输出本级的文件
        //std::cout << innerDir << std::endl;
        //printf("%s\n", innerDir.c_str());

        if (findData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
        {
            EnumDirFiles(dirPrefix, innerDir, vFiles);
        }
        else
        {
            vFiles.push_back(innerDir);
        }
    }
}

//压缩码流
int WriteInZipFile(zipFile zFile, const std::string& file)
{
    std::fstream f(file.c_str(), std::ios::binary | std::ios::in);
    f.seekg(0, std::ios::end);
    long size = f.tellg();
    f.seekg(0, std::ios::beg);
    if (size <= 0)
    {
        return zipWriteInFileInZip(zFile, NULL, 0);
    }

    char* buf = new char[size];
    f.read(buf, size);
    int ret = zipWriteInFileInZip(zFile, buf, size);
    delete[] buf;
    return ret;
}

//minizip 压缩
int Minizip(std::string src, std::string dest)
{
    if (src.find_last_of("\\") == src.length() - 1)
        src = src.substr(0, src.length() - 1);

    std::cout << "Minizip :" << src << " to " << dest << std::endl;

    struct stat fileInfo;
    stat(src.c_str(), &fileInfo);
    WIN32_FIND_DATAA findData;
    HANDLE hError;
    hError = ::FindFirstFileA(src.c_str(), &findData);
    if (hError == INVALID_HANDLE_VALUE)
    {
        printf("错误路径");
        return -1;
    }

    if (findData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
    {
        size_t pos = src.find_last_of("\\");
        std::string dirName = src.substr(pos + 1);
        std::string dirPrefix = src.substr(0, pos);

        zipFile zFile = zipOpen(dest.c_str(), APPEND_STATUS_CREATE);
        if (zFile == NULL)
        {
            std::cout << "openfile failed" << std::endl;
            return -1;
        }

        std::vector<std::string> vFiles;
        EnumDirFiles(dirPrefix, dirName, vFiles);
        std::vector<std::string>::iterator itF = vFiles.begin();
        for (; itF != vFiles.end(); ++itF)
        {
            zip_fileinfo zFileInfo = { 0 };
            //int ret = zipOpenNewFileInZip(zFile, itF->c_str(), &zFileInfo, NULL, 0, NULL, 0, NULL, 0, 0);
            //int ret = zipOpenNewFileInZip4(zFile,itF->c_str(),&zFileInfo, NULL, 0, NULL, 0, NULL, 0, 9);
            int level = 9;
            int ret = zipOpenNewFileInZip4(zFile, itF->c_str(), &zFileInfo, NULL, 0, NULL, 0, NULL, Z_DEFLATED, level, 0, -MAX_WBITS, DEF_MEM_LEVEL,
                Z_DEFAULT_STRATEGY, NULL, 0, 0, 0);// ZIP_GPBF_LANGUAGE_ENCODING_FLAG);
            if (ret != ZIP_OK)
            {
                std::cout << "openfile in zip failed" << std::endl;
                zipClose(zFile, NULL);
                return -1;
            }

            ret = WriteInZipFile(zFile, dirPrefix + "\\" + (*itF));
            if (ret != ZIP_OK)
            {
                std::cout << "write in zip failed" << std::endl;
                zipClose(zFile, NULL);
                return -1;
            }
        }

        zipClose(zFile, NULL);
        std::cout << "Minizip dir ok" << std::endl;
    }
    else
    {
        size_t pos = src.find_last_of("\\");
        std::string fileName = src.substr(pos + 1);

        zipFile zFile = zipOpen(dest.c_str(), APPEND_STATUS_CREATE);
        if (zFile == NULL)
        {
            std::cout << "openfile failed" << std::endl;
            return -1;
        }

        zip_fileinfo zFileInfo = { 0 };
        //int ret = zipOpenNewFileInZip(zFile, src.c_str(), &zFileInfo, NULL, 0, NULL, 0, NULL, 0, 0);
        int level = 9;
        int ret = zipOpenNewFileInZip4(zFile, fileName.c_str(), &zFileInfo, NULL, 0, NULL, 0, NULL, Z_DEFLATED, level, 0, -MAX_WBITS, DEF_MEM_LEVEL,
            Z_DEFAULT_STRATEGY, NULL, 0, 0, 0);// ZIP_GPBF_LANGUAGE_ENCODING_FLAG);
        if (ret != ZIP_OK)
        {
            std::cout << "openfile in zip failed" << std::endl;
            zipClose(zFile, NULL);
            return -1;
        }

        ret = WriteInZipFile(zFile, src);
        if (ret != ZIP_OK)
        {
            std::cout << "write in zip failed" << std::endl;
            zipClose(zFile, NULL);
            return -1;
        }
        zipClose(zFile, NULL);
        std::cout << "Minizip file ok" << std::endl;
    }

    return 0;
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

        for (auto path : output)
        {
            std::string outName = path.substr(path.find_last_of("\\") + 1);
            std::string outPath = destDir + outName;
            Minizip(path, outPath);
        }
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
