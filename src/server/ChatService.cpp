#include "ChatService.h"
#include "public.h"
#include <muduo/base/Logging.h>
#include <iostream>


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

void ChatService::handleRedisSubscribeMessage(int id, string message)//msg是一串json字符 包含msgid id messgage等等 客户端接收到后进行处理
{
    lock_guard<mutex> lock(_connMutex);
    auto it = _userConnMap.find(id);
    if (it != _userConnMap.end())
    {
        it->second->send(message);
        return;
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
    if (_redis.connect())
    {
        // 设置上报消息的回调
        _redis.init_notify_handler(std::bind(&ChatService::handleRedisSubscribeMessage, this, _1, _2));
    }

}
    // 处理登录业务
void ChatService::login(const TcpConnectionPtr &conn, json &js, Timestamp time){
    int id = js["id"].get<int>();
    string pwd = js["password"];//获取登录的id和密码
    User user = _userModel.query(id);
    if (user.getId() == id && user.getPwd() == pwd){
        if (user.getState() == "online")
        {
            // 该用户已经登录，不允许重复登录
            json response;
            response["msgid"] = LOGIN_MSG_ACK;
            response["errno"] = 2;
            response["errmsg"] = "this account is using, input another!";
            conn->send(response.dump());
        }else{
            // 登录成功，记录用户连接信息
            {
                lock_guard<mutex> lock(_connMutex);
                _userConnMap.insert({id, conn});
            }
            _redis.subscribe(id); //订阅这个通道消息
            user.setState("online");
            _userModel.updateState(user);
            json response;
            response["msgid"] = LOGIN_MSG_ACK;
            response["errno"] = 0;
            response["id"] = user.getId();
            response["name"] = user.getName();
            vector<string> vec = _offlineMsgModel.query(id);//查一下是否有离线的消息   //这个string写了谁发过来的消息
            if (!vec.empty())
            {
                response["offlinemsg"] = vec;
                // 读取该用户的离线消息后，把该用户的所有离线消息删除掉
                _offlineMsgModel.remove(id);
            }
            vector<User> userVec = _friendModel.query(id);
            if (!userVec.empty())
            {
                vector<string> vec2;
                for (User &user : userVec)
                {
                    json js;
                    js["id"] = user.getId();
                    js["name"] = user.getName();
                    js["state"] = user.getState();
                    vec2.push_back(js.dump());
                }
                response["friends"] = vec2; //每次登录都要返回有哪些好友 然后根据返回的信息做回调
            }
            vector<Group> groupuserVec = _groupModel.queryGroups(id);
            if (!groupuserVec.empty())
            {
                // group:[{groupid:[id, name, des, xxx]}]
                vector<string> groupV;
                for (Group &group : groupuserVec)
                {
                    json grpjson;
                    grpjson["id"] = group.getId();
                    grpjson["groupname"] = group.getName();
                    grpjson["groupdesc"] = group.getDesc();
                    vector<string> userV;
                    for (GroupUser &user : group.getUsers())
                    {
                        json js;
                        js["id"] = user.getId();
                        js["name"] = user.getName();
                        js["state"] = user.getState();
                        js["role"] = user.getRole();
                        userV.push_back(js.dump());
                    }
                    grpjson["users"] = userV;
                    groupV.push_back(grpjson.dump());
                }

                response["groups"] = groupV;
            }
            conn->send(response.dump());
        }
    }else{
        // 该用户不存在，用户存在但是密码错误，登录失败
        json response;
        response["msgid"] = LOGIN_MSG_ACK;
        response["errno"] = 1;
        response["errmsg"] = "(id or password is invalid!) or the user doesn't exist";
        conn->send(response.dump());
    }

}
// 处理注册业务
void ChatService::reg(const TcpConnectionPtr &conn, json &js, Timestamp time){
    string name = js["name"];
    string pwd = js["password"];

    User user;
    user.setName(name);
    user.setPwd(pwd);
    std::cout<<"开始注册"<<std::endl;
    bool state = _userModel.insert(user);
    if (state)
    {
        // 注册成功
        std::cout<<"注册成功"<<std::endl;
        json response;
        response["msgid"] = REG_MSG_ACK;
        response["errno"] = 0;
        response["id"] = user.getId();
        conn->send(response.dump());//返回到客户端成功的信息
    }
    else
    {
        // 注册失败
        json response;
        response["msgid"] = REG_MSG_ACK;//客户端可以根据这个msgid做出相应的回调
        response["errno"] = 1;
        conn->send(response.dump());//返回到客户端成功的信息
    }

}
// 一对一聊天业务
void ChatService::oneChat(const TcpConnectionPtr &conn, json &js, Timestamp time){
    int toid = js["toid"].get<int>();//对谁说

    {
        lock_guard<mutex> lock(_connMutex);
        auto it = _userConnMap.find(toid);//如果这个服务器能够连接倒 就直接进行转发
        if (it != _userConnMap.end())
        {
            // toid在线，转发消息   服务器主动推送消息给toid用户
            it->second->send(js.dump());
            return;
        }
    }

    // 查询toid是否在线 
    User user = _userModel.query(toid);
    if (user.getState() == "online")
    {
        _redis.publish(toid, js.dump());//在其他服务器  
        return;
    }
    // toid不在线，存储离线消息   离线的话就用数据库进行信息操作 因为连的是同一个数据库
    _offlineMsgModel.insert(toid, js.dump());
}
// 添加好友业务
void ChatService::addFriend(const TcpConnectionPtr &conn, json &js, Timestamp time){
    int userid = js["id"].get<int>();
    int friendid = js["friendid"].get<int>();
    // 存储好友信息
    _friendModel.insert(userid, friendid);
}
// 创建群组业务
void ChatService::createGroup(const TcpConnectionPtr &conn, json &js, Timestamp time){
    int userid = js["id"].get<int>(); //自己的用户id
    string name = js["groupname"];
    string desc = js["groupdesc"];

    // 存储新创建的群组信息
    Group group(-1, name, desc);
    if (_groupModel.createGroup(group))
    {
        // 存储群组创建人信息
        _groupModel.addGroup(userid, group.getId(), "creator");
    }
}
// 加入群组业务
void ChatService::addGroup(const TcpConnectionPtr &conn, json &js, Timestamp time){
    int userid = js["id"].get<int>();//谁要加入
    int groupid = js["groupid"].get<int>();//加入哪一个群组
    _groupModel.addGroup(userid, groupid, "normal");
}

// 群组聊天业务
void ChatService::groupChat(const TcpConnectionPtr &conn, json &js, Timestamp time){  //msg包含在json里面
    int userid = js["id"].get<int>();
    int groupid = js["groupid"].get<int>();
    //先查询这个群组里的其他成员
    vector<int> useridVec = _groupModel.queryGroupUsers(userid, groupid);

    lock_guard<mutex> lock(_connMutex);
    for (int id : useridVec)
    {
        auto it = _userConnMap.find(id);
        if (it != _userConnMap.end())
        {
            // 转发群消息
            it->second->send(js.dump());
        }
        else
        {
            // 查询toid是否在线 
            User user = _userModel.query(id);
            if (user.getState() == "online")
            {
                _redis.publish(id, js.dump());
            }
            else
            {
                // 存储离线群消息
                _offlineMsgModel.insert(id, js.dump());
            }
        }
    }

}
// 处理注销业务
void ChatService::loginout(const TcpConnectionPtr &conn, json &js, Timestamp time){
    int userid = js["id"].get<int>();

    {
        lock_guard<mutex> lock(_connMutex);
        auto it = _userConnMap.find(userid);
        if (it != _userConnMap.end())
        {
            _userConnMap.erase(it);
        }
    }

    // 用户注销，相当于就是下线，在redis中取消订阅通道
    _redis.unsubscribe(userid); 

    // 更新用户的状态信息
    User user(userid, "", "", "offline");
    _userModel.updateState(user);

}
// 处理客户端异常退出
void ChatService::clientCloseException(const TcpConnectionPtr &conn){
    //先找到异常退出的用户
    User user;
    {
        lock_guard<mutex> lock(_connMutex);
        for (auto it = _userConnMap.begin(); it != _userConnMap.end(); ++it)
        {
            if (it->second == conn)
            {
                // 从map表删除用户的链接信息
                user.setId(it->first);
                _userConnMap.erase(it);
                break;
            }
        }
    }

    // 用户注销，相当于就是下线，在redis中取消订阅通道
    _redis.unsubscribe(user.getId()); 

    // 更新用户的状态信息
    if (user.getId() != -1)
    {
        user.setState("offline");
        _userModel.updateState(user);
    }

}
// 服务器异常，业务重置方法
void ChatService::reset(){
//所有的用户都下线
    _userModel.resetState();
}