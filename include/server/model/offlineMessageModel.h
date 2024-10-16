#pragma once
#include <string>
#include <vector>
using namespace std;
//表格
//-----------userid------------messgae------------
//-------------1----------------'sadas'----------- //表示用户id有啥消息没有收到  //存储的是json字符串  这里有个问题 为什么不直接保存messgae
class OfflineMsgModel
{
public:
    // 存储用户的离线消息
    void insert(int userid, string msg);

    // 删除用户的离线消息
    void remove(int userid);

    // 查询用户的离线消息
    vector<string> query(int userid);
};