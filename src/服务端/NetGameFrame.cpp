#include "NetGameFrame.h"
#include <iostream>
#include "tinyxml/tinystr.h"
#include "tinyxml/tinyxml.h"
#include "GameServer.h"
#include "NetGameFrameProtocol.h"

void NetGameFrame::_LoadDB(const char* fn)
{
	//�����ĵ�����
	TiXmlDocument* doc = new TiXmlDocument();

	//���ⲿ�ļ��������ݵ��ĵ��������
	//����ʧ�ܣ��ʹ���һ���µĴ浵�ļ���
	//ֻ����һ�������Ϣ
	if (!doc->LoadFile(fn))
	{
		//�ͷ��ĵ�����
		delete doc;

		//�����ⲿ�ļ�
		doc = new TiXmlDocument();
		TiXmlElement* e = new TiXmlElement("admin");
		e->SetAttribute("password", "admin");
		e->SetAttribute("score", "-1");
		doc->LinkEndChild(e);
		doc->SaveFile(fn);
		delete doc;

		return;
	}

	//�õ������Ϣ���������ݿ��
	TiXmlElement* e;
	for (e = doc->FirstChildElement(); e != NULL; e = e->NextSiblingElement())
	{
		//�õ��˺�
		std::string id = e->Value();

		PLAYER_DB player_db;

		//�õ�����
		TiXmlAttribute* password = e->FirstAttribute();
		strcpy_s(player_db.password, password->Value());

		//�õ��÷�
		TiXmlAttribute* score = password->Next();
		player_db.score = atoi(score->Value());

		//��ӳ��
		m_PlayersDB.insert(
			std::pair<std::string, PLAYER_DB>(id, player_db));
	}

	delete doc;
}

void NetGameFrame::_SaveDB()
{
	//����
	for (std::map<SOCKET, PLAYER_OL>::iterator it = m_PlayersOL.begin(); it != m_PlayersOL.end(); ++it)
	{
		if (_PLAYER_OL_NO_LOGIN != it->second.state)
			m_PlayersDB.find(std::string(it->second.id))->second.score = it->second.score;
	}

	//�����ĵ�����
	TiXmlDocument* doc = new TiXmlDocument();

	//�õ������Ϣ�������ĵ�����
	std::map<std::string, PLAYER_DB>::iterator it;
	for (it = m_PlayersDB.begin(); it != m_PlayersDB.end(); ++it)
	{
		//�õ�����˺�
		TiXmlElement* e = new TiXmlElement(it->first.c_str());

		//�õ��������
		e->SetAttribute("password", it->second.password);

		//�õ���ҵ÷�
		char score[32];
		_itoa_s(it->second.score, score, 10);
		e->SetAttribute("score", score);

		//�����ĵ�
		doc->LinkEndChild(e);
	}

	//���浽�ĵ�
	doc->SaveFile(m_DB.c_str());

	//�ͷ��ĵ�����
	delete doc;
}

void NetGameFrame::_OnConnected(SOCKET sock)
{
	std::cout<<sock<<"���ӵ�������"<<std::endl;

	//����PLAYER_OL
	PLAYER_OL player_ol = {};
	player_ol.id[0] = 0;
	player_ol.score = -1;
	player_ol.state = _PLAYER_OL_NO_LOGIN;
	player_ol.room = -1;
	player_ol.seat = -1;

	//��ӳ��
	m_PlayersOL.insert(
		std::pair<SOCKET, PLAYER_OL>(sock, player_ol));
}

