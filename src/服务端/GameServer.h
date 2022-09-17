#ifndef _GAME_SERVER_H_
#define _GAME_SERVER_H_

#include <Winsock2.h>
#include <vector>
#include "SendPackage.h"
#include "RecvPackage.h"

//游戏服务器状态
#define _GAME_SERVER_STOP 0
#define _GAME_SERVER_RUN 1

//游戏服务器结束方式
#define _GAME_SERVER_OVER_OK 0
#define _GAME_SERVER_OVER_QUIT 1
#define _GAME_SERVER_OVER_DISCONNECTED 2

//Tcp服务器前置声明
class NetGameFrame;

class GameServer
{
	friend class NetGameFrame;

private:

	//状态
	//停止状态：_GAME_SERVER_STOP
	//运行状态：_GAME_SERVER_RUN
	int m_State;

	//网络游戏框架
	NetGameFrame* m_NGF;
	
	//套接字表
	std::vector<SOCKET> m_Sockets;

	//断线玩家
	//如果没有玩家断线，那么下面的座位号
	//就是-1，否则就是0~房间总人数-1
	int m_DisconnectedSeat;

protected:

	//发送数据，子类调用
	bool SendData(int seat, void* data, int size,int whichsend);

	//结束游戏，子类调用
	void End();

	//发送数据包
	SendPackage m_SP;
	RecvPackage m_RP;

public:

	//NetGameFrame用于得到游戏人数//相当于桌子有几个座位
	virtual int MaxPlayer() = 0;

	//当NetGameFrame发现房间人数已满且都已经
	//准备好了，那么就会调用下面的Init函数，
	//我们可以在里面进行游戏的初始化以及调用
	//SendData来发送数据可客户端
	virtual void Init() = 0;

	//当NetGameFrame收到当前房间中玩家发送的
	//消息CP_PLAY_ROOM的时候，就会调用下面的
	//函数并传入玩家位置以及数据
	virtual void OnPlayerSendData(//实际上是接受到了客户端发送来的数据，并且这个数据是在房间中的客户发送
		int seat,
		void* data,
		int size) = 0;

	//当NetGameFrame发现我们的房间已经处于运
	//行状态了，就会在每次游戏循环的时候调用
	//我们房间的更新函数，我们也可以在这个函
	//数中处理游戏逻辑以及发送数据
	virtual void OnUpdate();

	//当GameServer判断出来当前游戏已经结束，
	//那么就应该调用End来结束游戏，而End里面
	//就会调用OnEnd(_GAME_SERVER_OVER_OK, -1)
	//那么我们应该在OnEnd中进行某些游戏收尾工
	//作；当NetGameFrame发现正在游戏中的房间
	//里面有玩家退出或者断线，那么也会调用下
	//面的函数OnEnd(_GAME_SERVER_OVER_QUIT, 
	//离开玩家的座位)、OnEnd(_GAME_SERVER_OVER_DISCONNECTED,
	//断线玩家的座位)
	virtual void OnEnd(
		int type,
		int seat) = 0;
};

#endif