#include "DouDiZhuServer.h"
#include <time.h>

//游戏协议
#define _DDZ_CP_PLAYER_OK 100

//玩家身份验证 102 地主 101 农民 103 无身份(身份用于判断胜负)
#define _DDZ_SP_PLAYER_NONGMING 101 
#define _DDZ_SP_PLAYER_DIZHU 102 
#define _DDZ_SP_PLAYER_NONE 103

//二级级协议
#define _DDZ_SP_POKER_TYPE_SINGLE 1 //单张
#define _DDZ_SP_POKER_TYPE_DOUBLE 2 //对子
#define _DDZ_SP_POKER_TYPE_THREE 3 //三张
#define _DDZ_SP_POKER_TYPE_THREEANDONE 31 //三带一
#define _DDZ_SP_POKER_TYPE_THREEANDDOUBLE 32 //三带一对
#define _DDZ_SP_POKER_TYPE_FOREANDTWOSINGLE 41 //四带二
#define _DDZ_SP_POKER_TYPE_FOREANDTWODOUBLE 42 //四带两对
#define _DDZ_SP_POKER_TYPE_FORE 4 //炸弹
#define _DDZ_SP_POKER_TYPE_MOREFIVE 5 //顺子
#define _DDZ_SP_POKER_TYPE_MORETHREEDOUBLE 6 //连对
#define _DDZ_SP_POKER_TYPE_PLANE 7 //飞机
#define _DDZ_SP_POKER_TYPE_TWOKING 8 //王炸


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
	//初始化上一次出牌类型 (上一次未出牌-1)
	LastPokerType = -1;
	BCnum = 0;//不出次数

	//初始化所有玩家座位号（0-2）
	//初始化玩家身份（无身份）
	for (int i = 0;i < 3;++i)
	{
		m_CurPlayerSeet[i] = i;
		PlayerStatus[i] = _DDZ_SP_PLAYER_NONE;
		PlayerFightStatus[i] = -1;
		//初始化底牌
		DownCard[i] = -1;
	}

	//初始化争夺地主次数
	FightDiZhu = 0;

	//初始化抢地主次数
	QDZnum = 0;

	//初始化牌库(0-54)
	for (int i = 0;i < 54;++i)
		AllCard.push_back(i);

	//清空出牌索引
	CardIndex.clear();

	//初始化玩家拥有手牌(全部为空,标志-1)
	for (int i = 0;i < 20;++i)
	{
		PlayerCard0.push_back(-1);
		PlayerCard1.push_back(-1);
		PlayerCard2.push_back(-1);
	}

	//发牌(玩家现在拥有了自己的手牌ID)
	SetPlayerPoker();

	//传送牌的数据给客户端玩家
	//玩家1
	m_SP.clear();
	m_SP.add_int(_DDZ_FP);
	for (size_t i = 0; i < PlayerCard0.size(); ++i)
		m_SP.add_int(PlayerCard0[i]);
	SendData(m_CurPlayerSeet[0], m_SP.data(), m_SP.size(), 0);
	//玩家2
	m_SP.clear();
	m_SP.add_int(_DDZ_FP);
	for (size_t i = 0; i < PlayerCard0.size(); ++i)
		m_SP.add_int(PlayerCard1[i]);
	SendData(m_CurPlayerSeet[1], m_SP.data(), m_SP.size(), 0);
	//玩家3
	m_SP.clear();
	m_SP.add_int(_DDZ_FP);
	for (size_t i = 0; i < PlayerCard0.size(); ++i)
		m_SP.add_int(PlayerCard2[i]);
	SendData(m_CurPlayerSeet[2], m_SP.data(), m_SP.size(), 0);

	//发送争夺地主消息（随机玩家）
	m_SP.clear();
	m_SP.add_int(_DDZ_FIGHTSTART);
	SendData(m_CurPlayerSeet[rand() % 3], m_SP.data(), m_SP.size(), 0);

	//初始化时间
	m_Time = -1;
}

