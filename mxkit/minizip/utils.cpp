#include "utils.h"
#include <iostream>
#include <fstream>

#include <direct.h>
#include <io.h>
#include <windows.h>

#ifdef _WIN32
#define USEWIN32IOAPI
#include "contrib/minizip/iowin32.h"
#endif

#pragma  comment (lib,"zlibwapi.lib")

_BEGIN_MX_KIT_NAME_SPACE_

enum 
{
    _MX_MAX_WRITE_BUFFER_SIZE_ = 8192,
}; 

bool MiniZipUtils::Unzip(const char* zipFileName, const char* unzipDir, const char* password)
{
    char filename_try[mxkit::_MAX_FILE_NAME + 16] = "";
    int ret_value = 0;
    unzFile uf = nullptr;

    zlib_filefunc64_def ffunc;

    strncpy(filename_try, zipFileName, mxkit::_MAX_FILE_NAME - 1);
    filename_try[mxkit::_MAX_FILE_NAME] = '\0';

    fill_win32_filefunc64A(&ffunc);
    uf = unzOpen2_64(zipFileName, &ffunc);

    if (uf == nullptr)
    {
        strcat(filename_try, ".zip");
        uf = unzOpen2_64(filename_try, &ffunc);
    }


    if (uf == nullptr)
    {
        printf("Cannot open %s.\n", zipFileName);
        return false;
    }

    printf("%s opened\n", filename_try);

    MakeDir(const_cast<char*>(unzipDir));
    if (_chdir(unzipDir))
    {
        printf("Error changing into %s, aborting\n", unzipDir);
        return false;
    }

    ret_value = DoExtract(uf, password);

    unzClose(uf);
    return ret_value == 0;
}

void MiniZipUtils::ChangeFileDate(const char* filename, uLong dosdate, tm_unz tmu_date)
{
    HANDLE hFile;
    FILETIME ftm, ftLocal, ftCreate, ftLastAcc, ftLastWrite;

    hFile = CreateFileA(filename, GENERIC_READ | GENERIC_WRITE, 0, NULL, OPEN_EXISTING, 0, NULL);
    GetFileTime(hFile, &ftCreate, &ftLastAcc, &ftLastWrite);
    DosDateTimeToFileTime((WORD)(dosdate >> 16), (WORD)dosdate, &ftLocal);
    LocalFileTimeToFileTime(&ftLocal, &ftm);
    SetFileTime(hFile, &ftm, &ftLastAcc, &ftm);
    CloseHandle(hFile);
}

int MiniZipUtils::MakeDir(char* newdir)
{
    char* buffer = nullptr;
    char* p = nullptr;
    int  len = (int)strlen(newdir);

    if (len <= 0) return 0;

    buffer = (char*)malloc(len + 1);
    if (buffer == nullptr)
    {
        printf("Error allocating memory\n");
        return UNZ_INTERNALERROR;
    }
    strcpy(buffer, newdir);

    if (buffer[len - 1] == '/')
    {
        buffer[len - 1] = '\0';
    }

    if (_mkdir(buffer) == 0)
    {
        free(buffer);
        return 1;
    }

    p = buffer + 1;
    while (true)
    {
        char hold;

        while (*p && *p != '\\' && *p != '/')
            p++;
        hold = *p;
        *p = 0;
        if ((_mkdir(buffer) == -1) && (errno == ENOENT))
        {
            printf("couldn't create directory %s\n", buffer);
            free(buffer);
            return 0;
        }
        if (hold == 0)
            break;
        *p++ = hold;
    }

    free(buffer);
    return 1;
}

