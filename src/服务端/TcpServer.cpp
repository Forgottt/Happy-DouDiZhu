#include "TcpServer.h"

#pragma comment(lib, "Ws2_32.lib")

SOCKET TcpServer::my_accept(SOCKET listen_sock)
{
	//创建套接字集合，将套接字集合
	//中放入监听套接字，然后判断其
	//是否可读
	fd_set fs;
	FD_ZERO(&fs);
	FD_SET(listen_sock, &fs);
	timeval t = {0, 0};

	//多路复用
	select(0, &fs, 0, 0, &t);

	//根据fs的长度判断当前是否有人
	//发起连接请求
	if (0 == fs.fd_count)
		return INVALID_SOCKET;
	else
	{
		sockaddr_in si;
		int si_size = sizeof(si);
		SOCKET sock = accept(
			fs.fd_array[0],
			(sockaddr*)&si,
			&si_size);
		return sock;
	}
}

int TcpServer::my_send(SOCKET sock, char* buf, int len)
{
	fd_set fs;
	FD_ZERO(&fs);
	FD_SET(sock, &fs);
	timeval t = {0, 0};

	//多路复用判断可写性
	select(0, 0, &fs, 0, &t);

	//如果不可写
	if (0 == fs.fd_count)
		return 0;
	else
	{
		//send函数的返回值如果是SOCKET_ERROR就表示
		//当前的网络出错了，应该关闭套接字；否则其
		//返回值表示本次真正发送了多少字节，这个返
		//回值可能会比len小
		return send(sock, buf, len, 0);
	}
}

int TcpServer::my_recv(SOCKET sock, char* buf, int len)
{
	fd_set fs;
	FD_ZERO(&fs);
	FD_SET(sock, &fs);
	timeval t = {0, 0};

	//多路复用判断可读性
	select(0, &fs, 0, 0, &t);

	//如果不可读
	if (0 == fs.fd_count)
		return -2;
	else
	{
		//recv如果返回为0则表示对方优雅断开，返回
		//SOCKET_ERROR表示网络出错了，这两种情况下
		//都是客户端已经断开了，我们不能使用这个套
		//接字了，就应该关闭套接字；否则返回值就表
		//示收到了多少字节
		return recv(sock, buf, len, 0);
	}
}

void TcpServer::_Delete(CLIENT* client)
{
	//释放发送数据
	for (int i = 0; i < (int)client->send_data.size(); ++i)
		m_MemoryPool.Free(client->send_data[i].data);

	//释放接收数据
	m_MemoryPool.Free(client->recv_data);

	//关闭套接字
	closesocket(client->sock);
}

void TcpServer::_Accept()
{
	//判断当前人数是否已满
	if (m_Max == m_Clients.size())
		return;

	//接收连接
	SOCKET sock = my_accept(m_Listen);
	if (INVALID_SOCKET == sock)
		return;

	//客户端入表
	CLIENT client;
	client.sock = sock;
	client.recv_data = (char*)m_MemoryPool.Allocate(sizeof(int) + m_MaxRecv);
	client.recv_size = 0;
	m_Clients.push_back(client);

	//投递"客户端连接进入服务器"事件
	EVENT e;
	e.type = _TSE_CONNECTED;
	e.sock = sock;
	e.data = 0;
	e.size = 0;
	m_Events.push_back(e);
}

void TcpServer::_Send()
{
	//迭代器
	std::vector<CLIENT>::iterator it;

	//遍历所有客户端发送它们的数据
	for (it = m_Clients.begin(); it != m_Clients.end(); )
	{
		//没有发送的数据
		if (it->send_data.size() == 0)
			++it;
		//有待发送的数据
		else
		{
			//退出标志
			//0：正常发送完毕
			//1：暂时不能发送
			//2：出现网络错误
			int qf = 0;

			//得到第0个数据包
			SENDDATA* sd = &it->send_data[0];

			//循环发送数据
			while (sd->send_size < sd->size)
			{
				//发送本次数据
				int r = my_send(
					it->sock,
					sd->data + sd->send_size,
					sd->size - sd->send_size);

				//发送数据失败
				if (r <= 0)
				{
					//暂时不能发了
					if (0 == r)
					{
						qf = 1;
						break;
					}
					//出现网络错误
					else
					{
						qf = 2;
						break;
					}
				}
				else
				{
					//已经发送的字节数累积到send_size中
					sd->send_size += r;
				}
			}

			//根据跳出循环的不同进行分情况处理
			if (0 == qf)
			{
				//释放该包占据的堆内存
				m_MemoryPool.Free(sd->data);

				//发送完将该包从表中移除
				it->send_data.erase(it->send_data.begin());

				//迭代
				++it;
			}
			else if (1 == qf)
			{
				//迭代
				++it;
			}
			else
			{
				//删除该玩家所有信息
				_Delete(&(*it));

				//投递"客户端断开连接"事件
				EVENT e;
				e.type = _TSE_DISCONNECTED;
				e.sock = it->sock;
				e.data = 0;
				e.size = 0;
				m_Events.push_back(e);

				//从表中删除
				it = m_Clients.erase(it);
			}
		}
	}
}

