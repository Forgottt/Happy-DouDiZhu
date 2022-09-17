#ifndef _NET_GAME_FRAME_PROTOCOL_H_
#define _NET_GAME_FRAME_PROTOCOL_H_

//i��integer����
//s��string�ַ���
//a��array����

//�ͻ��˷��͸���������Э��
#define CP_LOGIN 0 //s(�˺�)��s(����)
#define CP_REGISTER 1 //s(�˺�)��s(����)
#define CP_CHOOSE_ROOM 2 //i(����)
#define CP_TALK_HALL 3 //s(����)
#define CP_TALK_ROOM 4 //s(����)
#define CP_PREPARE_ROOM 5 //null
#define CP_CANCELPREPARE_ROOM 10 //null
#define CP_CANCEL_ROOM 6 //null
#define CP_STOP_ROOM 7 //null
#define CP_QUIT_ROOM 8 //null
#define CP_PLAY_ROOM 9 //��2��Э��
#define CP_QUIT_HALL 10 //null

//���������͸��ͻ��˵�Э��
#define SP_NO_ID 0 //null
#define SP_WRONG_PASSWORD 1 //null
#define SP_LOGIN_OK 2 //i(���е�½�������n)��a({s�˺�,i״̬,i�÷�,i����,i��λ}*n)
#define SP_NEW_PLAYER_LOGIN 3//s(�˺�)��i(�÷�)
#define SP_SAME_ID 4 //null
#define SP_REGISTER_OK 5 //null
#define SP_ROOM_FULL 6 //null
#define SP_CHOOSE_ROOM_OK 7 //i(��λ)
#define SP_PLAYER_ENTER_ROOM 8 //s(�˺�)��i(����)��i(��λ)
#define SP_PLAYER_TALK_HALL 9 //s(�˺�)��s(����)
#define SP_PLAYER_QUIT_HALL 10 //s(�˺�)
#define SP_PLAYER_TALK_ROOM 11 //s(�˺�)��s(����)
#define SP_PLAYER_PREPARE_ROOM 12 //s(�˺�)
#define SP_BEGIN_PLAY_ROOM 13 //i(����)
#define SP_PLAYER_CANCEL_ROOM 14 //s(�˺�)
#define SP_END_PLAY_ROOM_STOP 15 //i(����)��s(�˺�)
#define SP_PLAYER_QUIT_ROOM 16 //s(�˺�)
#define SP_END_PLAY_ROOM 17 //i(����)
#define SP_PLAY_ROOM 18 //��2��Э��
#define SP_PLAYER_DISCONNECT_HALL 19 //s(�˺�)
#define SP_PLAYER_DISCONNECT_ROOM 20 //s(�˺�)
#define SP_PLAYER_DISCONNECT_PREPARE 21 //s(�˺�)
#define SP_PLAYER_DISCONNECT_GAME 22 //s(�˺�)
#define SP_END_PLAY_ROOM_DISCONNECT 23 //i(����)��s(�˺�)

//SP_END_PLAY_ROOM������������Ϸ
//SP_END_PLAY_ROOM_STOP����Ϊĳ����ֹ������Ϸ
//SP_END_PLAY_ROOM_DISCONNECT����Ϊĳ�˶Ͽ�������Ϸ

#endif