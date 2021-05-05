#pragma once

#include <string>
#include <windows.h>

typedef SOCKET socket_def;

class websocket_utils
{
public:
    websocket_utils();
    ~websocket_utils();

    static void socket_send(socket_def sc, const char* data, unsigned int len);
    static void socket_send(socket_def sc, const std::string& data);
    static void get_key(const std::string& clientkey, std::string& out);
    static bool check_connect(socket_def client);
    static int websocket_recv(socket_def client, char* buff, int buff_size);
    static int websocket_recv(socket_def client, char** buffer);
    static void websocket_send(socket_def client, const char* buff, int length, boolean finalFragment);
    static void release_buffer(char* buffer);
};

 