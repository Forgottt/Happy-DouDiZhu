#include "NetGameFrame.h"
#include <iostream>
#include "tinyxml/tinystr.h"
#include "tinyxml/tinyxml.h"
#include "GameServer.h"
#include "NetGameFrameProtocol.h"

void NetGameFrame::_LoadDB(const char* fn)
{
	//创建文档对象
	TiXmlDocument* doc = new TiXmlDocument();

	//从外部文件加载数据到文档对象，如果
	//加载失败，就创建一个新的存档文件，
	//只包含一个玩家信息
	if (!doc->LoadFile(fn))
	{
		//释放文档对象
		delete doc;

		//创建外部文件
		doc = new TiXmlDocument();
		TiXmlElement* e = new TiXmlElement("admin");
		e->SetAttribute("password", "admin");
		e->SetAttribute("score", "-1");
		doc->LinkEndChild(e);
		doc->SaveFile(fn);
		delete doc;

		return;
	}

	//得到玩家信息并放入数据库表
	TiXmlElement* e;
	for (e = doc->FirstChildElement(); e != NULL; e = e->NextSiblingElement())
	{
		//得到账号
		std::string id = e->Value();

		PLAYER_DB player_db;

		//得到密码
		TiXmlAttribute* password = e->FirstAttribute();
		strcpy_s(player_db.password, password->Value());

		//得到得分
		TiXmlAttribute* score = password->Next();
		player_db.score = atoi(score->Value());

		//入映射
		m_PlayersDB.insert(
			std::pair<std::string, PLAYER_DB>(id, player_db));
	}

	delete doc;
}

void NetGameFrame::_SaveDB()
{
	//更新
	for (std::map<SOCKET, PLAYER_OL>::iterator it = m_PlayersOL.begin(); it != m_PlayersOL.end(); ++it)
	{
		if (_PLAYER_OL_NO_LOGIN != it->second.state)
			m_PlayersDB.find(std::string(it->second.id))->second.score = it->second.score;
	}

	//创建文档对象
	TiXmlDocument* doc = new TiXmlDocument();

	//得到玩家信息并放入文档对象
	std::map<std::string, PLAYER_DB>::iterator it;
	for (it = m_PlayersDB.begin(); it != m_PlayersDB.end(); ++it)
	{
		//得到玩家账号
		TiXmlElement* e = new TiXmlElement(it->first.c_str());

		//得到玩家密码
		e->SetAttribute("password", it->second.password);

		//得到玩家得分
		char score[32];
		_itoa_s(it->second.score, score, 10);
		e->SetAttribute("score", score);

		//存入文档
		doc->LinkEndChild(e);
	}

	//保存到文档
	doc->SaveFile(m_DB.c_str());

	//释放文档对象
	delete doc;
}

void NetGameFrame::_OnConnected(SOCKET sock)
{
	std::cout<<sock<<"连接到服务器"<<std::endl;

	//创建PLAYER_OL
	PLAYER_OL player_ol = {};
	player_ol.id[0] = 0;
	player_ol.score = -1;
	player_ol.state = _PLAYER_OL_NO_LOGIN;
	player_ol.room = -1;
	player_ol.seat = -1;

	//入映射
	m_PlayersOL.insert(
		std::pair<SOCKET, PLAYER_OL>(sock, player_ol));
}

