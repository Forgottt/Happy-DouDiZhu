#ifndef _NET_GAME_FRAME_H_
#define _NET_GAME_FRAME_H_

#include "TcpServer.h"
#include "SendPackage.h"
#include "RecvPackage.h"
#include <vector>
#include <string>
#include <map>

//在线玩家状态
#define _PLAYER_OL_NO_LOGIN 0
#define _PLAYER_OL_HALL 1
#define _PLAYER_OL_ROOM 2
#define _PLAYER_OL_PREPARE 3
#define _PLAYER_OL_GAME 4

//游戏前置声明
class GameServer;

class NetGameFrame
{
	//内存池
	MemoryPool m_MP;

	//Tcp服务器
	TcpServer m_TcpServer;

	//数据库玩家
	struct PLAYER_DB
	{
		//密码
		char password[32];

		//得分
		int score;
	};
	std::map<std::string, PLAYER_DB> m_PlayersDB;

public:

	//在线玩家
	struct PLAYER_OL
	{
		//账号
		char id[32];

		//得分
		int score;

		//状态
		int state;

		//房间
		int room;
		
		//座位
		int seat;
	};
	std::map<SOCKET, PLAYER_OL> m_PlayersOL;

	//发送包
	SendPackage m_SP;

	//接收包
	RecvPackage m_RP;

	//游戏表
	std::vector<GameServer*> m_GameServers;

private:

	//数据库文件名
	std::string m_DB;

	//加载数据库玩家映射
	void _LoadDB(const char* fn);

	//存储数据库玩家映射
	void _SaveDB();

	//玩家连接
	void _OnConnected(SOCKET sock);

	//玩家断开
	void _OnDisconnected(SOCKET sock);

	//玩家发送数据
	void _OnData(SOCKET sock, const void* data, int size);

	//具体的消息处理
	void on_CP_LOGIN(SOCKET sock, RecvPackage* rp);
	void on_CP_REGISTER(SOCKET sock, RecvPackage* rp);
	void on_CP_CHOOSE_ROOM(SOCKET sock, RecvPackage* rp);
	void on_CP_TALK_HALL(SOCKET sock, RecvPackage* rp);
	void on_CP_TALK_ROOM(SOCKET sock, RecvPackage* rp);
	void on_CP_PREPARE_ROOM(SOCKET sock, RecvPackage* rp);
	void on_CP_CANCEL_ROOM(SOCKET sock, RecvPackage* rp);
	void on_CP_STOP_ROOM(SOCKET sock, RecvPackage* rp);
	void on_CP_QUIT_ROOM(SOCKET sock, RecvPackage* rp);
	void on_CP_PLAY_ROOM(SOCKET sock, RecvPackage* rp);
	void on_CP_QUIT_HALL(SOCKET sock, RecvPackage* rp);

public:

	//初始化
	void Init(
		
		//玩家信息数据库xml文件
		const char* db,
		
		//端口号
		unsigned short port,
		
		//最大连接人数
		int max);

	//装载游戏(加一个游戏对象就是加一个可以玩的游戏，相当于加了一张桌子)
	void AddGameServer(GameServer* gs);

	//运行
	void Run();

	//结束
	void End();

	//得到Tcp服务器
	TcpServer* GetTS();

	//得到发送包
	SendPackage* GetSP();

	//得到接收包
	RecvPackage* GetRP();
};

#endif