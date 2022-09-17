#include "DouDiZhuServer.h"
#include <time.h>

//��ϷЭ��
#define _DDZ_CP_PLAYER_OK 100

//��������֤ 102 ���� 101 ũ�� 103 �����(��������ж�ʤ��)
#define _DDZ_SP_PLAYER_NONGMING 101 
#define _DDZ_SP_PLAYER_DIZHU 102 
#define _DDZ_SP_PLAYER_NONE 103

//������Э��
#define _DDZ_SP_POKER_TYPE_SINGLE 1 //����
#define _DDZ_SP_POKER_TYPE_DOUBLE 2 //����
#define _DDZ_SP_POKER_TYPE_THREE 3 //����
#define _DDZ_SP_POKER_TYPE_THREEANDONE 31 //����һ
#define _DDZ_SP_POKER_TYPE_THREEANDDOUBLE 32 //����һ��
#define _DDZ_SP_POKER_TYPE_FOREANDTWOSINGLE 41 //�Ĵ���
#define _DDZ_SP_POKER_TYPE_FOREANDTWODOUBLE 42 //�Ĵ�����
#define _DDZ_SP_POKER_TYPE_FORE 4 //ը��
#define _DDZ_SP_POKER_TYPE_MOREFIVE 5 //˳��
#define _DDZ_SP_POKER_TYPE_MORETHREEDOUBLE 6 //����
#define _DDZ_SP_POKER_TYPE_PLANE 7 //�ɻ�
#define _DDZ_SP_POKER_TYPE_TWOKING 8 //��ը


DouDiZhuServer::DouDiZhuServer()
{
	m_MaxPlayer = 3;
	//m_MaxPlayer = 3;
}

int DouDiZhuServer::MaxPlayer()
{
	return m_MaxPlayer;
}

void DouDiZhuServer::Init()
{
	CurPlayerseat = -1;
	srand(GetTickCount());
	//��ʼ����һ�γ������� (��һ��δ����-1)
	LastPokerType = -1;
	BCnum = 0;//��������

	//��ʼ�����������λ�ţ�0-2��
	//��ʼ�������ݣ�����ݣ�
	for (int i = 0;i < 3;++i)
	{
		m_CurPlayerSeet[i] = i;
		PlayerStatus[i] = _DDZ_SP_PLAYER_NONE;
		PlayerFightStatus[i] = -1;
		//��ʼ������
		DownCard[i] = -1;
	}

	//��ʼ�������������
	FightDiZhu = 0;

	//��ʼ������������
	QDZnum = 0;

	//��ʼ���ƿ�(0-54)
	for (int i = 0;i < 54;++i)
		AllCard.push_back(i);

	//��ճ�������
	CardIndex.clear();

	//��ʼ�����ӵ������(ȫ��Ϊ��,��־-1)
	for (int i = 0;i < 20;++i)
	{
		PlayerCard0.push_back(-1);
		PlayerCard1.push_back(-1);
		PlayerCard2.push_back(-1);
	}

	//����(�������ӵ�����Լ�������ID)
	SetPlayerPoker();

	//�����Ƶ����ݸ��ͻ������
	//���1
	m_SP.clear();
	m_SP.add_int(_DDZ_FP);
	for (size_t i = 0; i < PlayerCard0.size(); ++i)
		m_SP.add_int(PlayerCard0[i]);
	SendData(m_CurPlayerSeet[0], m_SP.data(), m_SP.size(), 0);
	//���2
	m_SP.clear();
	m_SP.add_int(_DDZ_FP);
	for (size_t i = 0; i < PlayerCard0.size(); ++i)
		m_SP.add_int(PlayerCard1[i]);
	SendData(m_CurPlayerSeet[1], m_SP.data(), m_SP.size(), 0);
	//���3
	m_SP.clear();
	m_SP.add_int(_DDZ_FP);
	for (size_t i = 0; i < PlayerCard0.size(); ++i)
		m_SP.add_int(PlayerCard2[i]);
	SendData(m_CurPlayerSeet[2], m_SP.data(), m_SP.size(), 0);

	//�������������Ϣ�������ң�
	m_SP.clear();
	m_SP.add_int(_DDZ_FIGHTSTART);
	SendData(m_CurPlayerSeet[rand() % 3], m_SP.data(), m_SP.size(), 0);

	//��ʼ��ʱ��
	m_Time = -1;
}