void NetGameFrame::_OnDisconnected(SOCKET sock)
{
	//保存
	_SaveDB();

	//得到该玩家
	std::map<SOCKET, PLAYER_OL>::iterator it1;
	it1 = m_PlayersOL.find(sock);
	PLAYER_OL player_ol = it1->second;

	//删除该玩家
	m_PlayersOL.erase(it1);

	//如果正在游戏
	if (_PLAYER_OL_GAME == player_ol.state)
	{
		//更新游戏服务器断线玩家
		m_GameServers[player_ol.room]->m_DisconnectedSeat = player_ol.seat;

		//结束游戏
		m_GameServers[player_ol.room]->OnEnd(_GAME_SERVER_OVER_DISCONNECTED, player_ol.seat);

		//更新游戏服务器状态
		m_GameServers[player_ol.room]->m_State = _GAME_SERVER_STOP;
			
		//更新该玩家所在房间的所有玩家状态
		for (std::map<SOCKET, PLAYER_OL>::iterator it = m_PlayersOL.begin(); it != m_PlayersOL.end(); ++it)
		{
			if (player_ol.room == it->second.room && _PLAYER_OL_GAME == it->second.state)
				it->second.state = _PLAYER_OL_ROOM;
		}

		//群发SP_PLAYER_DISCONNECT_GAME
		m_SP.clear();
		m_SP.add_int(SP_PLAYER_DISCONNECT_GAME);
		m_SP.add_string(player_ol.id);
		for (std::map<SOCKET, PLAYER_OL>::iterator it = m_PlayersOL.begin(); it != m_PlayersOL.end(); ++it)
		{
			if (_PLAYER_OL_NO_LOGIN != it->second.state)
				m_TcpServer.Send2Client(it->first, m_SP.data(), m_SP.size());
		}

		//群发SP_END_PLAY_ROOM_DISCONNECT
		m_SP.clear();
		m_SP.add_int(SP_END_PLAY_ROOM_DISCONNECT);
		m_SP.add_int(player_ol.room);
		m_SP.add_string(player_ol.id);
		for (std::map<SOCKET, PLAYER_OL>::iterator it = m_PlayersOL.begin(); it != m_PlayersOL.end(); ++it)
		{
			if (_PLAYER_OL_NO_LOGIN != it->second.state)
				m_TcpServer.Send2Client(it->first, m_SP.data(), m_SP.size());
		}
	}

	//群发SP_PLAYER_DISCONNECT_PREPARE、SP_PLAYER_DISCONNECT_ROOM、SP_PLAYER_DISCONNECT_HALL
	m_SP.clear();
	switch (player_ol.state)
	{
	case _PLAYER_OL_PREPARE: m_SP.add_int(SP_PLAYER_DISCONNECT_PREPARE); break;
	case _PLAYER_OL_ROOM: m_SP.add_int(SP_PLAYER_DISCONNECT_ROOM); break;
	case _PLAYER_OL_HALL: m_SP.add_int(SP_PLAYER_DISCONNECT_HALL); break;
	}
	m_SP.add_string(player_ol.id);
	for (std::map<SOCKET, PLAYER_OL>::iterator it = m_PlayersOL.begin(); it != m_PlayersOL.end(); ++it)
	{
		if (_PLAYER_OL_NO_LOGIN != it->second.state)
			m_TcpServer.Send2Client(it->first, m_SP.data(), m_SP.size());
	}
}

void NetGameFrame::_OnData(SOCKET sock, const void* data, int size)
{
	//得到协议号
	m_RP.set_data(data, size);
	int id;
	m_RP.get_int(&id);

	//根据协议分情况处理
	switch (id)
	{
	case CP_LOGIN: on_CP_LOGIN(sock, &m_RP); break;
	case CP_REGISTER: on_CP_REGISTER(sock, &m_RP); break;
	case CP_CHOOSE_ROOM: on_CP_CHOOSE_ROOM(sock, &m_RP); break;
	case CP_TALK_HALL: on_CP_TALK_HALL(sock, &m_RP); break;
	case CP_TALK_ROOM: on_CP_TALK_ROOM(sock, &m_RP); break;
	case CP_PREPARE_ROOM: on_CP_PREPARE_ROOM(sock, &m_RP); break;
	case CP_CANCEL_ROOM: on_CP_CANCEL_ROOM(sock, &m_RP); break;
	case CP_STOP_ROOM: on_CP_STOP_ROOM(sock, &m_RP); break;
	case CP_QUIT_ROOM: on_CP_QUIT_ROOM(sock, &m_RP); break;
	case CP_PLAY_ROOM: on_CP_PLAY_ROOM(sock, &m_RP); break;
	case CP_QUIT_HALL: on_CP_QUIT_HALL(sock, &m_RP); break;
	}
}

