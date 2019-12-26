#include "OperateZip.h"
#include <iostream>
#include <fstream>
#include <windows.h>

OperateZip::OperateZip()
{
}


OperateZip::~OperateZip()
{
}

//minizip 压缩
int OperateZip::Zip(std::string src, std::string dest)
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

void OperateZip::EnumDirFiles(const std::string& dirPrefix, const std::string& dirName, std::vector<std::string>& vFiles)
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


    CHAR filePathName[MAX_PATH] = {0};
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
            strcmp(findData.cFileName, "..") == 0)
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
int OperateZip::WriteInZipFile(zipFile zFile, const std::string& file)
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