int MiniZipUtils::ExtractCurrentFile(unzFile uf, const char* password)
{
    char filename_inzip[256] = { 0 };
    char* filename_withoutpath = nullptr;
    char* p = nullptr;
    int err = UNZ_OK;
    FILE* fout = nullptr;
    void* buf;
    uInt size_buf;

    unz_file_info64 file_info;
    uLong ratio = 0;
    err = unzGetCurrentFileInfo64(uf, &file_info, filename_inzip, sizeof(filename_inzip), nullptr, 0, nullptr, 0);

    if (err != UNZ_OK)
    {
        printf("error %d with zipfile in unzGetCurrentFileInfo\n", err);
        return err;
    }

    size_buf = _MX_MAX_WRITE_BUFFER_SIZE_;
    buf = (void*)malloc(size_buf);
    if (buf == nullptr)
    {
        printf("Error allocating memory\n");
        return UNZ_INTERNALERROR;
    }

    p = filename_withoutpath = filename_inzip;
    while ((*p) != '\0')
    {
        if (((*p) == '/') || ((*p) == '\\'))
            filename_withoutpath = p + 1;
        p++;
    }

    if ((*filename_withoutpath) == '\0')
    {
        printf("creating directory: %s\n", filename_inzip);
        _mkdir(filename_inzip);
    }
    else
    {
        const char* write_filename = filename_inzip;
        int skip = 0;

        if (!password || strlen(password) <= 0)
            err = unzOpenCurrentFile(uf);
        else
            err = unzOpenCurrentFilePassword(uf, password);

        if (err != UNZ_OK)
        {
            printf("error %d with zipfile in unzOpenCurrentFilePassword\n", err);
        }

        if ((skip == 0) && (err == UNZ_OK))
        {
            fout = fopen64(write_filename, "wb");

            /* some zipfile don't contain directory alone before file */
            if ((fout == nullptr) && (filename_withoutpath != (char*)filename_inzip))
            {
                char c = *(filename_withoutpath - 1);
                *(filename_withoutpath - 1) = '\0';
                MakeDir(const_cast<char*>(write_filename));
                *(filename_withoutpath - 1) = c;
                fout = fopen64(write_filename, "wb");
            }

            if (fout == nullptr)
            {
                printf("error opening %s\n", write_filename);
            }
        }

        if (fout != nullptr)
        {
            printf("extracting: %s\n", write_filename);

            do
            {
                err = unzReadCurrentFile(uf, buf, size_buf);
                if (err < 0)
                {
                    printf("error %d with zipfile in unzReadCurrentFile\n", err);
                    break;
                }
                if (err > 0)
                    if (fwrite(buf, err, 1, fout) != 1)
                    {
                        printf("error in writing extracted file\n");
                        err = UNZ_ERRNO;
                        break;
                    }
            } while (err > 0);

            if (fout)
                fclose(fout);

            if (err == 0)
                ChangeFileDate(write_filename, file_info.dosDate, file_info.tmu_date);
        }

        if (err == UNZ_OK)
        {
            err = unzCloseCurrentFile(uf);
            if (err != UNZ_OK)
            {
                printf("error %d with zipfile in unzCloseCurrentFile\n", err);
            }
        }
        else
        {
            unzCloseCurrentFile(uf); /* don't lose the error */
        }
    }

    free(buf);
    return err;
}

int MiniZipUtils::DoExtract(unzFile uf, const char* password)
{
    unz_global_info64 gi;
    int err;

    err = unzGetGlobalInfo64(uf, &gi);
    if (err != UNZ_OK)
    {
        printf("error %d with zipfile in unzGetGlobalInfo \n", err);
        return -1;
    }

    for (uLong i = 0; i < gi.number_entry; i++)
    {
        if (ExtractCurrentFile(uf, password) != UNZ_OK)
        {
            break;
        }

        if ((i + 1) < gi.number_entry)
        {
            err = unzGoToNextFile(uf);
            if (err != UNZ_OK)
            {
                printf("error %d with zipfile in unzGoToNextFile\n", err);
                break;
            }
        }
    }

    return 0;
}

//minizip 压缩
int MiniZipUtils::Zip(std::string src, std::string dest)
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

void MiniZipUtils::EnumDirFiles(const std::string& dirPrefix, const std::string& dirName, std::vector<std::string>& vFiles)
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
int MiniZipUtils::WriteInZipFile(zipFile zFile, const std::string& file)
{
    std::fstream f(file.c_str(), std::ios::binary | std::ios::in);
    f.seekg(0, std::ios::end);
    std::streamoff size = f.tellg();
    f.seekg(0, std::ios::beg);
    if (size <= 0)
    {
        return zipWriteInFileInZip(zFile, NULL, 0);
    }

    char* buf = new char[static_cast<unsigned int>(size)];
    f.read(buf, size);
    int ret = zipWriteInFileInZip(zFile, buf, static_cast<unsigned int>(size));
    delete[] buf;
    return ret;
}

_END_MX_KIT_NAME_SPACE_