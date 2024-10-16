#pragma once 

#include <string>
using namespace std;
// User表的ORM类
class User
{
public:
    User(int id = -1, string name = "", string pwd = "", string state = "offline")
    {
        m_id = id;
        m_name = name;
        m_password = pwd;
        m_state = state;
    }

    void setId(int id) { m_id = id; }
    void setName(string name) { m_name = name; }
    void setPwd(string pwd) { m_password = pwd; }
    void setState(string state) { m_state = state; }

    int getId() { return m_id; }
    string getName() { return m_name; }
    string getPwd() { return m_password; }
    string getState() { return m_state; }

protected:
    int m_id;
    string m_name;
    string m_password;
    string m_state;
};

