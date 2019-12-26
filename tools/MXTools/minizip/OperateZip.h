#pragma once

#include "zip.h"
#include <string>
#include <vector>

class OperateZip
{
public:
    OperateZip();
    ~OperateZip();

    static int Zip(std::string src, std::string dest);

private:
    static void EnumDirFiles(const std::string& dirPrefix, const std::string& dirName, std::vector<std::string>& vFiles);
    static int WriteInZipFile(zipFile zFile, const std::string& file);
};

