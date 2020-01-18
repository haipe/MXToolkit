#pragma once

#include "fcntl.h"
#include "io.h"


/*一个应用只要包含一次*/

namespace mxtoolkit
{
    FILE* mx_console_view_fp = nullptr;

    void InitConsoleWindow()
    {
        AllocConsole();

        mx_console_view_fp = freopen("CONOUT$", "w+t", stdout);
        //mx_console_view_fp = freopen("CONERR$", "w+t", stderr);

//         int nCrt = _open_osfhandle((long)GetStdHandle(STD_ERROR_HANDLE), _O_TEXT);
//         FILE* fp = _fdopen(nCrt, "w");
//         *stderr = *fp;
//         setvbuf(stderr, NULL, _IONBF, 0);
    }

    void DestroyConsoleWindow()
    {
        if (mx_console_view_fp)
        {
            fclose(mx_console_view_fp);
        }

        mx_console_view_fp = nullptr;
    }

}