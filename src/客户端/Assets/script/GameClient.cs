using UnityEngine;
using System.Collections;
using System.Collections.Generic;

public abstract class GameClient : MonoBehaviour
{
	public GameObject CardBack;
	public List<GameObject> prefb;
	//状态
	public bool m_GameClientRun;

	//发送、接收
	public SendPackage m_SendPackage;
	public RecvPackage m_RecvPackage;

	//构造
	public GameClient()
	{
		m_GameClientRun = false;
		m_SendPackage = new SendPackage ();
		m_RecvPackage = new RecvPackage ();
		CardBack = new GameObject();
		CardBack = (GameObject)Resources.Load ("prefab/CardBack");
		prefb = new List<GameObject>();

		//总牌库
		for (int i=3; i<16; ++i) {
			//Spade Heart Clubs Dianmond
			string info = "prefab/Dianmond/Dianmond";
			info += i;
			prefb.Add((GameObject)Resources.Load(info));
			
			info = "prefab/Clubs/Clubs";
			info += i;	
			prefb.Add((GameObject)Resources.Load(info));
			
			info = "prefab/Heart/Heart";
			info += i;	
			prefb.Add((GameObject)Resources.Load(info));
			
			info = "prefab/Spade/Spade";
			info += i;	
			prefb.Add((GameObject)Resources.Load(info));
		}
		prefb.Add((GameObject)Resources.Load("prefab/CardSmallKing"));
		prefb.Add((GameObject)Resources.Load("prefab/CardBigKing"));
	}

	//发消息
	protected void SendData(byte[] data, int index, int count)
	{
		//清空发送
		MainScript.m_SP.clear ();

		//添加协议
		MainScript.m_SP.add_int (
			MainScript.CP_PLAY_ROOM);

		//添加数据
		MainScript.m_SP.add_byte_array (
			data, index, count);

		//发送数据
		MainScript.m_TC.Send (
			MainScript.m_SP.data(),
			0,
			MainScript.m_SP.size());
	}

	//初始化
	public virtual void Init()
	{}

	//结束
	public virtual void End()
	{}

	//收消息
	public virtual void OnServerSendData(byte[] data, int index, int count)
	{}

	//输入
	public virtual void Input(int type,int n)
	{}

	//每次游戏循环更新
	public virtual void OnUpdate()
	{}
}
