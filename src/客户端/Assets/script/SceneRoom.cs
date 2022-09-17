using UnityEngine;
using UnityEngine.UI;
using System.Collections;
using System.Collections.Generic;

public class SceneRoom : MonoBehaviour
{
    //所有UI
	public GameObject[] m_UI;
	//public Image[] CardNumImage;
	public Sprite[] UI_6;//2个
	public Sprite[] FightUI;//4个
	public Sprite[] CardNumUI;//10个
	public Sprite[] PlayerStatus;//2个
	int CureGameType;
	int ButtonDown;
	public GameObject[] SoundFight;//争夺地主音效 类型1
	public GameObject[] SoundPlaySpecial;//出牌音效 类型2
	public GameObject[] SoundPlay;//出牌音效 类型3
	public GameObject[] SoundFinish;//游戏完成音效 类型4
	public GameObject[] Effect;//游戏特效
	//设置争夺UI显示
	public void SetFightUI(int index,int num)
	{
		m_UI [index].GetComponent<Image>().sprite = FightUI [num];
	}
	//设置玩家牌数UI显示
	public void SetCardNumUI(int index,int num)
	{
		m_UI [index].GetComponent<Image>().sprite = CardNumUI [num];
	}
	//显示玩家身份
	public void SetPlayerStatus(int index,int num)
	{
		m_UI [index].GetComponent<Image> ().sprite = PlayerStatus [num];
	}

	//实例化音效
	public void Instantiate_Sound(int Type,int Index)
	{
		switch (Type) 
		{
		case 1:Instantiate(SoundFight[Index]);break;
		case 2:Instantiate(SoundPlaySpecial[Index]);break;
		case 3:Instantiate(SoundPlay[Index]);break;
		case 4:Instantiate(SoundFinish[Index]);break;
		}
	}

	//实例化特效
	public void Instantiate_Effect(int Type,Vector3 pos)
	{
		switch (Type) 
		{
		case 0:Instantiate(Effect[0],pos,Effect[0].transform.rotation);break;
		case 1:Instantiate(Effect[1],pos,Effect[1].transform.rotation);break;
		}
	}

    void EnableButtonUI(bool prepare, bool cancel,bool back)
    {
        m_UI[6].GetComponent<Button>().interactable = prepare;
        m_UI[7].GetComponent<Button>().interactable = cancel;
        m_UI[8].GetComponent<Button>().interactable = back;
    }

	void EnableUI(bool enable)
	{
		m_UI[0].SetActive(enable);
		m_UI[2].SetActive(enable);
		m_UI[4].SetActive(enable);
		m_UI[6].SetActive(enable);
		m_UI[7].SetActive(enable);
		m_UI[8].SetActive(enable);
		m_UI[9].SetActive(enable);
	}

    void AddInfo(string info)
    {
        Text tex =
            m_UI[3].GetComponent<Text>();
        tex.text += info + "\n";
        RectTransform rt =
            m_UI[2].GetComponent<RectTransform>();
        //rt.SetInsetAndSizeFromParentEdge(
        //    RectTransform.Edge.Bottom,
        //    0,
        //    tex.preferredHeight);
    }

