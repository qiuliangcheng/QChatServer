#include "connection.h"
#include <muduo/base/Logging.h>
Connection::Connection()
{
    _conn = mysql_init(nullptr);//给连接分配内存
}

Connection::~Connection()
{
	if (_conn != nullptr)
		mysql_close(_conn);//释放内存连接
}

bool Connection::connect(string ip, unsigned short port, string user, string password, string dbname)
{
    MYSQL *p = mysql_real_connect(_conn, ip.c_str(), user.c_str(),
								  password.c_str(), dbname.c_str(), 3306, nullptr, 0); //进行连接
    if (p != nullptr)
    {
        mysql_query(_conn, "set names gbk");//设置名字可以显示中文
        LOG_INFO << "connect mysql success!";
    }
    else
    {
        LOG_INFO << "connect mysql fail!";
    }
    return p!=nullptr;
}
//增删改
bool Connection::update(string sql)
{
	if (mysql_query(_conn, sql.c_str()))
	{
		LOG_INFO << __FILE__ << ":" << __LINE__ << ":" << sql << "更新失败!";
		return false;
	}
	return true;
}

MYSQL_RES *Connection::query(string sql)
{
	if (mysql_query(_conn, sql.c_str()))
	{
		LOG_INFO << __FILE__ << ":" << __LINE__ << ":" << sql << "查询失败!";
		return nullptr;
	}
	return mysql_use_result(_conn);
}

MYSQL *Connection::getConnection()
{
    return _conn;
}
