#pragma once

#include <mysql/mysql.h>
#include <ctime>
#include <string>
using namespace std;
class Connection
{
public:
    using ptr=shared_ptr<Connection>;
	// 初始化数据库连接
	Connection();
	// 释放数据库连接资源
	~Connection();
	/// @brief 数据库连接
	/// @param ip 数据库ip地址
	/// @param port 端口号 3306
	/// @param user root
	/// @param password  123456
	/// @param dbname 数据库表的名字 我这里用的是chat
	/// @return 
	bool connect(string ip,
				 unsigned short port,
				 string user,
				 string password,
				 string dbname);
	// 更新操作 insert、delete、update
	bool update(string sql);
	// 查询操作 select
	MYSQL_RES *query(string sql);

	MYSQL *getConnection();

	// 刷新一下连接的起始的空闲时间点
	void refreshAliveTime() { _alivetime = clock(); }//如果有连接一次就刷新一次
	// 返回存活的时间
	clock_t getAliveeTime() const { return clock() - _alivetime; }

private:
	MYSQL *_conn;		// 表示和MySQL Server的一条连接
	clock_t _alivetime; // 记录进入空闲状态后的起始存活时间
};