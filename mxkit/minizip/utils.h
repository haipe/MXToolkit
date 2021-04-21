#pragma once

#include <string>
#include <vector>

#include "zconf.h"
#include "contrib/minizip/zip.h"
#include "contrib/minizip/unzip.h"

#include "mxkit.h"

_BEGIN_MX_KIT_NAME_SPACE_


class MiniZipUtils
{
public:
    static bool Unzip(const char* zipfilename, const char* dirname, const char* password = "");

    static void ChangeFileDate(const char* filename, uLong dosdate, tm_unz tmu_date);
    static int MakeDir(char* newdir);
    static int ExtractCurrentFile(unzFile uf, const char* password);
    static int DoExtract(unzFile uf, const char* password);

public:
    static int Zip(std::string src, std::string dest);

    static void EnumDirFiles(const std::string& dirPrefix, const std::string& dirName, std::vector<std::string>& vFiles);
    static int WriteInZipFile(zipFile zFile, const std::string& file);
};

_END_MX_KIT_NAME_SPACE_