#include "SESSION.h"

#include <iostream>

SESSION::SESSION()
{
	//ZeroMemory(&recv_over.over, sizeof(recv_over.over));
	//recv_over.wsabuf.buf = recv_over.buffer;
	//recv_over.wsabuf.len = BUF_SIZE;
	//recv_over.mode = IO_RECV;
}

SESSION::~SESSION()
{
}

void SESSION::recv_packet()
{
	/*
		솔직히 여기 ring_buff() 더해주고 빼는 곳 이해가 안간다.
		왜 저렇게 해줘야 하지??
		전에 남아있던 곳에서
		
		=> 깨달았다.
		- 이건 받은게 아니라 다음에 그렇게 받겠다는 것이다.
		- 이미 받은 것에서 추가로 받겠다는 것이다.
		- 그래서 내가 지금 받은 것 플러스한 자리에 다음에 받겠다는 것이다.
	*/
	DWORD recv_flag = 0;
	ZeroMemory(&recv_over.over, sizeof(recv_over.over));
	recv_over.wsabuf.buf = recv_over.buffer + ring_buff.diff();
	recv_over.wsabuf.len = BUF_SIZE - ring_buff.diff();
	recv_over.mode = IO_RECV;
	WSARecv(sock, &recv_over.wsabuf, 1, 0, &recv_flag, &recv_over.over, 0);
}

void SESSION::send_packet(char* packet)
{
	OVER_EX* over = new OVER_EX;
	over->wsabuf.buf = reinterpret_cast<char*>(packet);
	over->wsabuf.len = packet[0];
	ZeroMemory(&over->over, sizeof(over->over));
	over->mode = IO_SEND;
	memcpy(over->buffer, packet, packet[0]);
	int ret = WSASend(sock, &over->wsabuf, 1, 0, 0, &over->over, 0);
	if (ret == SOCKET_ERROR)
	{
		int err = WSAGetLastError();
		std::cout << err << std::endl;
	}
}