void NetGameFrame::on_CP_LOGIN(SOCKET sock, RecvPackage* rp)
{
	//得到账号、密码
	char id[32], password[32];
	rp->get_string(id);
	rp->get_string(password);

	//查找这个账号
	std::map<std::string, PLAYER_DB>::iterator it1;
	it1 = m_PlayersDB.find(std::string(id));

	//没有找到
	if (m_PlayersDB.end() == it1)
	{
		//发送SP_NO_ID
		m_SP.clear();
		m_SP.add_int(SP_NO_ID);
		m_TcpServer.Send2Client(sock, m_SP.data(), m_SP.size());
	}
	//找到了
	else
	{
		//密码非法
		if (strcmp(password, it1->second.password) != 0)
		{
			//发送SP_WRONG_PASSWORD
			m_SP.clear();
			m_SP.add_int(SP_WRONG_PASSWORD);
			m_TcpServer.Send2Client(sock, m_SP.data(), m_SP.size());
		}
		//密码合法
		else
		{
			//1：更新玩家信息

			//1.1：得到OL表中的玩家
			std::map<SOCKET, PLAYER_OL>::iterator it2;
			it2 = m_PlayersOL.find(sock);

			//1.2：更新玩家账号
			strcpy_s(it2->second.id, it1->first.c_str());

			//1.3：更新玩家得分
			it2->second.score = it1->second.score;

			//1.4：更新玩家状态
			it2->second.state = _PLAYER_OL_HALL;

			//1.5：更新玩家房间
			it2->second.room = -1;

			//1.6：更新玩家座位
			it2->second.seat = -1;

			//2：发送SP_LOGIN_OK
			m_SP.clear();
			m_SP.add_int(SP_LOGIN_OK);
			
			//2.1：得到所有登录玩家的数量
			int login_player_num = 0;
			for (std::map<SOCKET, PLAYER_OL>::iterator it = m_PlayersOL.begin(); it != m_PlayersOL.end(); ++it)
			{
				if (_PLAYER_OL_NO_LOGIN != it->second.state)
					login_player_num += 1;
			}
			m_SP.add_int(login_player_num);

			//2.2：得到所有登录玩家的信息
			for (std::map<SOCKET, PLAYER_OL>::iterator it = m_PlayersOL.begin(); it != m_PlayersOL.end(); ++it)
			{
				if (_PLAYER_OL_NO_LOGIN != it->second.state)
				{
					m_SP.add_string(it->second.id);
					m_SP.add_int(it->second.state);
					m_SP.add_int(it->second.score);
					m_SP.add_int(it->second.room);
					m_SP.add_int(it->second.seat);
				}
			}
			
			//2.3：发送
			m_TcpServer.Send2Client(sock, m_SP.data(), m_SP.size());
			
			//3：群发SP_NEW_PLAYER_LOGIN
			m_SP.clear();
			m_SP.add_int(SP_NEW_PLAYER_LOGIN);
			m_SP.add_string(it2->second.id);
			m_SP.add_int(it2->second.score);
			for (std::map<SOCKET, PLAYER_OL>::iterator it = m_PlayersOL.begin(); it != m_PlayersOL.end(); ++it)
			{
				if (_PLAYER_OL_NO_LOGIN != it->second.state)
					m_TcpServer.Send2Client(it->first, m_SP.data(), m_SP.size());
			}
		}
	}
}

