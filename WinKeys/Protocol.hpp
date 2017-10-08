#pragma once

typedef unsigned char Key;
typedef struct { unsigned short x; unsigned short y; } Pos;

#pragma pack(push, 1)
typedef struct
{
	Key key;
	union
	{
		Pos mousePos;
		int keyUp;
	};
	struct
	{
		unsigned sec;
		unsigned short milliSec;
	} time;
} BinaryStruct;
#pragma pack(pop)

enum Command
{
	Start = 0x00,
	Stop,
	Ping
};

#define KO 0x00
#define OK 0x01

/* Special VKCode */

#define VK_MOUSEPOS		  0x07
