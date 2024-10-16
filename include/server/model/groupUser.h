#pragma once
#include "user.h"
//groupuser表

//-----------------groupid------------userid----------------------grouprole---------
//|                  组id       |       组员id            |          组员id的角色
//|                             |                        |
//|                             |                        |

class GroupUser : public User
{
public:
    void setRole(string role) { m_role = role; }
    string getRole() { return m_role; }

private:
    string m_role;
};