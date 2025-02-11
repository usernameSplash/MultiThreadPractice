#pragma once

#include <Windows.h>

#define WORKER_THREAD_NUM 3
#define MSG_TYPE_NUM 6;

typedef enum eMSG_TYPE
{
	ADD = 0,
	DEL = 1,
	SORT = 2,
	FIND = 3,
	PRINT = 4,
	QUIT = 5
} eMSG_TYPE;

typedef struct MSG_HEAD
{
	eMSG_TYPE _type;
	int _payloadLen;
} MSG_HEAD;

extern LONG g_TPSAdd;
extern LONG g_TPSDel;
extern LONG g_TPSSort;
extern LONG g_TPSFind;
extern LONG g_TPSPrint;

extern LONG g_TPSAll;
extern LONG g_TPSEachThread[WORKER_THREAD_NUM];