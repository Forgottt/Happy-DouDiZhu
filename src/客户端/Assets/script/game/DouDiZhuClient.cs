using UnityEngine;
using System.Collections;
using System.Collections.Generic;

public class DouDiZhuClient : GameClient
{
    public const int _DDZ_FP = 0;//发牌
	public const int _DDZ_BC = 1;//不牌
	public const int _DDZ_CP = 2;//出牌
	public const int _DDZ_CPOK = 3;//出牌OK
	public const int _DDZ_CPFAIL = 4;//出牌Fail

	public const int _DDZ_FIGHTSTART = 996;//争夺地主开始
	public const int _DDZ_FIGHT = 997;//争夺
	public const int _DDZ_PLAY = 998;//游戏阶段
	public const int _DDZ_FINISH = 999;//游戏结束

	public const int _DDZ_BJ =1000;//不叫
	public const int _DDZ_JDZ =1001;//叫地主
	public const int _DDZ_BQ= 1002;//不抢
	public const int _DDZ_QDZ =1003;//抢地主
	
	public const int _DDZ_WIN = 200;//胜利
	public const int _DDZ_LOSE = 201;//失败

	//玩家身份验证 102 地主 101 农民 103 无身份
	public const int _DDZ_SP_PLAYER_NONGMING = 101;
	public const int _DDZ_SP_PLAYER_DIZHU = 102;
	public const int _DDZ_SP_PLAYER_NONE = 103;

	//牌型
	public const int _DDZ_SP_POKER_TYPE_SINGLE = 1; //单张
	public const int _DDZ_SP_POKER_TYPE_DOUBLE = 2; //对子
	public const int _DDZ_SP_POKER_TYPE_THREE =3; //三张
	public const int _DDZ_SP_POKER_TYPE_THREEANDONE =31; //三带一
	public const int _DDZ_SP_POKER_TYPE_THREEANDDOUBLE =32; //三带一对
	public const int _DDZ_SP_POKER_TYPE_FOREANDTWOSINGLE =41; //四带二
	public const int _DDZ_SP_POKER_TYPE_FOREANDTWODOUBLE =42; //四带两对
	public const int _DDZ_SP_POKER_TYPE_FORE =4; //炸弹
	public const int _DDZ_SP_POKER_TYPE_MOREFIVE =5; //顺子
	public const int _DDZ_SP_POKER_TYPE_MORETHREEDOUBLE =6; //连对
	public const int _DDZ_SP_POKER_TYPE_PLANE =7; //飞机
	public const int _DDZ_SP_POKER_TYPE_TWOKING =8; //王炸
	
	public const int _DDZ_SOUND =2000; //音效
	enum DDZGameSound:int//全部音效定义
	{
		_DDZ_SOUND_BJ = 2001,
		_DDZ_SOUND_JDZ= 2002,
		_DDZ_SOUND_BQ =2003,
		_DDZ_SOUND_QDZ =2004,
        _DDZ_SOUND_TWOKING = 2005,//音效 王炸
        _DDZ_SOUND_FORE =2006,//音效 普通炸弹
        _DDZ_SOUND_PLANE =2007,//音效 飞机
        _DDZ_SOUND_MORETHREEDOUBLE =2008,//音效 连对
        _DDZ_SOUND_MOREFIVE= 2009,//音效 顺子
        _DDZ_SOUND_FOREANDTWODOUBLE =2010,//音效 四带两对
        _DDZ_SOUND_FOREANDTWOSINGLE =2011,//音效 四带二
        _DDZ_SOUND_THREEANDDOUBLE =2012,//音效 三带一对
        _DDZ_SOUND_THREEANDONE =2013,//音效 三带一
        _DDZ_SOUND_THREE =2014,//音效 三张
        _DDZ_SOUND_DOUBLE =2015,//音效 对子
        _DDZ_SOUND_SINGLE= 2016,//音效 单张
        _DDZ_SOUND_BC = 2017//音效 不出
    };

	//玩家牌数
	int MyCardNum = -1,LeftCardNum = -1,RightCardNum = -1;
	public List<int> PlayerPokerCards;//玩家牌的索引
	public List<GameObject> PlayerPokerCardsInstant;//玩家牌库实例化表
	int CurGameType;//当前游戏类型
	char CurFightNum;//播放当前音效
	GameObject[] OtherCardBack;//背面
	GameObject[] BackCard;//底牌背面
	GameObject[] DownCard;//底牌
	public List<int> CP_Index;//用于发送消息
	public List<float> SelectCardPos;//选牌位置（用于转换为索引）
	public List<int> SelectCardIndex;//选牌预制体索引
	public List<int> LastCP;//上一次出的牌
	int LastPokerType;//上一次出牌类型
	int CurPokerType;//当前出牌类型
	//初始化
	override
	public void Init()
	{
		PlayerPokerCards = new List<int> ();
		PlayerPokerCardsInstant = new List<GameObject> ();
		MyCardNum = LeftCardNum = RightCardNum = 17;
		CurGameType = 0;
		CurFightNum = (char)0;
		SelectCardPos = new List<float> ();
		SelectCardIndex = new List<int> ();
		CP_Index = new List<int> ();
		LastCP = new List<int> ();
		LastPokerType = -1;
		CurPokerType = -1;
		OtherCardBack = new GameObject[2];
		for(int i=0;i<3;++i)
		{
			BackCard = new GameObject[3];
			DownCard = new GameObject[3];
		}
	}

