#include "offlineMessageModel.h"
#include "connectionPool.h"
void OfflineMsgModel::insert(int userid, string msg)
{
    char sql[1024] = {0};
    sprintf(sql, "insert into offlinemessage values(%d, '%s')", userid, msg.c_str());
    ConnectionPool *cp = ConnectionPool::getConnectionPool();
    shared_ptr<Connection> sp = cp->getConnection();
    sp->update(sql);
}

void OfflineMsgModel::remove(int userid)
{
    char sql[1024] = {0};
    sprintf(sql, "delete from offlinemessage where userid=%d", userid);
    ConnectionPool *cp = ConnectionPool::getConnectionPool();
    shared_ptr<Connection> sp = cp->getConnection();
    sp->update(sql);
}

vector<string> OfflineMsgModel::query(int userid)
{
    vector<string> vec;
    // 1.组装sql语句
    char sql[1024] = {0};
    sprintf(sql, "select message from offlinemessage where userid = %d", userid);
    ConnectionPool *cp = ConnectionPool::getConnectionPool();
    shared_ptr<Connection> sp = cp->getConnection();
    sp->update(sql);
    MYSQL_RES *res = sp->query(sql);
    if (res != nullptr)
    {
        MYSQL_ROW row;
        while ((row = mysql_fetch_row(res)) != nullptr)
        {
            vec.push_back(row[0]);
        }
        mysql_free_result(res);
        return vec;
    }
    return vec;
}
