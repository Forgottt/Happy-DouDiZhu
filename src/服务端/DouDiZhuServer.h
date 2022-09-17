#ifndef _DOUDIZHU_SERVER_H_
#define _DOUDIZHU_SERVER_H_
#include<algorithm>
#include "GameServer.h"
#include <vector>
using namespace std;

#define _DDZ_FP 0//����
#define _DDZ_BC 1//����
#define _DDZ_CP 2//����
#define _DDZ_CPOK 3//����OK
#define _DDZ_CPFAIL 4//����Fail //����ȷ�����ʹ���˷Ƿ�����
#define _DDZ_FIGHTSTART 996//�������
#define _DDZ_FIGHT 997//����
#define _DDZ_PLAY 998//��Ϸ�׶�
#define _DDZ_FINISH 999//��Ϸ����
#define _DDZ_BJ 1000//����
#define _DDZ_JDZ 1001//�е���
#define _DDZ_BQ 1002//����
#define _DDZ_QDZ 1003//������

#define _DDZ_SOUND 2000//��Ч
#define _DDZ_SOUND_BJ 2001//��Ч ����
#define _DDZ_SOUND_JDZ 2002//��Ч �е���
#define _DDZ_SOUND_BQ 2003//��Ч ����
#define _DDZ_SOUND_QDZ 2004//��Ч ������
#define _DDZ_SOUND_TWOKING 2005//��Ч ��ը
#define _DDZ_SOUND_FORE 2006//��Ч ��ͨը��
#define _DDZ_SOUND_PLANE 2007//��Ч �ɻ�
#define _DDZ_SOUND_MORETHREEDOUBLE 2008//��Ч ����
#define _DDZ_SOUND_MOREFIVE 2009//��Ч ˳��
#define _DDZ_SOUND_FOREANDTWODOUBLE 2010//��Ч �Ĵ�����
#define _DDZ_SOUND_FOREANDTWOSINGLE 2011//��Ч �Ĵ���
#define _DDZ_SOUND_THREEANDDOUBLE 2012//��Ч ����һ��
#define _DDZ_SOUND_THREEANDONE 2013//��Ч ����һ
#define _DDZ_SOUND_THREE 2014//��Ч ����
#define _DDZ_SOUND_DOUBLE 2015//��Ч ����
#define _DDZ_SOUND_SINGLE 2016//��Ч ����
#define _DDZ_SOUND_BC 2017//��Ч ����

#define _DDZ_WIN 200//ʤ��
#define _DDZ_LOSE 201//ʧ��

class DouDiZhuServer : public GameServer
{
	int LastPokerType;//���Ƶ�����
	int m_MaxPlayer;
	int m_CurPlayerSeet[3];//��λ
	int PlayerStatus[3];//������
	int PlayerFightStatus[3];//���Fight���  0 ���� 1 �е���  2 ���� 3������
	int m_Time;
	char FightDiZhu;
	int QDZnum;
	int DownCard[3];//����
	int BCnum;//��������
	int CurPlayerseat;//��ǰ�����λ
	vector<int> CardIndex;//��������
	vector<int> AllCard;
	vector<int> PlayerCard0, PlayerCard1, PlayerCard2;
public:
	DouDiZhuServer();
	int MaxPlayer();
	void Init();
	void SetPlayerPoker();
	bool CPisOK();
	void OnPlayerSendData(int seat, void* data, int size);
	void OnUpdate();
	void OnEnd(int type, int seat);
	void OnPlayCP_Sound(int type, int index);
};
#endif