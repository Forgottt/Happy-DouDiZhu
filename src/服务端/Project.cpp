#include "NetGameFrame.h"
#include "GameServer.h"
#include "DouDiZhuServer.h"

class Game1 : public GameServer
{
	int MaxPlayer()
	{return 3;}
	virtual void Init()
	{}
	void OnPlayerSendData(int seat, void* data, int size)
	{}
	virtual void OnUpdate()
	{}
	virtual void OnEnd(int type, int seat)
	{}
};


int main()
{
	NetGameFrame ngf;

	ngf.Init("game_player_info.xml", 12345, 100);

	//创建5个房间
	ngf.AddGameServer(new DouDiZhuServer);
	ngf.AddGameServer(new DouDiZhuServer);
	ngf.AddGameServer(new DouDiZhuServer);
	ngf.AddGameServer(new DouDiZhuServer);
	ngf.AddGameServer(new DouDiZhuServer);

	while (true)
	{
		ngf.Run();

		if (GetAsyncKeyState(VK_ESCAPE) & 1)
		{
			ngf.End();
			break;
		}

		Sleep(1);
	}

	return 0;
}