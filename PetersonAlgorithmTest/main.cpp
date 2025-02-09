
#include <cstdio>
#include <process.h>
#include <Windows.h>

#define THREAD_CNT 2
#define LOOP_CNT_PER_THREAD 10000000

int cnt = 0;
int flag[2] = { 0, 0 };
int turn = 0;

volatile LONG debug = 0x00;
LONG debugOriginValue0Enter = 0x00;
LONG debugOriginValue0Exit = 0x00;
LONG debugOriginValue1Enter = 0x00;
LONG debugOriginValue1Exit = 0x00;

unsigned int WINAPI Lock0(void* arg);
unsigned int WINAPI Lock1(void* arg);

unsigned int WINAPI Lock0(void* arg)
{
	for (int iCnt = 0; iCnt < LOOP_CNT_PER_THREAD; ++iCnt)
	{
		flag[0] = 1;
		turn = 0;
		int tempFlag1 = flag[1];
		
		bool bIsWaited = false;
		while (turn == 0 && flag[1] == 1)
		{
			bIsWaited = true;
		}
		
		// Lock Acquired
		debugOriginValue0Enter = InterlockedOr(&debug, 0x0f);

		if (debugOriginValue0Enter == 0xf0)
		{
			__debugbreak();
		}

		cnt++;

		// Lock Released
		debugOriginValue0Exit = InterlockedAnd(&debug, 0xf0);

		if (debugOriginValue0Exit != 0x0f)
		{
			__debugbreak();
		}

		flag[0] = 0;
	}

	return 0;
}

unsigned int WINAPI Lock1(void* arg)
{
	for (int iCnt = 0; iCnt < LOOP_CNT_PER_THREAD; ++iCnt)
	{
		flag[1] = 1;
		turn = 1;
		int tempFlag0 = flag[0];

		bool bIsWaited = false;
		while (turn == 1 && flag[0] == 1)
		{
			bIsWaited = true;
		}

		// Lock Acquired
		debugOriginValue1Enter = InterlockedOr(&debug, 0xf0);

		if (debugOriginValue1Enter == 0x0f)
		{
			__debugbreak();
		}

		cnt++;

		// Lock Released
		debugOriginValue1Exit = InterlockedAnd(&debug, 0x0f);

		if (debugOriginValue1Exit != 0xf0)
		{
			__debugbreak();
		}

		flag[1] = 0;
	}

	return 0;
}

int wmain(void)
{
	HANDLE threadArr[2];

	threadArr[0] = (HANDLE)_beginthreadex(NULL, 0, Lock0, NULL, 0, NULL);
	threadArr[1] = (HANDLE)_beginthreadex(NULL, 0, Lock1, NULL, 0, NULL);

	WaitForMultipleObjects(THREAD_CNT, threadArr, true, INFINITE);

	wprintf(L"Cnt : %d\n", cnt);

	return 0;
}