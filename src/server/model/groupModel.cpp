#include "groupModel.h"
#include "connectionPool.h"
// 需要明确的是组和组user是一个关联表  需要用到关联操作
bool GroupModel::createGroup(Group &group)
{
    char sql[1024]={0};
    sprintf(sql,"insert into allgroup(groupname,groupdesc) values(%s,%s)",group.getName().c_str(),group.getDesc().c_str());
    ConnectionPool *cp=ConnectionPool::getConnectionPool();
    Connection::ptr sp=cp->getConnection();
    if (sp)
    {
        if (sp->update(sql))
        {
            group.setId(mysql_insert_id(sp->getConnection()));
            return true;
        }
    }
    return false;
}
//把userid加到groupid里面
void GroupModel::addGroup(int userid, int groupid, string role)  
{
    char sql[1024] = {0};
    sprintf(sql, "insert into groupuser values(%d, %d, '%s')",
            groupid, userid, role.c_str());

    ConnectionPool *cp = ConnectionPool::getConnectionPool();
    Connection::ptr sp = cp->getConnection();
    sp->update(sql);

}
//根据userid查找有哪些组拥有他
vector<Group> GroupModel::queryGroups(int userid)
{

    return vector<Group>();
}

vector<int> GroupModel::queryGroupUsers(int userid, int groupid)
{
    return vector<int>();
}
