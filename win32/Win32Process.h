#pragma once
#include <string>

namespace mxtoolkit
{



    int CreateProcess(const std::string& filePath, const std::string& runParam = "", unsigned int waitTimeout = -1);



}