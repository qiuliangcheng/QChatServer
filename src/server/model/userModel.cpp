#include "userModel.h"
#include "connectionPool.h"
bool UserModel::insert(User &user)
{
    char sql[1024] = {0};
    sprintf(sql, "insert into user(name, password, state) values('%s', '%s', '%s')",
            user.getName().c_str(), user.getPwd().c_str(), user.getState().c_str());
    ConnectionPool *cp = ConnectionPool::getConnectionPool();
    Connection::ptr conn= cp->getConnection();
    if(conn){
        if (conn->update(sql))
        {
            // 获取插入成功的用户数据生成的主键id
            user.setId(mysql_insert_id(conn->getConnection()));
            return true;
        }
    }
    return false;
}
//根据id查信息
User UserModel::query(int id)
{
    char sql[1024] = {0};
    sprintf(sql, "select * from user where id = %d", id);
    ConnectionPool *cp = ConnectionPool::getConnectionPool();
    Connection::ptr sp = cp->getConnection();
    MYSQL_RES *res = sp->query(sql);
    if (res != nullptr)
    {
        MYSQL_ROW row = mysql_fetch_row(res);
        if (row != nullptr)
        {
            User user;
            user.setId(atoi(row[0]));//atoi将字符串转换成整数。
            user.setName(row[1]);
            user.setPwd(row[2]);
            user.setState(row[3]);
            mysql_free_result(res);
            return user;
        }
    }
    return User();//说明没有
}

bool UserModel::updateState(User user)
{
    char sql[1024] = {0};
    sprintf(sql, "update user set state = '%s' where id = %d", user.getState().c_str(), user.getId());
    ConnectionPool *cp = ConnectionPool::getConnectionPool();
    Connection::ptr sp = cp->getConnection();
    if(sp){
        sp->update(sql);
        return true;
    }
    return false;
}

void UserModel::resetState()
{
    char sql[1024] = "update user set state = 'offline' where state = 'online'";//将所有的用户都下线

    ConnectionPool *cp = ConnectionPool::getConnectionPool();
    Connection::ptr sp = cp->getConnection();
    sp->update(sql);
}
