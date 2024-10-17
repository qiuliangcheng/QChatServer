#include <stdio.h>
#include <sys/socket.h>
#include <sys/types.h>        
#include <unistd.h>
#include <arpa/inet.h>
#include <string.h>
#include <iostream>
#include <pthread.h>   //使用多线程头文件
#include <netinet/in.h>

using namespace std;   //使用标准空间

//注意：在socket服务器编程当中有多个文件描述符
//其中socket返回的文件描述符sockfd是用来监听用的，不能读写，而accept返回的文件描述符clifd是用来连接的操作的文件描述符。可以读写操作
/*
//sockaddr_in结构体（用来管理server或者client的socket信息）定义：
struct sockaddr_in
  {
    __SOCKADDR_COMMON (sin_);           //ipv4 or ipv6
    in_port_t sin_port;                 //Port number.
    struct in_addr sin_addr;            //Internet address.

    // Pad to size of `struct sockaddr'. 
    unsigned char sin_zero[sizeof (struct sockaddr) -
                           __SOCKADDR_COMMON_SIZE -
                           sizeof (in_port_t) -
                           sizeof (struct in_addr)];
  };
*/
#define SERV_PORT 	8010                  //服务器端口
#define SERV_ADDR 	"127.0.0.1"		  //服务器ip
#define BACKLOG 	100    				  //服务器最多可以监听的客户端数量(最多排队数量)

int client_num = 0;   //连接的客户端计数

//线程1处理函数
void *client_func1(void *clifd_recv)
{
	int ret = -1;
	char recv_buf[200];        //定义接收缓存区
	char send_buf[200];        //定义发送缓存区
	int clifd = *(int *)clifd_recv;   //创建新线程首先获取传入的客户端描述符clifd
	printf("有新的客户端 %d 连接成功,子线程1开始工作\r\n",clifd);
	//5.在连接成功后使用recv函数来接收数据
	//函数原型：ssize_t recv(int socket, void *buffer, size_t length, int flags);
	while(1)
	{
		ret = recv(clifd, recv_buf, sizeof(recv_buf), 0);   //接收客户端发送来的数据，注意此处使用的网络描述符是clifd
		if(ret < 1)
		{
			cout<<"客户端 "<<clifd<<" 断开了连接"<<endl;
			cout<<"关闭连接并退出"<<endl;
			close(clifd);      //关闭accept文件描述符clifd
			client_num--;    //当前服务的客户端数量加一
			pthread_exit(NULL);  //退出当前的线程
		}
		cout<<"收到客户端  "<<clifd<<"  发送的数据:len= "<<ret<<"      buf= "<<recv_buf<<endl;
		memset(recv_buf,0,sizeof(recv_buf));   //清空接收缓存区
		//6.使用send函数发生数据
		strcpy(send_buf,"hello client1!");
		// ret = send(clifd, send_buf, strlen(send_buf), 0);
		// cout<<"发送了"<<ret<<"个数据"<<endl;
		// memset(send_buf,0,sizeof(send_buf));   //清空接收缓存区
	}
}

//线程2处理函数
void *client_func2(void *clifd_recv)
{
	int ret = -1;
	char recv_buf[200];        //定义接收缓存区
	char send_buf[200];        //定义发送缓存区
	int clifd = *(int *)clifd_recv;   //创建新线程首先获取传入的客户端描述符clifd
	printf("有新的客户端 %d 连接成功,子线程2开始工作\r\n",clifd);
	//5.在连接成功后使用recv函数来接收数据
	//函数原型：ssize_t recv(int socket, void *buffer, size_t length, int flags);
	while(1)
	{	
		ret = recv(clifd, recv_buf, sizeof(recv_buf), 0);   //接收客户端发送来的数据，注意此处使用的网络描述符是clifd
		if(ret < 1)
		{
			cout<<"客户端 "<<clifd<<" 断开了连接"<<endl;
			cout<<"关闭连接并退出"<<endl;
			close(clifd);      //关闭accept文件描述符clifd
			client_num--;    //当前服务的客户端数量加一
			pthread_exit(NULL);  //退出当前的线程
		}
		cout<<"收到客户端  "<<clifd<<"  发送的数据:len= "<<ret<<"      buf= "<<recv_buf<<endl;
		memset(recv_buf,0,sizeof(recv_buf));   //清空接收缓存区
		//6.使用send函数发生数据
		strcpy(send_buf,"hello client2!!");
		// ret = send(clifd, send_buf, strlen(send_buf), 0);
		// cout<<"发送了"<<ret<<"个数据"<<endl;
		// memset(send_buf,0,sizeof(send_buf));   //清空接收缓存区
	}
}

//线程3处理函数
void *client_func3(void *clifd_recv)
{
	int ret = -1;
	char recv_buf[200];        //定义接收缓存区
	char send_buf[200];        //定义发送缓存区
	int clifd = *(int *)clifd_recv;   //创建新线程首先获取传入的客户端描述符clifd
	printf("有新的客户端 %d 连接成功,子线程3开始工作\r\n",clifd);
	//5.在连接成功后使用recv函数来接收数据
	//函数原型：ssize_t recv(int socket, void *buffer, size_t length, int flags);
	while(1)
	{	
		ret = recv(clifd, recv_buf, sizeof(recv_buf), 0);   //接收客户端发送来的数据，注意此处使用的网络描述符是clifd
		if(ret < 1)
		{
			cout<<"客户端 "<<clifd<<" 断开了连接"<<endl;
			cout<<"关闭连接并退出"<<endl;
			close(clifd);      //关闭accept文件描述符clifd
			client_num--;    //当前服务的客户端数量加一
			pthread_exit(NULL);  //退出当前的线程
		}
		cout<<"收到客户端  "<<clifd<<"  发送的数据:len= "<<ret<<"      buf= "<<recv_buf<<endl;
		memset(recv_buf,0,sizeof(recv_buf));   //清空接收缓存区
		//6.使用send函数发生数据
		strcpy(send_buf,"hello client3!!!");
		// ret = send(clifd, send_buf, strlen(send_buf), 0);
		// cout<<"发送了"<<ret<<"个数据"<<endl;
		// memset(send_buf,0,sizeof(send_buf));   //清空接收缓存区
	}
}