    void UpdatePlayer()
    {
        if (null == MainScript.m_Players)
            return;

        //得到玩家名字
        string[] room_players_id =
            MainScript.GetRoomPlayer(MainScript.m_Room);

        //得到玩家座位
        int[] room_players_seat =
            new int[room_players_id.Length];
        for (int i = 0; i < room_players_id.Length; ++i)
        {
            room_players_seat[i] =
                MainScript.m_Players[room_players_id[i]].seat;
        }

        //根据座位排序名字、座位
        for (int i = room_players_seat.Length - 1; i > 0; --i)
        {
            for (int j = 0; j < i; ++j)
            {
                if (room_players_seat[j] > room_players_seat[j + 1])
                {
                    string t1 = room_players_id[j];
                    room_players_id[j] = room_players_id[j + 1];
                    room_players_id[j + 1] = t1;

                    int t2 = room_players_seat[j];
                    room_players_seat[j] = room_players_seat[j + 1];
                    room_players_seat[j + 1] = t2;
                }
            }
        }

        //得到信息
        string players_info = "";
        for (int i = 0; i < room_players_id.Length; ++i)
        {
            players_info += "" + room_players_seat[i] + "-";
            players_info += room_players_id[i] + "-";
            players_info += MainScript.m_Players[room_players_id[i]].score + "-";
            switch (MainScript.m_Players[room_players_id[i]].state)
            {
                case MainScript._PLAYER_OL_ROOM:
                    players_info += "房间\n";
                    break;
                case MainScript._PLAYER_OL_PREPARE:
                    players_info += "准备\n";
                    break;
                case MainScript._PLAYER_OL_GAME:
                    players_info += "游戏\n";
                    break;
            }
        }

        //更新信息
        m_UI[1].GetComponent<Text>().text = players_info;
    }


    void Awake()
    {
			MainScript.m_TC.OnRecvData += OnRecv;
			MainScript.m_TC.OnDisConnect += OnDisConnect;


		SetFightUI (10, 0);
		SetFightUI (11, 1);
		for(int i=10;i<16;++i)
			m_UI[i].SetActive(false);
		CureGameType = -1;
		ButtonDown = -1;
    }

    //本脚本中用于接受数据的委托函数
    void OnRecv(byte[] data)
    {
        MainScript.m_RP.set_data(data, 0, data.Length);
        int MsgType = -1;
        bool isOK = MainScript.m_RP.get_int(ref MsgType);
        //分情况讨论
        switch (MsgType)
        {
            case MainScript.SP_NEW_PLAYER_LOGIN:
                {
                    on_SP_NEW_PLAYER_LOGIN(MainScript.m_RP);
                    break;
                }
            case MainScript.SP_PLAYER_TALK_ROOM:
                {
                    on_SP_PLAYER_TALK_ROOM(MainScript.m_RP);
                    break;
                }
            case MainScript.SP_PLAYER_PREPARE_ROOM:
                {
                    on_SP_PLAYER_PREPARE_ROOM(MainScript.m_RP);
                    break;
                }
            case MainScript.SP_BEGIN_PLAY_ROOM:
                {
                    on_SP_BEGIN_PLAY_ROOM(MainScript.m_RP);
                    break;
                }
            case MainScript.SP_PLAYER_CANCEL_ROOM:
                {
                    on_SP_PLAYER_CANCEL_ROOM(MainScript.m_RP);
                    break;
                }

            case MainScript.SP_PLAYER_QUIT_ROOM:
                {
                    on_SP_PLAYER_QUIT_ROOM(MainScript.m_RP);
                    break;
                }
   
            case MainScript.SP_PLAY_ROOM:
                {
                    on_SP_PLAY_ROOM(MainScript.m_RP);
                    break;
                }

        }
    }

    //本脚本中用于处理断开连接的委托函数
    void OnDisConnect()
    {
        Debug.Log("");
    }
    void OnEnable()
    {
        //更新聊天信息
        m_UI[3].GetComponent<Text>().text = "";

        //更新按钮状态
		EnableButtonUI(true, true,  true);

        //更新玩家信息
        UpdatePlayer();
    }

