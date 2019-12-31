#pragma once

#include "fcntl.h"
#include "io.h"


/*一个应用只要包含一次*/

namespace mxtoolkit
{
    FILE* mx_console_view_fp = nullptr;

    void InitConsoleWindow()
    {
        int nCrt = 0;
        AllocConsole();

        mx_console_view_fp = freopen("CONOUT$", "w", stdout);

        //nCrt = _open_osfhandle((long)GetStdHandle(STD_OUTPUT_HANDLE), _O_TEXT);
        //fp = _fdopen(nCrt, "w");
        //*stdout = *fp;
        //setvbuf(stdout, NULL, _IONBF, 0);
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