int main(int argc,char **argv)
{
	int ret = -1;
	int sockfd = -1;  	//定义socket网络文件描述符
	int clifd = -1;   	//定义accept网络文件描述符
	pthread_t th = -1;   //定义一个多线程创建句柄
	
	struct sockaddr_in servaddr={0};  //服务器sockaddr_in定义成ipv4类型的服务器ip结构体（ipv6是sockaddr_inv6）
	struct sockaddr_in cliaddr={0};   //客户端sockaddr_in
	socklen_t address_len = 0;         //客户端长度
	
	//1.首先使用socket函数创建网络文件描述符（类似于文件IO中的open函数）
    //函数原型：int socket(int domain, int type, int protocol);   
	sockfd = socket(AF_INET, SOCK_STREAM, 0);   //ipv4,TCP,系统自动选择protocol
	if(sockfd < 0)
	{
		cout<<"创建socket文件描述符失败"<<endl;
		_exit(-1);
	}
	cout<<"sockfd="<<sockfd<<endl;

	//注意：由TCP套接字状态TIME_WAIT引起在结束本次会话后close立刻开启下次会话会Bind失败。
	//该状态在套接字关闭后约保留 2 到 4 分钟。在 TIME_WAIT 状态退出之后，套接字被删除，该地址才能被重新绑定而不出问题。
	//因此下面两句话的加入可以解决这个问题
	int on = 1;
	ret = setsockopt( sockfd, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on) );

	//2.使用bind函数绑定socket文件描述符和相关参数
	//函数原型：int bind(int sockfd, const struct sockaddr *addr,socklen_t addrlen); 
	servaddr.sin_family = AF_INET;             //定义servaddr的domain地址族为ipv4
	servaddr.sin_port = htons(SERV_PORT);      //定义servaddr的portnum为SERV_PORT(8010),host to net short
	servaddr.sin_addr.s_addr = inet_addr(SERV_ADDR);  //定义servaddr的address为SERV_ADDR(192.168.1.23)  person----->u32
	//servaddr.sin_addr.s_addr = INADDR_ANY;  //监听该电脑的所有IP
	memset(servaddr.sin_zero,0,sizeof(servaddr.sin_zero));   //设置servaddr的sin_zero区域为0
	ret = bind(sockfd, (const struct sockaddr *)&servaddr,sizeof(servaddr));   //使用bind函数绑定socket文件描述符和相关参数
	if(ret < 0)
	{
		cout<<"bind函数绑定socket文件描述符失败"<<endl;
		_exit(-1);
	}
	cout<<"bind绑定成功"<<endl;
    //3.使用listen函数进行接收监听(监听当前设置的ip地址下的端口号port)
	//函数原型：int listen(int socket, int backlog);
	ret = listen(sockfd, BACKLOG);     //sockfd,等待列表，最多可以排队排BACKLOG(100)个
	if(ret < 0)
	{
		cout<<"listen监听失败"<<endl;
		_exit(-1);
	}
	cout<<"listen监听成功,等待客户端连接:"<<endl;
	
	while(1)
	{
		//4.使用accept函数阻塞等待客户端连接，注意：会阻塞！！！
		//函数原型：int accept(int socket, struct sockaddr *restrict address,socklen_t *restrict address_len);
		address_len = sizeof(struct sockaddr);  //给client_len赋值
		clifd = accept(sockfd, (struct sockaddr *)&cliaddr,&address_len);     //此时会阻塞在这监听客户端连接
		if(clifd < 0)
		{
			cout<<"accept连接客户端失败"<<endl;
			_exit(-1);
		}
		cout<<"listen连接客户端成功,clifd ="<<clifd<<endl;
		cout<<"客户端端口号port= "<<ntohs(cliaddr.sin_port)<<endl;
		cout<<"客户端ip= "<<inet_ntoa(cliaddr.sin_addr)<<endl;
		
		//在收到客户端连接成功后创建一个新的线程，在线程里进行数据收发
		client_num++;    //当前服务的客户端数量加1
		switch(client_num)
		{
			case 1:
				ret = pthread_create(&th, NULL, client_func1, (void *)(&clifd)); //创建一个线程client_func,将客户端描述符clifd作为参数传递给多线程处理函数
			break;
			case 2:
				ret = pthread_create(&th, NULL, client_func2, (void *)(&clifd));
			break;
			case 3:
				ret = pthread_create(&th, NULL, client_func3, (void *)(&clifd));
			break;
			default:
				cout<<"连接的客户端数量超过3台，停止服务"<<endl;
			break;
		}
		if(ret == 0)
		{
			printf("创建线程 %d 成功,有新的客户端 %d 连接成功,线程创建成功\r\n",(int)th,clifd);
			pthread_detach(th);   //在线程创建成功后使用pthread_detach分离子线程，这样就可以在子线程退出后自动回收子线程资源了
		}
		else
		{
			printf("线程创建失败\r\n");
			_exit(-1);
		}
	}
	cout<<"所有线程全部关闭，退出"<<endl;
	close(sockfd);     //关闭socket文件描述符

	return 0;
}