	//结束
	override
	public void End()
	{
		
	}

	
	//收消息
	override
	public void OnServerSendData(byte[] data, int index, int count)
	{
		//准备反序列化协议
		m_RecvPackage.set_data (data, index, count);
		
		//得到消息id
		int id = -1;
		m_RecvPackage.get_int(ref id);
		
		//分情况讨论
		switch (id) 
		{
			//音效
			case _DDZ_SOUND:
			{
				int Sound = -1;
				m_RecvPackage.get_int(ref Sound);
				
				switch(Sound)
				{
					case (int)DDZGameSound._DDZ_SOUND_BJ:
					{
						GameObject.Find("SceneRoom").GetComponent<SceneRoom>().Instantiate_Sound(1,0);
						break;
					}
					case (int)DDZGameSound._DDZ_SOUND_JDZ:
					{
						GameObject.Find("SceneRoom").GetComponent<SceneRoom>().Instantiate_Sound(1,1);
						break;
					}
					case (int)DDZGameSound._DDZ_SOUND_BQ:
					{
						GameObject.Find("SceneRoom").GetComponent<SceneRoom>().Instantiate_Sound(1,2);
						break;
					}
					case (int)DDZGameSound._DDZ_SOUND_QDZ:
					{
						int QDZSound = -1;
						m_RecvPackage.get_int(ref QDZSound);
						GameObject.Find("SceneRoom").GetComponent<SceneRoom>().Instantiate_Sound(1,QDZSound+2);
						break;
					}
                    case (int)DDZGameSound._DDZ_SOUND_BC:
                    {
                        int a = (int)Random.Range(0, 2);
                        GameObject.Find("SceneRoom").GetComponent<SceneRoom>().Instantiate_Sound(2, a);
                        break;
                    }
                            
                    case (int)DDZGameSound._DDZ_SOUND_TWOKING:
                    {
                        GameObject.Find("SceneRoom").GetComponent<SceneRoom>().Instantiate_Sound(2, 2);
                        GameObject.Find("SceneRoom").GetComponent<SceneRoom>().Instantiate_Effect(0,new Vector3(0,3.0f,-2.0f));
                        break;
                    }
                    case (int)DDZGameSound._DDZ_SOUND_FORE:
                    {
                            GameObject.Find("SceneRoom").GetComponent<SceneRoom>().Instantiate_Sound(2, 3);
							GameObject.Find("SceneRoom").GetComponent<SceneRoom>().Instantiate_Effect(0,new Vector3(0,3.0f,-2.0f));
                            break;
                    }
                    case (int)DDZGameSound._DDZ_SOUND_PLANE:
                    {
                            GameObject.Find("SceneRoom").GetComponent<SceneRoom>().Instantiate_Sound(2, 4);
							GameObject.Find("SceneRoom").GetComponent<SceneRoom>().Instantiate_Effect(1,new Vector3(3.0f,3.0f,-2.0f));
                            break;
                    }
                    case (int)DDZGameSound._DDZ_SOUND_MORETHREEDOUBLE:
                    {
                            GameObject.Find("SceneRoom").GetComponent<SceneRoom>().Instantiate_Sound(2, 5);
                            break;
                    }
                    case (int)DDZGameSound._DDZ_SOUND_MOREFIVE:
                    {
                            GameObject.Find("SceneRoom").GetComponent<SceneRoom>().Instantiate_Sound(2, 6);
                            break;
                    }
                    case (int)DDZGameSound._DDZ_SOUND_FOREANDTWODOUBLE:
                    {
                        GameObject.Find("SceneRoom").GetComponent<SceneRoom>().Instantiate_Sound(2, 7);
                        break;
                    }
                    case (int)DDZGameSound._DDZ_SOUND_FOREANDTWOSINGLE:
                    {
                        GameObject.Find("SceneRoom").GetComponent<SceneRoom>().Instantiate_Sound(2, 8);
                        break;
                    }
                    case (int)DDZGameSound._DDZ_SOUND_THREEANDDOUBLE:
                    {
                        GameObject.Find("SceneRoom").GetComponent<SceneRoom>().Instantiate_Sound(2, 9);
                        break;
                    }
                    case (int)DDZGameSound._DDZ_SOUND_THREEANDONE:
                    {
                         GameObject.Find("SceneRoom").GetComponent<SceneRoom>().Instantiate_Sound(2, 10);
                        break;
                    }
                    case (int)DDZGameSound._DDZ_SOUND_THREE:
                    {
                       int Three = -1;
                       m_RecvPackage.get_int(ref Three);
                       GameObject.Find("SceneRoom").GetComponent<SceneRoom>().Instantiate_Sound(3, Three);
                       break;
                    }
                    case (int)DDZGameSound._DDZ_SOUND_DOUBLE:
                    {
                        int Double = -1;
                        m_RecvPackage.get_int(ref Double);
                        Double += 13;
                        GameObject.Find("SceneRoom").GetComponent<SceneRoom>().Instantiate_Sound(3, Double);
                        break;
                    }
                    case (int)DDZGameSound._DDZ_SOUND_SINGLE:
                    {
                        int Single = -1;
                        m_RecvPackage.get_int(ref Single);
                        if(14 == Single)//大王
                        {
                            GameObject.Find("SceneRoom").GetComponent<SceneRoom>().Instantiate_Sound(3, 40);
                        }else
                        {
                            Single += 26;
                            GameObject.Find("SceneRoom").GetComponent<SceneRoom>().Instantiate_Sound(3, Single);
                        }
                        break;
                    }
                  }
			  break;
			}

			//发牌事件处理
			case _DDZ_FP:
			{
				//发牌20张
				for(int i=0;i<20;++i)
				{
					int CardIndex = -1; 
					m_RecvPackage.get_int(ref CardIndex);
					if(CardIndex > -1)//无效牌剔除
						PlayerPokerCards.Add(CardIndex);
				}
				MyCardNum = PlayerPokerCards.Count;
				//发牌仅17张有效
				ShowCard(MyCardNum,true);
				//禁用手牌选择
				CanSelect(false);
				
				//其他玩家牌背显示并显示牌数
				OtherCardBack[0] = (GameObject)GameObject.Instantiate(CardBack,new Vector3(-5.0f,0,0),CardBack.transform.rotation);
				OtherCardBack[1] = (GameObject)GameObject.Instantiate(CardBack,new Vector3(5.0f,0,0),CardBack.transform.rotation);
				OtherCardNum(LeftCardNum,RightCardNum,true);

				//实例化底牌背面
				for(int i=0;i<3;++i)
				{
					Vector3 pos = CardBack.transform.position;
					pos.x -= -1.5f + i*1.5f;
					pos.y += 3.5f;
					BackCard[i] = (GameObject)GameObject.Instantiate(CardBack,pos,CardBack.transform.rotation);
				}	
				break;
			}
			//争夺地主开始事件处理
			case _DDZ_FIGHTSTART:
			{
				//显示房间争夺按钮
				GameObject.Find("SceneRoom").GetComponent<SceneRoom>().m_UI[10].SetActive(true);
				GameObject.Find("SceneRoom").GetComponent<SceneRoom>().m_UI[11].SetActive(true);
				CurGameType = _DDZ_FIGHTSTART;
				break;
			}
			case _DDZ_BJ:
			{
				GameObject.Find("SceneRoom").GetComponent<SceneRoom>().m_UI[10].SetActive(true);
				GameObject.Find("SceneRoom").GetComponent<SceneRoom>().m_UI[11].SetActive(true);
				m_RecvPackage.get_char(ref CurFightNum);
				CurGameType = _DDZ_BJ;
				break;
			}
			case _DDZ_JDZ:
			{
				GameObject.Find("SceneRoom").GetComponent<SceneRoom>().m_UI[10].SetActive(true);
				GameObject.Find("SceneRoom").GetComponent<SceneRoom>().m_UI[11].SetActive(true);
				GameObject.Find("SceneRoom").GetComponent<SceneRoom>().SetFightUI(10,2);
				GameObject.Find("SceneRoom").GetComponent<SceneRoom>().SetFightUI(11,3);
				m_RecvPackage.get_char(ref CurFightNum);
				CurGameType = _DDZ_JDZ;
				break;
			}
			case _DDZ_BQ:
			{
				GameObject.Find("SceneRoom").GetComponent<SceneRoom>().m_UI[10].SetActive(true);
				GameObject.Find("SceneRoom").GetComponent<SceneRoom>().m_UI[11].SetActive(true);
				GameObject.Find("SceneRoom").GetComponent<SceneRoom>().SetFightUI(10,2);
				GameObject.Find("SceneRoom").GetComponent<SceneRoom>().SetFightUI(11,3);
				m_RecvPackage.get_char(ref CurFightNum);
				CurGameType = _DDZ_BQ;
				break;
			}
			case _DDZ_QDZ:
			{
				GameObject.Find("SceneRoom").GetComponent<SceneRoom>().m_UI[10].SetActive(true);
				GameObject.Find("SceneRoom").GetComponent<SceneRoom>().m_UI[11].SetActive(true);
				GameObject.Find("SceneRoom").GetComponent<SceneRoom>().SetFightUI(10,2);
				GameObject.Find("SceneRoom").GetComponent<SceneRoom>().SetFightUI(11,3);
				m_RecvPackage.get_char(ref CurFightNum);
				CurGameType = _DDZ_QDZ;
				break;
			}
			case _DDZ_SP_PLAYER_NONGMING:
			{
				GameObject.Find("SceneRoom").GetComponent<SceneRoom>().m_UI[16].SetActive(true);
				GameObject.Find("SceneRoom").GetComponent<SceneRoom>().m_UI[17].SetActive(true);
				int Temp = 0; 
				m_RecvPackage.get_int(ref Temp);
				if(-1==Temp)
				{
					GameObject.Find("SceneRoom").GetComponent<SceneRoom>().SetPlayerStatus(16,1);
					//更新地主牌数
					LeftCardNum = 20;
					OtherCardNum(LeftCardNum,RightCardNum,true);
				}else
				{
					GameObject.Find("SceneRoom").GetComponent<SceneRoom>().SetPlayerStatus(17,1);
					//更新地主牌数
					RightCardNum = 20;
					OtherCardNum(LeftCardNum,RightCardNum,true);
				}
				//展示底牌
				for(int i=0;i<3;++i)
				{
					int BC = 0; 
					m_RecvPackage.get_int(ref BC);
					DownCard[i] = (GameObject)GameObject.Instantiate(prefb [BC],BackCard[i].transform.position,BackCard[i].transform.rotation);
					//取消底牌的移动功能
					Destroy(DownCard[i].GetComponent<BoxCollider>());
				}
				break;
			}
			case _DDZ_SP_PLAYER_DIZHU:
			{
				GameObject.Find("SceneRoom").GetComponent<SceneRoom>().m_UI[16].SetActive(true);
				GameObject.Find("SceneRoom").GetComponent<SceneRoom>().m_UI[17].SetActive(true);
				//显示出牌按钮
				GameObject.Find("SceneRoom").GetComponent<SceneRoom>().m_UI[19].SetActive(true);
				//获得底牌并展示底牌
				for(int i=0;i<3;++i)
				{
					int BC = 0; 
					m_RecvPackage.get_int(ref BC);
					PlayerPokerCards.Add(BC);
					DownCard[i] = (GameObject)GameObject.Instantiate(prefb [BC],BackCard[i].transform.position,BackCard[i].transform.rotation);
					//取消底牌的移动功能
					Destroy(DownCard[i].GetComponent<BoxCollider>());
				}
				//手牌重新排序并显示
				PlayerPokerCards.Sort();
				MyCardNum += 3;
				for(int i=PlayerPokerCardsInstant.Count -1;i>=0;--i)
				{
					Destroy(PlayerPokerCardsInstant[i]);
				}
				PlayerPokerCardsInstant.Clear();
				//展示手牌
				ShowCard(MyCardNum,true);
				//开启手牌选择
				CanSelect(true);
				break;
			}
            case _DDZ_BC:
            {
               //获得不出玩家
               int who = -2;
               m_RecvPackage.get_int(ref who);
               //获得出牌类型
               m_RecvPackage.get_int(ref LastPokerType);
               if (0 == who)
               {	   
                   GameObject.Find("SceneRoom").GetComponent<SceneRoom>().m_UI[18].SetActive(false);
                   GameObject.Find("SceneRoom").GetComponent<SceneRoom>().m_UI[19].SetActive(false);
                   
					ShowCard(MyCardNum, false);
                   //关闭手牌选择
                   CanSelect(false);
               }else
               {
                   //开启手牌选择
                   CanSelect(true);
                   //获得不出次数
                   int BCnum = -1;
                   m_RecvPackage.get_int(ref BCnum);
                   if(1 == BCnum)
                   {
                       GameObject.Find("SceneRoom").GetComponent<SceneRoom>().m_UI[18].SetActive(true);
                       GameObject.Find("SceneRoom").GetComponent<SceneRoom>().m_UI[19].SetActive(true);
                   }else   
                       GameObject.Find("SceneRoom").GetComponent<SceneRoom>().m_UI[19].SetActive(true);
               }

               SelectCardIndex.Clear();
               CP_Index.Clear();

               break;
            }
			case _DDZ_CPOK:
			{
                //获得出牌玩家
				int who = -2; 
				m_RecvPackage.get_int(ref who);
                //获得出牌数量
				int num = 0; 
				m_RecvPackage.get_int(ref num);
                //获得出牌类型
                int Type = 0; 
				m_RecvPackage.get_int(ref Type);
				LastPokerType = Type;

				LastCP.Clear();	
				for(int i=0;i<num;++i)
				{
					int temp = 0; 
					m_RecvPackage.get_int(ref temp);
					LastCP.Add(temp);
				}
				//清空上一次的牌（公共）
				GameObject.Find("CardOutput").GetComponent<CardOutput>().SetTime(0.0f);
				//实例化出的牌（公共）
				GameObject.Find("CardOutput").GetComponent<CardOutput>().SetCardOutputInst(LastCP,prefb);
				//如果是自身出牌，删除自身手牌的牌	
				if(0 == who)
				{
					MyCardNum -= num;
					GameObject.Find("SceneRoom").GetComponent<SceneRoom>().m_UI[18].SetActive(false);
					GameObject.Find("SceneRoom").GetComponent<SceneRoom>().m_UI[19].SetActive(false);

					for(int i= num -1 ;i>=0;--i)
					{
						Destroy(PlayerPokerCardsInstant[SelectCardIndex[i]]);
						PlayerPokerCardsInstant.RemoveAt(SelectCardIndex[i]);
						PlayerPokerCards.RemoveAt(SelectCardIndex[i]);
					}

					for(int j=0;j<PlayerPokerCardsInstant.Count;++j)
						PlayerPokerCardsInstant[j].GetComponent<PokerEvent>().SetPokerSelectState(false);

					ShowCard(MyCardNum,false);
					//关闭手牌选择
					CanSelect(false);
				}
				else if(-1 == who)
				{
					LeftCardNum -= num;
					OtherCardNum(LeftCardNum,RightCardNum,true);
					//开启手牌选择
					CanSelect(true);
					GameObject.Find("SceneRoom").GetComponent<SceneRoom>().m_UI[18].SetActive(true);
					GameObject.Find("SceneRoom").GetComponent<SceneRoom>().m_UI[19].SetActive(true);
				}
				else
				{
					RightCardNum -= num;
					OtherCardNum(LeftCardNum,RightCardNum,true);
				}

				SelectCardIndex.Clear();
				CP_Index.Clear();

				break;
			}
			case _DDZ_FINISH:
			{
				//获得完成游戏消息（胜利或者失败）
				int isWin = 0; 
				m_RecvPackage.get_int(ref isWin);

				if(_DDZ_WIN == isWin)
				{
					//显示结束界面
					GameObject.Find("SceneRoom").GetComponent<SceneRoom>().m_UI[20].SetActive(true);
					GameObject.Find("SceneRoom").GetComponent<SceneRoom>().m_UI[21].SetActive(true);
					//关闭手牌选择
					CanSelect(false);

					//销毁底牌
					for(int i=2;i>=0;--i)
					{
						Destroy(DownCard[i]);
						Destroy(BackCard[i]);
					}
					//销毁其他玩家UI
					for(int i=12;i<18;++i)
						GameObject.Find("SceneRoom").GetComponent<SceneRoom>().m_UI[i].SetActive(false);
					Destroy(OtherCardBack[0]);
					Destroy(OtherCardBack[1]);

					//删除手牌
					for(int i=PlayerPokerCardsInstant.Count - 1;i>=0;--i)	
						Destroy(PlayerPokerCardsInstant[i]);

					//关闭背景音效
					GameObject.Find("Quadruple").SetActive(false);

					//隐藏所有游戏设置
					GameObject.Find("SceneRoom").GetComponent<SceneRoom>().m_UI[18].SetActive(false);
					GameObject.Find("SceneRoom").GetComponent<SceneRoom>().m_UI[19].SetActive(false);
					GameObject.Find("SceneRoom").GetComponent<SceneRoom>().Instantiate_Sound(4,0);
				}else
				{
					//显示结束界面
					GameObject.Find("SceneRoom").GetComponent<SceneRoom>().m_UI[20].SetActive(true);
					GameObject.Find("SceneRoom").GetComponent<SceneRoom>().m_UI[21].SetActive(true);
					//关闭手牌选择
					CanSelect(false);

					//销毁底牌
					for(int i=2;i>=0;--i)
					{
						Destroy(DownCard[i]);
						Destroy(BackCard[i]);
					}
					//销毁其他玩家UI
					for(int i=12;i<18;++i)
						GameObject.Find("SceneRoom").GetComponent<SceneRoom>().m_UI[i].SetActive(false);
					Destroy(OtherCardBack[0]);
					Destroy(OtherCardBack[1]);

					//删除手牌
					for(int i=PlayerPokerCardsInstant.Count - 1;i>=0;--i)	
						Destroy(PlayerPokerCardsInstant[i]);

					//关闭背景音效
					GameObject.Find("Quadruple").SetActive(false);

					//隐藏所有游戏设置
					GameObject.Find("SceneRoom").GetComponent<SceneRoom>().m_UI[18].SetActive(false);
					GameObject.Find("SceneRoom").GetComponent<SceneRoom>().m_UI[19].SetActive(false);
					GameObject.Find("SceneRoom").GetComponent<SceneRoom>().Instantiate_Sound(4,1);
				}
				break;
			}
		}
	}

