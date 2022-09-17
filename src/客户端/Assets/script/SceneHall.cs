using UnityEngine;
using UnityEngine.UI;
using System.Collections;
using System.Collections.Generic;

public class SceneHall : MonoBehaviour
{
    //所有UI
	public GameObject[] m_UI;

    void UpdateUI(bool hall, bool player, bool room)
    {
        //更新大厅信息
        if (hall)
        {
            if (MainScript.m_HallInfo.Count > 100)
            {
                int del_num = MainScript.m_HallInfo.Count - 100;
                MainScript.m_HallInfo.RemoveRange(0, del_num);
            }
            Text tex = m_UI[14].GetComponent<Text>();
          	 tex.text = "";
            for (int i = 0; i < MainScript.m_HallInfo.Count; ++i)
                tex.text += MainScript.m_HallInfo[i] + "\n";
			RectTransform rt =
				m_UI[13].GetComponent<RectTransform>();
			rt.SetInsetAndSizeFromParentEdge(
				RectTransform.Edge.Bottom,
				0,
				tex.preferredHeight);
        }

        //更新大厅玩家信息
        if (player)
        {
            Text t = m_UI[12].GetComponent<Text>();
            t.text = "";
            foreach (KeyValuePair<string, MainScript.PLAYER> kvp in MainScript.m_Players)
            {
                if (kvp.Value.state == MainScript._PLAYER_OL_HALL)
                    t.text += kvp.Key + "-" + kvp.Value.score + "\n";
            }
        }

        //更新房间玩家信息
        if (room)
        {
            string[] s = new string[MainScript.m_GameRooms.Count];
            for (int i = 0; i < s.Length; ++i)
                s[i] = "";
            foreach (KeyValuePair<string, MainScript.PLAYER> kvp in MainScript.m_Players)
            {
                if (kvp.Value.state != MainScript._PLAYER_OL_HALL)
                {
                    string si = "";
                    switch (kvp.Value.state)
                    {
                        case MainScript._PLAYER_OL_ROOM:
                            si = "-房间";
                            break;
                        case MainScript._PLAYER_OL_PREPARE:
                            si = "-准备";
                            break;
                        case MainScript._PLAYER_OL_GAME:
                            si = "-游戏";
                            break;
                    }
                    s[kvp.Value.room] += kvp.Key + "-" + kvp.Value.score + si + " ";
                }
            }
            for (int i = 0, j = 2; i < s.Length; ++i, j += 2)
            {
                Text tex = m_UI[j].GetComponent<Text>();
                tex.text = s[i];
            }
        }
    }

    void Awake()
	{
		//将委托加入
		MainScript.m_TC.OnRecvData += OnRecv;
		MainScript.m_TC.OnDisConnect += OnDisConnect;
    }

    //本脚本中用于接受数据的委托函数
    void OnRecv(byte[] data)
    {
        MainScript.m_RP.set_data(data, 0, data.Length);
        int MsgType = -1;
        bool isOK = MainScript.m_RP.get_int(ref MsgType);
        if (isOK) {
			//分情况讨论
			switch (MsgType) {
			case MainScript.SP_NEW_PLAYER_LOGIN:
				{
					on_SP_NEW_PLAYER_LOGIN (MainScript.m_RP);
					break;
				}
			case MainScript.SP_ROOM_FULL:
				{
					on_SP_ROOM_FULL (
                            MainScript.m_RP);
					break;
				}
			case MainScript.SP_CHOOSE_ROOM_OK:
				{
					on_SP_CHOOSE_ROOM_OK (
                            MainScript.m_RP);
					break;
				}
			case MainScript.SP_PLAYER_TALK_HALL:
				{
					on_SP_PLAYER_TALK_HALL (
                            MainScript.m_RP);
					break;
				} 
			case MainScript.SP_PLAYER_QUIT_HALL:
			{
				break;
			}
			}
		}
    }
    //本脚本中用于处理断开连接的委托函数
    void OnDisConnect()
    {
		int MsgType = -1;
		bool isOK = MainScript.m_RP.get_int(ref MsgType);
		if (!isOK)
			Application.LoadLevel(1);
    }

    void OnEnable()
    {
		UpdateUI (true, true, true);
    }
    void Update()
    {

    }

    //选择房间
    void ChooseRoom(int room)
    {
        //发送CP_CHOOSE_ROOM
        MainScript.m_SP.clear();
        MainScript.m_SP.add_int(MainScript.CP_CHOOSE_ROOM);
        MainScript.m_SP.add_int(room);
        MainScript.m_TC.Send(MainScript.m_SP.data(), 0, MainScript.m_SP.size());
    }