void DouDiZhuServer::OnPlayerSendData(int seat, //seat表示当前玩家座位
	void* data, int size)
{
	//准备反序列化数据
	m_RP.set_data(data, size);

	//得到消息
	//获得当前游戏类型
	int GameType;
	m_RP.get_int(&GameType);


	//获得地主阶段（服务器记录玩家身份）
	switch (GameType)
	{
		//具体类型具体分析
		case _DDZ_FIGHT://争夺阶段
		{
			int GameTypeType;
			m_RP.get_int(&GameTypeType);
			switch (GameTypeType)
			{
				case _DDZ_BJ://不叫
				{
					//发送音效消息
					for (int i = 0;i < 3;++i)
					{
						m_SP.clear();
						m_SP.add_int(_DDZ_SOUND);
						m_SP.add_int(_DDZ_SOUND_BJ);
						SendData(m_CurPlayerSeet[i], m_SP.data(), m_SP.size(), 0);
					}

					++FightDiZhu;
					PlayerFightStatus[seat] = 0;
					if (3 == FightDiZhu)//都没叫地主,随机分配一个玩家为地主
					{
						int dz = rand() % 3;

						//发送身份给每个客户端
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

						//地主获得底牌获得出牌权力
						PlayerStatus[dz] = _DDZ_SP_PLAYER_DIZHU;
						m_SP.clear();
						m_SP.add_int(_DDZ_SP_PLAYER_DIZHU);
						m_SP.add_int(DownCard[0]);
						m_SP.add_int(DownCard[1]);
						m_SP.add_int(DownCard[2]);
						SendData(m_CurPlayerSeet[dz], m_SP.data(), m_SP.size(), 0);

						//将底牌添加至玩家牌组
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
						int nextseat = (seat + 1) % 3;//下一个位置
						m_SP.clear();
						m_SP.add_int(_DDZ_BJ);
						m_SP.add_char(FightDiZhu);
						SendData(m_CurPlayerSeet[nextseat], m_SP.data(), m_SP.size(), 0);
					}
					break;
				}
				case _DDZ_JDZ://叫地主
				{
					//发送音效消息
					for (int i = 0;i < 3;++i)
					{
						m_SP.clear();
						m_SP.add_int(_DDZ_SOUND);
						m_SP.add_int(_DDZ_SOUND_JDZ);
						SendData(m_CurPlayerSeet[i], m_SP.data(), m_SP.size(), 0);
					}

					++FightDiZhu;
					PlayerFightStatus[seat] = 1;
					if (3 == FightDiZhu)//最后一个玩家叫地主，确定玩家身份
					{
						//发送身份给每个客户端
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

						//地主获得底牌获得出牌权力
						PlayerStatus[seat] = _DDZ_SP_PLAYER_DIZHU;
						m_SP.clear();
						m_SP.add_int(_DDZ_SP_PLAYER_DIZHU);
						m_SP.add_int(DownCard[0]);
						m_SP.add_int(DownCard[1]);
						m_SP.add_int(DownCard[2]);
						SendData(m_CurPlayerSeet[seat], m_SP.data(), m_SP.size(), 0);

						//将底牌添加至玩家牌组
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
					int nextseat = (seat + 1) % 3;//下一个位置
					m_SP.clear();
					m_SP.add_int(_DDZ_JDZ);
					m_SP.add_char(FightDiZhu);
					SendData(m_CurPlayerSeet[nextseat], m_SP.data(), m_SP.size(), 0);
					break;
				}
				case _DDZ_BQ://不抢
				{
					//发送音效消息
					for (int i = 0;i < 3;++i)
					{
						m_SP.clear();
						m_SP.add_int(_DDZ_SOUND);
						m_SP.add_int(_DDZ_SOUND_BQ);
						SendData(m_CurPlayerSeet[i], m_SP.data(), m_SP.size(), 0);
					}

					++FightDiZhu;
					PlayerFightStatus[seat] = 2;
					//玩家不抢且前一个玩家叫地主，确定玩家身份
					if (3 == FightDiZhu && PlayerFightStatus[(seat + 2) % 3] == 1)
					{
						//发送身份给每个客户端
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

						//地主获得底牌获得出牌权力
						PlayerStatus[(seat + 2) % 3] = _DDZ_SP_PLAYER_DIZHU;
						m_SP.clear();
						m_SP.add_int(_DDZ_SP_PLAYER_DIZHU);
						m_SP.add_int(DownCard[0]);
						m_SP.add_int(DownCard[1]);
						m_SP.add_int(DownCard[2]);
						SendData(m_CurPlayerSeet[(seat + 2) % 3], m_SP.data(), m_SP.size(), 0);

						//将底牌添加至玩家牌组
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
					//玩家不抢且前一个玩家不抢，确定玩家身份
					else if (3 == FightDiZhu && PlayerFightStatus[(seat + 2) % 3] == 2)
					{
						//发送身份给每个客户端
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

						//地主获得底牌获得出牌权力
						PlayerStatus[(seat + 1) % 3] = _DDZ_SP_PLAYER_DIZHU;
						m_SP.clear();
						m_SP.add_int(_DDZ_SP_PLAYER_DIZHU);
						m_SP.add_int(DownCard[0]);
						m_SP.add_int(DownCard[1]);
						m_SP.add_int(DownCard[2]);
						SendData(m_CurPlayerSeet[(seat + 1) % 3], m_SP.data(), m_SP.size(), 0);

						//将底牌添加至玩家牌组
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
					//最后一个玩家不抢且前一个玩家不抢，确定玩家身份
					else if (4 == FightDiZhu && PlayerFightStatus[(seat + 2) % 3] == 2)
					{
						//发送身份给每个客户端
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

						//地主获得底牌获得出牌权力
						PlayerStatus[(seat + 1) % 3] = _DDZ_SP_PLAYER_DIZHU;
						m_SP.clear();
						m_SP.add_int(_DDZ_SP_PLAYER_DIZHU);
						m_SP.add_int(DownCard[0]);
						m_SP.add_int(DownCard[1]);
						m_SP.add_int(DownCard[2]);
						SendData(m_CurPlayerSeet[(seat + 1) % 3], m_SP.data(), m_SP.size(), 0);

						//将底牌添加至玩家牌组
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
					//最后一个玩家不抢且前一个玩家抢地主，确定玩家身份
					else if (4 == FightDiZhu && PlayerFightStatus[(seat + 2) % 3] == 3)
					{
						//发送身份给每个客户端
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

						//地主获得底牌获得出牌权力
						PlayerStatus[(seat + 2) % 3] = _DDZ_SP_PLAYER_DIZHU;
						m_SP.clear();
						m_SP.add_int(_DDZ_SP_PLAYER_DIZHU);
						m_SP.add_int(DownCard[0]);
						m_SP.add_int(DownCard[1]);
						m_SP.add_int(DownCard[2]);
						m_SP.add_int(LastPokerType);//确定起始出牌类型
						SendData(m_CurPlayerSeet[(seat + 2) % 3], m_SP.data(), m_SP.size(), 0);

						//将底牌添加至玩家牌组
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

					int nextseat = (seat + 1) % 3;//下一个位置
					m_SP.clear();
					m_SP.add_int(_DDZ_BQ);
					m_SP.add_char(FightDiZhu);
					SendData(m_CurPlayerSeet[nextseat], m_SP.data(), m_SP.size(), 0);
					break;
				}
				case _DDZ_QDZ://抢地主
				{
					++QDZnum;
					//发送音效消息
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
					//当前玩家抢地主且第一个玩家不叫
					if (3 == FightDiZhu && PlayerStatus[(seat + 1) % 3] == 0)
					{
						int nextseat = (seat + 2) % 3;//下一个位置
						m_SP.clear();
						m_SP.add_int(_DDZ_QDZ);
						SendData(m_CurPlayerSeet[nextseat], m_SP.data(), m_SP.size(), 0);
						break;
					}
					//当前玩家抢地主且第一个玩家叫地主
					else if (3 == FightDiZhu && PlayerStatus[(seat + 1) % 3] == 1)
					{
						int nextseat = (seat + 1) % 3;//下一个位置
						m_SP.clear();
						m_SP.add_int(_DDZ_QDZ);
						SendData(m_CurPlayerSeet[nextseat], m_SP.data(), m_SP.size(), 0);
						break;
					}
					else if ((4 == FightDiZhu))//最后一个玩家抢地主，确定玩家身份
					{
						//发送身份给每个客户端
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

						//地主获得底牌获得出牌权力
						PlayerStatus[seat] = _DDZ_SP_PLAYER_DIZHU;
						m_SP.clear();
						m_SP.add_int(_DDZ_SP_PLAYER_DIZHU);
						m_SP.add_int(DownCard[0]);
						m_SP.add_int(DownCard[1]);
						m_SP.add_int(DownCard[2]);
						SendData(m_CurPlayerSeet[seat], m_SP.data(), m_SP.size(), 0);

						//将底牌添加至玩家牌组
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
					int nextseat = (seat + 1) % 3;//下一个位置
					m_SP.clear();
					m_SP.add_int(_DDZ_QDZ);
					SendData(m_CurPlayerSeet[nextseat], m_SP.data(), m_SP.size(), 0);
					break;
				}
			}
			break;
		}
		case _DDZ_PLAY://游戏进行阶段(判断牌型及大小是否合理)
		{
			int GameTypeType;
			m_RP.get_int(&GameTypeType);
			switch (GameTypeType)
			{
				case _DDZ_BC:
				{
					//发送不出音效给所有玩家
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
						//上家不出
						m_SP.add_int(-1);
						//发送出牌类型
						m_SP.add_int(LastPokerType);
						//发送不出次数
						m_SP.add_int(BCnum);
						SendData(m_CurPlayerSeet[(seat + 1) % 3], m_SP.data(), m_SP.size(), 0);

						m_SP.clear();
						m_SP.add_int(_DDZ_BC);
						//自身不出
						m_SP.add_int(0);
						//发送出牌类型
						m_SP.add_int(LastPokerType);
						SendData(m_CurPlayerSeet[seat], m_SP.data(), m_SP.size(), 0);
					}
					else
					{
						m_SP.clear();
						m_SP.add_int(_DDZ_BC);
						//上家不出
						m_SP.add_int(-1);
						//发送出牌类型
						m_SP.add_int(LastPokerType);
						//发送不出次数
						m_SP.add_int(BCnum);
						SendData(m_CurPlayerSeet[(seat + 1) % 3], m_SP.data(), m_SP.size(), 0);

						m_SP.clear();
						m_SP.add_int(_DDZ_BC);
						//自身不出
						m_SP.add_int(0);
						//发送出牌类型
						m_SP.add_int(LastPokerType);
						SendData(m_CurPlayerSeet[seat], m_SP.data(), m_SP.size(), 0);
					}
					
					break;
				}	
				case _DDZ_CP:
				{
					//重置服务端
					BCnum = 0;

					//获取当前玩家座位号（用于牌组索引）
					CurPlayerseat = seat;
					//出牌数量
					int CardCount;
					m_RP.get_int(&CardCount);

					//出牌类型
					int CardType;
					m_RP.get_int(&CardType);
					LastPokerType = CardType;

					//清空出牌索引
					CardIndex.clear();
					for (int i = 0; i < CardCount; ++i)
					{
						int cardtemp;
						m_RP.get_int(&cardtemp);
						CardIndex.push_back(cardtemp);
					}

					//判断是否能出（可用于外挂检测）
					bool CPOK = CPisOK();
					if (CPOK)
					{
						//发送出牌音效消息
						OnPlayCP_Sound(LastPokerType, CardIndex[0]);

						m_SP.clear();
						m_SP.add_int(_DDZ_CPOK);
						//自身出牌
						m_SP.add_int(0);
						//发送牌的数量
						m_SP.add_int(CardCount);
						//出牌类型
						m_SP.add_int(CardType);
						//发送牌索引
						for (int i = 0;i < CardCount;++i)
							m_SP.add_int(CardIndex[i]);
						SendData(m_CurPlayerSeet[seat], m_SP.data(), m_SP.size(), 0);

						m_SP.clear();
						m_SP.add_int(_DDZ_CPOK);
						//上家出牌
						m_SP.add_int(-1);
						//发送牌的数量
						m_SP.add_int(CardCount);
						//出牌类型
						m_SP.add_int(CardType);
						//发送牌索引
						for (int i = 0;i < CardCount;++i)
							m_SP.add_int(CardIndex[i]);
						SendData(m_CurPlayerSeet[(seat + 1) % 3], m_SP.data(), m_SP.size(), 0);

						m_SP.clear();
						m_SP.add_int(_DDZ_CPOK);
						//下家出牌
						m_SP.add_int(1);
						//发送牌的数量
						m_SP.add_int(CardCount);
						//出牌类型
						m_SP.add_int(CardType);
						//发送牌索引
						for (int i = 0;i < CardCount;++i)
							m_SP.add_int(CardIndex[i]);
						SendData(m_CurPlayerSeet[(seat + 2) % 3], m_SP.data(), m_SP.size(), 0);
					}
					else
					{		
						//玩家牌组中没有发送过来的牌（可能使用了非法程序）
					}
					break;
				}		
			}
			break;
		}
		case _DDZ_FINISH://胜负判断
		{
			//发送胜利消息给所有玩家
			m_SP.clear();
			m_SP.add_int(_DDZ_FINISH);
			m_SP.add_int(_DDZ_WIN);
			SendData(m_CurPlayerSeet[seat], m_SP.data(), m_SP.size(), 0);

			//当前玩家是地主
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
	//发牌每人17张
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

	//底牌赋值
	DownCard[0] = AllCard[0];
	DownCard[1] = AllCard[1];
	DownCard[2] = AllCard[2];

	//牌排好序(小到大)
	sort(PlayerCard0.begin(), PlayerCard0.end());
	sort(PlayerCard1.begin(), PlayerCard1.end());
	sort(PlayerCard2.begin(), PlayerCard2.end());
}

//判断出牌
bool DouDiZhuServer::CPisOK()
{
	int CPnum = CardIndex.size();
	//判断玩家牌组中是否存在所出的牌
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
		//王炸
		if (_DDZ_SP_POKER_TYPE_TWOKING == type)		
			m_SP.add_int(_DDZ_SOUND_TWOKING);
		//普通炸弹
		else if (_DDZ_SP_POKER_TYPE_FORE == type)
			m_SP.add_int(_DDZ_SOUND_FORE);
		//飞机
		else if (_DDZ_SP_POKER_TYPE_PLANE == type)
			m_SP.add_int(_DDZ_SOUND_PLANE);
		//连对
		else if (_DDZ_SP_POKER_TYPE_MORETHREEDOUBLE == type)
			m_SP.add_int(_DDZ_SOUND_MORETHREEDOUBLE);
		//顺子
		else if (_DDZ_SP_POKER_TYPE_MOREFIVE == type)
			m_SP.add_int(_DDZ_SOUND_MOREFIVE);
		//四带两对
		else if (_DDZ_SP_POKER_TYPE_FOREANDTWODOUBLE == type)
			m_SP.add_int(_DDZ_SOUND_FOREANDTWODOUBLE);
		//四带二
		else if (_DDZ_SP_POKER_TYPE_FOREANDTWOSINGLE == type)
			m_SP.add_int(_DDZ_SOUND_FOREANDTWOSINGLE);
		//三带一对
		else if (_DDZ_SP_POKER_TYPE_THREEANDDOUBLE == type)
			m_SP.add_int(_DDZ_SOUND_THREEANDDOUBLE);
		//三带一
		else if (_DDZ_SP_POKER_TYPE_THREEANDONE == type)
			m_SP.add_int(_DDZ_SOUND_THREEANDONE);
		//三张
		else if (_DDZ_SP_POKER_TYPE_THREE == type)
		{
			m_SP.add_int(_DDZ_SOUND_THREE);
			m_SP.add_int(index / 4);
		}
		//对子
		else if (_DDZ_SP_POKER_TYPE_DOUBLE == type)
		{
			m_SP.add_int(_DDZ_SOUND_DOUBLE);
			m_SP.add_int(index / 4);
		}
		//单张
		else if (_DDZ_SP_POKER_TYPE_SINGLE == type)
		{
			if (index == 53)//大王
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