	//输入
	override
	public void Input(int type,int n)//type 游戏阶段 0争夺 1游戏中 2完成游戏 n 按键信息   （记住发消息前多发送一个游戏类型的消息）
	{
		switch (type) 
		{
			case 0://抢夺地主阶段
			{
				if(n == 0 && CurGameType == _DDZ_FIGHTSTART)
				{
					m_SendPackage.clear();
					m_SendPackage.add_int(_DDZ_FIGHT);
					m_SendPackage.add_int(_DDZ_BJ);
					SendData(m_SendPackage.data(),0,m_SendPackage.size());
				}
				else if(n == 1 && CurGameType == _DDZ_FIGHTSTART)
				{
					m_SendPackage.clear();
					m_SendPackage.add_int(_DDZ_FIGHT);
					m_SendPackage.add_int(_DDZ_JDZ);
					SendData(m_SendPackage.data(),0,m_SendPackage.size());
				}
				else if(n == 0 && CurGameType == _DDZ_BJ)
				{
					m_SendPackage.clear();
					m_SendPackage.add_int(_DDZ_FIGHT);
					m_SendPackage.add_int(_DDZ_BJ);
					SendData(m_SendPackage.data(),0,m_SendPackage.size());
				}
				else if(n == 1 && CurGameType == _DDZ_BJ)
				{
					m_SendPackage.clear();
					m_SendPackage.add_int(_DDZ_FIGHT);
					m_SendPackage.add_int(_DDZ_JDZ);
					SendData(m_SendPackage.data(),0,m_SendPackage.size());
				}
				else if(n == 0 && CurGameType == _DDZ_JDZ)
				{
					m_SendPackage.clear();
					m_SendPackage.add_int(_DDZ_FIGHT);
					m_SendPackage.add_int(_DDZ_BQ);
					SendData(m_SendPackage.data(),0,m_SendPackage.size());
				}
				else if(n == 1 && CurGameType == _DDZ_JDZ)
				{
					m_SendPackage.clear();
					m_SendPackage.add_int(_DDZ_FIGHT);
					m_SendPackage.add_int(_DDZ_QDZ);
					SendData(m_SendPackage.data(),0,m_SendPackage.size());
				}
				else if(n == 0 && CurGameType == _DDZ_BQ)
				{
					m_SendPackage.clear();
					m_SendPackage.add_int(_DDZ_FIGHT);
					m_SendPackage.add_int(_DDZ_BQ);
					SendData(m_SendPackage.data(),0,m_SendPackage.size());
				}
				else if(n == 1 && CurGameType == _DDZ_BQ)
				{
					m_SendPackage.clear();
					m_SendPackage.add_int(_DDZ_FIGHT);
					m_SendPackage.add_int(_DDZ_QDZ);
					SendData(m_SendPackage.data(),0,m_SendPackage.size());
				}
				else if(n == 0 && CurGameType == _DDZ_QDZ)
				{
					m_SendPackage.clear();
					m_SendPackage.add_int(_DDZ_FIGHT);
					m_SendPackage.add_int(_DDZ_BQ);
					SendData(m_SendPackage.data(),0,m_SendPackage.size());
				}
				else if(n == 1 && CurGameType == _DDZ_QDZ)
				{
					m_SendPackage.clear();
					m_SendPackage.add_int(_DDZ_FIGHT);
					m_SendPackage.add_int(_DDZ_QDZ);
					SendData(m_SendPackage.data(),0,m_SendPackage.size());
				}
				break;
			}
			case 1://出牌阶段
			{
				if(n == 0)//不出
				{
					//清空选牌列表
					GameObject.Find ("SelectCard").GetComponent<SelectCard> ().ClearList();
					//发送不出消息
					m_SendPackage.clear();
					m_SendPackage.add_int(_DDZ_PLAY);
					m_SendPackage.add_int(_DDZ_BC);
					SendData(m_SendPackage.data(),0,m_SendPackage.size());
				}
				if(n == 1)//出牌
				{
					//获得选牌（索引）
					bool isOK = SelectCardList();
					if(!isOK)//出牌为空
					{
						GameObject.Find("SceneRoom").GetComponent<PromptUI>().SetPromptUI("出牌不能为空");
					}else//出牌
					{
						//获得出牌数量
						int SelectCount = SelectCardIndex.Count;
                        //获得上一次出牌的数量
                        int LastCount = LastCP.Count;
                        //判断出牌类型和大小
                        bool TypeOk = CP_Type(LastCount, SelectCount,LastPokerType);
						if(!TypeOk)//出牌类型或大小错误
						{
                            //重置牌位置
                            ShowCard(MyCardNum, false);

                            GameObject.Find("SceneRoom").GetComponent<PromptUI>().SetPromptUI("出牌类型错误或太小");
							//清空选择索引
							SelectCardIndex.Clear();
                            CP_Index.Clear();
                           
						}
						//发送出牌消息
						else
						{
							m_SendPackage.clear();
							m_SendPackage.add_int(_DDZ_PLAY);
							m_SendPackage.add_int(_DDZ_CP);
							//出牌数量
							m_SendPackage.add_int(SelectCount);
                     
                            //出牌类型
                            m_SendPackage.add_int(CurPokerType);
							//出牌索引
							for(int i=0;i<SelectCount;++i)
								m_SendPackage.add_int(CP_Index[i]);
							SendData(m_SendPackage.data(),0,m_SendPackage.size());
						}
					}
				}
				break;
			}
			case 2://游戏结束阶段
			{
				if(n == 0)//返回大厅
				{
					GameObject.Find("SceneRoom").GetComponent<PromptUI>().SetPromptUI("返回大厅");
				}
				else{//退出游戏
					GameObject.Find("SceneRoom").GetComponent<PromptUI>().SetPromptUI("退出游戏");
				}
				break;
			}
		}
	}

	//每次游戏循环更新
	override
	public void OnUpdate()
	{
		//游戏是否胜利
		if (0 == MyCardNum) {
            MyCardNum = -1;
            m_SendPackage.clear();
			m_SendPackage.add_int(_DDZ_FINISH);
			SendData(m_SendPackage.data(),0,m_SendPackage.size());
		}
	}

	//展示手牌
	void ShowCard(int num,bool isFP)
	{
		//第一张牌的位置
		float posx = (num - 1) / 4.0f;
		//发牌阶段
		if (isFP) 
		{
			for (int i=0; i<num; ++i) {
				Vector3 pos = new Vector3 (posx, -3.0f, 0.0f);
				//间隔0.5f;
				pos.x = pos.x - i * 0.5f;
                pos.z += i * 0.5f;
				GameObject tempobj = new GameObject();
				tempobj = (GameObject)GameObject.Instantiate(prefb [PlayerPokerCards [i]], pos, prefb [PlayerPokerCards [i]].gameObject.transform.rotation);
				PlayerPokerCardsInstant.Add(tempobj);
			}
		} 
		//非发牌阶段
		else 
		{
			//更新手牌位置显示
			for (int i=0; i<num; ++i) 
			{
				Vector3 pos = new Vector3 (posx, -3.0f, 0.0f);
				pos.x = pos.x - i * 0.5f;
				pos.z +=i * 0.5f;
				PlayerPokerCardsInstant[i].GetComponent<PokerEvent>().SetPokerSelectState(false);
                PlayerPokerCardsInstant[i].GetComponent<PokerEvent>().SetPos(pos);
            }
		}
	}
	
	//设置其他玩家牌数UI
	void OtherCardNum(int l,int r,bool Active)
	{
		int num1 = 0,num2 = 0,num3 = 0,num4 = 0; 
		num1 = l / 10;
		num2 = l % 10;
		num3 = r / 10;
		num4 = r % 10;

		for(int i=12;i<16;++i)
		{
			GameObject.Find("SceneRoom").GetComponent<SceneRoom>().m_UI[i].SetActive(Active);
			if(i==12)
				GameObject.Find("SceneRoom").GetComponent<SceneRoom>().SetCardNumUI(i,num1);
			else if(i==13)
				GameObject.Find("SceneRoom").GetComponent<SceneRoom>().SetCardNumUI(i,num2);
			else if(i==14)
				GameObject.Find("SceneRoom").GetComponent<SceneRoom>().SetCardNumUI(i,num3);
			else if(i==15)
				GameObject.Find("SceneRoom").GetComponent<SceneRoom>().SetCardNumUI(i,num4);
		}
	}

	//能否进行手牌选择
	void CanSelect(bool select)
	{
		if (select) {
			for(int i=0;i<PlayerPokerCardsInstant.Count;++i)
				PlayerPokerCardsInstant[i].GetComponent<BoxCollider>().enabled = select;
		} else {
			for (int i=0; i<PlayerPokerCardsInstant.Count; ++i)
				PlayerPokerCardsInstant [i].GetComponent<BoxCollider> ().enabled = select;
		}
	}

	//牌位置转换为下标 false 空选 ture 选择了牌
	bool SelectCardList()
	{
		SelectCardPos = GameObject.Find ("SelectCard").GetComponent<SelectCard> ().SelectCardPos;
		if (0 == SelectCardPos.Count)
			return false;
		else//位置转换成下标
		{	
			//排序位置
			SelectCardPos.Sort();
			//起始位置
			float posx = (MyCardNum - 1) / 4.0f;
			for(int i=0;i<SelectCardPos.Count;++i)
			{
				int index= (int)((SelectCardPos[i] - posx)/-0.5f);
				SelectCardIndex.Add (index);
			}
			SelectCardIndex.Sort();
			SelectCardPos.Clear();
			GameObject.Find ("SelectCard").GetComponent<SelectCard> ().ClearList();
			return true;
		}
	}

	bool CP_Type(int lastnum,int num,int LastType)//牌 类型
	{
        //炸弹可以越牌型  仅需判断炸弹大小
        if (2 == num)//王炸
        {
            if (TwoKing())
                return true;
        }

        if (4 == num && _DDZ_SP_POKER_TYPE_TWOKING != LastType)//普通炸弹
        {
            if (NomaleBoom(LastType))
                return true;
        }

        //任意出牌 判断所有出牌类型
        if (-1 == LastType) 
		{
			if(1==num)
                return SingleCard();
			else if(2 == num)
                return DoubleCard();
			else if(3 == num)
                return ThreeCard();
			else if(4 == num)
                return ForeCard();
            else if(5 == num)
                return FiveCard();
            else if(6 == num)
                return SixCard();
            else if(7 == num)
                return SevenCard();
            else if(8 == num)
                return EightCard();
            else if (9 == num)
                return NineCard();
            else if (10 == num)
                return TenCard();
            else if (11 == num)
                return ElevenCard();
            else if (12 == num)
                return TwelveCard();
            else if (14 == num)
                return FourteenCard();
            else if (15 == num)
                return FifteenCard();
            else if (16 == num)
                return SixteenCard();
            else if (18 == num)
                return EighteenCard();
            else if (20 == num)
                return TwentyCard();

        }
        else if(lastnum != num)//判断与上一次出的牌数量是否相同
            return false;
        else if (_DDZ_SP_POKER_TYPE_SINGLE == LastType) //出单牌 1张
            return SingleCard(LastType);
        else if(_DDZ_SP_POKER_TYPE_DOUBLE == LastType)//出对子 2张
            return DoubleCard(LastType);
        else if (_DDZ_SP_POKER_TYPE_THREE == LastType)//出三不带 3张
            return ThreeCard(LastType);
        else if(_DDZ_SP_POKER_TYPE_THREEANDONE == LastType)//出三带一 4张
            return ForeCard(LastType);
        else if (_DDZ_SP_POKER_TYPE_THREEANDDOUBLE == LastType)//出三带一对 5张
            return FiveCard(LastType);
        else if(_DDZ_SP_POKER_TYPE_FOREANDTWOSINGLE == LastType)//出四带二 6张
            return SixCard(LastType);
        else if (_DDZ_SP_POKER_TYPE_FOREANDTWODOUBLE == LastType)//出四带两队 8张
            return EightCard(LastType);
        else if (_DDZ_SP_POKER_TYPE_MOREFIVE == LastType)//出顺子 5-11张
        {
            if(5 == lastnum)
                return FiveCard(LastType);
            else if(6 == lastnum)
                return SixCard(LastType);
            else if(7 == lastnum)
                return SevenCard(LastType);
            else if(8 == lastnum)
                return EightCard(LastType); 
            else if(9 == lastnum)
                return NineCard(LastType);
            else if(10 == lastnum)
                return TenCard(LastType);
            else if(11 == lastnum)
                return ElevenCard(LastType);
            //else if(12 == lastnum)
            //  return TwelveCard(LastType);
        }
        else if(_DDZ_SP_POKER_TYPE_MORETHREEDOUBLE == LastType)//出连对
        {
            if (6 == lastnum)
                return SixCard(LastType);
            else if (8 == lastnum)
                return EightCard(LastType);
            else if (10 == lastnum)
                return TenCard(LastType);
            else if (12 == lastnum)
                return TwelveCard(LastType);
            else if (14 == lastnum)
                return FourteenCard(LastType);
            else if (16 == lastnum)
                return SixteenCard(LastType);
            else if (18 == lastnum)
                return EighteenCard(LastType);
            else if (20 == lastnum)
                return TwentyCard(LastType);
        }
        else if(_DDZ_SP_POKER_TYPE_PLANE == LastType)//出飞机
        {
            if (6 == lastnum)
                return SixCard(LastType);
            else if (8 == lastnum)
                return EightCard(LastType);
            else if (9 == lastnum)
                return NineCard(LastType);
            else if (10 == lastnum)
                return TenCard(LastType);
            else if (12 == lastnum)
                return TwelveCard(LastType);
            else if (15 == lastnum)
                return FifteenCard(LastType);
        }
        return false;
	}