    //按钮消息响应
    public void On_Button_Room0()
    {
        MainScript.m_Room = 0;
        ChooseRoom(MainScript.m_Room);
    }
    public void On_Button_Room1()
    {
        MainScript.m_Room = 1;
        ChooseRoom(MainScript.m_Room);
    }
    public void On_Button_Room2()
    {
        MainScript.m_Room = 2;
        ChooseRoom(MainScript.m_Room);
    }
    public void On_Button_Room3()
    {
        MainScript.m_Room = 3;
        ChooseRoom(MainScript.m_Room);
    }
    public void On_Button_Room4()
    {
        MainScript.m_Room = 4;
        ChooseRoom(MainScript.m_Room);
    }
    public void On_Button_Send()
    {
        //得到话语
        string talk =
            m_UI[16].GetComponent<Text>().text;

        //判断是否为空
        if (0 == talk.Length)
        {
            MainScript.AddHallInfo("说话不能为空");
            UpdateUI(true, false, false);
            return;
        }

        //发送CP_TALK_HALL
        MainScript.m_SP.clear();
        MainScript.m_SP.add_int(MainScript.CP_TALK_HALL);
        MainScript.m_SP.add_string(talk);
        MainScript.m_TC.Send(
            MainScript.m_SP.data(),
            0,
            MainScript.m_SP.size());

        //清空话语
        m_UI[18].GetComponent<InputField>().text = "";
    }
    public void On_Button_Quit()
    {
        //返回大厅
		MainScript.m_SP.clear();
		MainScript.m_SP.add_int(MainScript.CP_QUIT_HALL);
		MainScript.m_TC.Send(
			MainScript.m_SP.data(),
			0,
			MainScript.m_SP.size());

		//去掉委托
		MainScript.m_TC.OnRecvData -= OnRecv;
		MainScript.m_TC.OnDisConnect -= OnDisConnect;

		MainScript.m_Players.Remove (MainScript.m_Me);
		//断开网络
		MainScript.m_TC.Disconnect ();
        //切换场景
        Application.LoadLevel(1);
    }

    //网络消息响应
    void on_SP_NEW_PLAYER_LOGIN(RecvPackage rp)
    {
        //得到账号、得分
        string id = "";
        int score = -1;
        rp.get_string(ref id);
        rp.get_int(ref score);

        //添加信息
        MainScript.AddHallInfo(id + "登录大厅");
        UpdateUI(true, false, false);

        //不是自己
        if (MainScript.m_Me != id)
        {
            //创建新玩家信息
            MainScript.PLAYER player = new MainScript.PLAYER();
            player.state = MainScript._PLAYER_OL_HALL;
            player.score = score;
            player.room = -1;
            player.seat = -1;

            //入字典
            MainScript.m_Players.Add(id, player);

            //更新信息
            UpdateUI(false, true, true);
        }
    }
    void on_SP_ROOM_FULL(RecvPackage rp)
    {
        //添加信息
        MainScript.AddHallInfo(MainScript.m_Room + "号房间玩家已满");
        UpdateUI(true, false, false);
    }
    void on_SP_CHOOSE_ROOM_OK(RecvPackage rp)
    {
        //得到座位
		string id = "";
        int seat = -1;
		rp.get_string(ref id);
        rp.get_int(ref seat);

        //更新自己
        MainScript.m_Players[MainScript.m_Me].state = MainScript._PLAYER_OL_ROOM;
        MainScript.m_Players[MainScript.m_Me].room = MainScript.m_Room;
        MainScript.m_Players[MainScript.m_Me].seat = seat;

		//更新信息
		if (MainScript.m_Me != id) 
		{
			UpdateUI (false, false, true);
		}
		//去掉委托
		MainScript.m_TC.OnRecvData -= OnRecv;
		MainScript.m_TC.OnDisConnect -= OnDisConnect;
		//切换场景
		Application.LoadLevel(3);
    }

    void on_SP_PLAYER_TALK_HALL(RecvPackage rp)
    {
        //得到账号、话语
        string id = "";
        string talk = "";
        rp.get_string(ref id);
        rp.get_string(ref talk);

        //添加信息
        MainScript.AddHallInfo(id + "说：" + talk);
        UpdateUI(true, false, false);
    }	
}
