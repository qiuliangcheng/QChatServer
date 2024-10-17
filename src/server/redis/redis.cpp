#include "redis.h"
#include <muduo/base/Logging.h>
#include <iostream>
#include <thread>
Redis::Redis()
    :_publish_context(nullptr)
    ,_subcribe_context(nullptr)
{
}

Redis::~Redis()
{
    if (_publish_context != nullptr)
    {
        redisFree(_publish_context);
    }

    if (_subcribe_context != nullptr)
    {
        redisFree(_subcribe_context);
    }
}

bool Redis::connect()
{
    _publish_context = redisConnect("127.0.0.1", 6379);
    if (nullptr == _publish_context)
    {
        LOG_DEBUG << "connect redis failed!";
        return false;
    }
    _subcribe_context = redisConnect("127.0.0.1", 6379);
    if (nullptr == _subcribe_context)
    {
        LOG_DEBUG << "connect redis failed!";
        return false;
    }
    thread t([&]() {
        observer_channel_message();//单独开一个线程去接收redis中订阅的通道
    });
    t.detach();

    LOG_DEBUG << "connect redis-server success!";
    return true;
}

bool Redis::publish(int channel, string message)
{
    redisReply *reply = (redisReply *)redisCommand(_publish_context, "PUBLISH %d %s", channel, message.c_str());
    if (nullptr == reply)
    {
        LOG_DEBUG << "publish command failed!" ;
        return false;
    }
    freeReplyObject(reply);
    return true;

}
    // SUBSCRIBE命令本身会造成线程阻塞等待通道里面发生消息，这里只做订阅通道，不接收通道消息
    // 通道消息的接收专门在observer_channel_message函数中的独立线程中进行
    // 只负责发送命令，不阻塞接收redis server响应消息，否则和notifyMsg线程抢占响应资源
bool Redis::subscribe(int channel)
{
    //sub分为三个步骤 第一个发送命令  第二个
    if (REDIS_ERR == redisAppendCommand(this->_subcribe_context, "SUBSCRIBE %d", channel))
    {
        cerr << "subscribe command failed!" << endl;
        return false;
    }
    // 从hiredis的redisCommand源码上可以看出，它实际上相当于调用了这三个函数：

    // redisAppendCommand 把命令写入本地发送缓冲区
    // redisBufferWrite 把本地缓冲区的命令通过网络发送出去
    // redisGetReply 阻塞等待redis server响应消息
    int done = 0;
    while (!done)
    {
        if (REDIS_ERR == redisBufferWrite(this->_subcribe_context, &done))
        {
            cerr << "subscribe command failed!" << endl;
            return false;
        }
    }
    //redisGetReplyh会一直阻塞线程
    return true;
}

bool Redis::unsubscribe(int channel)
{
    if (REDIS_ERR == redisAppendCommand(this->_subcribe_context, "UNSUBSCRIBE %d", channel))
    {
        cerr << "unsubscribe command failed!" << endl;
        return false;
    }
    // redisBufferWrite可以循环发送缓冲区，直到缓冲区数据发送完毕（done被置为1）
    int done = 0;
    while (!done)
    {
        if (REDIS_ERR == redisBufferWrite(this->_subcribe_context, &done))
        {
            cerr << "unsubscribe command failed!" << endl;
            return false;
        }
    }
    return true;

}

void Redis::observer_channel_message()
{
    redisReply *reply = nullptr;
    while (REDIS_OK == redisGetReply(this->_subcribe_context, (void **)&reply))  //这个会一直运行
    {
        // 订阅收到的消息是一个带三元素的数组   第二个是订阅的通道信号 第二个是消息
        if (reply != nullptr && reply->element[2] != nullptr && reply->element[2]->str != nullptr)
        {
            // 给业务层上报通道上发生的消息
            _notify_message_handler(atoi(reply->element[1]->str) , reply->element[2]->str);
        }

        freeReplyObject(reply);
    }
    cerr << ">>>>>>>>>>>>> observer_channel_message quit <<<<<<<<<<<<<" << endl;
}

void Redis::init_notify_handler(function<void(int, string)> fn)
{
    _notify_message_handler=std::move(fn);
}
