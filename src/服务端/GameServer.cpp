#include "GameServer.h"
#include "NetGameFrame.h"
#include "NetGameFrameProtocol.h"

bool GameServer::SendData(int seat, void* data, int size, int whichsend)
{
	//我们的NetGameFrame会在发现某个房间中的所有位置
	//都已经准备好了之后，为这个房间中的m_Sockets表
	//填充对应座位的套接字，方便我们下面的处理，但是
	//如果发生了断线，那么这个断线玩家我们就不能发送
	//消息了，所以下面还要加上断线玩家的判断

	//参数判断
	if (seat < 0 ||
		seat >= (int)m_Sockets.size() ||
		seat == m_DisconnectedSeat)
		return false;

	//组织发送数据包
	m_NGF->GetSP()->clear();
	m_NGF->GetSP()->add_int(SP_PLAY_ROOM);
	m_NGF->GetSP()->add_byte_array(data, size);

	//发送数据
	if (whichsend == 0)
	{
		m_NGF->GetTS()->Send2Client(
			m_Sockets[seat],
			m_NGF->GetSP()->data(),
			m_NGF->GetSP()->size());
	}
	else if (whichsend == 1)
	{
		m_NGF->GetTS()->Send2AllClient(
			m_NGF->GetSP()->data(),
			m_NGF->GetSP()->size());
	}
	else
	{
		m_NGF->GetTS()->Send2OtherClient(
			m_Sockets[seat],
			m_NGF->GetSP()->data(),
			m_NGF->GetSP()->size());
	}


	return true;
}

void GameServer::End()
{
	//调用结束处理
	OnEnd(_GAME_SERVER_OVER_OK, -1);

	//更新状态
	m_State = _GAME_SERVER_STOP;

	//得到本游戏对象的房间下标
	int room;
	for (room = 0; room < (int)m_NGF->m_GameServers.size(); ++room)
	{
		if (this == m_NGF->m_GameServers[room])
			break;
	}

	//组织SP_END_PLAY_ROOM协议包
	m_NGF->GetSP()->clear();
	m_NGF->GetSP()->add_int(SP_END_PLAY_ROOM);
	m_NGF->GetSP()->add_int(room);

	//群发所有登陆玩家，本房间的本次游戏结束
	std::map<SOCKET, NetGameFrame::PLAYER_OL>::iterator it;
	for (it = m_NGF->m_PlayersOL.begin(); it != m_NGF->m_PlayersOL.end(); ++it)
	{
		if (it->second.state != _PLAYER_OL_NO_LOGIN)
		{
			m_NGF->GetTS()->Send2Client(
				it->first,
				m_NGF->GetSP()->data(),
				m_NGF->GetSP()->size());
		}
	}
}

void GameServer::OnUpdate()
{}