#include <stdio.h>
#include <sys/socket.h>
#include <sys/types.h>        
#include <unistd.h>
#include <arpa/inet.h>
#include <string.h>
#include <iostream>

using namespace std;   //使用标准空间
/*
//sockaddr_in结构体定义：
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
#define SERV_PORT 	8010                   //服务器端口
#define SERV_ADDR 	"127.0.0.1"         //服务器ip
#define CLI_ADDR 	"127.0.0.1"		   //服务器开放给我们的ip地址
char recv_buf[1000];        //定义接收缓存区
char send_buf[1000];        //定义发送缓存区
int main(int argc,char **argv)
{
	int ret = -1;
	int sockfd = -1;  //定义网络文件描述符
	
	struct sockaddr_in servaddr={0};  //服务器sockaddr_in定义成ipv4类型的服务器ip结构体（ipv6是sockaddr_inv6）

	//1.首先使用socket函数创建网络文件描述符（类似于文件IO中的open函数）
    //函数原型：int socket(int domain, int type, int protocol);   
	sockfd = socket(AF_INET, SOCK_STREAM, 0);   //ipv4,TCP,系统自动选择protocol
	if(sockfd < 0)
	{
		cout<<"创建socket文件描述符失败"<<endl;
		_exit(-1);
	}
	cout<<"sockfd="<<sockfd<<endl;
	//2.使用connect函数连接服务器
	//函数原型：int connect(int socket, const struct sockaddr *address,socklen_t address_len);
	servaddr.sin_family = AF_INET;             //定义servaddr的domain地址族为ipv4
	servaddr.sin_port = htons(SERV_PORT);      //定义servaddr的portnum为SERV_PORT(8010),host to net short
	servaddr.sin_addr.s_addr = inet_addr(SERV_ADDR);  //定义servaddr的address为SERV_ADDR(192.168.1.23)  person----->u32

    ret = connect(sockfd, (const struct sockaddr *)&servaddr,sizeof(servaddr));
	if(ret < 0)
	{
		cout<<"客户端connect服务器失败"<<endl;
		_exit(-1);
	}
	cout<<"客户端connect服务器成功"<<endl;
	//下面客户端和服务器互相收发
	while(1)
	{
		//6.使用send函数发生数据
		cout<<"请输入要发送给服务器的内容：";
		cin >> send_buf;
		if(!strncmp(send_buf,"+++",3))break;
		ret = send(sockfd, send_buf, strlen(send_buf), 0);
		cout<<"发送了"<<ret<<"个数据"<<endl;
		memset(send_buf,0,sizeof(send_buf));   //清空接收缓存区
		
		ret = recv(sockfd, recv_buf, sizeof(recv_buf), 0);   //接收客户端发送来的数据，注意此处使用的网络描述符是clifd
		if(ret < 1)
		{
			cout<<"服务器断开了连接"<<endl;
			break;
		}
		cout<<"收到服务器发送的数据:len= "<<ret<<"     buf= "<<recv_buf<<endl;
		memset(recv_buf,0,sizeof(recv_buf));   //清空接收缓存区
	}
	cout<<"关闭连接并退出"<<endl;
	close(sockfd);     //关闭socket文件描述符
	
	return 0;
}
