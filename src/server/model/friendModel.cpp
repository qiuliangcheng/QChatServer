#include "friendModel.h"
#include "connectionPool.h"
void FriendModel::insert(int userid, int friendid)
{
    char sql[1024] = {0};
    sprintf(sql, "insert into friend values(%d, %d)", userid, friendid);

    ConnectionPool *cp = ConnectionPool::getConnectionPool();
    shared_ptr<Connection> sp = cp->getConnection();
    sp->update(sql);
}

vector<User> FriendModel::query(int userid)
{
    vector<User> vec;
    char sql[1024] = {0};
    sprintf(sql, "select a.id,a.name,a.state from user a inner join friend b on b.friendid = a.id where b.userid=%d", userid);//再user表中查询得到 b的friendid
    ConnectionPool *cp = ConnectionPool::getConnectionPool();
    shared_ptr<Connection> sp = cp->getConnection();
    MYSQL_RES *res = sp->query(sql);
    if (res != nullptr)
    {
        MYSQL_ROW row;
        while ((row = mysql_fetch_row(res)) != nullptr)//遍历得到的结果
        {
            User user;
            user.setId(atoi(row[0]));
            user.setName(row[1]);
            user.setState(row[2]);
            vec.push_back(user);
        }
        mysql_free_result(res);
        return vec;
    }
    return vec;
}
