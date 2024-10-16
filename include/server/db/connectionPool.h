#pragma once
#include "connection.h"
#include <queue>
#include <mutex>
#include <thread>
#include <condition_variable>
#include <atomic>
#include <memory>
#include <functional>
using namespace std;

//TODO 根据配置文件进行线程池的设置

class ConnectionPool{
public:
    static ConnectionPool *getConnectionPool();
    shared_ptr<Connection> getConnection();//从连接池获取一个连接
    
private:
    ConnectionPool();
    void init(string url,unsigned short port, string User, string PassWord, string DataBaseName, int _initSize=100); 
    void produceConnectionTask();//生产连接的线程
	// 扫描超过maxIdleTime时间的空闲连接，进行对于的连接回收
	void scannerConnectionTask();
    
	queue<Connection *> _connectionQue; // 存储mysql连接的队列   先进先出的特性
	mutex _queueMutex;					// 维护连接队列的线程安全互斥锁
	atomic_int _connectionCnt;			// 记录连接所创建的connection连接的总数量
	condition_variable cv;				// 设置条件变量，用于连接生产线程和连接消费线程的通信
	string m_url;			 //主机地址
	unsigned short m_Port;		 //数据库端口号
	string m_User;		 //登陆数据库用户名
	string m_PassWord;	 //登陆数据库密码
	string m_DatabaseName; //使用数据库名
    int _initSize;
};