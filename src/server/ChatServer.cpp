#include "ChatServer.h"
#include "json.hpp"
#include <iostream>
#include "ChatService.h"
using namespace std;
using namespace placeholders;
using json = nlohmann::json;
ChatServer::ChatServer(EventLoop *loop, const InetAddress &listenAddr, const string &nameArg)
            :_server(loop,listenAddr,nameArg)
{
    // 注册链接回调
    _server.setConnectionCallback(std::bind(&ChatServer::onConnection, this, _1));

    // 注册消息回调
    _server.setMessageCallback(std::bind(&ChatServer::onMessage, this, _1, _2, _3));
    _server.setThreadNum(4);
}

void ChatServer::start()
{
    _server.start();
}

void ChatServer::onConnection(const TcpConnectionPtr &conn)
{
    if(!conn->connected()){
        conn->connectDestroyed();
    }
}
//服务器read了message
void ChatServer::onMessage(const TcpConnectionPtr &conn, Buffer *buffer, Timestamp time)
{
    string buf = buffer->retrieveAllAsString();
    json js = json::parse(buf);//反序列化 转为json对象
    // conn->send(buf);
    // std::cout<<"消息发送成功"<<js["msgid"]<<std::endl;
    // auto msgHandler = ChatService::instance()->getHandler(js["msgid"].get<int>());
    // msgHandler(conn, js, time);

}