void DouDiZhuServer::OnPlayerSendData(int seat, //seat��ʾ��ǰ�����λ
	void* data, int size)
{
	//׼�������л�����
	m_RP.set_data(data, size);

	//�õ���Ϣ
	//��õ�ǰ��Ϸ����
	int GameType;
	m_RP.get_int(&GameType);


	//��õ����׶Σ���������¼�����ݣ�
	switch (GameType)
	{
		//�������;������
		case _DDZ_FIGHT://����׶�
		{
			int GameTypeType;
			m_RP.get_int(&GameTypeType);
			switch (GameTypeType)
			{
				case _DDZ_BJ://����
				{
					//������Ч��Ϣ
					for (int i = 0;i < 3;++i)
					{
						m_SP.clear();
						m_SP.add_int(_DDZ_SOUND);
						m_SP.add_int(_DDZ_SOUND_BJ);
						SendData(m_CurPlayerSeet[i], m_SP.data(), m_SP.size(), 0);
					}

					++FightDiZhu;
					PlayerFightStatus[seat] = 0;
					if (3 == FightDiZhu)//��û�е���,�������һ�����Ϊ����
					{
						int dz = rand() % 3;

						//������ݸ�ÿ���ͻ���
						PlayerStatus[(dz + 1) % 3] = _DDZ_SP_PLAYER_NONGMING;
						m_SP.clear();
						m_SP.add_int(_DDZ_SP_PLAYER_NONGMING);
						m_SP.add_int(-1);
						m_SP.add_int(DownCard[0]);
						m_SP.add_int(DownCard[1]);
						m_SP.add_int(DownCard[2]);
						SendData(m_CurPlayerSeet[(dz + 1) % 3], m_SP.data(), m_SP.size(), 0);

						PlayerStatus[(dz + 2) % 3] = _DDZ_SP_PLAYER_NONGMING;
						m_SP.clear();
						m_SP.add_int(_DDZ_SP_PLAYER_NONGMING);
						m_SP.add_int(1);
						m_SP.add_int(DownCard[0]);
						m_SP.add_int(DownCard[1]);
						m_SP.add_int(DownCard[2]);
						SendData(m_CurPlayerSeet[(dz + 2) % 3], m_SP.data(), m_SP.size(), 0);

						//������õ��ƻ�ó���Ȩ��
						PlayerStatus[dz] = _DDZ_SP_PLAYER_DIZHU;
						m_SP.clear();
						m_SP.add_int(_DDZ_SP_PLAYER_DIZHU);
						m_SP.add_int(DownCard[0]);
						m_SP.add_int(DownCard[1]);
						m_SP.add_int(DownCard[2]);
						SendData(m_CurPlayerSeet[dz], m_SP.data(), m_SP.size(), 0);

						//������������������
						if (0 == dz)
						{
							for (size_t i = 0; i < 3; i++)
								PlayerCard0.push_back(DownCard[i]);
							sort(PlayerCard0.begin(), PlayerCard0.end());
						}
						else if (1 == dz)
						{
							for (size_t i = 0; i < 3; i++)
								PlayerCard1.push_back(DownCard[i]);
							sort(PlayerCard1.begin(), PlayerCard1.end());
						}
						else
						{
							for (size_t i = 0; i < 3; i++)
								PlayerCard2.push_back(DownCard[i]);
							sort(PlayerCard2.begin(), PlayerCard2.end());
						}
						break;
					}
					else
					{
						int nextseat = (seat + 1) % 3;//��һ��λ��
						m_SP.clear();
						m_SP.add_int(_DDZ_BJ);
						m_SP.add_char(FightDiZhu);
						SendData(m_CurPlayerSeet[nextseat], m_SP.data(), m_SP.size(), 0);
					}
					break;
				}
				case _DDZ_JDZ://�е���
				{
					//������Ч��Ϣ
					for (int i = 0;i < 3;++i)
					{
						m_SP.clear();
						m_SP.add_int(_DDZ_SOUND);
						m_SP.add_int(_DDZ_SOUND_JDZ);
						SendData(m_CurPlayerSeet[i], m_SP.data(), m_SP.size(), 0);
					}

					++FightDiZhu;
					PlayerFightStatus[seat] = 1;
					if (3 == FightDiZhu)//���һ����ҽе�����ȷ��������
					{
						//������ݸ�ÿ���ͻ���
						PlayerStatus[(seat + 1) % 3] = _DDZ_SP_PLAYER_NONGMING;
						m_SP.clear();
						m_SP.add_int(_DDZ_SP_PLAYER_NONGMING);
						m_SP.add_int(-1);
						m_SP.add_int(DownCard[0]);
						m_SP.add_int(DownCard[1]);
						m_SP.add_int(DownCard[2]);
						SendData(m_CurPlayerSeet[(seat + 1) % 3], m_SP.data(), m_SP.size(), 0);

						PlayerStatus[(seat + 2) % 3] = _DDZ_SP_PLAYER_NONGMING;
						m_SP.clear();
						m_SP.add_int(_DDZ_SP_PLAYER_NONGMING);
						m_SP.add_int(1);
						m_SP.add_int(DownCard[0]);
						m_SP.add_int(DownCard[1]);
						m_SP.add_int(DownCard[2]);
						SendData(m_CurPlayerSeet[(seat + 2) % 3], m_SP.data(), m_SP.size(), 0);

						//������õ��ƻ�ó���Ȩ��
						PlayerStatus[seat] = _DDZ_SP_PLAYER_DIZHU;
						m_SP.clear();
						m_SP.add_int(_DDZ_SP_PLAYER_DIZHU);
						m_SP.add_int(DownCard[0]);
						m_SP.add_int(DownCard[1]);
						m_SP.add_int(DownCard[2]);
						SendData(m_CurPlayerSeet[seat], m_SP.data(), m_SP.size(), 0);

						//������������������
						if (0 == seat)
						{
							for (size_t i = 0; i < 3; i++)
								PlayerCard0.push_back(DownCard[i]);
							sort(PlayerCard0.begin(), PlayerCard0.end());
						}
						else if(1 == seat)
						{
							for (size_t i = 0; i < 3; i++)
								PlayerCard1.push_back(DownCard[i]);
							sort(PlayerCard1.begin(), PlayerCard1.end());
						}
						else
						{
							for (size_t i = 0; i < 3; i++)
								PlayerCard2.push_back(DownCard[i]);
							sort(PlayerCard2.begin(), PlayerCard2.end());
						}

						break;
					}
					int nextseat = (seat + 1) % 3;//��һ��λ��
					m_SP.clear();
					m_SP.add_int(_DDZ_JDZ);
					m_SP.add_char(FightDiZhu);
					SendData(m_CurPlayerSeet[nextseat], m_SP.data(), m_SP.size(), 0);
					break;
				}
				case _DDZ_BQ://����
				{
					//������Ч��Ϣ
					for (int i = 0;i < 3;++i)
					{
						m_SP.clear();
						m_SP.add_int(_DDZ_SOUND);
						m_SP.add_int(_DDZ_SOUND_BQ);
						SendData(m_CurPlayerSeet[i], m_SP.data(), m_SP.size(), 0);
					}

					++FightDiZhu;
					PlayerFightStatus[seat] = 2;
					//��Ҳ�����ǰһ����ҽе�����ȷ��������
					if (3 == FightDiZhu && PlayerFightStatus[(seat + 2) % 3] == 1)
					{
						//������ݸ�ÿ���ͻ���
						PlayerStatus[(seat + 1) % 3] = _DDZ_SP_PLAYER_NONGMING;
						m_SP.clear();
						m_SP.add_int(_DDZ_SP_PLAYER_NONGMING);
						m_SP.add_int(1);
						m_SP.add_int(DownCard[0]);
						m_SP.add_int(DownCard[1]);
						m_SP.add_int(DownCard[2]);
						SendData(m_CurPlayerSeet[(seat + 1) % 3], m_SP.data(), m_SP.size(), 0);

						PlayerStatus[seat] = _DDZ_SP_PLAYER_NONGMING;
						m_SP.clear();
						m_SP.add_int(_DDZ_SP_PLAYER_NONGMING);
						m_SP.add_int(-1);
						m_SP.add_int(DownCard[0]);
						m_SP.add_int(DownCard[1]);
						m_SP.add_int(DownCard[2]);
						SendData(m_CurPlayerSeet[seat], m_SP.data(), m_SP.size(), 0);

						//������õ��ƻ�ó���Ȩ��
						PlayerStatus[(seat + 2) % 3] = _DDZ_SP_PLAYER_DIZHU;
						m_SP.clear();
						m_SP.add_int(_DDZ_SP_PLAYER_DIZHU);
						m_SP.add_int(DownCard[0]);
						m_SP.add_int(DownCard[1]);
						m_SP.add_int(DownCard[2]);
						SendData(m_CurPlayerSeet[(seat + 2) % 3], m_SP.data(), m_SP.size(), 0);

						//������������������
						if (0 == seat)
						{
							for (size_t i = 0; i < 3; i++)
								PlayerCard2.push_back(DownCard[i]);
							sort(PlayerCard2.begin(), PlayerCard2.end());
						}
						else if (1 == seat)
						{
							for (size_t i = 0; i < 3; i++)
								PlayerCard0.push_back(DownCard[i]);
							sort(PlayerCard0.begin(), PlayerCard0.end());
						}
						else
						{
							for (size_t i = 0; i < 3; i++)
								PlayerCard1.push_back(DownCard[i]);
							sort(PlayerCard1.begin(), PlayerCard1.end());
						}

						break;
					}
					//��Ҳ�����ǰһ����Ҳ�����ȷ��������
					else if (3 == FightDiZhu && PlayerFightStatus[(seat + 2) % 3] == 2)
					{
						//������ݸ�ÿ���ͻ���
						PlayerStatus[(seat + 2) % 3] = _DDZ_SP_PLAYER_NONGMING;
						m_SP.clear();
						m_SP.add_int(_DDZ_SP_PLAYER_NONGMING);
						m_SP.add_int(-1);
						m_SP.add_int(DownCard[0]);
						m_SP.add_int(DownCard[1]);
						m_SP.add_int(DownCard[2]);
						SendData(m_CurPlayerSeet[(seat + 2) % 3], m_SP.data(), m_SP.size(), 0);

						PlayerStatus[seat] = _DDZ_SP_PLAYER_NONGMING;
						m_SP.clear();
						m_SP.add_int(_DDZ_SP_PLAYER_NONGMING);
						m_SP.add_int(1);
						m_SP.add_int(DownCard[0]);
						m_SP.add_int(DownCard[1]);
						m_SP.add_int(DownCard[2]);
						SendData(m_CurPlayerSeet[seat], m_SP.data(), m_SP.size(), 0);

						//������õ��ƻ�ó���Ȩ��
						PlayerStatus[(seat + 1) % 3] = _DDZ_SP_PLAYER_DIZHU;
						m_SP.clear();
						m_SP.add_int(_DDZ_SP_PLAYER_DIZHU);
						m_SP.add_int(DownCard[0]);
						m_SP.add_int(DownCard[1]);
						m_SP.add_int(DownCard[2]);
						SendData(m_CurPlayerSeet[(seat + 1) % 3], m_SP.data(), m_SP.size(), 0);

						//������������������
						if (0 == seat)
						{
							for (size_t i = 0; i < 3; i++)
								PlayerCard1.push_back(DownCard[i]);
							sort(PlayerCard1.begin(), PlayerCard1.end());
						}
						else if (1 == seat)
						{
							for (size_t i = 0; i < 3; i++)
								PlayerCard2.push_back(DownCard[i]);
							sort(PlayerCard2.begin(), PlayerCard2.end());
						}
						else
						{
							for (size_t i = 0; i < 3; i++)
								PlayerCard0.push_back(DownCard[i]);
							sort(PlayerCard0.begin(), PlayerCard0.end());
						}

						break;
					}
					//���һ����Ҳ�����ǰһ����Ҳ�����ȷ��������
					else if (4 == FightDiZhu && PlayerFightStatus[(seat + 2) % 3] == 2)
					{
						//������ݸ�ÿ���ͻ���
						PlayerStatus[(seat + 2) % 3] = _DDZ_SP_PLAYER_NONGMING;
						m_SP.clear();
						m_SP.add_int(_DDZ_SP_PLAYER_NONGMING);
						m_SP.add_int(-1);
						m_SP.add_int(DownCard[0]);
						m_SP.add_int(DownCard[1]);
						m_SP.add_int(DownCard[2]);
						SendData(m_CurPlayerSeet[(seat + 2) % 3], m_SP.data(), m_SP.size(), 0);

						PlayerStatus[seat] = _DDZ_SP_PLAYER_NONGMING;
						m_SP.clear();
						m_SP.add_int(_DDZ_SP_PLAYER_NONGMING);
						m_SP.add_int(1);
						m_SP.add_int(DownCard[0]);
						m_SP.add_int(DownCard[1]);
						m_SP.add_int(DownCard[2]);
						SendData(m_CurPlayerSeet[seat], m_SP.data(), m_SP.size(), 0);

						//������õ��ƻ�ó���Ȩ��
						PlayerStatus[(seat + 1) % 3] = _DDZ_SP_PLAYER_DIZHU;
						m_SP.clear();
						m_SP.add_int(_DDZ_SP_PLAYER_DIZHU);
						m_SP.add_int(DownCard[0]);
						m_SP.add_int(DownCard[1]);
						m_SP.add_int(DownCard[2]);
						SendData(m_CurPlayerSeet[(seat + 1) % 3], m_SP.data(), m_SP.size(), 0);

						//������������������
						if (0 == seat)
						{
							for (size_t i = 0; i < 3; i++)
								PlayerCard1.push_back(DownCard[i]);
							sort(PlayerCard1.begin(), PlayerCard1.end());
						}
						else if (1 == seat)
						{
							for (size_t i = 0; i < 3; i++)
								PlayerCard2.push_back(DownCard[i]);
							sort(PlayerCard2.begin(), PlayerCard2.end());
						}
						else
						{
							for (size_t i = 0; i < 3; i++)
								PlayerCard0.push_back(DownCard[i]);
							sort(PlayerCard0.begin(), PlayerCard0.end());
						}

						break;
					}
					//���һ����Ҳ�����ǰһ�������������ȷ��������
					else if (4 == FightDiZhu && PlayerFightStatus[(seat + 2) % 3] == 3)
					{
						//������ݸ�ÿ���ͻ���
						PlayerStatus[(seat + 1) % 3] = _DDZ_SP_PLAYER_NONGMING;
						m_SP.clear();
						m_SP.add_int(_DDZ_SP_PLAYER_NONGMING);
						m_SP.add_int(1);
						m_SP.add_int(DownCard[0]);
						m_SP.add_int(DownCard[1]);
						m_SP.add_int(DownCard[2]);
						SendData(m_CurPlayerSeet[(seat + 1) % 3], m_SP.data(), m_SP.size(), 0);

						PlayerStatus[seat] = _DDZ_SP_PLAYER_NONGMING;
						m_SP.clear();
						m_SP.add_int(_DDZ_SP_PLAYER_NONGMING);
						m_SP.add_int(-1);
						m_SP.add_int(DownCard[0]);
						m_SP.add_int(DownCard[1]);
						m_SP.add_int(DownCard[2]);
						SendData(m_CurPlayerSeet[seat], m_SP.data(), m_SP.size(), 0);

						//������õ��ƻ�ó���Ȩ��
						PlayerStatus[(seat + 2) % 3] = _DDZ_SP_PLAYER_DIZHU;
						m_SP.clear();
						m_SP.add_int(_DDZ_SP_PLAYER_DIZHU);
						m_SP.add_int(DownCard[0]);
						m_SP.add_int(DownCard[1]);
						m_SP.add_int(DownCard[2]);
						m_SP.add_int(LastPokerType);//ȷ����ʼ��������
						SendData(m_CurPlayerSeet[(seat + 2) % 3], m_SP.data(), m_SP.size(), 0);

						//������������������
						if (0 == seat)
						{
							for (size_t i = 0; i < 3; i++)
								PlayerCard2.push_back(DownCard[i]);
							sort(PlayerCard2.begin(), PlayerCard2.end());
						}
						else if (1 == seat)
						{
							for (size_t i = 0; i < 3; i++)
								PlayerCard0.push_back(DownCard[i]);
							sort(PlayerCard0.begin(), PlayerCard0.end());
						}
						else
						{
							for (size_t i = 0; i < 3; i++)
								PlayerCard1.push_back(DownCard[i]);
							sort(PlayerCard1.begin(), PlayerCard1.end());
						}

						break;
					}

					int nextseat = (seat + 1) % 3;//��һ��λ��
					m_SP.clear();
					m_SP.add_int(_DDZ_BQ);
					m_SP.add_char(FightDiZhu);
					SendData(m_CurPlayerSeet[nextseat], m_SP.data(), m_SP.size(), 0);
					break;
				}
				case _DDZ_QDZ://������
				{
					++QDZnum;
					//������Ч��Ϣ
					for (int i = 0;i < 3;++i)
					{
						m_SP.clear();
						m_SP.add_int(_DDZ_SOUND);
						m_SP.add_int(_DDZ_SOUND_QDZ);
						if (1 == FightDiZhu && 1 == QDZnum)
							m_SP.add_int(QDZnum);
						else if (2 == FightDiZhu && 1 == QDZnum)
							m_SP.add_int(QDZnum);
						else if (3 == FightDiZhu && (1 == QDZnum || 2 == QDZnum))
							m_SP.add_int(3);
						else
							m_SP.add_int(QDZnum);
						SendData(m_CurPlayerSeet[i], m_SP.data(), m_SP.size(), 0);
					}

					++FightDiZhu;
					PlayerFightStatus[seat] = 3;
					//��ǰ����������ҵ�һ����Ҳ���
					if (3 == FightDiZhu && PlayerStatus[(seat + 1) % 3] == 0)
					{
						int nextseat = (seat + 2) % 3;//��һ��λ��
						m_SP.clear();
						m_SP.add_int(_DDZ_QDZ);
						SendData(m_CurPlayerSeet[nextseat], m_SP.data(), m_SP.size(), 0);
						break;
					}
					//��ǰ����������ҵ�һ����ҽе���
					else if (3 == FightDiZhu && PlayerStatus[(seat + 1) % 3] == 1)
					{
						int nextseat = (seat + 1) % 3;//��һ��λ��
						m_SP.clear();
						m_SP.add_int(_DDZ_QDZ);
						SendData(m_CurPlayerSeet[nextseat], m_SP.data(), m_SP.size(), 0);
						break;
					}
					else if ((4 == FightDiZhu))//���һ�������������ȷ��������
					{
						//������ݸ�ÿ���ͻ���
						PlayerStatus[(seat + 1) % 3] = _DDZ_SP_PLAYER_NONGMING;
						m_SP.clear();
						m_SP.add_int(_DDZ_SP_PLAYER_NONGMING);
						m_SP.add_int(-1);
						m_SP.add_int(DownCard[0]);
						m_SP.add_int(DownCard[1]);
						m_SP.add_int(DownCard[2]);
						SendData(m_CurPlayerSeet[(seat + 1) % 3], m_SP.data(), m_SP.size(), 0);

						PlayerStatus[(seat + 2) % 3] = _DDZ_SP_PLAYER_NONGMING;
						m_SP.clear();
						m_SP.add_int(_DDZ_SP_PLAYER_NONGMING);
						m_SP.add_int(1);
						m_SP.add_int(DownCard[0]);
						m_SP.add_int(DownCard[1]);
						m_SP.add_int(DownCard[2]);
						SendData(m_CurPlayerSeet[(seat + 2) % 3], m_SP.data(), m_SP.size(), 0);

						//������õ��ƻ�ó���Ȩ��
						PlayerStatus[seat] = _DDZ_SP_PLAYER_DIZHU;
						m_SP.clear();
						m_SP.add_int(_DDZ_SP_PLAYER_DIZHU);
						m_SP.add_int(DownCard[0]);
						m_SP.add_int(DownCard[1]);
						m_SP.add_int(DownCard[2]);
						SendData(m_CurPlayerSeet[seat], m_SP.data(), m_SP.size(), 0);

						//������������������
						if (0 == seat)
						{
							for (size_t i = 0; i < 3; i++)
								PlayerCard0.push_back(DownCard[i]);
							sort(PlayerCard0.begin(), PlayerCard0.end());
						}
						else if (1 == seat)
						{
							for (size_t i = 0; i < 3; i++)
								PlayerCard1.push_back(DownCard[i]);
							sort(PlayerCard1.begin(), PlayerCard1.end());
						}
						else
						{
							for (size_t i = 0; i < 3; i++)
								PlayerCard2.push_back(DownCard[i]);
							sort(PlayerCard2.begin(), PlayerCard2.end());
						}

						break;
					}
					int nextseat = (seat + 1) % 3;//��һ��λ��
					m_SP.clear();
					m_SP.add_int(_DDZ_QDZ);
					SendData(m_CurPlayerSeet[nextseat], m_SP.data(), m_SP.size(), 0);
					break;
				}
			}
			break;
		}
		case _DDZ_PLAY://��Ϸ���н׶�(�ж����ͼ���С�Ƿ����)
		{
			int GameTypeType;
			m_RP.get_int(&GameTypeType);
			switch (GameTypeType)
			{
				case _DDZ_BC:
				{
					//���Ͳ�����Ч���������
					for (int i = 0;i < 3;++i)
					{
						m_SP.clear();
						m_SP.add_int(_DDZ_SOUND);
						m_SP.add_int(_DDZ_SOUND_BC);
						SendData(m_CurPlayerSeet[i], m_SP.data(), m_SP.size(), 0);
					}

					++BCnum;
					if (BCnum == 2)
					{
						BCnum = 0;
						LastPokerType = -1;
						m_SP.clear();
						m_SP.add_int(_DDZ_BC);
						//�ϼҲ���
						m_SP.add_int(-1);
						//���ͳ�������
						m_SP.add_int(LastPokerType);
						//���Ͳ�������
						m_SP.add_int(BCnum);
						SendData(m_CurPlayerSeet[(seat + 1) % 3], m_SP.data(), m_SP.size(), 0);

						m_SP.clear();
						m_SP.add_int(_DDZ_BC);
						//������
						m_SP.add_int(0);
						//���ͳ�������
						m_SP.add_int(LastPokerType);
						SendData(m_CurPlayerSeet[seat], m_SP.data(), m_SP.size(), 0);
					}
					else
					{
						m_SP.clear();
						m_SP.add_int(_DDZ_BC);
						//�ϼҲ���
						m_SP.add_int(-1);
						//���ͳ�������
						m_SP.add_int(LastPokerType);
						//���Ͳ�������
						m_SP.add_int(BCnum);
						SendData(m_CurPlayerSeet[(seat + 1) % 3], m_SP.data(), m_SP.size(), 0);

						m_SP.clear();
						m_SP.add_int(_DDZ_BC);
						//������
						m_SP.add_int(0);
						//���ͳ�������
						m_SP.add_int(LastPokerType);
						SendData(m_CurPlayerSeet[seat], m_SP.data(), m_SP.size(), 0);
					}
					
					break;
				}	
				case _DDZ_CP:
				{
					//���÷����
					BCnum = 0;

					//��ȡ��ǰ�����λ�ţ���������������
					CurPlayerseat = seat;
					//��������
					int CardCount;
					m_RP.get_int(&CardCount);

					//��������
					int CardType;
					m_RP.get_int(&CardType);
					LastPokerType = CardType;

					//��ճ�������
					CardIndex.clear();
					for (int i = 0; i < CardCount; ++i)
					{
						int cardtemp;
						m_RP.get_int(&cardtemp);
						CardIndex.push_back(cardtemp);
					}

					//�ж��Ƿ��ܳ�����������Ҽ�⣩
					bool CPOK = CPisOK();
					if (CPOK)
					{
						//���ͳ�����Ч��Ϣ
						OnPlayCP_Sound(LastPokerType, CardIndex[0]);

						m_SP.clear();
						m_SP.add_int(_DDZ_CPOK);
						//�������
						m_SP.add_int(0);
						//�����Ƶ�����
						m_SP.add_int(CardCount);
						//��������
						m_SP.add_int(CardType);
						//����������
						for (int i = 0;i < CardCount;++i)
							m_SP.add_int(CardIndex[i]);
						SendData(m_CurPlayerSeet[seat], m_SP.data(), m_SP.size(), 0);

						m_SP.clear();
						m_SP.add_int(_DDZ_CPOK);
						//�ϼҳ���
						m_SP.add_int(-1);
						//�����Ƶ�����
						m_SP.add_int(CardCount);
						//��������
						m_SP.add_int(CardType);
						//����������
						for (int i = 0;i < CardCount;++i)
							m_SP.add_int(CardIndex[i]);
						SendData(m_CurPlayerSeet[(seat + 1) % 3], m_SP.data(), m_SP.size(), 0);

						m_SP.clear();
						m_SP.add_int(_DDZ_CPOK);
						//�¼ҳ���
						m_SP.add_int(1);
						//�����Ƶ�����
						m_SP.add_int(CardCount);
						//��������
						m_SP.add_int(CardType);
						//����������
						for (int i = 0;i < CardCount;++i)
							m_SP.add_int(CardIndex[i]);
						SendData(m_CurPlayerSeet[(seat + 2) % 3], m_SP.data(), m_SP.size(), 0);
					}
					else
					{		
						//���������û�з��͹������ƣ�����ʹ���˷Ƿ�����
					}
					break;
				}		
			}
			break;
		}
		case _DDZ_FINISH://ʤ���ж�
		{
			//����ʤ����Ϣ���������
			m_SP.clear();
			m_SP.add_int(_DDZ_FINISH);
			m_SP.add_int(_DDZ_WIN);
			SendData(m_CurPlayerSeet[seat], m_SP.data(), m_SP.size(), 0);

			//��ǰ����ǵ���
			if (_DDZ_SP_PLAYER_DIZHU == PlayerStatus[seat])
			{
				m_SP.clear();
				m_SP.add_int(_DDZ_FINISH);
				m_SP.add_int(_DDZ_LOSE);
				SendData(m_CurPlayerSeet[(seat + 1) % 3], m_SP.data(), m_SP.size(), 0);

				m_SP.clear();
				m_SP.add_int(_DDZ_FINISH);
				m_SP.add_int(_DDZ_LOSE);
				SendData(m_CurPlayerSeet[(seat + 2) % 3], m_SP.data(), m_SP.size(), 0);
			}
			else
			{
				if (_DDZ_SP_PLAYER_DIZHU == PlayerStatus[(seat + 1) % 3])
				{
					m_SP.clear();
					m_SP.add_int(_DDZ_FINISH);
					m_SP.add_int(_DDZ_LOSE);
					SendData(m_CurPlayerSeet[(seat + 1) % 3], m_SP.data(), m_SP.size(), 0);

					m_SP.clear();
					m_SP.add_int(_DDZ_FINISH);
					m_SP.add_int(_DDZ_WIN);
					SendData(m_CurPlayerSeet[(seat + 2) % 3], m_SP.data(), m_SP.size(), 0);
				}
				else
				{
					m_SP.clear();
					m_SP.add_int(_DDZ_FINISH);
					m_SP.add_int(_DDZ_WIN);
					SendData(m_CurPlayerSeet[(seat + 1) % 3], m_SP.data(), m_SP.size(), 0);

					m_SP.clear();
					m_SP.add_int(_DDZ_FINISH);
					m_SP.add_int(_DDZ_LOSE);
					SendData(m_CurPlayerSeet[(seat + 2) % 3], m_SP.data(), m_SP.size(), 0);
				}
			}
			break;
		}
	}
}

