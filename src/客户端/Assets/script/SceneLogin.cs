using UnityEngine;
using UnityEngine.UI;
using System.Collections;

public class SceneLogin : MonoBehaviour
{
    public GameObject[] m_UI;

    void ShowMessage(string msg)
    {
        //得到文字信息
        m_UI[9].GetComponent<Text>().text = msg;

        //显示
        m_UI[8].SetActive(true);
    }

    public void CloseMessage()
    {
        m_UI[8].SetActive(false);
    }

    string password;

    public void On_Button_Login()
    {
        //得到账号密码
        MainScript.m_Me = m_UI[5].GetComponent<Text>().text;
        password = m_UI[6].GetComponent<InputField>().text;

        //判断是否为空
        if (0 == MainScript.m_Me.Length || 0 == password.Length)
        {
            ShowMessage("账号和密码不能为空");
            return;
        }

        //连接服务器
        if (MainScript.m_TC.Connect("127.0.0.1", 12345, 1024))
        {
            //发送CP_LOGIN
            MainScript.m_SP.clear();
            MainScript.m_SP.add_int(MainScript.CP_LOGIN);
            MainScript.m_SP.add_string(MainScript.m_Me);
            MainScript.m_SP.add_string(password);
            MainScript.m_TC.Send(MainScript.m_SP.data(), 0, MainScript.m_SP.size());
        }
        else
            ShowMessage("无法连接服务器");
    }

    public void On_Button_Register()
    {
        //得到账号密码
        MainScript.m_Me = m_UI[5].GetComponent<Text>().text;
        password = m_UI[6].GetComponent<InputField>().text;

        //判断是否为空
        if (0 == MainScript.m_Me.Length || 0 == password.Length)
        {
            ShowMessage("账号和密码不能为空");
            return;
        }

        //连接服务器
		if (MainScript.m_TC.Connect ("127.0.0.1", 12345, 1024)) {
			//发送CP_LOGIN
			MainScript.m_SP.clear ();
			MainScript.m_SP.add_int (MainScript.CP_REGISTER);
			MainScript.m_SP.add_string (MainScript.m_Me);
			MainScript.m_SP.add_string (password);
			MainScript.m_TC.Send (MainScript.m_SP.data (), 0, MainScript.m_SP.size ());
		} else {
			ShowMessage ("无法连接服务器123");
		}
    }

    public void On_Button_Exit()
    {
        Application.Quit();
    }


    void Awake()
    {
		MainScript.m_TC.OnRecvData += OnRecv;
		MainScript.m_TC.OnDisConnect += OnDisConnect;
    }
    //本脚本中用于接受数据的委托函数
    void OnRecv(byte[] data)
    {
        MainScript.m_RP.set_data(data, 0, data.Length);
        int MsgType = -1;
        bool isOK = MainScript.m_RP.get_int(ref MsgType);
        if (isOK)
        {
            //分情况讨论
            switch (MsgType)
            {    
                case MainScript.SP_NO_ID:
                    {
                        on_SP_NO_ID(MainScript.m_RP);
                        break;
                    }
                case MainScript.SP_WRONG_PASSWORD:
                    {
                        on_SP_WRONG_PASSWORD(MainScript.m_RP);
                        break;
                    }
                case MainScript.SP_LOGIN_OK:
                    {
                        on_SP_LOGIN_OK(MainScript.m_RP);
                        break;
                    }
                case MainScript.SP_SAME_ID:
                    {
                        on_SP_SAME_ID(MainScript.m_RP);
                        break;
                    }
                case MainScript.SP_REGISTER_OK:
                    {
                        on_SP_REGISTER_OK(MainScript.m_RP);
                        break;
                    }
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
        //清空玩家字典
        MainScript.m_Players.Clear();

        //清空大厅信息
        MainScript.m_HallInfo.Clear();
    }

    void Update()
    {
        if (MainScript.m_TC.IsConnected())
        {
            MainScript.m_TC.Run();
        }
    }

    void on_SP_NO_ID(RecvPackage rp)
    {
        //显示信息
        ShowMessage("账号不存在");

        //断开
        MainScript.m_TC.Disconnect();
    }

    void on_SP_WRONG_PASSWORD(RecvPackage rp)
    {
        //显示信息
        ShowMessage("密码错误");

        //断开
        MainScript.m_TC.Disconnect();
    }

    void on_SP_LOGIN_OK(RecvPackage rp)
    {
        //得到玩家信息
        int login_player_num = 0;
        rp.get_int(ref login_player_num);
        for (int i = 0; i < login_player_num; ++i)
        {
            string id = "";
            rp.get_string(ref id);

            MainScript.PLAYER player = new MainScript.PLAYER();
            rp.get_int(ref player.state);
            rp.get_int(ref player.score);
            rp.get_int(ref player.room);
            rp.get_int(ref player.seat);

            MainScript.m_Players.Add(id, player);
        }
		MainScript.m_TC.OnRecvData -= OnRecv;
		MainScript.m_TC.OnDisConnect -= OnDisConnect;
        Application.LoadLevel(2);
    }

    void on_SP_SAME_ID(RecvPackage rp)
    {
        //显示信息
        ShowMessage("账号有重复");

        //断开
        MainScript.m_TC.Disconnect();
    }

    void on_SP_REGISTER_OK(RecvPackage rp)
    {

        //显示信息
        ShowMessage("注册成功");

        //断开
        MainScript.m_TC.Disconnect();
    }
}
