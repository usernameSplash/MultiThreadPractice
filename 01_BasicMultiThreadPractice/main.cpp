
#include <Windows.h>
#include <process.h>

#include <cstdio>

int g_Data = 0;
int g_Connect = 0;
bool g_Shutdown = false;

unsigned int WINAPI AcceptProc(void* arg);
unsigned int WINAPI DisconnectProc(void* arg);
unsigned int WINAPI UpdateProc(void* arg);

int wmain(void)
{
	HANDLE threadArr[5];

	threadArr[0] = (HANDLE)_beginthreadex(nullptr, 0, AcceptProc, nullptr, 0, nullptr);
	threadArr[1] = (HANDLE)_beginthreadex(nullptr, 0, DisconnectProc, nullptr, 0, nullptr);
	threadArr[2] = (HANDLE)_beginthreadex(nullptr, 0, UpdateProc, nullptr, 0, nullptr);
	threadArr[3] = (HANDLE)_beginthreadex(nullptr, 0, UpdateProc, nullptr, 0, nullptr);
	threadArr[4] = (HANDLE)_beginthreadex(nullptr, 0, UpdateProc, nullptr, 0, nullptr);


	for (int iCnt = 0; iCnt < 20; iCnt++)
	{
		wprintf(L"g_Connect : %d\n", g_Connect);
		Sleep(1000);
	}

	g_Shutdown = true;

	WaitForMultipleObjects(5, threadArr, true, INFINITE);

	return 0;
}

unsigned int WINAPI AcceptProc(void* arg)
{
	DWORD threadID = GetCurrentThreadId();
	srand(threadID);

	while (!g_Shutdown)
	{
		InterlockedIncrement((long*)&g_Connect);

		int sleepTime = rand() % 901 + 100;
		Sleep(sleepTime);
	}

	return 0;
}

unsigned int WINAPI DisconnectProc(void* arg)
{
	DWORD threadID = GetCurrentThreadId();
	srand(threadID);

	while (!g_Shutdown)
	{
		InterlockedDecrement((long*)&g_Connect);

		int sleepTime = rand() % 901 + 100;
		Sleep(sleepTime);
	}

	return 0;
}

unsigned int WINAPI UpdateProc(void* arg)
{
	while (!g_Shutdown)
	{
		int prevValue = InterlockedIncrement((long*)&g_Data);

		if (prevValue % 1000 == 0)
		{
			wprintf(L"g_Data : %d\n", prevValue);
		}

		Sleep(10);
	}

	return 0;
}