void DouDiZhuServer::OnUpdate()
{
	if (m_Time > -1)
	{
		if (time(0) - m_Time > 3)
			End();
	}
}

void DouDiZhuServer::OnEnd(int type, int seat)
{}

void DouDiZhuServer::SetPlayerPoker()
{
	//����ÿ��17��
	for (size_t i = 0; i < 17;++i)
	{
		int CardIndex = rand() % AllCard.size();
		PlayerCard0[i] = AllCard[CardIndex];
		AllCard.erase(AllCard.begin() + CardIndex);

		CardIndex = rand() % AllCard.size();
		PlayerCard1[i] = AllCard[CardIndex];
		AllCard.erase(AllCard.begin() + CardIndex);

		CardIndex = rand() % AllCard.size();
		PlayerCard2[i] = AllCard[CardIndex];
		AllCard.erase(AllCard.begin() + CardIndex);
	}

	//���Ƹ�ֵ
	DownCard[0] = AllCard[0];
	DownCard[1] = AllCard[1];
	DownCard[2] = AllCard[2];

	//���ź���(С����)
	sort(PlayerCard0.begin(), PlayerCard0.end());
	sort(PlayerCard1.begin(), PlayerCard1.end());
	sort(PlayerCard2.begin(), PlayerCard2.end());
}

