#pragma once
#include <string>
#include <functional>
#include <windows.h>

#include "websocket_utils.h"


class websocket_server
{
public:
    websocket_server();
    ~websocket_server();
    
    bool is_running();
    void kill();

    int start_listen(unsigned int port, std::function<void(socket_def)> on_new_client, std::function<void()> on_server_stop);

protected:
    socket_def m_server_socket = 0;
    std::function<void(socket_def)>     m_on_new_client;
};

 