#ifndef _NET_GAME_FRAME_PROTOCOL_H_
#define _NET_GAME_FRAME_PROTOCOL_H_

//i：integer整数
//s：string字符串
//a：array数组

//客户端发送给服务器的协议
#define CP_LOGIN 0 //s(账号)、s(密码)
#define CP_REGISTER 1 //s(账号)、s(密码)
#define CP_CHOOSE_ROOM 2 //i(房间)
#define CP_TALK_HALL 3 //s(话语)
#define CP_TALK_ROOM 4 //s(话语)
#define CP_PREPARE_ROOM 5 //null
#define CP_CANCELPREPARE_ROOM 10 //null
#define CP_CANCEL_ROOM 6 //null
#define CP_STOP_ROOM 7 //null
#define CP_QUIT_ROOM 8 //null
#define CP_PLAY_ROOM 9 //带2级协议
#define CP_QUIT_HALL 10 //null

//服务器发送给客户端的协议
#define SP_NO_ID 0 //null
#define SP_WRONG_PASSWORD 1 //null
#define SP_LOGIN_OK 2 //i(所有登陆玩家数量n)、a({s账号,i状态,i得分,i房间,i座位}*n)
#define SP_NEW_PLAYER_LOGIN 3//s(账号)、i(得分)
#define SP_SAME_ID 4 //null
#define SP_REGISTER_OK 5 //null
#define SP_ROOM_FULL 6 //null
#define SP_CHOOSE_ROOM_OK 7 //i(座位)
#define SP_PLAYER_ENTER_ROOM 8 //s(账号)、i(房间)、i(座位)
#define SP_PLAYER_TALK_HALL 9 //s(账号)、s(话语)
#define SP_PLAYER_QUIT_HALL 10 //s(账号)
#define SP_PLAYER_TALK_ROOM 11 //s(账号)、s(话语)
#define SP_PLAYER_PREPARE_ROOM 12 //s(账号)
#define SP_BEGIN_PLAY_ROOM 13 //i(房间)
#define SP_PLAYER_CANCEL_ROOM 14 //s(账号)
#define SP_END_PLAY_ROOM_STOP 15 //i(房间)、s(账号)
#define SP_PLAYER_QUIT_ROOM 16 //s(账号)
#define SP_END_PLAY_ROOM 17 //i(房间)
#define SP_PLAY_ROOM 18 //带2级协议
#define SP_PLAYER_DISCONNECT_HALL 19 //s(账号)
#define SP_PLAYER_DISCONNECT_ROOM 20 //s(账号)
#define SP_PLAYER_DISCONNECT_PREPARE 21 //s(账号)
#define SP_PLAYER_DISCONNECT_GAME 22 //s(账号)
#define SP_END_PLAY_ROOM_DISCONNECT 23 //i(房间)、s(账号)

//SP_END_PLAY_ROOM：正常结束游戏
//SP_END_PLAY_ROOM_STOP：因为某人终止结束游戏
//SP_END_PLAY_ROOM_DISCONNECT：因为某人断开结束游戏

#endif