void NetGameFrame::on_CP_REGISTER(SOCKET sock, RecvPackage* rp)
{
	//得到账号、密码
	char id[32], password[32];
	rp->get_string(id);
	rp->get_string(password);

	//查找这个账号
	std::map<std::string, PLAYER_DB>::iterator it1;
	it1 = m_PlayersDB.find(std::string(id));

	//没有找到
	if (m_PlayersDB.end() == it1)
	{
		//1：入映射
		PLAYER_DB player_db;
		strcpy_s(player_db.password, password);
		player_db.score = 0;
		m_PlayersDB.insert(std::pair<std::string, PLAYER_DB>(std::string(id), player_db));

		//2：保存
		_SaveDB();

		//3：发送SP_REGISTER_OK
		m_SP.clear();
		m_SP.add_int(SP_REGISTER_OK);
		m_TcpServer.Send2Client(sock, m_SP.data(), m_SP.size());
	}
	//找到了
	else
	{
		//发送CP_SAME_ID
		m_SP.clear();
		m_SP.add_int(SP_SAME_ID);
		m_TcpServer.Send2Client(sock, m_SP.data(), m_SP.size());
	}
}

void NetGameFrame::on_CP_CHOOSE_ROOM(SOCKET sock, RecvPackage* rp)
{
	//得到房间号
	int room;
	rp->get_int(&room);

	//得到房间人数
	int room_player_num = 0;
	for (std::map<SOCKET, PLAYER_OL>::iterator it = m_PlayersOL.begin(); it != m_PlayersOL.end(); ++it)
	{
		if (_PLAYER_OL_NO_LOGIN != it->second.state && _PLAYER_OL_HALL != it->second.state)
		{
			if (room == it->second.room)
				room_player_num += 1;
		}
	}

	//房间已满
	if (room_player_num == m_GameServers[room]->MaxPlayer())
	{
		//发送SP_ROOM_FULL
		m_SP.clear();
		m_SP.add_int(SP_ROOM_FULL);
		m_TcpServer.Send2Client(sock, m_SP.data(), m_SP.size());
	}
	else
	{
		//1：更新玩家信息

		//1.1：得到OL表中的玩家
		std::map<SOCKET, PLAYER_OL>::iterator it2;
		it2 = m_PlayersOL.find(sock);

		//1.2：更新玩家状态
		it2->second.state = _PLAYER_OL_ROOM;

		//1.3：更新玩家座位

		//1.3.1：找到一个空的位置
		int iSize = m_GameServers[room]->MaxPlayer();
		bool* bSeatUse = new bool[iSize];
		for (int i = 0; i < iSize; ++i)
			bSeatUse[i] = false;
		for (std::map<SOCKET, PLAYER_OL>::iterator it = m_PlayersOL.begin(); it != m_PlayersOL.end(); ++it)
		{
			if (room == it->second.room)
				bSeatUse[it->second.seat] = true;
		}

		//1.3.2：为玩家分配位置
		it2->second.seat = 0;
		for(; it2->second.seat < iSize; ++it2->second.seat)
		{
			if(!bSeatUse[it2->second.seat])
				break;
		}
		delete [] bSeatUse;

		//1.4：更新玩家房间
		it2->second.room = room;

		//2：发送SP_CHOOSE_ROOM_OK
		m_SP.clear();
		m_SP.add_int(SP_CHOOSE_ROOM_OK);
		m_SP.add_int(it2->second.seat);
		m_TcpServer.Send2Client(sock, m_SP.data(), m_SP.size());
		
		//3：群发SP_PLAYER_ENTER_ROOM
		m_SP.clear();
		m_SP.add_int(SP_PLAYER_ENTER_ROOM);
		m_SP.add_string(it2->second.id);
		m_SP.add_int(it2->second.room);
		m_SP.add_int(it2->second.seat);
		for (std::map<SOCKET, PLAYER_OL>::iterator it = m_PlayersOL.begin(); it != m_PlayersOL.end(); ++it)
		{
			if (_PLAYER_OL_NO_LOGIN != it->second.state)
				m_TcpServer.Send2Client(it->first, m_SP.data(), m_SP.size());
		}
	}
}

