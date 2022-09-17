using System;
using System.Net;
using System.Net.Sockets;
using System.Collections;
using System.Collections.Generic;
using UnityEngine;


//处理收到数据的函数委托类型
public delegate void RecvDataCallBack(byte[] data);
//连接断开函数委托类型
public delegate void DisConnectCallBack();
//异常处理函数委托类型
public delegate void ExceptionCallBack(Exception e);

public class TcpClient
{
    RecvDataCallBack m_OnRecvData;
    DisConnectCallBack m_OnDisConnect;


	//套接字
	Socket m_Socket;

	//套接字集合
	ArrayList m_SocketSet;

	//网络事件：断开连接
	public const int _TCE_DISCONNECTED = 0;

	//网络事件：服务器发送过来数据
	public const int _TCE_DATA = 1;

	//网络事件
	public struct EVENT
	{
		//网络事件类型
		public int type;

		//数据
		public byte[] data;
	};

	//返回-1：发生网络错误
	//返回0：发送缓冲区已满暂时不能发送数据
	//返回>0：已经发送了多少字节
	int my_send(byte[] data, int begin, int size)
	{
		//判断当前套接字是否可以发送数据

		//1：清空套接字集合
		m_SocketSet.Clear ();

		//2：添加套接字到套接字集合
		m_SocketSet.Add (m_Socket);

		//3：判断套接字集合的可写性
		Socket.Select (null, m_SocketSet, null, 0);

		//4：得到检查结果为可写
		if (1 == m_SocketSet.Count)
		{
			//用r得到发送的字节数
			int r = 0;
			try
			{
				//一旦发送真正的网络错误则Send函数抛出异常
				r = m_Socket.Send(
					data, begin, size, SocketFlags.None);
			}
			catch (Exception e)
			{
				return -1;
			}

			return r;
		}
		//5：得到检查结果为不可写
		else 
		{
			return 0;
		}
	}
	
	//返回-1：发生网络错误
	//返回0：接收缓冲区是空的当前不可读
	//返回>0：读取了多少字节
	int my_recv(byte[] data, int begin, int size)
	{
		//判断当前套接字是否可以接收数据
		
		//1：清空套接字集合
		m_SocketSet.Clear ();
		
		//2：添加套接字到套接字集合
		m_SocketSet.Add (m_Socket);
		
		//3：判断套接字集合的可读性
		Socket.Select (m_SocketSet, null, null, 0);
		
		//4：得到检查结果为可读
		if (1 == m_SocketSet.Count)
		{
			//用r得到接收的字节数
			int r = 0;
			try
			{
				//一旦发送真正的网络错误则Send函数抛出异常
				r = m_Socket.Receive(
					data, begin, size, SocketFlags.None);
			}
			catch (Exception e)
			{
				return -1;
			}
			
			return r;
		}
		//5：得到检查结果为不可读
		else 
		{
			return 0;
		}
	}

	//网络事件表
	List<EVENT> m_Events;

	//发送数据包
	struct SENDDATA
	{
		//发送数据
		public byte[] data;

		//已经发送的长度
		public int send_size;
	};

	//发送数据表
	List<SENDDATA> m_SendData;

	//接收数据
	byte[] m_RecvData;
	int m_RecvSize;

	//发送数据
	void _Send()
	{
		//没有发送的数据
		if (m_SendData.Count == 0)
			return;
		
		//退出标志
		//0：正常发送完毕
		//1：暂时不能发送
		//2：出现网络错误
		int qf = 0;
		
		//得到第0个数据包
		SENDDATA sd = m_SendData[0];
		
		//循环发送数据
		while (sd.send_size < sd.data.Length)
		{
			//发送本次数据
			int r = my_send(
				sd.data,
				sd.send_size,
				sd.data.Length - sd.send_size);
			
			//发送数据失败
			if (r <= 0)
			{
				//暂时不能发了
				if (0 == r)
				{
					qf = 1;
					break;
				}
				//出现网络错误
				else
				{
					qf = 2;
					break;
				}
			}
			else
			{
				//已经发送的字节数累积到send_size中
				sd.send_size += r;
			}
		}
		
		//根据跳出循环的不同进行分情况处理
		if (0 == qf) 
		{
			//发送完将该包从表中移除
			m_SendData.RemoveAt (0);
		} 
		else if (1 == qf)
		{
			//更新表中发送情况
			m_SendData[0] = sd;
		}
		else if (2 == qf)
		{
			//断开网络
			Disconnect();
		}
	}