//�жϳ���
bool DouDiZhuServer::CPisOK()
{
	int CPnum = CardIndex.size();
	//�ж�����������Ƿ������������
	if (0 == CurPlayerseat)
	{
		for (int j = 0; j < CPnum; ++j)
		{
			for (size_t i = 0; i < PlayerCard0.size(); ++i)
			{
				if (CardIndex[j] == PlayerCard0[i])
					break;

				if (i == PlayerCard0.size()-1)
					return false;
			}
		}
		return true;
	}
	else if (1 == CurPlayerseat)
	{
		for (int j = 0; j < CPnum; ++j)
		{
			for (size_t i = 0; i < PlayerCard1.size(); ++i)
			{
				if (CardIndex[j] == PlayerCard1[i])
					break;

				if (i == PlayerCard1.size() - 1)
					return false;
			}
		}
		return true;
	}
	else
	{
		for (int j = 0; j < CPnum; ++j)
		{
			for (size_t i = 0; i < PlayerCard2.size(); ++i)
			{
				if (CardIndex[j] == PlayerCard2[i])
					break;

				if (i == PlayerCard2.size() - 1)
					return false;
			}
		}
		return true;
	}
}

void DouDiZhuServer::OnPlayCP_Sound(int type,int index)
{
	for (int i = 0;i < 3;++i)
	{
		m_SP.clear();
		m_SP.add_int(_DDZ_SOUND);
		//��ը
		if (_DDZ_SP_POKER_TYPE_TWOKING == type)		
			m_SP.add_int(_DDZ_SOUND_TWOKING);
		//��ͨը��
		else if (_DDZ_SP_POKER_TYPE_FORE == type)
			m_SP.add_int(_DDZ_SOUND_FORE);
		//�ɻ�
		else if (_DDZ_SP_POKER_TYPE_PLANE == type)
			m_SP.add_int(_DDZ_SOUND_PLANE);
		//����
		else if (_DDZ_SP_POKER_TYPE_MORETHREEDOUBLE == type)
			m_SP.add_int(_DDZ_SOUND_MORETHREEDOUBLE);
		//˳��
		else if (_DDZ_SP_POKER_TYPE_MOREFIVE == type)
			m_SP.add_int(_DDZ_SOUND_MOREFIVE);
		//�Ĵ�����
		else if (_DDZ_SP_POKER_TYPE_FOREANDTWODOUBLE == type)
			m_SP.add_int(_DDZ_SOUND_FOREANDTWODOUBLE);
		//�Ĵ���
		else if (_DDZ_SP_POKER_TYPE_FOREANDTWOSINGLE == type)
			m_SP.add_int(_DDZ_SOUND_FOREANDTWOSINGLE);
		//����һ��
		else if (_DDZ_SP_POKER_TYPE_THREEANDDOUBLE == type)
			m_SP.add_int(_DDZ_SOUND_THREEANDDOUBLE);
		//����һ
		else if (_DDZ_SP_POKER_TYPE_THREEANDONE == type)
			m_SP.add_int(_DDZ_SOUND_THREEANDONE);
		//����
		else if (_DDZ_SP_POKER_TYPE_THREE == type)
		{
			m_SP.add_int(_DDZ_SOUND_THREE);
			m_SP.add_int(index / 4);
		}
		//����
		else if (_DDZ_SP_POKER_TYPE_DOUBLE == type)
		{
			m_SP.add_int(_DDZ_SOUND_DOUBLE);
			m_SP.add_int(index / 4);
		}
		//����
		else if (_DDZ_SP_POKER_TYPE_SINGLE == type)
		{
			if (index == 53)//����
			{
				m_SP.add_int(_DDZ_SOUND_SINGLE);
				m_SP.add_int(14);
			}
			else
			{
				m_SP.add_int(_DDZ_SOUND_SINGLE);
				m_SP.add_int(index / 4);
			}
		}
		SendData(m_CurPlayerSeet[i], m_SP.data(), m_SP.size(), 0);
	}
}