#ifndef _TCP_SERVER_H_
#define _TCP_SERVER_H_

#include <vector>
#include <Winsock2.h>
#include "MemoryPool.h"

//客户端连接进入服务器
#define _TSE_CONNECTED 0

//客户端断开连接
#define _TSE_DISCONNECTED 1

//客户端发送过来数据
#define _TSE_DATA 2

class TcpServer
{
public:

	//网络事件
	struct EVENT
	{
		//网络事件类型
		int type;

		//客户端标示
		SOCKET sock;

		//数据
		char* data;

		//长度
		int size;
	};

private:

	//返回INVALID_SOCKET：无人连接
	//返回正常套接字：有人连接成功之后的套接字
	static SOCKET my_accept(SOCKET listen_sock);

	//返回-1：发生网络错误
	//返回0：发送缓冲区已满暂时不能发送数据
	//返回>0：已经发送了多少字节
	static int my_send(SOCKET sock, char* buf, int len);

	//返回-2：接收缓冲区是空的当前不可读
	//返回-1：发生网络错误
	//返回0：对方优雅断开
	//返回>0：读取了多少字节
	static int my_recv(SOCKET sock, char* buf, int len);

	//接收连接
	void _Accept();

	//发送数据
	void _Send();

	//接收数据
	void _Recv();

	//最大连接数
	int m_Max;

	//最大数据接收长度
	int m_MaxRecv;

	//监听套接字
	SOCKET m_Listen;

	//网络事件表
	std::vector<EVENT> m_Events;

	//内存池
	MemoryPool m_MemoryPool;

	//发送数据包
	struct SENDDATA
	{
		//发送数据的首地址
		char* data;

		//发送数据的总长度
		int size;

		//已经发送的长度
		int send_size;
	};

	//客户端
	struct CLIENT
	{
		//套接字
		SOCKET sock;

		//发送数据表
		std::vector<SENDDATA> send_data;

		//接收数据
		char* recv_data;
		int recv_size;
	};

	//客户端表
	std::vector<CLIENT> m_Clients;

	//释放客户端相关信息
	void _Delete(CLIENT* client);

public:

	//初始化
	bool Init(
		
		//端口号
		unsigned short port,
		
		//最大连接人数
		int max,
		
		//最大接收数据长度
		int max_recv = 1024);

	//运行
	void Run();

	//得到网络事件
	bool GetEvent(EVENT* e);

	//释放网络事件内存
	void FreeEventMemory(EVENT* e);

	//发送数据给指定客户端
	bool Send2Client(SOCKET sock, const void* data, int size);
	//发送到所有客户
	bool Send2AllClient(void* data, int size);
	//发送到其他客户
	bool Send2OtherClient(SOCKET sock, void* data, int size);

	//断开指定客户端
	bool Disconnect(SOCKET sock);

	//结束
	void End();
};

#endif