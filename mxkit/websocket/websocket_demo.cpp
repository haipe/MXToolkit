// WSServer.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <iostream>
#include <string>
#include <windows.h>

#include "websocket_server.h"

using namespace std;


int _tmain(int argc, _TCHAR* argv[])
{
    websocket_server s;
    s.start_listen(8899,nullptr);

    CHAR path[MAX_PATH];
    GetModuleFileNameA(NULL, path, MAX_PATH);

    cout << path << endl;
    int c;
    cin >> c;
    if (c == 1)
        ShellExecuteA(NULL, "runas", path, NULL, NULL, SW_SHOW);

    return 0;
}

 