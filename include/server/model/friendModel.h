#pragma once
#include "user.h"
#include <vector>
using namespace std;
//更改friend的表格

class FriendModel
{
public:
    // 添加好友关系
    void insert(int userid, int friendid);

    // 返回用户好友列表
    vector<User> query(int userid);
};