    //按钮响应处理
    public void On_Button_Send()
    {
		Debug.Log("send");
        //得到输入框信息
        Text tex =
            m_UI[5].GetComponent<Text>();

        //如果没有信息
        if (0 == tex.text.Length)
        {
            AddInfo("说话不能为空");
            return;
        }

        //发送CP_TALK_ROOM
        MainScript.m_SP.clear();
        MainScript.m_SP.add_int(MainScript.CP_TALK_ROOM);
        MainScript.m_SP.add_string(tex.text);
        MainScript.m_TC.Send(
            MainScript.m_SP.data(),
            0,
            MainScript.m_SP.size());

        //清空发送话语
        m_UI[9].GetComponent<InputField>().text = "";
    }
    public void On_Button_Prepare()
    {
		if (m_UI [6].GetComponent<Image> ().sprite == UI_6 [0]) {
			m_UI [6].GetComponent<Image> ().sprite = UI_6 [1];
			//CP_PREPARE_ROOM
			MainScript.m_SP.clear ();
			MainScript.m_SP.add_int (MainScript.CP_PREPARE_ROOM);
			MainScript.m_TC.Send (
			MainScript.m_SP.data (),
			0,
			MainScript.m_SP.size ());
		} else {
			m_UI [6].GetComponent<Image> ().sprite = UI_6 [0];
		}   
    }
    public void On_Button_Cancel()
    {
        //CP_CANCEL_ROOM
        MainScript.m_SP.clear();
        MainScript.m_SP.add_int(MainScript.CP_CANCEL_ROOM);
        MainScript.m_TC.Send(
            MainScript.m_SP.data(),
            0,
            MainScript.m_SP.size());
    }
    public void On_Button_Stop()
    {
		Application.Quit();
    }
    public void On_Button_Return()
    {
        //CP_QUIT_ROOM
        MainScript.m_SP.clear();
        MainScript.m_SP.add_int(MainScript.CP_QUIT_ROOM);
		MainScript.m_SP.add_int(MainScript.m_Room);
        MainScript.m_TC.Send(
            MainScript.m_SP.data(),
            0,
            MainScript.m_SP.size());

    }

	public void On_FightLeft()
	{
		m_UI [10].SetActive (false);
		m_UI [11].SetActive (false);
		CureGameType = 0;
		ButtonDown = 0;
	}
	public void On_FightRight()
	{
		m_UI [10].SetActive (false);
		m_UI [11].SetActive (false);
		CureGameType = 0;
		ButtonDown = 1;
	}

	public void On_Check()
	{
		m_UI [18].SetActive (false);
		m_UI [19].SetActive (false);
		CureGameType = 1;
		ButtonDown = 0;
	}
	public void On_PlayCard()
	{
		CureGameType = 1;
		ButtonDown = 1;
	}

	public void On_BackHall()
	{
		//CureGameType = 2;
		//ButtonDown = 0;
		MainScript.m_HallInfo.Clear ();
		//MainScript.m_TC.OnRecvData -= OnRecv;
		//MainScript.m_TC.OnDisConnect -= OnDisConnect;
		On_Button_Return ();

		//Application.LoadLevel (2);
	}
	public void On_QuitGame()
	{
		//CureGameType = 2;
		//ButtonDown = 1;
		Application.Quit();
	}

    void Update()
    {
		//游戏更新
		if (-1 != MainScript.m_Room)
		{
			GameClient gc = MainScript.m_GameClients[MainScript.m_GameRooms[MainScript.m_Room]];
			if (gc.m_GameClientRun)
			{
				//输入更新
				if (ButtonDown != -1)
				{
					gc.Input(CureGameType,ButtonDown);
					ButtonDown = -1;
				}
				
				//逻辑更新
				gc.OnUpdate();
			}
		}
    }

    void on_SP_NEW_PLAYER_LOGIN(RecvPackage rp)
    {
        //得到账号、得分
        string id = "";
        int score = -1;
        rp.get_string(ref id);
        rp.get_int(ref score);

        //添加信息
        MainScript.AddHallInfo(id + "登录大厅");

        //创建新玩家信息
        MainScript.PLAYER player = new MainScript.PLAYER();
        player.state = MainScript._PLAYER_OL_HALL;
        player.score = score;
        player.room = -1;
        player.seat = -1;

        //入字典
        MainScript.m_Players.Add(id, player);
    }

