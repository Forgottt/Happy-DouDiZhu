using UnityEngine;
using System.Collections;
using System.Collections.Generic;

public  class MainScript : MonoBehaviour
{
	//游戏协议

	//客户端发送给服务器的协议
	public const int CP_LOGIN = 0; //s(账号)、s(密码)
	public const int CP_REGISTER = 1; //s(账号)、s(密码)
	public const int CP_CHOOSE_ROOM = 2; //i(房间)
	public const int CP_TALK_HALL = 3; //s(话语)
	public const int CP_TALK_ROOM = 4; //s(话语)
	public const int CP_PREPARE_ROOM = 5; //null
	public const int CP_CANCELPREPARE_ROOM = 10; //null
	public const int CP_CANCEL_ROOM = 6; //null
	public const int CP_STOP_ROOM = 7; //null
	public const int CP_QUIT_ROOM = 8; //null
	public const int CP_PLAY_ROOM = 9; //带2级协议
	public const int CP_QUIT_HALL = 10; //null

	//服务器发送给客户端的协议
	public const int SP_NO_ID = 0; //null
	public const int SP_WRONG_PASSWORD = 1; //null
	public const int SP_LOGIN_OK = 2; //i(所有登陆玩家数量n)、a({s账号,i状态,i得分,i房间,i座位}*n)
	public const int SP_NEW_PLAYER_LOGIN = 3;//s(账号)、i(得分)
	public const int SP_SAME_ID = 4; //null
	public const int SP_REGISTER_OK = 5; //null
	public const int SP_ROOM_FULL = 6; //null
	public const int SP_CHOOSE_ROOM_OK = 7; //i(座位)
	public const int SP_PLAYER_ENTER_ROOM = 8; //s(账号)、i(房间)、i(座位)
	public const int SP_PLAYER_TALK_HALL = 9; //s(账号)、s(话语)
	public const int SP_PLAYER_QUIT_HALL = 10; //s(账号)
	public const int SP_PLAYER_TALK_ROOM = 11; //s(账号)、s(话语)
	public const int SP_PLAYER_PREPARE_ROOM = 12; //s(账号)
	public const int SP_BEGIN_PLAY_ROOM = 13; //i(房间)
	public const int SP_PLAYER_CANCEL_ROOM = 14; //s(账号)
	public const int SP_END_PLAY_ROOM_STOP = 15; //i(房间)、s(账号)
	public const int SP_PLAYER_QUIT_ROOM = 16; //s(账号)
	public const int SP_END_PLAY_ROOM = 17; //i(房间)
	public const int SP_PLAY_ROOM = 18; //带2级协议
	public const int SP_PLAYER_DISCONNECT_HALL = 19; //s(账号)
	public const int SP_PLAYER_DISCONNECT_ROOM = 20; //s(账号)
	public const int SP_PLAYER_DISCONNECT_PREPARE = 21; //s(账号)
	public const int SP_PLAYER_DISCONNECT_GAME = 22; //s(账号)
	public const int SP_END_PLAY_ROOM_DISCONNECT = 23; //i(房间)、s(账号)

	//玩家状态
	public const int _PLAYER_OL_HALL = 1;
	public const int _PLAYER_OL_ROOM = 2;
	public const int _PLAYER_OL_PREPARE = 3;
	public const int _PLAYER_OL_GAME = 4;

	//玩家信息
	public class PLAYER
	{
		public int state;
        public int score;
        public int room;
        public int seat;
		public PLAYER()
		{
			state = -1;
			score = -1;
			room = -1;
			seat = -1;
		}
	}

	//玩家字典
	public static Dictionary<string, PLAYER> m_Players;

	//得到指定房间玩家
	public static string[] GetRoomPlayer(int room)
	{
		//得到该房间玩家的数量
		int room_player_num = 0;
		foreach (KeyValuePair<string, MainScript.PLAYER> kvp in m_Players)
		{
			if (kvp.Value.room == room)
				room_player_num += 1;
		}

		//创建该房间玩家id数组
		string[] room_player = new string[room_player_num];

		//得到该房间玩家所有id
		int i = 0;
		foreach (KeyValuePair<string, MainScript.PLAYER> kvp in m_Players)
		{
			if (kvp.Value.room == room)
				room_player[i++] = kvp.Key;
		}
		return room_player;
	}

	//大厅信息
	public static List<string> m_HallInfo;

	//添加消息到信息框
	public static void AddHallInfo(string info)
	{
		m_HallInfo.Add (info);
	}

	//本人账号
	public static string m_Me;

	//房间选择
	public static int m_Room;

	//游戏客户端
	public static List<GameClient> m_GameClients;

	//房间索引表
	public static List<int> m_GameRooms;

	//网络
	public static TcpClient m_TC;
	public static TcpClient.EVENT e_init = new TcpClient.EVENT ();
	public static SendPackage m_SP;
	public static RecvPackage m_RP;

	void InitNet()
	{
		m_TC = new TcpClient ();
		m_SP = new SendPackage ();
		m_RP = new RecvPackage ();
	}
    static bool isOnce = true;
	void Awake ()
	{
        if (isOnce)
        {
            //初始化玩家字典
            m_Players = new Dictionary<string, PLAYER>();

            //初始化大厅信息
            m_HallInfo = new List<string>();

            //初始化游戏客户端
            m_GameClients = new List<GameClient>();

            //初始化房间索引表
            m_GameRooms = new List<int>();

            //装载游戏客户端
			m_GameClients.Add(new DouDiZhuClient());

            //装载房间索引表
            m_GameRooms.Add(0);
            m_GameRooms.Add(0);
            m_GameRooms.Add(0);
            m_GameRooms.Add(0);
            m_GameRooms.Add(0);

            //初始化网络
            InitNet();
        }
        isOnce = false;
	}

	void Update ()
	{
		//网络运行
		m_TC.Run ();
	}
}