	//接收数据
	void _Recv()
	{
		//用于得到本次recv函数要接收多长的数据
		int len;
		
		//本次接收就接收长度
		if (m_RecvSize < sizeof(int))
		{
			//之后的recv就收len这么长
			len = sizeof(int) - m_RecvSize;
		}
		//本次接收就接收数据
		else
		{
			//得到本次数据包的总长度
			int all_len = BitConverter.ToInt32(m_RecvData, 0);
			
			//之后的recv就收len这么长
			len = all_len - (m_RecvSize - sizeof(int));
		}
		
		//接收数据
		int r = my_recv(
			m_RecvData,
			m_RecvSize,
			len);
		
		//正常接收了数据
		if (r > 0) 
		{
			//总接收长度递增
			m_RecvSize += r;
			
			//如果一个包的长度接收完毕
			if (m_RecvSize > sizeof(int))
			{
				//得到本次数据包的总长度
				int all_len = BitConverter.ToInt32 (m_RecvData, 0);
				
				//本次数据已经接收完毕
				if (m_RecvSize - sizeof(int) == all_len)
				{
					//投递"服务器发送过来数据"事件
                    //EVENT e;
                    //e.type = _TCE_DATA;
                    //e.data = new byte[all_len];
                    //for (int i = 0; i < all_len; ++i)
                    //    e.data[i] = m_RecvData[sizeof(int) + i];
                    //m_Events.Add(e);


                    byte[] temp = new byte[all_len];
                    for (int i = 0; i < all_len; ++i)
                    {
                        temp[i] = m_RecvData[sizeof(int) + i];
                    }

                    if (m_RecvData != null)
                    {
                        //TcpClient.EVENT edata
                        m_OnRecvData(temp);
                    }
					//接收数据辅助变量归零
					m_RecvSize = 0;
				}
			}
		}
		else if (-1 == r) 
		{
			//断开网络
			Disconnect ();
		}
		else if (0 == r) 
		{
			//r为0表示当前没有数据到来
		}
	}

	//构造
	public TcpClient()
	{
		m_Socket = null;
		m_SocketSet = null;
		m_Events = new List<EVENT>();
		m_SendData = null;
		m_RecvData = null;
        m_OnDisConnect = null;
        m_OnRecvData = null;
		m_RecvSize = -1;
	}

	//连接
	public bool Connect(
		
		//服务器ip地址
		string ip,
		
		//服务器端口号地址
		int port,
		
		//最大接收数
		int max_recv)
	{
		//已经连接了
		//清空数据重连
		//if (null != m_Socket)
		//	return false;

		//初始化套接字
		m_Socket = new Socket (
			AddressFamily.InterNetwork,
			SocketType.Stream,
			ProtocolType.Tcp);

		//连接服务器
		try
		{
			m_Socket.Connect(IPAddress.Parse(ip), port);
		}
		catch (Exception e)
		{
			//重置套接字
			m_Socket.Close();
			m_Socket = null;

			//返回失败
			return false;
		}

		//初始化数据
		m_SocketSet = new ArrayList();
		m_SendData = new List<SENDDATA>();
		m_RecvData = new byte[sizeof(int) + max_recv];
		m_RecvSize = 0;

		//返回成功
		return true;
	}

	//发送数据
	public bool Send(
		
		//发送数据首地址
		byte[] data,

		//发送数据起始位置
		int begin,
		
		//发送数据总长度
		int size)
	{
		//未连接直接返回失败
		if (null == m_Socket)
			return false;

		//创建发送数据结构体对象
		SENDDATA sd = new SENDDATA ();

		//1：创建发送字节数组
		sd.data = new byte[sizeof(int) + size];

		//2：设置发送数据长度
		byte[] t = BitConverter.GetBytes (size);
		SendPackage.ByteCopy (sd.data, 0, t, 0, sizeof(int));

		//3：设置发送数据
		for (int i = 0; i < size; ++i)
			sd.data [sizeof(int) + i] = data [begin + i];

		//4：设置已经发送数据长度
		sd.send_size = 0;

		//入表
		m_SendData.Add (sd);

		return true;
	}

	//断开
	public bool Disconnect()
	{
		//未连接直接返回失败
		if (null == m_Socket)
			return false;

		//关闭套接字
		m_Socket.Close ();
		m_Socket = null;
		m_SocketSet = null;
		m_SendData = null;
		m_RecvData = null;
		m_RecvSize = -1;

		//投递断开事件
		EVENT e = new EVENT ();
		e.type = _TCE_DISCONNECTED;
		e.data = null;
		m_Events.Add (e);

		return true;
	}
	
	//得到网络事件
	public bool GetEvent(ref EVENT e)
	{
		if (0 == m_Events.Count)
			return false;

		e = m_Events [0];
		m_Events.RemoveAt (0);
		return true;
	}

	//运行
	public void Run()
	{
		if (null == m_Socket)
			return;

		//发送数据
		_Send();
		
		//接收数据
		_Recv();
	}

	//状态
	public bool IsConnected()
	{
		return null != m_Socket;
	}

    public RecvDataCallBack OnRecvData
    {
        get { return m_OnRecvData; }
        set { m_OnRecvData = value; }
    }
    public DisConnectCallBack OnDisConnect
    {
        get { return m_OnDisConnect; }
        set { m_OnDisConnect = value; }
    }
}
