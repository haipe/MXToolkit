#include "stdafx.h"
#include "websocket_utils.h"
#include <vector>

#define MAX_SOCKET_BUFF_SIZE 1024*8

#include "sha1.h"
#include "base64.h"

#include "easyloggingpp/utils.h"

websocket_utils::websocket_utils()
{
}


websocket_utils::~websocket_utils()
{
}

/*发送*/
void websocket_utils::socket_send(socket_def sc, const char* data, unsigned int len)
{
    if (!data)
        return;

    send(sc, data, len, 0);
}

void websocket_utils::socket_send(socket_def sc, const std::string& data)
{
    if (data.empty())
        return;

    socket_send(sc, data.c_str(), data.length());
}

/*协议
这个过程就是拿到客户端的key然后经过sha加密，再拼接返回的协议发给客户端
*/
void websocket_utils::get_key(const std::string& clientkey, std::string& out)
{
    std::string server_key = clientkey;
    server_key += "258EAFA5-E914-47DA-95CA-C5AB0DC85B11";

    SHA1  sha;
    unsigned int message_digest[5];
    LOG_UTILS(INFO , "server_key:" << server_key );
    sha.Reset();
    sha << server_key.c_str();
    sha.Result(message_digest);
    for (int i = 0; i < 5; i++)
        message_digest[i] = htonl(message_digest[i]);

    server_key = base64_encode(reinterpret_cast<const unsigned char*>(message_digest), 20);
    server_key += "\r\n";

    out += "HTTP/1.1 101 Switching Protocols\r\n";
    out += "Connection: upgrade\r\n";
    out += "Sec-WebSocket-Accept: ";
    out += server_key;
    out += "Upgrade: websocket\r\n\r\n";
    LOG_UTILS(INFO , "shal server_key:" << server_key );
}

/*获取key和协议*/
bool websocket_utils::check_connect(socket_def client)
{
    std::vector<char> buffer(MAX_SOCKET_BUFF_SIZE, 0);
    char* recvBuffer = &buffer[0];
    int len = recv(client, recvBuffer, MAX_SOCKET_BUFF_SIZE, 0);
    if (len < 0)
        return false;

    std::string s = recvBuffer;
    static std::string c = "Sec-WebSocket-Key";
    int i = s.find(c);
    if (i != std::string::npos)
    {
        s = s.substr(i + 19, 24);//i+19
        //以上是为了得到客户端请求信息的key，关于key的作用可以去了解握手协议
        //以下是服务器拼接协议返回给客户端
        std::string data;
        get_key(s, data);
        socket_send(client, data);
        return true;
    }

    return false;
}

