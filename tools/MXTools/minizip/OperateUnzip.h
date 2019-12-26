#pragma once

#include "zip.h"
#include "unzip.h"

class OperateUnzip
{
public:
    OperateUnzip();
    ~OperateUnzip();

public:
    static bool Unzip(const char* zipfilename, const char* dirname, const char* password = "");

private:
    static void ChangeFileDate(const char *filename, uLong dosdate, tm_unz tmu_date);
    static int MakeDir(char *newdir);
    static int ExtractCurrentFile(unzFile uf, const char* password);
    static int DoExtract(unzFile uf, const char* password);
};