void NetGameFrame::on_CP_TALK_HALL(SOCKET sock, RecvPackage* rp)
{
	//得到话语
	char talk[256];
	rp->get_string(talk);

	//群发SP_PLAYER_TALK_HALL
	m_SP.clear();
	m_SP.add_int(SP_PLAYER_TALK_HALL);
	m_SP.add_string(m_PlayersOL.find(sock)->second.id);
	m_SP.add_string(talk);
	for (std::map<SOCKET, PLAYER_OL>::iterator it = m_PlayersOL.begin(); it != m_PlayersOL.end(); ++it)
	{
		if (_PLAYER_OL_NO_LOGIN != it->second.state)
			m_TcpServer.Send2Client(it->first, m_SP.data(), m_SP.size());
	}
}

void NetGameFrame::on_CP_TALK_ROOM(SOCKET sock, RecvPackage* rp)
{
	//得到话语
	char talk[256];
	rp->get_string(talk);

	//群发SP_PLAYER_TALK_ROOM
	m_SP.clear();
	m_SP.add_int(SP_PLAYER_TALK_ROOM);
	m_SP.add_string(m_PlayersOL.find(sock)->second.id);
	m_SP.add_string(talk);
	for (std::map<SOCKET, PLAYER_OL>::iterator it = m_PlayersOL.begin(); it != m_PlayersOL.end(); ++it)
	{
		if (_PLAYER_OL_NO_LOGIN != it->second.state)
			m_TcpServer.Send2Client(it->first, m_SP.data(), m_SP.size());
	}
}

void NetGameFrame::on_CP_PREPARE_ROOM(SOCKET sock, RecvPackage* rp)
{
	//1：更新玩家信息

	//1.1：得到OL表中的玩家
	std::map<SOCKET, PLAYER_OL>::iterator it1;
	it1 = m_PlayersOL.find(sock);

	//1.2：更新玩家状态
	it1->second.state = _PLAYER_OL_PREPARE;

	//2：得到该房间准备人数
	int room_prepare_num = 0;
	for (std::map<SOCKET, PLAYER_OL>::iterator it = m_PlayersOL.begin(); it != m_PlayersOL.end(); ++it)
	{
		if (it1->second.room == it->second.room && _PLAYER_OL_PREPARE == it->second.state)
			room_prepare_num += 1;
	}

	//3：根据房间情况分类处理

	//3.1：该房间所有玩家都已经准备好
	if (room_prepare_num == m_GameServers[it1->second.room]->MaxPlayer())
	{
		//3.1.1：群发SP_BEGIN_PLAY_ROOM
		m_SP.clear();
		m_SP.add_int(SP_BEGIN_PLAY_ROOM);
		m_SP.add_int(it1->second.room);
		for (std::map<SOCKET, PLAYER_OL>::iterator it = m_PlayersOL.begin(); it != m_PlayersOL.end(); ++it)
		{
			if (_PLAYER_OL_NO_LOGIN != it->second.state)
				m_TcpServer.Send2Client(it->first, m_SP.data(), m_SP.size());
		}

		//3.1.2：更新该房间所有玩家状态为游戏
		for (std::map<SOCKET, PLAYER_OL>::iterator it = m_PlayersOL.begin(); it != m_PlayersOL.end(); ++it)
		{
			if (it1->second.room == it->second.room && _PLAYER_OL_PREPARE == it->second.state)
				it->second.state = _PLAYER_OL_GAME;
		}

		//3.1.3：更新游戏服务器状态
		m_GameServers[it1->second.room]->m_State = _GAME_SERVER_RUN;

		//3.1.4：更新游戏服务器断线玩家
		m_GameServers[it1->second.room]->m_DisconnectedSeat = -1;

		//3.1.5：更新游戏服务器套接字
		for (std::map<SOCKET, PLAYER_OL>::iterator it = m_PlayersOL.begin(); it != m_PlayersOL.end(); ++it)
		{
			if (it1->second.room == it->second.room && _PLAYER_OL_GAME == it->second.state)
				m_GameServers[it1->second.room]->m_Sockets[it->second.seat] = it->first;
		}

		//3.1.6：初始化游戏
		m_GameServers[it1->second.room]->Init();
	}
	//3.2：该房间并非玩家都已经准备好
	else
	{
		//群发SP_PLAYER_PREPARE_ROOM
		m_SP.clear();
		m_SP.add_int(SP_PLAYER_PREPARE_ROOM);
		m_SP.add_string(it1->second.id);
		for (std::map<SOCKET, PLAYER_OL>::iterator it = m_PlayersOL.begin(); it != m_PlayersOL.end(); ++it)
		{
			if (_PLAYER_OL_NO_LOGIN != it->second.state)
				m_TcpServer.Send2Client(it->first, m_SP.data(), m_SP.size());
		}
	}
}

