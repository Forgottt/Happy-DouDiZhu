#ifndef _GAME_SERVER_H_
#define _GAME_SERVER_H_

#include <Winsock2.h>
#include <vector>
#include "SendPackage.h"
#include "RecvPackage.h"

//��Ϸ������״̬
#define _GAME_SERVER_STOP 0
#define _GAME_SERVER_RUN 1

//��Ϸ������������ʽ
#define _GAME_SERVER_OVER_OK 0
#define _GAME_SERVER_OVER_QUIT 1
#define _GAME_SERVER_OVER_DISCONNECTED 2

//Tcp������ǰ������
class NetGameFrame;

class GameServer
{
	friend class NetGameFrame;

private:

	//״̬
	//ֹͣ״̬��_GAME_SERVER_STOP
	//����״̬��_GAME_SERVER_RUN
	int m_State;

	//������Ϸ���
	NetGameFrame* m_NGF;
	
	//�׽��ֱ�
	std::vector<SOCKET> m_Sockets;

	//�������
	//���û����Ҷ��ߣ���ô�������λ��
	//����-1���������0~����������-1
	int m_DisconnectedSeat;

protected:

	//�������ݣ��������
	bool SendData(int seat, void* data, int size,int whichsend);

	//������Ϸ���������
	void End();

	//�������ݰ�
	SendPackage m_SP;
	RecvPackage m_RP;

public:

	//NetGameFrame���ڵõ���Ϸ����//�൱�������м�����λ
	virtual int MaxPlayer() = 0;

	//��NetGameFrame���ַ������������Ҷ��Ѿ�
	//׼�����ˣ���ô�ͻ���������Init������
	//���ǿ��������������Ϸ�ĳ�ʼ���Լ�����
	//SendData���������ݿɿͻ���
	virtual void Init() = 0;

	//��NetGameFrame�յ���ǰ��������ҷ��͵�
	//��ϢCP_PLAY_ROOM��ʱ�򣬾ͻ���������
	//�������������λ���Լ�����
	virtual void OnPlayerSendData(//ʵ�����ǽ��ܵ��˿ͻ��˷����������ݣ���������������ڷ����еĿͻ�����
		int seat,
		void* data,
		int size) = 0;

	//��NetGameFrame�������ǵķ����Ѿ�������
	//��״̬�ˣ��ͻ���ÿ����Ϸѭ����ʱ�����
	//���Ƿ���ĸ��º���������Ҳ�����������
	//���д�����Ϸ�߼��Լ���������
	virtual void OnUpdate();

	//��GameServer�жϳ�����ǰ��Ϸ�Ѿ�������
	//��ô��Ӧ�õ���End��������Ϸ����End����
	//�ͻ����OnEnd(_GAME_SERVER_OVER_OK, -1)
	//��ô����Ӧ����OnEnd�н���ĳЩ��Ϸ��β��
	//������NetGameFrame����������Ϸ�еķ���
	//����������˳����߶��ߣ���ôҲ�������
	//��ĺ���OnEnd(_GAME_SERVER_OVER_QUIT, 
	//�뿪��ҵ���λ)��OnEnd(_GAME_SERVER_OVER_DISCONNECTED,
	//������ҵ���λ)
	virtual void OnEnd(
		int type,
		int seat) = 0;
};

#endif