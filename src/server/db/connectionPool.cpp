#include "connectionPool.h"
#include "log.h"

const int _maxIdleTime=60;
const int _maxSize=1024;
const int _connectionTimeout=1000;
ConnectionPool *ConnectionPool::getConnectionPool()
{
    static ConnectionPool pool;
    pool.init("127.0.0.1",(unsigned short)3306,"root","123456","chat",10);
    return &pool;    
}

shared_ptr<Connection> ConnectionPool::getConnection()
{
	unique_lock<mutex> lock(_queueMutex);
	while (_connectionQue.empty())
	{
		// sleep
		if (cv_status::timeout == cv.wait_for(lock, chrono::milliseconds(_connectionTimeout)))
		{
			if (_connectionQue.empty())
			{
				LOG("获取空闲连接超时了...获取连接失败!");
				return nullptr;
			}
		}
    }
    shared_ptr<Connection> sp(_connectionQue.front(),
                            [&](Connection *pcon)
                            {
                                // 这里是在服务器应用线程中调用的，所以一定要考虑队列的线程安全操作
                                unique_lock<mutex> lock(_queueMutex);
                                pcon->refreshAliveTime(); // 刷新一下开始空闲的起始时间
                                _connectionQue.push(pcon);
                            });

    _connectionQue.pop();
    cv.notify_all(); // 消费完连接以后，通知生产者线程检查一下，如果队列为空了，赶紧生产连接
    return sp;
}
    


void ConnectionPool::init(string url, unsigned short port, string User, string PassWord, string DataBaseName, int initSize)
{
	m_url = url;
	m_Port = port;
	m_User = User;
	m_PassWord = PassWord;
	m_DatabaseName = DataBaseName;
    _initSize=initSize;
	for (int i = 0; i < _initSize; ++i)
	{
		Connection *p = new Connection();
		p->connect(m_url, m_Port, m_User, m_PassWord, m_DatabaseName);
		p->refreshAliveTime(); // 刷新一下开始空闲的起始时间
		_connectionQue.push(p);
		_connectionCnt++;
	}
    //用了detach后不能用join了
	// 启动一个新的线程，作为连接的生产者 linux thread => pthread_create
	thread produce(std::bind(&ConnectionPool::produceConnectionTask, this));
	produce.detach();

	// 启动一个新的定时线程，扫描超过maxIdleTime时间的空闲连接，进行对于的连接回收
	thread scanner(std::bind(&ConnectionPool::scannerConnectionTask, this));
	scanner.detach();
}

ConnectionPool::ConnectionPool()
{
    _connectionCnt=0;
}

void ConnectionPool::produceConnectionTask()
{
	for (;;)
	{
		unique_lock<mutex> lock(_queueMutex);
		while (!_connectionQue.empty())
		{
			cv.wait(lock); // 队列不空，此处生产线程进入等待状态
		}

		// 连接数量没有到达上限，继续创建新的连接
		if (_connectionCnt < _maxSize)
		{
			Connection *p = new Connection();
			p->connect(m_url, m_Port, m_User, m_PassWord, m_DatabaseName);
			p->refreshAliveTime(); // 刷新一下开始空闲的起始时间
			_connectionQue.push(p);
			_connectionCnt++;
		}

		// 通知消费者线程，可以消费连接了
		cv.notify_all();
	}
}

void ConnectionPool::scannerConnectionTask()
{
	for (;;)
	{
		// 通过sleep模拟定时效果
		this_thread::sleep_for(chrono::seconds(_maxIdleTime));//空闲多少秒检测一次

		// 扫描整个队列，释放多余的连接
		unique_lock<mutex> lock(_queueMutex);
		while (_connectionCnt > _initSize)
		{
			Connection *p = _connectionQue.front();
			if (p->getAliveeTime() >= (_maxIdleTime * 1000))
			{
				_connectionQue.pop();
				_connectionCnt--;
				delete p; // 调用~Connection()释放连接
			}
			else
			{
				break; // 队头的连接没有超过_maxIdleTime，其它连接肯定没有
			}
		}
	}

}