void NetGameFrame::on_CP_CANCEL_ROOM(SOCKET sock, RecvPackage* rp)
{
	//更新玩家状态
	std::map<SOCKET, PLAYER_OL>::iterator it1;
	it1 = m_PlayersOL.find(sock);
	it1->second.state = _PLAYER_OL_ROOM;

	//群发SP_PLAYER_CANCEL_ROOM
	m_SP.clear();
	m_SP.add_int(SP_PLAYER_CANCEL_ROOM);
	m_SP.add_string(it1->second.id);
	for (std::map<SOCKET, PLAYER_OL>::iterator it = m_PlayersOL.begin(); it != m_PlayersOL.end(); ++it)
	{
		if (_PLAYER_OL_NO_LOGIN != it->second.state)
			m_TcpServer.Send2Client(it->first, m_SP.data(), m_SP.size());
	}
}

void NetGameFrame::on_CP_STOP_ROOM(SOCKET sock, RecvPackage* rp)
{
	//得到该玩家
	std::map<SOCKET, PLAYER_OL>::iterator it1;
	it1 = m_PlayersOL.find(sock);

	//更新游戏服务器断线玩家
	m_GameServers[it1->second.room]->m_DisconnectedSeat = it1->second.seat;

	//结束游戏
	m_GameServers[it1->second.room]->OnEnd(_GAME_SERVER_OVER_QUIT, it1->second.seat);

	//更新游戏服务器状态
	m_GameServers[it1->second.room]->m_State = _GAME_SERVER_STOP;
		
	//更新该玩家所在房间的所有玩家状态
	for (std::map<SOCKET, PLAYER_OL>::iterator it = m_PlayersOL.begin(); it != m_PlayersOL.end(); ++it)
	{
		if (it1->second.room == it->second.room && _PLAYER_OL_GAME == it->second.state)
			it->second.state = _PLAYER_OL_ROOM;
	}

	//群发SP_END_PLAY_ROOM_STOP
	m_SP.clear();
	m_SP.add_int(SP_END_PLAY_ROOM_STOP);
	m_SP.add_int(it1->second.room);
	m_SP.add_string(it1->second.id);
	for (std::map<SOCKET, PLAYER_OL>::iterator it = m_PlayersOL.begin(); it != m_PlayersOL.end(); ++it)
	{
		if (_PLAYER_OL_NO_LOGIN != it->second.state)
			m_TcpServer.Send2Client(it->first, m_SP.data(), m_SP.size());
	}
}

void NetGameFrame::on_CP_QUIT_ROOM(SOCKET sock, RecvPackage* rp)
{
	//更新玩家状态
	std::map<SOCKET, PLAYER_OL>::iterator it1;
	it1 = m_PlayersOL.find(sock);
	it1->second.state = _PLAYER_OL_HALL;
	it1->second.room = -1;
	it1->second.seat = -1;

	//群发SP_PLAYER_QUIT_ROOM
	m_SP.clear();
	m_SP.add_int(SP_PLAYER_QUIT_ROOM);
	m_SP.add_string(it1->second.id);
	for (std::map<SOCKET, PLAYER_OL>::iterator it = m_PlayersOL.begin(); it != m_PlayersOL.end(); ++it)
	{
		if (_PLAYER_OL_NO_LOGIN != it->second.state)
			m_TcpServer.Send2Client(it->first, m_SP.data(), m_SP.size());
	}
}

