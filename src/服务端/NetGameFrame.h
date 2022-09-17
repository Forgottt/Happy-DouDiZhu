#ifndef _NET_GAME_FRAME_H_
#define _NET_GAME_FRAME_H_

#include "TcpServer.h"
#include "SendPackage.h"
#include "RecvPackage.h"
#include <vector>
#include <string>
#include <map>

//�������״̬
#define _PLAYER_OL_NO_LOGIN 0
#define _PLAYER_OL_HALL 1
#define _PLAYER_OL_ROOM 2
#define _PLAYER_OL_PREPARE 3
#define _PLAYER_OL_GAME 4

//��Ϸǰ������
class GameServer;

class NetGameFrame
{
	//�ڴ��
	MemoryPool m_MP;

	//Tcp������
	TcpServer m_TcpServer;

	//���ݿ����
	struct PLAYER_DB
	{
		//����
		char password[32];

		//�÷�
		int score;
	};
	std::map<std::string, PLAYER_DB> m_PlayersDB;

public:

	//�������
	struct PLAYER_OL
	{
		//�˺�
		char id[32];

		//�÷�
		int score;

		//״̬
		int state;

		//����
		int room;
		
		//��λ
		int seat;
	};
	std::map<SOCKET, PLAYER_OL> m_PlayersOL;

	//���Ͱ�
	SendPackage m_SP;

	//���հ�
	RecvPackage m_RP;

	//��Ϸ��
	std::vector<GameServer*> m_GameServers;

private:

	//���ݿ��ļ���
	std::string m_DB;

	//�������ݿ����ӳ��
	void _LoadDB(const char* fn);

	//�洢���ݿ����ӳ��
	void _SaveDB();

	//�������
	void _OnConnected(SOCKET sock);

	//��ҶϿ�
	void _OnDisconnected(SOCKET sock);

	//��ҷ�������
	void _OnData(SOCKET sock, const void* data, int size);

	//�������Ϣ����
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

	//��ʼ��
	void Init(
		
		//�����Ϣ���ݿ�xml�ļ�
		const char* db,
		
		//�˿ں�
		unsigned short port,
		
		//�����������
		int max);

	//װ����Ϸ(��һ����Ϸ������Ǽ�һ�����������Ϸ���൱�ڼ���һ������)
	void AddGameServer(GameServer* gs);

	//����
	void Run();

	//����
	void End();

	//�õ�Tcp������
	TcpServer* GetTS();

	//�õ����Ͱ�
	SendPackage* GetSP();

	//�õ����հ�
	RecvPackage* GetRP();
};

#endif