void TcpServer::_Recv()
{
	//迭代器
	std::vector<CLIENT>::iterator it;

	//遍历所有客户端接收数据
	for (it = m_Clients.begin(); it != m_Clients.end(); )
	{
		//用于得到本次recv函数要接收多长的数据
		int len;

		//本次接收就接收长度
		if (it->recv_size < sizeof(int))
		{
			//之后的recv就收len这么长
			len = sizeof(int) - it->recv_size;
		}
		//本次接收就接收数据
		else
		{
			//得到本次数据包的总长度
			int all_len = *((int*)it->recv_data);

			//之后的recv就收len这么长
			len = all_len - (it->recv_size - sizeof(int));
		}

		//接收数据
		int r = my_recv(
			it->sock,
			it->recv_data + it->recv_size,
			len);

		//正常接收了数据
		if (r > 0)
		{
			//总接收长度递增
			it->recv_size += r;

			//如果一个包的长度接收完毕
			if (it->recv_size > sizeof(int))
			{
				//得到本次数据包的总长度
				int all_len = *((int*)it->recv_data);

				//本次数据已经接收完毕
				if (it->recv_size - sizeof(int) == all_len)
				{
					//投递"客户端发送过来数据"事件
					EVENT e;
					e.type = _TSE_DATA;
					e.sock = it->sock;
					e.data = (char*)m_MemoryPool.Allocate(all_len);
					memcpy(e.data, it->recv_data + sizeof(int), all_len);
					e.size = all_len;
					m_Events.push_back(e);

					//接收数据辅助变量归零
					it->recv_size = 0;
				}
			}

			//迭代
			++it;
		}
		else if (-2 == r)
		{
			//迭代
			++it;
		}
		else if (-1 == r || 0 == r)
		{
			//删除该玩家所有信息
			_Delete(&(*it));

			//投递"客户端断开连接"事件
			EVENT e;
			e.type = _TSE_DISCONNECTED;
			e.sock = it->sock;
			e.data = 0;
			e.size = 0;
			m_Events.push_back(e);

			//从表中删除
			it = m_Clients.erase(it);
		}
	}
}

bool TcpServer::Init(unsigned short port,
					 int max,
					 int max_recv)
{
	//参数检查
	if (port <= 1024 || max < 1 || max_recv < 1)
		return false;

	//得到参数
	m_Max = max;
	m_MaxRecv = max_recv;

	//开启网络环境
	WSADATA wd;
	WSAStartup(0x0101, &wd);

	//创建监听套接字
	m_Listen = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

	//填充地址信息结构体
	sockaddr_in si;
	si.sin_family = AF_INET;
	si.sin_port = htons(port);
	si.sin_addr.S_un.S_addr = ADDR_ANY;

	//绑定监听套接字和地址信息结构体
	bind(m_Listen, (sockaddr*)&si, sizeof(si));

	//设置监听套接字为监听工作模式
	listen(m_Listen, 8);

	return true;
}

void TcpServer::Run()
{
	//接收连接
	_Accept();

	//发送数据
	_Send();

	//接收数据
	_Recv();
}

bool TcpServer::GetEvent(EVENT* e)
{
	if (m_Events.size() == 0)
		return false;

	//得到事件
	*e = m_Events[0];

	//删除事件
	m_Events.erase(m_Events.begin());

	return true;
}

void TcpServer::FreeEventMemory(EVENT* e)
{
	if (_TSE_DATA == e->type)
		m_MemoryPool.Free(e->data);
}

bool TcpServer::Send2Client(SOCKET sock, const void* data, int size)
{
	//查找指定套接字
	int index = 0;
	for (; index < (int)m_Clients.size(); ++index)
	{
		if (m_Clients[index].sock == sock)
			break;
	}

	//没有找到指定客户端
	if (index == (int)m_Clients.size())
		return false;

	//创建发送数据
	SENDDATA sd;
	sd.size = sizeof(int) + size;
	sd.data = (char*)m_MemoryPool.Allocate(sd.size);
	*((int*)sd.data) = size;
	memcpy(sd.data + sizeof(int), data, size);
	sd.send_size = 0;

	m_Clients[index].send_data.push_back(sd);

	return true;
}

//发送到所有客户
bool TcpServer::Send2AllClient(void* data, int size)
{
	//创建发送数据
	SENDDATA sd;
	sd.size = sizeof(int) + size;
	sd.data = (char*)m_MemoryPool.Allocate(sd.size);
	*((int*)sd.data) = size;
	memcpy(sd.data + sizeof(int), data, size);
	sd.send_size = 0;

	for (int index = 0; index < (int)m_Clients.size(); ++index)
		m_Clients[index].send_data.push_back(sd);

	return true;
}
//发送到其他客户
bool TcpServer::Send2OtherClient(SOCKET sock, void* data, int size)
{
	//创建发送数据
	SENDDATA sd;
	sd.size = sizeof(int) + size;
	sd.data = (char*)m_MemoryPool.Allocate(sd.size);
	*((int*)sd.data) = size;
	memcpy(sd.data + sizeof(int), data, size);
	sd.send_size = 0;

	for (int index = 0; index < (int)m_Clients.size(); ++index)
	{
		if (m_Clients[index].sock != sock)
			m_Clients[index].send_data.push_back(sd);
	}
		
	return true;
}

bool TcpServer::Disconnect(SOCKET sock)
{
	//查找指定套接字
	int index = 0;
	for (; index < (int)m_Clients.size(); ++index)
	{
		if (m_Clients[index].sock == sock)
			break;
	}

	//没有找到指定客户端
	if (index == (int)m_Clients.size())
		return false;

	//断开连接
	_Delete(&m_Clients[index]);

	//投递"客户端断开连接"事件
	EVENT e;
	e.type = _TSE_DISCONNECTED;
	e.sock = sock;
	e.data = 0;
	e.size = 0;
	m_Events.push_back(e);

	//从表中删除
	m_Clients.erase(m_Clients.begin() + index);

	return true;
}

void TcpServer::End()
{
	//断开所有玩家
	for (int i = 0; i < (int)m_Clients.size(); ++i)
		_Delete(&m_Clients[i]);
	m_Clients.clear();

	//释放监听套接字
	closesocket(m_Listen);

	//关闭网络环境
	WSACleanup();
}