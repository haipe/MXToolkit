#pragma once

#include "fcntl.h"
#include "io.h"

#include "mxkit.h"
/*一个应用只要包含一次*/

_BEGIN_MX_KIT_NAME_SPACE_

class ConsoleWindow
{
    ConsoleWindow() {}
public:

    static ConsoleWindow* Instance()
    {
        static ConsoleWindow cw;

        return &cw;
    }

    void InitConsoleWindow()
    {
        AllocConsole();

        console_view_fp = freopen("CONOUT$", "w+t", stdout);
        //mx_console_view_fp = freopen("CONERR$", "w+t", stderr);

    //         int nCrt = _open_osfhandle((long)GetStdHandle(STD_ERROR_HANDLE), _O_TEXT);
    //         FILE* fp = _fdopen(nCrt, "w");
    //         *stderr = *fp;
    //         setvbuf(stderr, NULL, _IONBF, 0);
    }

    void DestroyConsoleWindow()
    {
        if (console_view_fp)
        {
            fclose(console_view_fp);
        }

        console_view_fp = nullptr;
    }

private:
    FILE* console_view_fp = nullptr;
};

_END_MX_KIT_NAME_SPACE_