#include "websocket_server.h"
#include <iostream>
#include <future>

#include <windows.h>

#pragma comment(lib, "ws2_32.lib")

#include "sha1.h"
#include "base64.h"

#include "base/string_convert.h"

//using namespace  std;

websocket_server::websocket_server()
{
}


websocket_server::~websocket_server()
{
}

bool websocket_server::is_running()
{
    return m_server_socket != 0;
}

void websocket_server::kill()
{
    closesocket(m_server_socket);
    m_server_socket = 0;
}

int websocket_server::start_listen(unsigned int port, std::function<void(socket_def)> on_new_client, std::function<void()> on_server_stop)
{
    std::async(std::launch::async, [this, port, on_new_client, on_server_stop]() -> int
    {
        WORD imgrequest;
        WSADATA wsadata;
        imgrequest = MAKEWORD(1, 1);
        int err;
        err = WSAStartup(imgrequest, &wsadata);
        if (!err)
        {
            printf("服务已经启动\n");
        }
        else
        {
            printf("服务未启动\n");
            return -1;
        }

        m_on_new_client = on_new_client;
        m_server_socket = socket(AF_INET, SOCK_STREAM, 0);

        SOCKADDR_IN addr;
        addr.sin_family = AF_INET;
        addr.sin_addr.S_un.S_addr = htonl(INADDR_ANY);      //ip地址
        addr.sin_port = htons(port);                        //绑定端口
        bind(m_server_socket, (SOCKADDR*)&addr, sizeof(SOCKADDR));//绑定完成
        listen(m_server_socket, 10);                              //其中第二个参数代表能够接收的最多的连接数

        SOCKADDR_IN clientsocket;
        int len = sizeof(SOCKADDR);
        boolean isConnected = false;

        int i = 0;
        while (true)
        {
            socket_def serConn = accept(m_server_socket, (SOCKADDR*)&clientsocket, &len);

            if (m_server_socket == 0)
                break;

            printf("client connect %d\n", serConn);
            m_on_new_client(serConn);
        }

        closesocket(m_server_socket);
        m_server_socket = 0;
        
        if (on_server_stop)
            on_server_stop();

        return 0;
    });

    return 0;
}