    //王炸
    bool TwoKing()
    {
        //王炸
        if (PlayerPokerCards[SelectCardIndex[0]] == 52 && PlayerPokerCards[SelectCardIndex[1]] == 53)
        {
            CP_Index.Add(PlayerPokerCards[SelectCardIndex[0]]);
            CP_Index.Add(PlayerPokerCards[SelectCardIndex[1]]);
            CurPokerType = _DDZ_SP_POKER_TYPE_TWOKING;
            return true;
        }
        return false;
    }
    //普通炸弹
    bool NomaleBoom(int isAnyType)
    {
        int temp00, temp01, temp10, temp11;
        temp00 = PlayerPokerCards[SelectCardIndex[0]];
        temp01 = PlayerPokerCards[SelectCardIndex[3]];
        temp10 = PlayerPokerCards[SelectCardIndex[0]] % 4;
        temp11 = PlayerPokerCards[SelectCardIndex[3]] % 4;
        if (_DDZ_SP_POKER_TYPE_FORE != isAnyType)//上一次不是炸弹
        {
            //判断第一张和最后一张是否同区间
            if (temp00 + 3 == temp01 && temp10 + 3 == temp11)
            {
                for(int i=0;i<4;++i)
                    CP_Index.Add(PlayerPokerCards[SelectCardIndex[i]]);
                CurPokerType = _DDZ_SP_POKER_TYPE_FORE;
                return true;
            }
        }
        else//上一次也是普通炸弹
        {
            //上一次的炸弹第一张
            int lastTemp;
            lastTemp = LastCP[0];
            if (temp00 + 3 == temp01 && temp10 + 3 == temp11 && temp00 > lastTemp)
            {
                for (int i = 0; i < 4; ++i)
                    CP_Index.Add(PlayerPokerCards[SelectCardIndex[i]]);
                CurPokerType = _DDZ_SP_POKER_TYPE_FORE;
                return true;
            }
            else//大小不合适
            {
                //重置牌位置
                ShowCard(MyCardNum, false);
                for (int i = 0; i < MyCardNum; ++i)
                    PlayerPokerCardsInstant[i].GetComponent<PokerEvent>().SetPokerSelectState(false);
                SelectCardIndex.Clear();
                CP_Index.Clear();
            }
        }
        return false;
    }
    //1张 单牌
    bool SingleCard(int isAnyType = 0)
	{
		if (0 == isAnyType) {
			CP_Index.Add (PlayerPokerCards [SelectCardIndex [0]]);
			CurPokerType = _DDZ_SP_POKER_TYPE_SINGLE;
			return true;
		}
		else 
		{
			CP_Index.Add (PlayerPokerCards [SelectCardIndex [0]]);
			int temp1 = CP_Index[0]/4;
			int temp2 = LastCP[0]/4;
			int temp3 = CP_Index[0];
			int temp4 = LastCP[0];
            //小王大王单独判断
            if(temp1 == 13 && temp3 > temp4)
            {
                CurPokerType = _DDZ_SP_POKER_TYPE_SINGLE;
                return true;
            }

            if (temp1!=temp2 && temp3 > temp4)//大小合适  出牌成功
			{
				CurPokerType = _DDZ_SP_POKER_TYPE_SINGLE;
				return true;
			}
		}
        //重置牌位置
        ShowCard(MyCardNum, false);
        for (int i = 0; i < MyCardNum; ++i)
            PlayerPokerCardsInstant[i].GetComponent<PokerEvent>().SetPokerSelectState(false);
        SelectCardIndex.Clear();
        CP_Index.Clear();
        return false;
	}
	//2张  对子
	bool DoubleCard(int isAnyType = 0)
	{
		if (0 == isAnyType) 
		{
            int temp00 = PlayerPokerCards[SelectCardIndex[0]] / 4;
            int temp01 = PlayerPokerCards[SelectCardIndex[1]] / 4;

            if(temp00 == temp01)
            {
            	CP_Index.Add(PlayerPokerCards[SelectCardIndex[0]]);
            	CP_Index.Add(PlayerPokerCards[SelectCardIndex[1]]);
            	CurPokerType = _DDZ_SP_POKER_TYPE_DOUBLE;
            	return true;
            }
		} 
		else 
		{
            int temp00 = PlayerPokerCards[SelectCardIndex[0]] / 4;
            int temp01 = PlayerPokerCards[SelectCardIndex[1]] / 4;

            int LastTemp;
            LastTemp = LastCP[0]/4;

            if (temp00 == temp01 && temp00 > LastTemp)
            {
                CP_Index.Add(PlayerPokerCards[SelectCardIndex[0]]);
                CP_Index.Add(PlayerPokerCards[SelectCardIndex[1]]);
                CurPokerType = _DDZ_SP_POKER_TYPE_DOUBLE;
                return true;
            }
        }
        //重置牌位置
        ShowCard(MyCardNum, false);
        for (int i = 0; i < MyCardNum; ++i)
            PlayerPokerCardsInstant[i].GetComponent<PokerEvent>().SetPokerSelectState(false);
        SelectCardIndex.Clear();
        CP_Index.Clear();
        return false;
	}
	//3张 三不带
	bool ThreeCard(int isAnyType = 0)
	{
        if (0 == isAnyType)
		{
            int temp00 = PlayerPokerCards[SelectCardIndex[0]] / 4;
            int temp01 = PlayerPokerCards[SelectCardIndex[1]] / 4;
            int temp02 = PlayerPokerCards[SelectCardIndex[2]] / 4;
            //三张同区间
            if (temp00 == temp01 && temp01 == temp02)
			{
				CP_Index.Add(PlayerPokerCards[SelectCardIndex[0]]);
				CP_Index.Add(PlayerPokerCards[SelectCardIndex[1]]);
				CP_Index.Add(PlayerPokerCards[SelectCardIndex[2]]);
				CurPokerType = _DDZ_SP_POKER_TYPE_THREE;
				return true;
			}
		}
		else
		{
            int temp00 = PlayerPokerCards[SelectCardIndex[0]] / 4;
            int temp01 = PlayerPokerCards[SelectCardIndex[1]] / 4;
            int temp02 = PlayerPokerCards[SelectCardIndex[2]] / 4;

            int LastTemp;
            LastTemp = LastCP[0] / 4;

            if (temp00 == temp01 && temp01 == temp02 && temp00 > LastTemp)
            {
                CP_Index.Add(PlayerPokerCards[SelectCardIndex[0]]);
                CP_Index.Add(PlayerPokerCards[SelectCardIndex[1]]);
                CP_Index.Add(PlayerPokerCards[SelectCardIndex[2]]);
                CurPokerType = _DDZ_SP_POKER_TYPE_THREE;
                return true;
            }
        }
        //重置牌位置
        ShowCard(MyCardNum, false);
        for (int i = 0; i < MyCardNum; ++i)
            PlayerPokerCardsInstant[i].GetComponent<PokerEvent>().SetPokerSelectState(false);
        SelectCardIndex.Clear();
        CP_Index.Clear();
        return false;
	}
	//4张 三带一
	bool ForeCard(int isAnyType = 0)
	{
        if (0 == isAnyType) 
		{
            int temp00 = PlayerPokerCards[SelectCardIndex[0]] / 4;
            int temp01 = PlayerPokerCards[SelectCardIndex[1]] / 4;
            int temp02 = PlayerPokerCards[SelectCardIndex[2]] / 4;
            int temp03 = PlayerPokerCards[SelectCardIndex[3]] / 4;

            if ((temp00 == temp01 && temp01 == temp02) || (temp01 == temp02 && temp02 == temp03))
            {
                for (int i=0;i<4;++i)
                    CP_Index.Add(PlayerPokerCards[SelectCardIndex[i]]);
                CurPokerType = _DDZ_SP_POKER_TYPE_THREEANDONE;
                return true;
            }
        } 
		else 
		{
            int temp00 = PlayerPokerCards[SelectCardIndex[0]] / 4;
            int temp01 = PlayerPokerCards[SelectCardIndex[1]] / 4;
            int temp02 = PlayerPokerCards[SelectCardIndex[2]] / 4;
            int temp03 = PlayerPokerCards[SelectCardIndex[3]] / 4;

            int LastTemp0, LastTemp1,LastTemp;
            LastTemp0 = LastCP[0] / 4;
            LastTemp1 = LastCP[1] / 4;
            LastTemp = (LastTemp0 == LastTemp1) ? LastTemp0 : LastTemp1;

            if (((temp00 == temp01 && temp01 == temp02) && temp00 > LastTemp) ||
                ((temp01 == temp02 && temp02 == temp03) && temp01 > LastTemp))
            {
                for (int i = 0; i < 4; ++i)
                    CP_Index.Add(PlayerPokerCards[SelectCardIndex[i]]);
                CurPokerType = _DDZ_SP_POKER_TYPE_THREEANDONE;
                return true;
            }
        }
        //重置牌位置
        ShowCard(MyCardNum, false);
        for (int i = 0; i < MyCardNum; ++i)
            PlayerPokerCardsInstant[i].GetComponent<PokerEvent>().SetPokerSelectState(false);
        SelectCardIndex.Clear();
        CP_Index.Clear();
        return false;
	}
    //5张 三带一对  顺子
    bool FiveCard(int isAnyType = 0)
    {
        int temp00 = PlayerPokerCards[SelectCardIndex[0]] / 4;
        int temp01 = PlayerPokerCards[SelectCardIndex[1]] / 4;
        int temp02 = PlayerPokerCards[SelectCardIndex[2]] / 4;
        int temp03 = PlayerPokerCards[SelectCardIndex[3]] / 4;
        int temp04 = PlayerPokerCards[SelectCardIndex[4]] / 4;
        if (0 == isAnyType)
        {
            //顺子判断
            if ((temp00 + 1 == temp01) && 
                (temp01 + 1 == temp02) && 
                (temp02 + 1 == temp03) &&
                (temp03 + 1 == temp04) &&
                 8 > temp00)
            {
                for (int i = 0; i < 5; ++i)
                    CP_Index.Add(PlayerPokerCards[SelectCardIndex[i]]);
                CurPokerType = _DDZ_SP_POKER_TYPE_MOREFIVE;
                return true;
            }else if((temp00 == temp01 && temp01 == temp02 && temp03 == temp04)||
                    (temp00 == temp01 && temp02 == temp03 && temp03 == temp04))//三带一对判断
            {
                for (int i = 0; i < 5; ++i)
                    CP_Index.Add(PlayerPokerCards[SelectCardIndex[i]]);
                CurPokerType = _DDZ_SP_POKER_TYPE_THREEANDDOUBLE;
                return true;
            }
        }else
        {
            if(_DDZ_SP_POKER_TYPE_MOREFIVE == isAnyType)
            {
                int LastTemp = LastCP[0] / 4;
                //顺子判断 大小判断
                if ((temp00 + 1 == temp01) &&
                    (temp01 + 1 == temp02) &&
                    (temp02 + 1 == temp03) &&
                    (temp03 + 1 == temp04) &&
                     8 > temp00 &&
                    temp00 > LastTemp)
                {
                    for (int i = 0; i < 5; ++i)
                        CP_Index.Add(PlayerPokerCards[SelectCardIndex[i]]);
                    CurPokerType = _DDZ_SP_POKER_TYPE_MOREFIVE;
                    return true;
                } 
            }else if(_DDZ_SP_POKER_TYPE_THREEANDDOUBLE == isAnyType)
            {
                int LastTemp0, LastTemp1, LastTemp;
                LastTemp0 = LastCP[1] / 4;
                LastTemp1 = LastCP[2] / 4;
                LastTemp = (LastTemp0 == LastTemp1) ? LastTemp0 : LastTemp1;
                //三带一对判断 大小判断
                if (((temp00 == temp01 && temp01 == temp02 && temp03 == temp04 && temp00> LastTemp) ||
                    (temp00 == temp01 && temp02 == temp03 && temp03 == temp04 && temp02> LastTemp)))
                {
                    for (int i = 0; i < 5; ++i)
                        CP_Index.Add(PlayerPokerCards[SelectCardIndex[i]]);
                    CurPokerType = _DDZ_SP_POKER_TYPE_THREEANDDOUBLE;
                    return true;
                }  
            }
        }
        //重置牌位置
        ShowCard(MyCardNum, false);
        for (int i = 0; i < MyCardNum; ++i)
            PlayerPokerCardsInstant[i].GetComponent<PokerEvent>().SetPokerSelectState(false);
        SelectCardIndex.Clear();
        CP_Index.Clear();
        return false;
    }
    //6张  顺子 四带二 连对 飞机(双三无翅膀) 
    bool SixCard(int isAnyType = 0)
    {
        int temp00 = PlayerPokerCards[SelectCardIndex[0]] / 4;
        int temp01 = PlayerPokerCards[SelectCardIndex[1]] / 4;
        int temp02 = PlayerPokerCards[SelectCardIndex[2]] / 4;
        int temp03 = PlayerPokerCards[SelectCardIndex[3]] / 4;
        int temp04 = PlayerPokerCards[SelectCardIndex[4]] / 4;
        int temp05 = PlayerPokerCards[SelectCardIndex[5]] / 4;

        if (0 == isAnyType)
        {      
            if ((temp00 + 1 == temp01) &&
                (temp01 + 1 == temp02) &&
                (temp02 + 1 == temp03) &&
                (temp03 + 1 == temp04) &&
                (temp04 + 1 == temp05) &&
                7 > temp00) //顺子判断
            {
                for (int i = 0; i < 6; ++i)
                    CP_Index.Add(PlayerPokerCards[SelectCardIndex[i]]);
                CurPokerType = _DDZ_SP_POKER_TYPE_MOREFIVE;
                return true;
            }
            else if ((temp00== temp01 && temp01 == temp02 && temp02 == temp03) ||
                    (temp01 == temp02 && temp02 == temp03 && temp03 == temp04) ||
                   (temp02 == temp03 && temp03 == temp04 && temp04 == temp05))//四带二判断
            {
                for (int i = 0; i < 6; ++i)
                    CP_Index.Add(PlayerPokerCards[SelectCardIndex[i]]);
                CurPokerType = _DDZ_SP_POKER_TYPE_FOREANDTWOSINGLE;
                return true;
            }else if((temp00 == temp01) && (temp02 == temp03) && (temp04 == temp05) && 
                    (temp00 + 1 == temp02) && (temp02 + 1 == temp04) && 10 > temp00)//连对判断
            {
                for (int i = 0; i < 6; ++i)
                    CP_Index.Add(PlayerPokerCards[SelectCardIndex[i]]);
                CurPokerType = _DDZ_SP_POKER_TYPE_MORETHREEDOUBLE;
                return true;
            }else if((temp00 == temp01) && (temp01 == temp02) &&
                     (temp03 == temp04) && (temp04 == temp05) &&
                     temp00 + 1 == temp03 && 11 > temp00)//飞机判断
            {
                for (int i = 0; i < 6; ++i)
                    CP_Index.Add(PlayerPokerCards[SelectCardIndex[i]]);
                CurPokerType = _DDZ_SP_POKER_TYPE_PLANE;
                return true;
            }
        }
        else
        {
            if (_DDZ_SP_POKER_TYPE_MOREFIVE == isAnyType)
            {
                int LastTemp = LastCP[0] / 4;
                //顺子判断 大小判断
                if ((temp00 + 1 == temp01) &&
                    (temp01 + 1 == temp02) &&
                    (temp02 + 1 == temp03) &&
                    (temp03 + 1 == temp04) &&
                    (temp04 + 1 == temp05) &&
                     7 > temp00 &&
                    temp00 > LastTemp)
                {
                    for (int i = 0; i < 6; ++i)
                        CP_Index.Add(PlayerPokerCards[SelectCardIndex[i]]);
                    CurPokerType = _DDZ_SP_POKER_TYPE_MOREFIVE;
                    return true;
                }
            }
            else if (_DDZ_SP_POKER_TYPE_FOREANDTWOSINGLE == isAnyType)
            {
                int LastTemp = LastCP[2] / 4;

                //四带二判断 大小判断
                if ((temp00 == temp01 && temp01 == temp02 && temp02 == temp03) ||
                    (temp01 == temp02 && temp02 == temp03 && temp03 == temp04) ||
                   (temp02 == temp03 && temp03 == temp04 && temp04 == temp05) &&
                    temp02 > LastTemp)
                {
                    for (int i = 0; i < 6; ++i)
                        CP_Index.Add(PlayerPokerCards[SelectCardIndex[i]]);
                    CurPokerType = _DDZ_SP_POKER_TYPE_FOREANDTWOSINGLE;
                    return true;
                }
            }else if(_DDZ_SP_POKER_TYPE_MORETHREEDOUBLE == isAnyType)
            {
                int LastTemp = LastCP[0] / 4;
                //连对判断 大小判断
                if ((temp00 == temp01) && (temp02 == temp03) && (temp04 == temp05) &&
                    (temp00 + 1 == temp02) && (temp02 + 1 == temp04) && 10 > temp00 &&
                    temp00 > LastTemp)
                {
                    for (int i = 0; i < 6; ++i)
                        CP_Index.Add(PlayerPokerCards[SelectCardIndex[i]]);
                    CurPokerType = _DDZ_SP_POKER_TYPE_MORETHREEDOUBLE;
                    return true;
                }
            }else if(_DDZ_SP_POKER_TYPE_PLANE == isAnyType)
            {
                int LastTemp = LastCP[0] / 4;
                //飞机判断 大小判断
                if ((temp00 == temp01) && (temp01 == temp02) &&
                    (temp03 == temp04) && (temp04 == temp05) &&
                     temp00 + 1 == temp03 && 11 > temp00 &&
                     temp00 > LastTemp)
                {
                    for (int i = 0; i < 6; ++i)
                        CP_Index.Add(PlayerPokerCards[SelectCardIndex[i]]);
                    CurPokerType = _DDZ_SP_POKER_TYPE_PLANE;
                    return true;
                }
            }
        }
        //重置牌位置
        ShowCard(MyCardNum, false);
        for (int i = 0; i < MyCardNum; ++i)
            PlayerPokerCardsInstant[i].GetComponent<PokerEvent>().SetPokerSelectState(false);
        SelectCardIndex.Clear();
        CP_Index.Clear();
        return false;
    }
    //7张  顺子
    bool SevenCard(int isAnyType = 0)
    {
        int[] temp;
        temp = new int[7];
        for (int k = 0; k < 7; ++k)
            temp[k] = PlayerPokerCards[SelectCardIndex[k]] / 4;

        if (0 == isAnyType)
        {
            if ((temp[0] + 1 == temp[1]) &&
                (temp[1] + 1 == temp[2]) &&
                (temp[2] + 1 == temp[3]) &&
                (temp[3] + 1 == temp[4]) &&
                (temp[4] + 1 == temp[5]) &&
                (temp[5] + 1 == temp[6]) &&
                6 > temp[0]) //顺子判断
            {
                for (int i = 0; i < 7; ++i)
                    CP_Index.Add(PlayerPokerCards[SelectCardIndex[i]]);
                CurPokerType = _DDZ_SP_POKER_TYPE_MOREFIVE;
                return true;
            }
        }else
        {
            int LastTemp = LastCP[0] / 4;
            //顺子判断 大小判断
            if ((temp[0] + 1 == temp[1]) &&
                (temp[1] + 1 == temp[2]) &&
                (temp[2] + 1 == temp[3]) &&
                (temp[3] + 1 == temp[4]) &&
                (temp[4] + 1 == temp[5]) &&
                (temp[5] + 1 == temp[6]) &&
                6 > temp[0] &&
                temp[0]> LastTemp) 
            {
                for (int i = 0; i < 7; ++i)
                    CP_Index.Add(PlayerPokerCards[SelectCardIndex[i]]);
                CurPokerType = _DDZ_SP_POKER_TYPE_MOREFIVE;
                return true;
            }    
        }
        //重置牌位置
        ShowCard(MyCardNum, false);
        for (int i = 0; i < MyCardNum; ++i)
            PlayerPokerCardsInstant[i].GetComponent<PokerEvent>().SetPokerSelectState(false);
        SelectCardIndex.Clear();
        CP_Index.Clear();
        return false;
    }
    //8张  顺子  四带两队 连对 飞机(双三带两单) 
    bool EightCard(int isAnyType = 0)
    {
        int[] temp;
        temp = new int[8];
        for (int k = 0; k < 8; ++k)
            temp[k] = PlayerPokerCards[SelectCardIndex[k]] / 4;

        if (0 == isAnyType)
        {
            if ((temp[0] + 1 == temp[1]) &&
                (temp[1] + 1 == temp[2]) &&
                (temp[2] + 1 == temp[3]) &&
                (temp[3] + 1 == temp[4]) &&
                (temp[4] + 1 == temp[5]) &&
                (temp[5] + 1 == temp[6]) &&
                (temp[6] + 1 == temp[7]) &&
                5 > temp[0])//顺子判断
            {
                for (int i = 0; i < 8; ++i)
                    CP_Index.Add(PlayerPokerCards[SelectCardIndex[i]]);
                CurPokerType = _DDZ_SP_POKER_TYPE_MOREFIVE;
                return true;
            }else if(((temp[0] == temp[1]) && (temp[2] == temp[3]) && (temp[4] == temp[5]) && (temp[5] == temp[6]) && (temp[6] == temp[7])) ||
                     ((temp[0] == temp[1]) && (temp[2] == temp[3]) && (temp[3] == temp[4]) && (temp[4] == temp[5]) && (temp[6] == temp[7])) ||
                     ((temp[0] == temp[1]) && (temp[1] == temp[2]) && (temp[2] == temp[3]) && (temp[4] == temp[5]) && (temp[6] == temp[7])))//四带两队判断
            {
                for (int i = 0; i < 8; ++i)
                    CP_Index.Add(PlayerPokerCards[SelectCardIndex[i]]);
                CurPokerType = _DDZ_SP_POKER_TYPE_FOREANDTWODOUBLE;
                return true;
            }else if((temp[0] == temp[1]) && (temp[2] == temp[3]) && (temp[4] == temp[5]) && (temp[6] == temp[7]) &&
                     (temp[0]+1 == temp[2]) && (temp[2] + 1 == temp[4]) && (temp[4] + 1 == temp[6]) && 9 > temp[0])//连对判断
            {
                for (int i = 0; i < 8; ++i)
                    CP_Index.Add(PlayerPokerCards[SelectCardIndex[i]]);
                CurPokerType = _DDZ_SP_POKER_TYPE_MORETHREEDOUBLE;
                return true;
            }else if(((temp[0] == temp[1]) && (temp[1] == temp[2]) && (temp[3] == temp[4]) && (temp[4] == temp[5]) && temp[0]+1 == temp[3] && 11 > temp[0]) ||
                     ((temp[1] == temp[2]) && (temp[2] == temp[3]) && (temp[4] == temp[5]) && (temp[5] == temp[6]) && temp[1]+1 == temp[4] && 11 > temp[1]) ||
                     ((temp[2] == temp[3]) && (temp[3] == temp[4]) && (temp[5] == temp[6]) && (temp[6] == temp[7]) && temp[2]+1 == temp[5] && 11 > temp[2]))//飞机(双三带两单) 判断
            {
                for (int i = 0; i < 8; ++i)
                    CP_Index.Add(PlayerPokerCards[SelectCardIndex[i]]);
                CurPokerType = _DDZ_SP_POKER_TYPE_PLANE;
                return true;
            }
        }
        else
        {
            if (_DDZ_SP_POKER_TYPE_MOREFIVE == isAnyType)
            {
                int LastTemp = LastCP[0] / 4;
                //顺子判断 大小判断
                if ((temp[0] + 1 == temp[1]) &&
                    (temp[1] + 1 == temp[2]) &&
                    (temp[2] + 1 == temp[3]) &&
                    (temp[3] + 1 == temp[4]) &&
                    (temp[4] + 1 == temp[5]) &&
                    (temp[5] + 1 == temp[6]) &&
                    (temp[6] + 1 == temp[7]) &&
                    5 > temp[0] &&
                    temp[0] > LastTemp)
                {
                    for (int i = 0; i < 8; ++i)
                        CP_Index.Add(PlayerPokerCards[SelectCardIndex[i]]);
                    CurPokerType = _DDZ_SP_POKER_TYPE_MOREFIVE;
                    return true;
                }
            } else if (_DDZ_SP_POKER_TYPE_FOREANDTWODOUBLE == isAnyType)
            {
                int Temp0, Temp1, Temp2, Temp3, LastTemp;
                Temp0 = LastCP[0] / 4;
                Temp1 = LastCP[2] / 4;
                Temp2 = LastCP[4] / 4;
                Temp3 = LastCP[6] / 4;
                LastTemp = (Temp0 == Temp1) ? Temp0 : ((Temp1 == Temp2) ? Temp1 : Temp3);
                //四带两对判断  大小判断
                if (((temp[0] == temp[1]) && (temp[2] == temp[3]) && (temp[4] == temp[5]) && (temp[5] == temp[6]) && (temp[6] == temp[7]) && temp[4] > LastTemp) ||
                    ((temp[0] == temp[1]) && (temp[2] == temp[3]) && (temp[3] == temp[4]) && (temp[4] == temp[5]) && (temp[6] == temp[7]) && temp[2] > LastTemp) ||
                    ((temp[0] == temp[1]) && (temp[1] == temp[2]) && (temp[2] == temp[3]) && (temp[4] == temp[5]) && (temp[6] == temp[7]) && temp[0] > LastTemp))
                {
                    for (int i = 0; i < 8; ++i)
                        CP_Index.Add(PlayerPokerCards[SelectCardIndex[i]]);
                    CurPokerType = _DDZ_SP_POKER_TYPE_FOREANDTWODOUBLE;
                    return true;
                }
            } else if (_DDZ_SP_POKER_TYPE_MORETHREEDOUBLE == isAnyType)
            {
                int LastTemp = LastCP[0] / 4;
                //连对判断 大小判断
                if (temp[0] == temp[1] && temp[2] == temp[3] && temp[4] == temp[5] && temp[6] == temp[7] &&
                    (temp[0] + 1 == temp[2]) && (temp[2] + 1 == temp[4]) && (temp[4] + 1 == temp[6]) && 9 > temp[0] && temp[0] > LastTemp)
                {
                    for (int i = 0; i < 8; ++i)
                        CP_Index.Add(PlayerPokerCards[SelectCardIndex[i]]);
                    CurPokerType = _DDZ_SP_POKER_TYPE_MORETHREEDOUBLE;
                    return true;
                }
            } else if (_DDZ_SP_POKER_TYPE_PLANE == isAnyType)
            {
                int temp0, temp1;
                temp0 = LastCP[0] / 4;
                temp1 = LastCP[2] / 4;
                int LastTemp = (temp0 == temp1) ? temp0 : temp1;
                //飞机(双三带两单) 判断
                if (((temp[0] == temp[1]) && (temp[1] == temp[2]) && (temp[3] == temp[4]) && (temp[4] == temp[5]) && (temp[0] + 1 == temp[3]) && 11 > temp[0] && temp[0]>LastTemp) ||
                    ((temp[1] == temp[2]) && (temp[2] == temp[3]) && (temp[4] == temp[5]) && (temp[5] == temp[6]) && (temp[1] + 1 == temp[4]) && 11 > temp[1] && temp[1]>LastTemp) ||
                    ((temp[2] == temp[3]) && (temp[3] == temp[4]) && (temp[5] == temp[6]) && (temp[6] == temp[7]) && (temp[2] + 1 == temp[5]) && 11 > temp[2] && temp[2]>LastTemp))
                {
                    for (int i = 0; i < 8; ++i)
                        CP_Index.Add(PlayerPokerCards[SelectCardIndex[i]]);
                    CurPokerType = _DDZ_SP_POKER_TYPE_PLANE;
                    return true;
                }
            }   
        }
        //重置牌位置
        ShowCard(MyCardNum, false);
        for (int i = 0; i < MyCardNum; ++i)
            PlayerPokerCardsInstant[i].GetComponent<PokerEvent>().SetPokerSelectState(false);
        SelectCardIndex.Clear();
        CP_Index.Clear();
        return false;
    }
    //9张 顺子 飞机(三三无翅膀) 
    bool NineCard(int isAnyType = 0)
    {
        int[] temp;
        temp = new int[9];
        for (int k = 0; k < 9; ++k)
            temp[k] = PlayerPokerCards[SelectCardIndex[k]] / 4;

        if (0 == isAnyType)
        {
            if ((temp[0] + 1 == temp[1]) && (temp[1] + 1 == temp[2]) && (temp[2] + 1 == temp[3]) && 
                (temp[3] + 1 == temp[4]) && (temp[4] + 1 == temp[5]) && (temp[5] + 1 == temp[6]) &&
                (temp[6] + 1 == temp[7]) && (temp[7] + 1 == temp[8]) &&  4 > temp[0])//顺子判断
            {
                for (int i = 0; i < 9; ++i)
                    CP_Index.Add(PlayerPokerCards[SelectCardIndex[i]]);
                CurPokerType = _DDZ_SP_POKER_TYPE_MOREFIVE;
                return true;
            }else if(temp[0] == temp[1] && temp[1] == temp[2] &&
                     temp[3] == temp[4] && temp[4] == temp[5] &&
                     temp[6] == temp[7] && temp[7] == temp[8] &&
                     (temp[0]+1 == temp[3]) && (temp[3] + 1 == temp[6]) && 10 > temp[0]) //飞机(三三无翅膀) 判断
            {
                for (int i = 0; i < 9; ++i)
                    CP_Index.Add(PlayerPokerCards[SelectCardIndex[i]]);
                CurPokerType = _DDZ_SP_POKER_TYPE_PLANE;

                return true;
            }
        }else
        {
            if (_DDZ_SP_POKER_TYPE_MOREFIVE == isAnyType)
            {
                int LastTemp = LastCP[0] / 4;
                //顺子判断 大小判断
                if ((temp[0] + 1 == temp[1]) &&
                    (temp[1] + 1 == temp[2]) &&
                    (temp[2] + 1 == temp[3]) &&
                    (temp[3] + 1 == temp[4]) &&
                    (temp[4] + 1 == temp[5]) &&
                    (temp[5] + 1 == temp[6]) &&
                    (temp[6] + 1 == temp[7]) &&
                    (temp[7] + 1 == temp[8]) &&
                    4 > temp[0] &&
                    temp[0] > LastTemp)
                {
                    for (int i = 0; i < 9; ++i)
                        CP_Index.Add(PlayerPokerCards[SelectCardIndex[i]]);
                    CurPokerType = _DDZ_SP_POKER_TYPE_MOREFIVE;
                    return true;
                }
            } else if (_DDZ_SP_POKER_TYPE_PLANE == isAnyType)
            {
                int LastTemp = LastCP[0] / 4;
                //飞机(三三无翅膀) 判断  大小判断
                if (temp[0] == temp[1] && temp[1] == temp[2] &&
                    temp[3] == temp[4] && temp[4] == temp[5] &&
                    temp[6] == temp[7] && temp[7] == temp[8] &&
                    (temp[0] + 1 == temp[3]) && (temp[3] + 1 == temp[6])&&
                    10 > temp[0] && temp[0] > LastTemp) 
                {
                    for (int i = 0; i < 9; ++i)
                        CP_Index.Add(PlayerPokerCards[SelectCardIndex[i]]);
                    CurPokerType = _DDZ_SP_POKER_TYPE_PLANE;
                    return true;
                }
            }
        }
        //重置牌位置
        ShowCard(MyCardNum, false);
        for (int i = 0; i < MyCardNum; ++i)
            PlayerPokerCardsInstant[i].GetComponent<PokerEvent>().SetPokerSelectState(false);
        SelectCardIndex.Clear();
        CP_Index.Clear();
        return false;
    }
    //10张 顺子 连对 飞机(双三带两对) 
    bool TenCard(int isAnyType = 0)
    {
        int[] temp;
        temp = new int[10];
        for (int k = 0; k < 10; ++k)
            temp[k] = PlayerPokerCards[SelectCardIndex[k]] / 4;

        if (0 == isAnyType)
        {
            if ((temp[0] + 1 == temp[1]) && (temp[1] + 1 == temp[2]) && (temp[2] + 1 == temp[3]) &&
                (temp[3] + 1 == temp[4]) && (temp[4] + 1 == temp[5]) && (temp[5] + 1 == temp[6]) &&
                (temp[6] + 1 == temp[7]) && (temp[7] + 1 == temp[8]) && (temp[8] + 1 == temp[9]) && 
                3 > temp[0])//顺子判断
            {
                for (int i = 0; i < 10; ++i)
                    CP_Index.Add(PlayerPokerCards[SelectCardIndex[i]]);
                CurPokerType = _DDZ_SP_POKER_TYPE_MOREFIVE;
                return true;
            }
            else if((temp[0] == temp[1]) && (temp[2] == temp[3]) && (temp[4] == temp[5]) &&
                    (temp[6] == temp[7]) && (temp[8] == temp[9]) && (temp[0]+1 == temp[2]) &&
                    (temp[2] + 1 == temp[4]) && (temp[4] + 1 == temp[6]) && (temp[6] + 1 == temp[8]) && 
                    8 > temp[0])//连对判断
            {
                for (int i = 0; i < 10; ++i)
                    CP_Index.Add(PlayerPokerCards[SelectCardIndex[i]]);
                CurPokerType = _DDZ_SP_POKER_TYPE_MORETHREEDOUBLE;
                return true;
            }
            else if (((temp[0] == temp[1]) && (temp[1] == temp[2]) && (temp[3] == temp[4]) && (temp[4] == temp[5]) && (temp[6] == temp[7]) && (temp[8] == temp[9]) && temp[0] + 1 == temp[3] && 11 > temp[0]) ||
                     ((temp[0] == temp[1]) && (temp[2] == temp[3]) && (temp[3] == temp[4]) && (temp[5] == temp[6]) && (temp[6] == temp[7]) && (temp[8] == temp[9]) && temp[2] + 1 == temp[5] && 11 > temp[2]) ||
                     ((temp[0] == temp[1]) && (temp[2] == temp[3]) && (temp[4] == temp[5]) && (temp[5] == temp[6]) && (temp[7] == temp[8]) && (temp[8] == temp[9]) && temp[4] + 1 == temp[7] && 11 > temp[4])) //飞机(双三带两对) 判断
            {
                for (int i = 0; i < 10; ++i)
                    CP_Index.Add(PlayerPokerCards[SelectCardIndex[i]]);
                CurPokerType = _DDZ_SP_POKER_TYPE_PLANE;
                return true;
            }
        }
        else
        {
            if(_DDZ_SP_POKER_TYPE_MOREFIVE == isAnyType)
            {
                int LastTemp = LastCP[0] / 4;
                //顺子判断 大小判断
                if ((temp[0] + 1 == temp[1]) && (temp[1] + 1 == temp[2]) && (temp[2] + 1 == temp[3]) &&
                    (temp[3] + 1 == temp[4]) && (temp[4] + 1 == temp[5]) && (temp[5] + 1 == temp[6]) &&
                    (temp[6] + 1 == temp[7]) && (temp[7] + 1 == temp[8]) && (temp[8] + 1 == temp[9]) &&
                    3 > temp[0] && temp[0] > LastTemp)
                {
                    for (int i = 0; i < 10; ++i)
                        CP_Index.Add(PlayerPokerCards[SelectCardIndex[i]]);
                    CurPokerType = _DDZ_SP_POKER_TYPE_MOREFIVE;
                    return true;
                }
            }
            else if(_DDZ_SP_POKER_TYPE_MORETHREEDOUBLE == isAnyType)
            {
                int LastTemp = LastCP[0] / 4;
                //连对判断 大小判断
                if ((temp[0] == temp[1]) && (temp[2] == temp[3]) && (temp[4] == temp[5]) &&
                    (temp[6] == temp[7]) && (temp[8] == temp[9]) && (temp[0] + 1 == temp[2]) &&
                    (temp[2] + 1 == temp[4]) && (temp[4] + 1 == temp[6]) && (temp[6] + 1 == temp[8]) && 
                    8 > temp[0] && temp[0] > LastTemp)
                {
                    for (int i = 0; i < 10; ++i)
                        CP_Index.Add(PlayerPokerCards[SelectCardIndex[i]]);
                    CurPokerType = _DDZ_SP_POKER_TYPE_MORETHREEDOUBLE;
                    return true;
                }
            }
            else if(_DDZ_SP_POKER_TYPE_PLANE == isAnyType)
            {
                int temp0, temp1, temp2, temp3, LastTemp;
                temp0 = LastCP[0] / 4;
                temp1 = LastCP[2] / 4;
                temp2 = LastCP[4] / 4;
                LastTemp = (temp0 == temp1) ? temp0 : ((temp1 == temp2) ? temp1 : temp2);
                //飞机(双三带两对)判断 大小判断
                if (((temp[0] == temp[1]) && (temp[1] == temp[2]) && (temp[3] == temp[4]) && (temp[4] == temp[5]) && (temp[6] == temp[7]) && (temp[8] == temp[9]) && temp[0] + 1 == temp[3] && 11 > temp[0] && temp[0] > LastTemp) ||
                    ((temp[0] == temp[1]) && (temp[2] == temp[3]) && (temp[3] == temp[4]) && (temp[5] == temp[6]) && (temp[6] == temp[7]) && (temp[8] == temp[9]) && temp[2] + 1 == temp[5] && 11 > temp[2] && temp[2] > LastTemp) ||
                    ((temp[0] == temp[1]) && (temp[2] == temp[3]) && (temp[4] == temp[5]) && (temp[5] == temp[6]) && (temp[7] == temp[8]) && (temp[8] == temp[9]) && temp[4] + 1 == temp[7] && 11 > temp[4] && temp[4] > LastTemp)) 
                {
                    for (int i = 0; i < 10; ++i)
                        CP_Index.Add(PlayerPokerCards[SelectCardIndex[i]]);
                    CurPokerType = _DDZ_SP_POKER_TYPE_PLANE;
                    return true;
                }
            }
        }
        //重置牌位置
        ShowCard(MyCardNum, false);
        for (int i = 0; i < MyCardNum; ++i)
            PlayerPokerCardsInstant[i].GetComponent<PokerEvent>().SetPokerSelectState(false);
        SelectCardIndex.Clear();
        CP_Index.Clear();
        return false;
    }
    //11张 顺子
    bool ElevenCard(int isAnyType = 0)
    {
        int[] temp;
        temp = new int[11];
        for (int k = 0; k < 11; ++k)
            temp[k] = PlayerPokerCards[SelectCardIndex[k]] / 4;

        if (0 == isAnyType)
        {
            if ((temp[0] + 1 == temp[1]) && (temp[1] + 1 == temp[2]) && (temp[2] + 1 == temp[3]) &&
                (temp[3] + 1 == temp[4]) && (temp[4] + 1 == temp[5]) && (temp[5] + 1 == temp[6]) &&
                (temp[6] + 1 == temp[7]) && (temp[7] + 1 == temp[8]) && (temp[8] + 1 == temp[9]) &&
                (temp[9] + 1 == temp[10]) && 2 > temp[0])//顺子判断
            {
                for (int i = 0; i < 11; ++i)
                    CP_Index.Add(PlayerPokerCards[SelectCardIndex[i]]);
                CurPokerType = _DDZ_SP_POKER_TYPE_MOREFIVE;
                return true;
            }
        }
        else
        {
            if (_DDZ_SP_POKER_TYPE_MOREFIVE == isAnyType)
            {
                int LastTemp = LastCP[0] / 4;
                //顺子判断 大小判断
                if ((temp[0] + 1 == temp[1]) && (temp[1] + 1 == temp[2]) && (temp[2] + 1 == temp[3]) &&
                    (temp[3] + 1 == temp[4]) && (temp[4] + 1 == temp[5]) && (temp[5] + 1 == temp[6]) &&
                    (temp[6] + 1 == temp[7]) && (temp[7] + 1 == temp[8]) && (temp[8] + 1 == temp[9]) &&
                    (temp[9] + 1 == temp[10])&& 2 > temp[0] && temp[0] > LastTemp)
                {
                    for (int i = 0; i < 11; ++i)
                        CP_Index.Add(PlayerPokerCards[SelectCardIndex[i]]);
                    CurPokerType = _DDZ_SP_POKER_TYPE_MOREFIVE;
                    return true;
                }
            }
        }
        //重置牌位置
        ShowCard(MyCardNum, false);
        for (int i = 0; i < MyCardNum; ++i)
            PlayerPokerCardsInstant[i].GetComponent<PokerEvent>().SetPokerSelectState(false);
        SelectCardIndex.Clear();
        CP_Index.Clear();
        return false;
    }
    //12张 顺子 连对 飞机(三三带三单)
    bool TwelveCard(int isAnyType = 0)
    {
        int[] temp;
        temp = new int[12];
        for (int k = 0; k < 12; ++k)
            temp[k] = PlayerPokerCards[SelectCardIndex[k]] / 4;

        if (0 == isAnyType)
        {
            if ((temp[0] + 1 == temp[1]) && (temp[1] + 1 == temp[2]) && (temp[2] + 1 == temp[3]) &&
                (temp[3] + 1 == temp[4]) && (temp[4] + 1 == temp[5]) && (temp[5] + 1 == temp[6]) &&
                (temp[6] + 1 == temp[7]) && (temp[7] + 1 == temp[8]) && (temp[8] + 1 == temp[9]) &&
                (temp[9] + 1 == temp[10])&& (temp[10] + 1 == temp[11])&& 1 > temp[0])//顺子判断
            {
                for (int i = 0; i < 12; ++i)
                    CP_Index.Add(PlayerPokerCards[SelectCardIndex[i]]);
                CurPokerType = _DDZ_SP_POKER_TYPE_MOREFIVE;
                return true;
            }
            else if ((temp[0] == temp[1]) && (temp[2] == temp[3]) && (temp[4] == temp[5]) &&
                     (temp[6] == temp[7]) && (temp[8] == temp[9]) && (temp[10] == temp[11]) &&
                     (temp[0] +1 == temp[2]) && (temp[2] + 1 == temp[4]) && (temp[4] + 1 == temp[6]) &&
                     (temp[6] + 1 == temp[8]) && (temp[8] + 1 == temp[10]) && 7 > temp[0])//连对判断
            {
                for (int i = 0; i < 12; ++i)
                    CP_Index.Add(PlayerPokerCards[SelectCardIndex[i]]);
                CurPokerType = _DDZ_SP_POKER_TYPE_MORETHREEDOUBLE;
                return true;
            }
            else if (((temp[0] == temp[1]) && (temp[1] == temp[2]) && (temp[3] == temp[4]) && (temp[4] == temp[5]) && (temp[6] == temp[7]) && (temp[7] == temp[8]) && (temp[0] + 1 == temp[3]) && (temp[3] + 1 == temp[6]) && 9 > temp[0])  ||
                     ((temp[1] == temp[2]) && (temp[2] == temp[3]) && (temp[4] == temp[5]) && (temp[5] == temp[6]) && (temp[7] == temp[8]) && (temp[8] == temp[9]) && (temp[1] + 1 == temp[4]) && (temp[4] + 1 == temp[7]) && 9 > temp[1])  ||
                     ((temp[2] == temp[3]) && (temp[3] == temp[4]) && (temp[5] == temp[6]) && (temp[6] == temp[7]) && (temp[8] == temp[9]) && (temp[9] == temp[10]) && (temp[2] + 1 == temp[5]) && (temp[5] + 1 == temp[8]) && 9 > temp[2]) ||
                     ((temp[3] == temp[4]) && (temp[4] == temp[5]) && (temp[6] == temp[7]) && (temp[7] == temp[8]) && (temp[9] == temp[10]) && (temp[10] == temp[11]) && (temp[3] + 1 == temp[6]) && (temp[6] + 1 == temp[9]) && 9 > temp[3]))//飞机(三三带三单)判断
            {
                for (int i = 0; i < 12; ++i)
                    CP_Index.Add(PlayerPokerCards[SelectCardIndex[i]]);
                CurPokerType = _DDZ_SP_POKER_TYPE_PLANE;
                return true;
            }
        }
        else
        {
            if (_DDZ_SP_POKER_TYPE_MOREFIVE == isAnyType)
            {
                int LastTemp = LastCP[0] / 4;
                //顺子判断 大小判断
                if ((temp[0] + 1 == temp[1]) && (temp[1] + 1 == temp[2]) && (temp[2] + 1 == temp[3]) &&
                    (temp[3] + 1 == temp[4]) && (temp[4] + 1 == temp[5]) && (temp[5] + 1 == temp[6]) &&
                    (temp[6] + 1 == temp[7]) && (temp[7] + 1 == temp[8]) && (temp[8] + 1 == temp[9]) &&
                    (temp[9] + 1 == temp[10])&& (temp[10] + 1 == temp[11])&& 1 > temp[0] && temp[0] > LastTemp)
                {
                    for (int i = 0; i < 12; ++i)
                        CP_Index.Add(PlayerPokerCards[SelectCardIndex[i]]);
                    CurPokerType = _DDZ_SP_POKER_TYPE_MOREFIVE;
                    return true;
                }
            }else if(_DDZ_SP_POKER_TYPE_MORETHREEDOUBLE == isAnyType)
            {
                int LastTemp = LastCP[0] / 4;
                //连对判断 大小判断
                if ((temp[0] == temp[1]) && (temp[2] == temp[3]) && (temp[4] == temp[5]) &&
                    (temp[6] == temp[7]) && (temp[8] == temp[9]) && (temp[10] == temp[11]) &&
                    (temp[0] + 1 == temp[2]) && (temp[2] + 1 == temp[4]) && (temp[4] + 1 == temp[6]) &&
                    (temp[6] + 1 == temp[8]) && (temp[8] + 1 == temp[10]) && 7 > temp[0] && temp[0] > LastTemp)
                {
                    for (int i = 0; i < 12; ++i)
                        CP_Index.Add(PlayerPokerCards[SelectCardIndex[i]]);
                    CurPokerType = _DDZ_SP_POKER_TYPE_MORETHREEDOUBLE;
                    return true;
                }
            }else if(_DDZ_SP_POKER_TYPE_PLANE == isAnyType)
            {
                int temp0, temp1, temp2, temp3, LastTemp;
                temp0 = LastCP[0] / 4;
                temp1 = LastCP[2] / 4;
                temp2 = LastCP[3] / 4;
                LastTemp = (temp0 == temp1) ? temp0 : ((temp1 == temp2) ? temp1 : temp2);
                //飞机(三三带三单)判断 大小判断
                if (((temp[0] == temp[1]) && (temp[1] == temp[2]) && (temp[3] == temp[4]) && (temp[4] == temp[5]) && (temp[6] == temp[7]) && (temp[7] == temp[8]) && (temp[0] + 1 == temp[3]) && (temp[3]+1 == temp[6]) && 9 > temp[0] && temp[0] > LastTemp)  ||
                    ((temp[1] == temp[2]) && (temp[2] == temp[3]) && (temp[4] == temp[5]) && (temp[5] == temp[6]) && (temp[7] == temp[8]) && (temp[8] == temp[9]) && (temp[1] + 1 == temp[4]) && (temp[4] + 1 == temp[7]) && 9 > temp[1] && temp[1] > LastTemp)  ||
                    ((temp[2] == temp[3]) && (temp[3] == temp[4]) && (temp[5] == temp[6]) && (temp[6] == temp[7]) && (temp[8] == temp[9]) && (temp[9] == temp[10]) && (temp[2] + 1 == temp[5]) && (temp[5] + 1 == temp[8]) && 9 > temp[2] && temp[2] > LastTemp) ||
                    ((temp[3] == temp[4]) && (temp[4] == temp[5]) && (temp[6] == temp[7]) && (temp[7] == temp[8]) && (temp[9] == temp[10]) && (temp[10] == temp[11]) && (temp[3] + 1 == temp[6]) && (temp[6] + 1 == temp[9]) && 9 > temp[3] && temp[3] > LastTemp))
                {
                    for (int i = 0; i < 12; ++i)
                        CP_Index.Add(PlayerPokerCards[SelectCardIndex[i]]);
                    CurPokerType = _DDZ_SP_POKER_TYPE_PLANE;
                    return true;
                }
            }
        }
        //重置牌位置
        ShowCard(MyCardNum, false);
        for (int i = 0; i < MyCardNum; ++i)
            PlayerPokerCardsInstant[i].GetComponent<PokerEvent>().SetPokerSelectState(false);
        SelectCardIndex.Clear();
        CP_Index.Clear();
        return false;
    }
    //13张 无
    //14张 连对
    bool FourteenCard(int isAnyType = 0)
    {
        int[] temp;
        temp = new int[14];
        for (int k = 0; k < 14; ++k)
            temp[k] = PlayerPokerCards[SelectCardIndex[k]] / 4;

        if (0 == isAnyType)
        {
            if ((temp[0] == temp[1]) && (temp[2] == temp[3]) && (temp[4] == temp[5]) &&
                (temp[6] == temp[7]) && (temp[8] == temp[9]) && (temp[10] == temp[11]) && 
                (temp[12] == temp[13]) && (temp[0] + 1 == temp[2]) && (temp[2] + 1 == temp[4]) && 
                (temp[4] + 1 == temp[6]) && (temp[6] + 1 == temp[8]) && (temp[8] + 1 == temp[10]) &&
                (temp[10] + 1 == temp[12]) && 6 > temp[0])//连对判断
            {
                for (int i = 0; i < 14; ++i)
                    CP_Index.Add(PlayerPokerCards[SelectCardIndex[i]]);
                CurPokerType = _DDZ_SP_POKER_TYPE_MORETHREEDOUBLE;
                return true;
            }
        }
        else
        {
            if (_DDZ_SP_POKER_TYPE_MORETHREEDOUBLE == isAnyType)
            {
                int LastTemp = LastCP[0] / 4;
                //连对判断 大小判断
                if ((temp[0] == temp[1]) && (temp[2] == temp[3]) && (temp[4] == temp[5]) &&
                    (temp[6] == temp[7]) && (temp[8] == temp[9]) && (temp[10] == temp[11]) &&
                    (temp[12] == temp[13]) && (temp[0] + 1 == temp[2]) && (temp[2] + 1 == temp[4]) &&
                    (temp[4] + 1 == temp[6]) && (temp[6] + 1 == temp[8]) && (temp[8] + 1 == temp[10]) &&
                    (temp[10] + 1 == temp[12]) && 6 > temp[0] && temp[0] > LastTemp)
                {
                    for (int i = 0; i < 14; ++i)
                        CP_Index.Add(PlayerPokerCards[SelectCardIndex[i]]);
                    CurPokerType = _DDZ_SP_POKER_TYPE_MORETHREEDOUBLE;
                    return true;
                }
            }
        }
        //重置牌位置
        ShowCard(MyCardNum, false);
        for (int i = 0; i < MyCardNum; ++i)
            PlayerPokerCardsInstant[i].GetComponent<PokerEvent>().SetPokerSelectState(false);
        SelectCardIndex.Clear();
        CP_Index.Clear();
        return false;
    }
    //15张 飞机(三三带三对)
    bool FifteenCard(int isAnyType = 0)
    {
        int[] temp;
        temp = new int[15];
        for (int k = 0; k < 15; ++k)
            temp[k] = PlayerPokerCards[SelectCardIndex[k]] / 4;

        if (0 == isAnyType)
        {
            if(((temp[0] == temp[1]) && (temp[1] == temp[2]) && (temp[3] == temp[4]) && (temp[4] == temp[5]) && (temp[6] == temp[7]) && (temp[7] == temp[8]) && 
                (temp[9] == temp[10]) && (temp[11] == temp[12]) && (temp[13] == temp[14]) && (temp[0] + 1 == temp[3]) && (temp[3] + 1 == temp[6]) && (9 > temp[0]))   ||
               ((temp[0] == temp[1]) && (temp[2] == temp[3]) && (temp[3] == temp[4]) && (temp[5] == temp[6]) && (temp[6] == temp[7]) && (temp[8] == temp[9]) &&
                (temp[9] == temp[10]) && (temp[11] == temp[12]) && (temp[13] == temp[14]) && (temp[2] + 1 == temp[5]) && (temp[5] + 1 == temp[8]) && (9 > temp[2]))   ||
               ((temp[0] == temp[1]) && (temp[2] == temp[3]) && (temp[4] == temp[5]) && (temp[5] == temp[6]) && (temp[7] == temp[8]) && (temp[8] == temp[9]) &&
                (temp[10] == temp[11]) && (temp[11] == temp[12]) && (temp[13] == temp[14]) && (temp[4] + 1 == temp[7]) && (temp[7] + 1 == temp[10]) && (9 > temp[4])) ||
               ((temp[0] == temp[1]) && (temp[2] == temp[3]) && (temp[4] == temp[5]) && (temp[6] == temp[7]) && (temp[7] == temp[8]) && (temp[9] == temp[10]) &&
                (temp[10] == temp[11]) && (temp[12] == temp[13]) && (temp[13] == temp[14]) && (temp[6] + 1 == temp[9]) && (temp[9] + 1 == temp[12]) && (9 > temp[6])))//飞机(三三带三对)判断
            {
                for (int i = 0; i < 15; ++i)
                    CP_Index.Add(PlayerPokerCards[SelectCardIndex[i]]);
                CurPokerType = _DDZ_SP_POKER_TYPE_PLANE;
                return true;
            }
        }
        else
        {
            if(_DDZ_SP_POKER_TYPE_PLANE == isAnyType)
            {
                int temp0, temp1, temp2, temp3, LastTemp;
                temp0 = LastCP[0] / 4;
                temp1 = LastCP[2] / 4;
                temp2 = LastCP[4] / 4;
                temp3 = LastCP[6] / 4;
                LastTemp = (temp0 == temp1) ? temp0 : ((temp1 == temp2) ? temp1 : ((temp2 == temp3) ? temp2 : temp3));
                //飞机(三三带三对)判断 大小判断
                if (((temp[0] == temp[1]) && (temp[1] == temp[2]) && (temp[3] == temp[4]) && (temp[4] == temp[5]) && (temp[6] == temp[7]) && (temp[7] == temp[8]) &&
                     (temp[9] == temp[10]) && (temp[11] == temp[12]) && (temp[13] == temp[14]) && (temp[0] + 1 == temp[3]) && (temp[3] + 1 == temp[6]) && (9 > temp[0]) && temp[0] > LastTemp) ||
                    ((temp[0] == temp[1]) && (temp[2] == temp[3]) && (temp[3] == temp[4]) && (temp[5] == temp[6]) && (temp[6] == temp[7]) && (temp[8] == temp[9]) &&
                     (temp[9] == temp[10]) && (temp[11] == temp[12]) && (temp[13] == temp[14]) && (temp[2] + 1 == temp[5]) && (temp[5] + 1 == temp[8]) && (9 > temp[2]) && temp[2] > LastTemp) ||
                    ((temp[0] == temp[1]) && (temp[2] == temp[3]) && (temp[4] == temp[5]) && (temp[5] == temp[6]) && (temp[7] == temp[8]) && (temp[8] == temp[9]) &&
                     (temp[10] == temp[11]) && (temp[11] == temp[12]) && (temp[13] == temp[14]) && (temp[4] + 1 == temp[7]) && (temp[7] + 1 == temp[10]) && (9 > temp[4]) && temp[4] > LastTemp) ||
                    ((temp[0] == temp[1]) && (temp[2] == temp[3]) && (temp[4] == temp[5]) && (temp[6] == temp[7]) && (temp[7] == temp[8]) && (temp[9] == temp[10]) &&
                     (temp[10] == temp[11]) && (temp[12] == temp[13]) && (temp[13] == temp[14]) && (temp[6] + 1 == temp[9]) && (temp[9] + 1 == temp[12]) && (9 > temp[6]) && temp[6] > LastTemp))
                {
                    for (int i = 0; i < 15; ++i)
                        CP_Index.Add(PlayerPokerCards[SelectCardIndex[i]]);
                    CurPokerType = _DDZ_SP_POKER_TYPE_PLANE;
                    return true;
                }
            }
        }
        //重置牌位置
        ShowCard(MyCardNum, false);
        for (int i = 0; i < MyCardNum; ++i)
            PlayerPokerCardsInstant[i].GetComponent<PokerEvent>().SetPokerSelectState(false);
        SelectCardIndex.Clear();
        CP_Index.Clear();
        return false;
    }
    //16张 连对
    bool SixteenCard(int isAnyType = 0)
    {
        int[] temp;
        temp = new int[16];
        for (int k = 0; k < 16; ++k)
            temp[k] = PlayerPokerCards[SelectCardIndex[k]] / 4;

        if (0 == isAnyType)
        {
            if ((temp[0] == temp[1]) && (temp[2] == temp[3]) && (temp[4] == temp[5]) &&
                (temp[6] == temp[7]) && (temp[8] == temp[9]) && (temp[10] == temp[11]) &&
                (temp[12] == temp[13]) && (temp[14] == temp[15]) &&  (temp[0] + 1 == temp[2]) && 
                (temp[2] + 1 == temp[4]) && (temp[4] + 1 == temp[6]) && (temp[6] + 1 == temp[8]) && 
                (temp[8] + 1 == temp[10]) && (temp[10] + 1 == temp[12]) && (temp[12] + 1 == temp[14]) &&
                5 > temp[0])//连对判断
            {
                for (int i = 0; i < 16; ++i)
                    CP_Index.Add(PlayerPokerCards[SelectCardIndex[i]]);
                CurPokerType = _DDZ_SP_POKER_TYPE_MORETHREEDOUBLE;
                return true;
            }
        }
        else
        {
            if (_DDZ_SP_POKER_TYPE_MORETHREEDOUBLE == isAnyType)
            {
                int LastTemp = LastCP[0] / 4;
                //连对判断 大小判断
                if ((temp[0] == temp[1]) && (temp[2] == temp[3]) && (temp[4] == temp[5]) &&
                    (temp[6] == temp[7]) && (temp[8] == temp[9]) && (temp[10] == temp[11]) &&
                    (temp[12] == temp[13]) && (temp[14] == temp[15]) && (temp[0] + 1 == temp[2]) &&
                    (temp[2] + 1 == temp[4]) && (temp[4] + 1 == temp[6]) && (temp[6] + 1 == temp[8]) &&
                    (temp[8] + 1 == temp[10]) && (temp[10] + 1 == temp[12]) && (temp[12] + 1 == temp[14]) &&
                    5 > temp[0] && temp[0] > LastTemp)
                {
                    for (int i = 0; i < 16; ++i)
                        CP_Index.Add(PlayerPokerCards[SelectCardIndex[i]]);
                    CurPokerType = _DDZ_SP_POKER_TYPE_MORETHREEDOUBLE;
                    return true;
                }
            }
        }
        //重置牌位置
        ShowCard(MyCardNum, false);
        for (int i = 0; i < MyCardNum; ++i)
            PlayerPokerCardsInstant[i].GetComponent<PokerEvent>().SetPokerSelectState(false);
        SelectCardIndex.Clear();
        CP_Index.Clear();
        return false;
    }
    //17张 无
    //18张 连对  
    bool EighteenCard(int isAnyType = 0)
    {
        int[] temp;
        temp = new int[18];
        for (int k = 0; k < 18; ++k)
            temp[k] = PlayerPokerCards[SelectCardIndex[k]] / 4;

        if (0 == isAnyType)
        {
            if ((temp[0] == temp[1]) && (temp[2] == temp[3]) && (temp[4] == temp[5]) &&
                (temp[6] == temp[7]) && (temp[8] == temp[9]) && (temp[10] == temp[11]) &&
                (temp[12] == temp[13]) && (temp[14] == temp[15]) && (temp[16] == temp[17]) &&
                (temp[0] + 1 == temp[2]) && (temp[2] + 1 == temp[4]) && (temp[4] + 1 == temp[6]) && 
                (temp[6] + 1 == temp[8]) && (temp[8] + 1 == temp[10]) && (temp[10] + 1 == temp[12]) && 
                (temp[12] + 1 == temp[14]) && (temp[14] + 1 == temp[16]) && 4 > temp[0])//连对判断
            {
                for (int i = 0; i < 18; ++i)
                    CP_Index.Add(PlayerPokerCards[SelectCardIndex[i]]);
                CurPokerType = _DDZ_SP_POKER_TYPE_MORETHREEDOUBLE;
                return true;
            }
        }
        else
        {
            if (_DDZ_SP_POKER_TYPE_MORETHREEDOUBLE == isAnyType)
            {
                int LastTemp = LastCP[0] / 4;
                //连对判断 大小判断
                if ((temp[0] == temp[1]) && (temp[2] == temp[3]) && (temp[4] == temp[5]) &&
                    (temp[6] == temp[7]) && (temp[8] == temp[9]) && (temp[10] == temp[11]) &&
                    (temp[12] == temp[13]) && (temp[14] == temp[15]) && (temp[16] == temp[17]) &&
                    (temp[0] + 1 == temp[2]) && (temp[2] + 1 == temp[4]) && (temp[4] + 1 == temp[6]) &&
                    (temp[6] + 1 == temp[8]) && (temp[8] + 1 == temp[10]) && (temp[10] + 1 == temp[12]) &&
                    (temp[12] + 1 == temp[14]) && (temp[14] + 1 == temp[16]) && 4 > temp[0] && temp[0] > LastTemp)
                {
                    for (int i = 0; i < 18; ++i)
                        CP_Index.Add(PlayerPokerCards[SelectCardIndex[i]]);
                    CurPokerType = _DDZ_SP_POKER_TYPE_MORETHREEDOUBLE;
                    return true;
                }
            }
        }
        //重置牌位置
        ShowCard(MyCardNum, false);
        for (int i = 0; i < MyCardNum; ++i)
            PlayerPokerCardsInstant[i].GetComponent<PokerEvent>().SetPokerSelectState(false);
        SelectCardIndex.Clear();
        CP_Index.Clear();
        return false;
    }
    //19张 无
    //20张 连对
    bool TwentyCard(int isAnyType = 0)
    {
        int[] temp;
        temp = new int[20];
        for (int k = 0; k < 20; ++k)
            temp[k] = PlayerPokerCards[SelectCardIndex[k]] / 4;

        if (0 == isAnyType)
        {
            if ((temp[0] == temp[1]) && (temp[2] == temp[3]) && (temp[4] == temp[5]) &&
                (temp[6] == temp[7]) && (temp[8] == temp[9]) && (temp[10] == temp[11]) &&
                (temp[12] == temp[13]) && (temp[14] == temp[15]) && (temp[16] == temp[17]) &&
                (temp[18] == temp[19]) && (temp[0] + 1 == temp[2]) && (temp[2] + 1 == temp[4]) && 
                (temp[4] + 1 == temp[6]) && (temp[6] + 1 == temp[8]) && (temp[8] + 1 == temp[10]) && 
                (temp[10] + 1 == temp[12]) && (temp[12] + 1 == temp[14]) && (temp[14] + 1 == temp[16]) &&
                (temp[16] + 1 == temp[18]) && 3 > temp[0])//连对判断
            {
                for (int i = 0; i < 20; ++i)
                    CP_Index.Add(PlayerPokerCards[SelectCardIndex[i]]);
                CurPokerType = _DDZ_SP_POKER_TYPE_MORETHREEDOUBLE;
                return true;
            }
        }
        else
        {
            if (_DDZ_SP_POKER_TYPE_MORETHREEDOUBLE == isAnyType)
            {
                int LastTemp = LastCP[0] / 4;
                //连对判断 大小判断
                if ((temp[0] == temp[1]) && (temp[2] == temp[3]) && (temp[4] == temp[5]) &&
                    (temp[6] == temp[7]) && (temp[8] == temp[9]) && (temp[10] == temp[11]) &&
                    (temp[12] == temp[13]) && (temp[14] == temp[15]) && (temp[16] == temp[17]) &&
                    (temp[18] == temp[19]) && (temp[0] + 1 == temp[2]) && (temp[2] + 1 == temp[4]) &&
                    (temp[4] + 1 == temp[6]) && (temp[6] + 1 == temp[8]) && (temp[8] + 1 == temp[10]) &&
                    (temp[10] + 1 == temp[12]) && (temp[12] + 1 == temp[14]) && (temp[14] + 1 == temp[16]) &&
                    (temp[16] + 1 == temp[18]) && 3 > temp[0] && temp[0] > LastTemp)
                {
                    for (int i = 0; i < 20; ++i)
                        CP_Index.Add(PlayerPokerCards[SelectCardIndex[i]]);
                    CurPokerType = _DDZ_SP_POKER_TYPE_MORETHREEDOUBLE;
                    return true;
                }
            }
        }
        //重置牌位置
        ShowCard(MyCardNum, false);
        for (int i = 0; i < MyCardNum; ++i)
            PlayerPokerCardsInstant[i].GetComponent<PokerEvent>().SetPokerSelectState(false);
        SelectCardIndex.Clear();
        CP_Index.Clear();
        return false;
    }
}
