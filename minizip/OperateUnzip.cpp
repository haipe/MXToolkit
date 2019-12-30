#include "OperateUnzip.h"
#include <fstream>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <errno.h>
#include <fcntl.h>
#include <direct.h>
#include <io.h>
#include <Windows.h>
#define CASESENSITIVITY (0)
#define WRITEBUFFERSIZE (8192)
#define MAXFILENAME (256)
#ifdef _WIN32
#define USEWIN32IOAPI
#include "iowin32.h"
#endif


OperateUnzip::OperateUnzip()
{
}


OperateUnzip::~OperateUnzip()
{
}

bool OperateUnzip::Unzip(const char* zipFileName ,const char* unzipDir ,const char* password)
{
    char filename_try[MAXFILENAME + 16] = "";
    int ret_value = 0;
    unzFile uf = nullptr;

    zlib_filefunc64_def ffunc;

    strncpy(filename_try, zipFileName, MAXFILENAME - 1);
    filename_try[MAXFILENAME] = '\0';

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

void OperateUnzip::ChangeFileDate(const char *filename, uLong dosdate, tm_unz tmu_date)
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

int OperateUnzip::MakeDir(char *newdir) 
{
    char *buffer = nullptr;
    char *p = nullptr;
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

int OperateUnzip::ExtractCurrentFile(unzFile uf, const char* password)
{
    char filename_inzip[256] = { 0 };
    char* filename_withoutpath = nullptr;
    char* p = nullptr;
    int err = UNZ_OK;
    FILE *fout = nullptr;
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

    size_buf = WRITEBUFFERSIZE;
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

        if(!password || strlen(password) <= 0)
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

int OperateUnzip::DoExtract(unzFile uf, const char* password)
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