int websocket_utils::websocket_recv(socket_def client, char* buff, int buff_size)
{
    if (buff_size < MAX_SOCKET_BUFF_SIZE)
        return -3;

    int point = 0;            //字节指针位置
    int tmppoint = 0;         //临时指针变量

    std::vector<char> buffer(MAX_SOCKET_BUFF_SIZE, 0);
    char* recv_buff = &buffer[0];
    int len = recv(client, recv_buff, MAX_SOCKET_BUFF_SIZE, 0);
    if (len < 0)
    {
        return -1;
    }

    std::string ss = recv_buff;
    //LOG_UTILS(INFO , "len：" << len << " ," << ss LOG_ENDL;

    /*这里b字节数组是客户端的请求信息，需要注意point这个指针的变化，具体需要去理解它的协议，协议中每段字节里面包含了什么信息需要把    它解析出来*/
    byte* recv_byte_buff = (byte*)recv_buff;
    //     LOG_UTILS(INFO , "字节数据：" << b LOG_ENDL;
    //     for (int i = 0; i <= 33; i++)
    //         printf("%d\t", b[i]);            
    //             
    //     printf("\n");

    //取第一个字节
    int first = recv_byte_buff[point] & 0xFF;
    //printf("第一个：%d,%d,%d\n", point, b[point], first);
    byte opCode = (byte)(first & 0x0F);             //0000 1111 后四位为opCode 00001111
    if (opCode == 8)
    {
        closesocket(client);
        return -2;
    }

    //取第二个字节
    first = recv_byte_buff[++point];
    //负载长度
    int payloadLength = first & 0x7F;
    //printf("第二个：%d,[%d],%d\n", point, b[point], payloadLength);
    if (payloadLength == 126)
    {
        byte extended[2] = "";
        extended[0] = recv_byte_buff[++point];
        extended[1] = recv_byte_buff[++point];
        int shift = 0;
        payloadLength = 0;
        for (int i = 2 - 1; i >= 0; i--) {
            payloadLength = payloadLength + ((extended[i] & 0xFF) << shift);
            shift += 8;
        }
    }
    else if (payloadLength == 127)
    {
        byte extended[8] = "";
        tmppoint = ++point;     //保存临时指针
        point = --point;
        for (int i = 0; i < 8; i++){
            extended[i] = recv_byte_buff[tmppoint + i];
            point++;
        }
        int shift = 0;
        payloadLength = 0;
        for (int i = 8 - 1; i >= 0; i--)
        {
            payloadLength = payloadLength + ((extended[i] & 0xFF) << shift);
            shift += 8;
        }
    }

    //非126和127置回来
    if ((payloadLength != 126) || (payloadLength != 127)){
        point = 1;
    }

    //LOG_UTILS(INFO , "负载长度:" << payloadLength );
    //第三个字节，掩码
    byte mask[4] = "";
    tmppoint = ++point;
    //因为自增了一次，这里需要减掉
    point = --point;
    //取掩码值
    for (int i = 0; i < 4; i++){
        mask[i] = recv_byte_buff[tmppoint + i];
        point++;
        //printf("第三mask个：%d,[%d],%d\t\n", point, mask[i], payloadLength);
    }


    //内容的长度保留，循环里面已经被改变
    int length = payloadLength;
    int readThisFragment = 1;

    memset(buff, 0, buff_size);

    int count = 0;
    //通过掩码计算真实的数据
    while (payloadLength > 0)
    {
        count++;
        int maskbyte = recv_byte_buff[++point];
        int index = (readThisFragment - 1) % 4;
        maskbyte = maskbyte ^ (mask[index] & 0xFF);
        buff[readThisFragment - 1] = (byte)maskbyte;
        //printf("real data: %d,[%d],%d\n", point, maskbyte, readThisFragment);
        payloadLength--;
        readThisFragment++;
    }

    return count;
}

int websocket_utils::websocket_recv(socket_def client, char** b)
{
    char* buff = new char[MAX_SOCKET_BUFF_SIZE];
    int len = websocket_recv(client, (char*)buff, MAX_SOCKET_BUFF_SIZE);
    if (len > 0)
        *b = buff;
    else
        delete[] buff;

    return len;
}

/*给客户端发送数据也需要进行加密处理，就是保持通讯协议*/
void websocket_utils::websocket_send(socket_def client, const char* buff, int length, boolean finalFragment)
{
    char* buf = new char[length+128];// &buffer[0];

    int first = 0x00;
    int data_len = 0;
    if (finalFragment)
    {
        first = first + 0x80;
        first = first + 0x1;
    }

    buf[0] = first;
    data_len = 1;
    //LOG_UTILS(INFO , "数组长度:" << length );
    unsigned int nuNum = (unsigned)length;
    if (length < 126)
    {
        buf[1] = length;
        data_len = 2;
    }
    else if (length < 65536)
    {
        buf[1] = 126;
        buf[2] = nuNum >> 8;
        buf[3] = length & 0xFF;
        data_len = 4;
    }
    else
    {
        //数据长度超过65536
        buf[1] = 127;
        buf[2] = 0;
        buf[3] = 0;
        buf[4] = 0;
        buf[5] = 0;
        buf[6] = nuNum >> 24;
        buf[7] = nuNum >> 16;
        buf[8] = nuNum >> 8;
        buf[9] = nuNum & 0xFF;
        data_len = 10;
    }

    for (int i = 0; i < length; i++)
    {
        buf[data_len + i] = buff[i];
        //printf("要发送的数据字节：%d\n", charb[i]);
    }

    socket_send(client, (const char*)buf, length + data_len);
    delete[] buf;
}

void websocket_utils::release_buffer(char* buffer)
{
    if (!buffer)
        return;

    delete[] buffer;
}
