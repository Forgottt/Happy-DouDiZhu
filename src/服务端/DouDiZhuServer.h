#ifndef _DOUDIZHU_SERVER_H_
#define _DOUDIZHU_SERVER_H_
#include<algorithm>
#include "GameServer.h"
#include <vector>
using namespace std;

#define _DDZ_FP 0//发牌
#define _DDZ_BC 1//不出
#define _DDZ_CP 2//出牌
#define _DDZ_CPOK 3//出牌OK
#define _DDZ_CPFAIL 4//出牌Fail //可以确定玩家使用了非法程序
#define _DDZ_FIGHTSTART 996//争夺地主
#define _DDZ_FIGHT 997//争夺
#define _DDZ_PLAY 998//游戏阶段
#define _DDZ_FINISH 999//游戏结束
#define _DDZ_BJ 1000//不叫
#define _DDZ_JDZ 1001//叫地主
#define _DDZ_BQ 1002//不抢
#define _DDZ_QDZ 1003//抢地主

#define _DDZ_SOUND 2000//音效
#define _DDZ_SOUND_BJ 2001//音效 不叫
#define _DDZ_SOUND_JDZ 2002//音效 叫地主
#define _DDZ_SOUND_BQ 2003//音效 不抢
#define _DDZ_SOUND_QDZ 2004//音效 抢地主
#define _DDZ_SOUND_TWOKING 2005//音效 王炸
#define _DDZ_SOUND_FORE 2006//音效 普通炸弹
#define _DDZ_SOUND_PLANE 2007//音效 飞机
#define _DDZ_SOUND_MORETHREEDOUBLE 2008//音效 连对
#define _DDZ_SOUND_MOREFIVE 2009//音效 顺子
#define _DDZ_SOUND_FOREANDTWODOUBLE 2010//音效 四带两对
#define _DDZ_SOUND_FOREANDTWOSINGLE 2011//音效 四带二
#define _DDZ_SOUND_THREEANDDOUBLE 2012//音效 三带一对
#define _DDZ_SOUND_THREEANDONE 2013//音效 三带一
#define _DDZ_SOUND_THREE 2014//音效 三张
#define _DDZ_SOUND_DOUBLE 2015//音效 对子
#define _DDZ_SOUND_SINGLE 2016//音效 单张
#define _DDZ_SOUND_BC 2017//音效 不出

#define _DDZ_WIN 200//胜利
#define _DDZ_LOSE 201//失败

class DouDiZhuServer : public GameServer
{
	int LastPokerType;//出牌的类型
	int m_MaxPlayer;
	int m_CurPlayerSeet[3];//座位
	int PlayerStatus[3];//玩家身份
	int PlayerFightStatus[3];//玩家Fight身份  0 不叫 1 叫地主  2 不抢 3抢地主
	int m_Time;
	char FightDiZhu;
	int QDZnum;
	int DownCard[3];//底牌
	int BCnum;//不出次数
	int CurPlayerseat;//当前玩家座位
	vector<int> CardIndex;//出牌索引
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