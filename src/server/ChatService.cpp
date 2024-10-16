#include "ChatService.h"
#include "public.h"
#include <muduo/base/Logging.h>



ChatService *ChatService::instance()
{
    static ChatService chatservice;
    return &chatservice;
}

MsgHandler ChatService::getHandler(int msgid)
{
    auto it=_msgHandlerMap.find(msgid);
    if (it == _msgHandlerMap.end())
    {
        // 返回一个默认的处理器，空操作
        return [=](const TcpConnectionPtr &conn, json &js, Timestamp) {
            LOG_ERROR << "msgid:" << msgid << " can not find handler!";
        };
    }else{
        return _msgHandlerMap[msgid];
    }

}

ChatService::ChatService()
{
    _msgHandlerMap.insert({LOGIN_MSG, std::bind(&ChatService::login, this, _1, _2, _3)});
    _msgHandlerMap.insert({LOGINOUT_MSG, std::bind(&ChatService::loginout, this, _1, _2, _3)});
    _msgHandlerMap.insert({REG_MSG, std::bind(&ChatService::reg, this, _1, _2, _3)});
    _msgHandlerMap.insert({ONE_CHAT_MSG, std::bind(&ChatService::oneChat, this, _1, _2, _3)});
    _msgHandlerMap.insert({ADD_FRIEND_MSG, std::bind(&ChatService::addFriend, this, _1, _2, _3)});

    // 群组业务管理相关事件处理回调注册
    _msgHandlerMap.insert({CREATE_GROUP_MSG, std::bind(&ChatService::createGroup, this, _1, _2, _3)});
    _msgHandlerMap.insert({ADD_GROUP_MSG, std::bind(&ChatService::addGroup, this, _1, _2, _3)});
    _msgHandlerMap.insert({GROUP_CHAT_MSG, std::bind(&ChatService::groupChat, this, _1, _2, _3)});

}
    // 处理登录业务
void ChatService::login(const TcpConnectionPtr &conn, json &js, Timestamp time){


}
// 处理注册业务
void ChatService::reg(const TcpConnectionPtr &conn, json &js, Timestamp time){


}
// 一对一聊天业务
void ChatService::oneChat(const TcpConnectionPtr &conn, json &js, Timestamp time){


}
// 添加好友业务
void ChatService::addFriend(const TcpConnectionPtr &conn, json &js, Timestamp time){

}
// 创建群组业务
void ChatService::createGroup(const TcpConnectionPtr &conn, json &js, Timestamp time){

}
// 加入群组业务
void ChatService::addGroup(const TcpConnectionPtr &conn, json &js, Timestamp time){

}
// 群组聊天业务
void ChatService::groupChat(const TcpConnectionPtr &conn, json &js, Timestamp time){

}
// 处理注销业务
void ChatService::loginout(const TcpConnectionPtr &conn, json &js, Timestamp time){


}
// 处理客户端异常退出
void ChatService::clientCloseException(const TcpConnectionPtr &conn){


}
// 服务器异常，业务重置方法
void ChatService::reset(){



}