    void on_SP_PLAYER_TALK_ROOM(RecvPackage rp)
    {
		Debug.Log("talk");
        //得到账号、话语
        string id = "";
        string talk = "";
        rp.get_string(ref id);
        rp.get_string(ref talk);

        //判断说话玩家是否在本人所在房间
        if (MainScript.m_Room == MainScript.m_Players[id].room)
        {
            //添加房间对话
            AddInfo(id + "说：" + talk);
        }
    }
    void on_SP_PLAYER_PREPARE_ROOM(RecvPackage rp)
    {
        //得到账号
        string id = "";
        rp.get_string(ref id);

        //更新该玩家状态
        MainScript.m_Players[id].state = MainScript._PLAYER_OL_PREPARE;

        //如果准备的玩家是在本人所在房间
        if (MainScript.m_Players[id].room == MainScript.m_Room)
        {
            //更新当前房间玩家状态
            UpdatePlayer();
        }

        //如果是自己
        if (id == MainScript.m_Me)
        {
            //更新按钮
			EnableButtonUI(true, true,true);
        }
    }
    void on_SP_PLAYER_CANCEL_ROOM(RecvPackage rp)
    {
        //得到账号
        string id = "";
        rp.get_string(ref id);

        //更新该玩家状态
        MainScript.m_Players[id].state = MainScript._PLAYER_OL_ROOM;

        //如果撤销的玩家是在本人所在房间
        if (MainScript.m_Players[id].room == MainScript.m_Room)
        {
            //更新当前房间玩家状态
            UpdatePlayer();
        }

        //如果是自己
        if (id == MainScript.m_Me)
        {
            //更新按钮
            EnableButtonUI(true, false, true);
        }
    }
    void on_SP_PLAYER_QUIT_ROOM(RecvPackage rp)
    {
        //得到账号
        string id = "";
        rp.get_string(ref id);

        //添加信息
        MainScript.AddHallInfo(id + "回到大厅");

        //记录退出的房间
        int quit_room =
            MainScript.m_Players[id].room;

        //更新玩家信息
        MainScript.m_Players[id].state = MainScript._PLAYER_OL_HALL;
        MainScript.m_Players[id].room = -1;
        MainScript.m_Players[id].seat = -1;

        //退出房间是否是本人所在房间
        if (MainScript.m_Room == quit_room)
        {
            //更新房间中玩家信息
            UpdatePlayer();
        }

        //是否就是本人
        if (MainScript.m_Me == id)
        {
			//去掉委托
			MainScript.m_TC.OnRecvData -= OnRecv;
			MainScript.m_TC.OnDisConnect -= OnDisConnect;
			//切换场景
			Application.LoadLevel(2);
        }
    }
    void on_SP_BEGIN_PLAY_ROOM(RecvPackage rp)
    {
        //得到房间
        int room = -1;
        rp.get_int(ref room);

        //更新玩家信息
        string[] players_id = MainScript.GetRoomPlayer(room);
        for (int i = 0; i < players_id.Length; ++i)
        {
            MainScript.m_Players[players_id[i]].state
                = MainScript._PLAYER_OL_GAME;
        }

        //是本人所在房间
        if (MainScript.m_Room == room)
        {
            //更新UI
			EnableUI(false);

            //更新房间中玩家信息
            UpdatePlayer();

            //得到游戏对象
            GameClient gc =
                MainScript.m_GameClients[MainScript.m_GameRooms[MainScript.m_Room]];

            //设置游戏对象状态
            gc.m_GameClientRun = true;

            //调用游戏对象初始化
            gc.Init();
        }
    }
    void on_SP_PLAY_ROOM(RecvPackage rp)
    {
        //得到游戏对象
        GameClient gc =
            MainScript.m_GameClients[MainScript.m_GameRooms[MainScript.m_Room]];

        //得到游戏数据
        byte[] ba = null;
        rp.get_byte_array(ref ba);

        //调用游戏对象接收服务器数据
        gc.OnServerSendData(ba, 0, ba.Length);
    }

}