void NetGameFrame::on_CP_PLAY_ROOM(SOCKET sock, RecvPackage* rp)
{
	//1：找到该玩家所在的房间
	std::map<SOCKET, PLAYER_OL>::iterator it;
	it = m_PlayersOL.find(sock);

	//2：得到二级协议
	int size = rp->get_byte_array_length();
	void* data = m_MP.Allocate(size);
	rp->get_byte_array(data);

	//3：调用游戏来处理消息，多态
	m_GameServers[it->second.room]->OnPlayerSendData(
		it->second.seat,
		data,
		size);

	//4：释放内存
	m_MP.Free(data);
}

void NetGameFrame::on_CP_QUIT_HALL(SOCKET sock, RecvPackage* rp)
{
	//更新玩家状态
	std::map<SOCKET, PLAYER_OL>::iterator it1;
	it1 = m_PlayersOL.find(sock);
	it1->second.state = _PLAYER_OL_NO_LOGIN;
	it1->second.room = -1;
	it1->second.seat = -1;

	//群发SP_PLAYER_QUIT_HALL
	m_SP.clear();
	m_SP.add_int(SP_PLAYER_QUIT_HALL);
	m_SP.add_string(it1->second.id);
	for (std::map<SOCKET, PLAYER_OL>::iterator it = m_PlayersOL.begin(); it != m_PlayersOL.end(); ++it)
	{
		if (_PLAYER_OL_NO_LOGIN != it->second.state)
			m_TcpServer.Send2Client(it->first, m_SP.data(), m_SP.size());
	}
}

void NetGameFrame::Init(const char* db,
						unsigned short port,
						int max)
{
	//得到数据库文件名
	m_DB = db;

	//加载数据库
	_LoadDB(m_DB.c_str());

	//初始化Tcp服务器
	m_TcpServer.Init(port, max);
}

void NetGameFrame::AddGameServer(GameServer* gs)
{
	//设置游戏状态为停止
	gs->m_State = _GAME_SERVER_STOP;

	//设置套接字表长度
	gs->m_Sockets.resize(gs->MaxPlayer());

	//设置游戏框架
	gs->m_NGF = this;

	//入游戏表
	m_GameServers.push_back(gs);
}

//run就是循环处理网络事件的地方
void NetGameFrame::Run()
{
	//Tcp服务器运行
	m_TcpServer.Run();

	//处理网络时间
	TcpServer::EVENT e;
	while (m_TcpServer.GetEvent(&e))
	{
		switch (e.type)
		{
		case _TSE_CONNECTED:
			{
				_OnConnected(e.sock);
				break;
			}
		case _TSE_DISCONNECTED:
			{
				_OnDisconnected(e.sock);
				break;
			}
		case _TSE_DATA:
			{
				_OnData(e.sock, e.data, e.size);
				break;
			}
		}
		
		m_TcpServer.FreeEventMemory(&e);
	}

	//游戏服务器逻辑运行（处理每一张桌子的逻辑）
	for (int i = 0; i < (int)m_GameServers.size(); ++i)
	{
		if (m_GameServers[i]->m_State == _GAME_SERVER_RUN)
			m_GameServers[i]->OnUpdate();
	}
}

void NetGameFrame::End()
{
	//Tcp服务器结束
	m_TcpServer.End();
}

TcpServer* NetGameFrame::GetTS()
{
	return &m_TcpServer;
}

SendPackage* NetGameFrame::GetSP()
{
	return &m_SP;
}

RecvPackage* NetGameFrame::GetRP()
{
	return &m_RP;
}