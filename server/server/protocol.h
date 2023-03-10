#pragma once

constexpr int PORT_NUM = 7000;
constexpr int BUF_SIZE = 2048;

constexpr int MAX_USER = 10;

enum class type { cs_move, sc_move };

struct cs_move_packet {
	char size;
	char type;
	double x;
	double y;
	double z;
};

struct sc_move_packet {
	char size;
	char type;
	int client_id;
	double x;
	double y;
	double z;
};