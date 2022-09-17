#include "GameServer.h"
#include "NetGameFrame.h"
#include "NetGameFrameProtocol.h"

bool GameServer::SendData(int seat, void* data, int size, int whichsend)
{
	//���ǵ�NetGameFrame���ڷ���ĳ�������е�����λ��
	//���Ѿ�׼������֮��Ϊ��������е�m_Sockets��
	//����Ӧ��λ���׽��֣�������������Ĵ�������
	//��������˶��ߣ���ô�������������ǾͲ��ܷ���
	//��Ϣ�ˣ��������滹Ҫ���϶�����ҵ��ж�

	//�����ж�
	if (seat < 0 ||
		seat >= (int)m_Sockets.size() ||
		seat == m_DisconnectedSeat)
		return false;

	//��֯�������ݰ�
	m_NGF->GetSP()->clear();
	m_NGF->GetSP()->add_int(SP_PLAY_ROOM);
	m_NGF->GetSP()->add_byte_array(data, size);

	//��������
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
	//���ý�������
	OnEnd(_GAME_SERVER_OVER_OK, -1);

	//����״̬
	m_State = _GAME_SERVER_STOP;

	//�õ�����Ϸ����ķ����±�
	int room;
	for (room = 0; room < (int)m_NGF->m_GameServers.size(); ++room)
	{
		if (this == m_NGF->m_GameServers[room])
			break;
	}

	//��֯SP_END_PLAY_ROOMЭ���
	m_NGF->GetSP()->clear();
	m_NGF->GetSP()->add_int(SP_END_PLAY_ROOM);
	m_NGF->GetSP()->add_int(room);

	//Ⱥ�����е�½��ң�������ı�����Ϸ����
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