void NetGameFrame::_OnDisconnected(SOCKET sock)
{
	//����
	_SaveDB();

	//�õ������
	std::map<SOCKET, PLAYER_OL>::iterator it1;
	it1 = m_PlayersOL.find(sock);
	PLAYER_OL player_ol = it1->second;

	//ɾ�������
	m_PlayersOL.erase(it1);

	//���������Ϸ
	if (_PLAYER_OL_GAME == player_ol.state)
	{
		//������Ϸ�������������
		m_GameServers[player_ol.room]->m_DisconnectedSeat = player_ol.seat;

		//������Ϸ
		m_GameServers[player_ol.room]->OnEnd(_GAME_SERVER_OVER_DISCONNECTED, player_ol.seat);

		//������Ϸ������״̬
		m_GameServers[player_ol.room]->m_State = _GAME_SERVER_STOP;
			
		//���¸�������ڷ�����������״̬
		for (std::map<SOCKET, PLAYER_OL>::iterator it = m_PlayersOL.begin(); it != m_PlayersOL.end(); ++it)
		{
			if (player_ol.room == it->second.room && _PLAYER_OL_GAME == it->second.state)
				it->second.state = _PLAYER_OL_ROOM;
		}

		//Ⱥ��SP_PLAYER_DISCONNECT_GAME
		m_SP.clear();
		m_SP.add_int(SP_PLAYER_DISCONNECT_GAME);
		m_SP.add_string(player_ol.id);
		for (std::map<SOCKET, PLAYER_OL>::iterator it = m_PlayersOL.begin(); it != m_PlayersOL.end(); ++it)
		{
			if (_PLAYER_OL_NO_LOGIN != it->second.state)
				m_TcpServer.Send2Client(it->first, m_SP.data(), m_SP.size());
		}

		//Ⱥ��SP_END_PLAY_ROOM_DISCONNECT
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

	//Ⱥ��SP_PLAYER_DISCONNECT_PREPARE��SP_PLAYER_DISCONNECT_ROOM��SP_PLAYER_DISCONNECT_HALL
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
	//�õ�Э���
	m_RP.set_data(data, size);
	int id;
	m_RP.get_int(&id);

	//����Э����������
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
	//�õ��˺š�����
	char id[32], password[32];
	rp->get_string(id);
	rp->get_string(password);

	//��������˺�
	std::map<std::string, PLAYER_DB>::iterator it1;
	it1 = m_PlayersDB.find(std::string(id));

	//û���ҵ�
	if (m_PlayersDB.end() == it1)
	{
		//����SP_NO_ID
		m_SP.clear();
		m_SP.add_int(SP_NO_ID);
		m_TcpServer.Send2Client(sock, m_SP.data(), m_SP.size());
	}
	//�ҵ���
	else
	{
		//����Ƿ�
		if (strcmp(password, it1->second.password) != 0)
		{
			//����SP_WRONG_PASSWORD
			m_SP.clear();
			m_SP.add_int(SP_WRONG_PASSWORD);
			m_TcpServer.Send2Client(sock, m_SP.data(), m_SP.size());
		}
		//����Ϸ�
		else
		{
			//1�����������Ϣ

			//1.1���õ�OL���е����
			std::map<SOCKET, PLAYER_OL>::iterator it2;
			it2 = m_PlayersOL.find(sock);

			//1.2����������˺�
			strcpy_s(it2->second.id, it1->first.c_str());

			//1.3��������ҵ÷�
			it2->second.score = it1->second.score;

			//1.4���������״̬
			it2->second.state = _PLAYER_OL_HALL;

			//1.5��������ҷ���
			it2->second.room = -1;

			//1.6�����������λ
			it2->second.seat = -1;

			//2������SP_LOGIN_OK
			m_SP.clear();
			m_SP.add_int(SP_LOGIN_OK);
			
			//2.1���õ����е�¼��ҵ�����
			int login_player_num = 0;
			for (std::map<SOCKET, PLAYER_OL>::iterator it = m_PlayersOL.begin(); it != m_PlayersOL.end(); ++it)
			{
				if (_PLAYER_OL_NO_LOGIN != it->second.state)
					login_player_num += 1;
			}
			m_SP.add_int(login_player_num);

			//2.2���õ����е�¼��ҵ���Ϣ
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
			
			//2.3������
			m_TcpServer.Send2Client(sock, m_SP.data(), m_SP.size());
			
			//3��Ⱥ��SP_NEW_PLAYER_LOGIN
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
	//�õ��˺š�����
	char id[32], password[32];
	rp->get_string(id);
	rp->get_string(password);

	//��������˺�
	std::map<std::string, PLAYER_DB>::iterator it1;
	it1 = m_PlayersDB.find(std::string(id));

	//û���ҵ�
	if (m_PlayersDB.end() == it1)
	{
		//1����ӳ��
		PLAYER_DB player_db;
		strcpy_s(player_db.password, password);
		player_db.score = 0;
		m_PlayersDB.insert(std::pair<std::string, PLAYER_DB>(std::string(id), player_db));

		//2������
		_SaveDB();

		//3������SP_REGISTER_OK
		m_SP.clear();
		m_SP.add_int(SP_REGISTER_OK);
		m_TcpServer.Send2Client(sock, m_SP.data(), m_SP.size());
	}
	//�ҵ���
	else
	{
		//����CP_SAME_ID
		m_SP.clear();
		m_SP.add_int(SP_SAME_ID);
		m_TcpServer.Send2Client(sock, m_SP.data(), m_SP.size());
	}
}

void NetGameFrame::on_CP_CHOOSE_ROOM(SOCKET sock, RecvPackage* rp)
{
	//�õ������
	int room;
	rp->get_int(&room);

	//�õ���������
	int room_player_num = 0;
	for (std::map<SOCKET, PLAYER_OL>::iterator it = m_PlayersOL.begin(); it != m_PlayersOL.end(); ++it)
	{
		if (_PLAYER_OL_NO_LOGIN != it->second.state && _PLAYER_OL_HALL != it->second.state)
		{
			if (room == it->second.room)
				room_player_num += 1;
		}
	}

	//��������
	if (room_player_num == m_GameServers[room]->MaxPlayer())
	{
		//����SP_ROOM_FULL
		m_SP.clear();
		m_SP.add_int(SP_ROOM_FULL);
		m_TcpServer.Send2Client(sock, m_SP.data(), m_SP.size());
	}
	else
	{
		//1�����������Ϣ

		//1.1���õ�OL���е����
		std::map<SOCKET, PLAYER_OL>::iterator it2;
		it2 = m_PlayersOL.find(sock);

		//1.2���������״̬
		it2->second.state = _PLAYER_OL_ROOM;

		//1.3�����������λ

		//1.3.1���ҵ�һ���յ�λ��
		int iSize = m_GameServers[room]->MaxPlayer();
		bool* bSeatUse = new bool[iSize];
		for (int i = 0; i < iSize; ++i)
			bSeatUse[i] = false;
		for (std::map<SOCKET, PLAYER_OL>::iterator it = m_PlayersOL.begin(); it != m_PlayersOL.end(); ++it)
		{
			if (room == it->second.room)
				bSeatUse[it->second.seat] = true;
		}

		//1.3.2��Ϊ��ҷ���λ��
		it2->second.seat = 0;
		for(; it2->second.seat < iSize; ++it2->second.seat)
		{
			if(!bSeatUse[it2->second.seat])
				break;
		}
		delete [] bSeatUse;

		//1.4��������ҷ���
		it2->second.room = room;

		//2������SP_CHOOSE_ROOM_OK
		m_SP.clear();
		m_SP.add_int(SP_CHOOSE_ROOM_OK);
		m_SP.add_int(it2->second.seat);
		m_TcpServer.Send2Client(sock, m_SP.data(), m_SP.size());
		
		//3��Ⱥ��SP_PLAYER_ENTER_ROOM
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
	//�õ�����
	char talk[256];
	rp->get_string(talk);

	//Ⱥ��SP_PLAYER_TALK_HALL
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
	//�õ�����
	char talk[256];
	rp->get_string(talk);

	//Ⱥ��SP_PLAYER_TALK_ROOM
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
	//1�����������Ϣ

	//1.1���õ�OL���е����
	std::map<SOCKET, PLAYER_OL>::iterator it1;
	it1 = m_PlayersOL.find(sock);

	//1.2���������״̬
	it1->second.state = _PLAYER_OL_PREPARE;

	//2���õ��÷���׼������
	int room_prepare_num = 0;
	for (std::map<SOCKET, PLAYER_OL>::iterator it = m_PlayersOL.begin(); it != m_PlayersOL.end(); ++it)
	{
		if (it1->second.room == it->second.room && _PLAYER_OL_PREPARE == it->second.state)
			room_prepare_num += 1;
	}

	//3�����ݷ���������ദ��

	//3.1���÷���������Ҷ��Ѿ�׼����
	if (room_prepare_num == m_GameServers[it1->second.room]->MaxPlayer())
	{
		//3.1.1��Ⱥ��SP_BEGIN_PLAY_ROOM
		m_SP.clear();
		m_SP.add_int(SP_BEGIN_PLAY_ROOM);
		m_SP.add_int(it1->second.room);
		for (std::map<SOCKET, PLAYER_OL>::iterator it = m_PlayersOL.begin(); it != m_PlayersOL.end(); ++it)
		{
			if (_PLAYER_OL_NO_LOGIN != it->second.state)
				m_TcpServer.Send2Client(it->first, m_SP.data(), m_SP.size());
		}

		//3.1.2�����¸÷����������״̬Ϊ��Ϸ
		for (std::map<SOCKET, PLAYER_OL>::iterator it = m_PlayersOL.begin(); it != m_PlayersOL.end(); ++it)
		{
			if (it1->second.room == it->second.room && _PLAYER_OL_PREPARE == it->second.state)
				it->second.state = _PLAYER_OL_GAME;
		}

		//3.1.3��������Ϸ������״̬
		m_GameServers[it1->second.room]->m_State = _GAME_SERVER_RUN;

		//3.1.4��������Ϸ�������������
		m_GameServers[it1->second.room]->m_DisconnectedSeat = -1;

		//3.1.5��������Ϸ�������׽���
		for (std::map<SOCKET, PLAYER_OL>::iterator it = m_PlayersOL.begin(); it != m_PlayersOL.end(); ++it)
		{
			if (it1->second.room == it->second.room && _PLAYER_OL_GAME == it->second.state)
				m_GameServers[it1->second.room]->m_Sockets[it->second.seat] = it->first;
		}

		//3.1.6����ʼ����Ϸ
		m_GameServers[it1->second.room]->Init();
	}
	//3.2���÷��䲢����Ҷ��Ѿ�׼����
	else
	{
		//Ⱥ��SP_PLAYER_PREPARE_ROOM
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
	//�������״̬
	std::map<SOCKET, PLAYER_OL>::iterator it1;
	it1 = m_PlayersOL.find(sock);
	it1->second.state = _PLAYER_OL_ROOM;

	//Ⱥ��SP_PLAYER_CANCEL_ROOM
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
	//�õ������
	std::map<SOCKET, PLAYER_OL>::iterator it1;
	it1 = m_PlayersOL.find(sock);

	//������Ϸ�������������
	m_GameServers[it1->second.room]->m_DisconnectedSeat = it1->second.seat;

	//������Ϸ
	m_GameServers[it1->second.room]->OnEnd(_GAME_SERVER_OVER_QUIT, it1->second.seat);

	//������Ϸ������״̬
	m_GameServers[it1->second.room]->m_State = _GAME_SERVER_STOP;
		
	//���¸�������ڷ�����������״̬
	for (std::map<SOCKET, PLAYER_OL>::iterator it = m_PlayersOL.begin(); it != m_PlayersOL.end(); ++it)
	{
		if (it1->second.room == it->second.room && _PLAYER_OL_GAME == it->second.state)
			it->second.state = _PLAYER_OL_ROOM;
	}

	//Ⱥ��SP_END_PLAY_ROOM_STOP
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
	//�������״̬
	std::map<SOCKET, PLAYER_OL>::iterator it1;
	it1 = m_PlayersOL.find(sock);
	it1->second.state = _PLAYER_OL_HALL;
	it1->second.room = -1;
	it1->second.seat = -1;

	//Ⱥ��SP_PLAYER_QUIT_ROOM
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
	//1���ҵ���������ڵķ���
	std::map<SOCKET, PLAYER_OL>::iterator it;
	it = m_PlayersOL.find(sock);

	//2���õ�����Э��
	int size = rp->get_byte_array_length();
	void* data = m_MP.Allocate(size);
	rp->get_byte_array(data);

	//3��������Ϸ��������Ϣ����̬
	m_GameServers[it->second.room]->OnPlayerSendData(
		it->second.seat,
		data,
		size);

	//4���ͷ��ڴ�
	m_MP.Free(data);
}

void NetGameFrame::on_CP_QUIT_HALL(SOCKET sock, RecvPackage* rp)
{
	//�������״̬
	std::map<SOCKET, PLAYER_OL>::iterator it1;
	it1 = m_PlayersOL.find(sock);
	it1->second.state = _PLAYER_OL_NO_LOGIN;
	it1->second.room = -1;
	it1->second.seat = -1;

	//Ⱥ��SP_PLAYER_QUIT_HALL
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
	//�õ����ݿ��ļ���
	m_DB = db;

	//�������ݿ�
	_LoadDB(m_DB.c_str());

	//��ʼ��Tcp������
	m_TcpServer.Init(port, max);
}

void NetGameFrame::AddGameServer(GameServer* gs)
{
	//������Ϸ״̬Ϊֹͣ
	gs->m_State = _GAME_SERVER_STOP;

	//�����׽��ֱ���
	gs->m_Sockets.resize(gs->MaxPlayer());

	//������Ϸ���
	gs->m_NGF = this;

	//����Ϸ��
	m_GameServers.push_back(gs);
}

//run����ѭ�����������¼��ĵط�
void NetGameFrame::Run()
{
	//Tcp����������
	m_TcpServer.Run();

	//��������ʱ��
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

	//��Ϸ�������߼����У�����ÿһ�����ӵ��߼���
	for (int i = 0; i < (int)m_GameServers.size(); ++i)
	{
		if (m_GameServers[i]->m_State == _GAME_SERVER_RUN)
			m_GameServers[i]->OnUpdate();
	}
}

void NetGameFrame::End()
{
	//Tcp����������
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