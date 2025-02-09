#pragma comment(lib, "Synchronization.lib")

#include <Windows.h>
#include <process.h>
#include <list>
#include <synchapi.h>

#include <cstdio>
#include <conio.h>

std::list<int> g_List;
bool g_Shutdown = false;

SRWLOCK g_ListLock;

const int g_PrintWaitObject = 0;
const int g_WorkWaitObject = 0;
const int g_DeleteWaitObject = 0;
const int g_SaveWaitObject = 0;

unsigned int WINAPI PrintProc(void* arg);
unsigned int WINAPI WorkerProc(void* arg);
unsigned int WINAPI DeleteProc(void* arg);
unsigned int WINAPI SaveProc(void* arg);

int wmain(void)
{
	InitializeSRWLock(&g_ListLock);
	HANDLE threadArr[6];

	threadArr[0] = (HANDLE)_beginthreadex(nullptr, 0, PrintProc, nullptr, 0, nullptr);
	threadArr[1] = (HANDLE)_beginthreadex(nullptr, 0, WorkerProc, (void*)1, 0, nullptr);
	threadArr[2] = (HANDLE)_beginthreadex(nullptr, 0, WorkerProc, (void*)10, 0, nullptr);
	threadArr[3] = (HANDLE)_beginthreadex(nullptr, 0, WorkerProc, (void*)100, 0, nullptr);
	threadArr[4] = (HANDLE)_beginthreadex(nullptr, 0, DeleteProc, nullptr, 0, nullptr);
	threadArr[4] = (HANDLE)_beginthreadex(nullptr, 0, SaveProc, nullptr, 0, nullptr);

	while (!g_Shutdown)
	{
		if (_kbhit())
		{
			int ch = _getch();

			if (ch == 27) // esc
			{
				g_Shutdown = true;
				break;
			}

			if (ch == 'S' || ch == 's')
			{
				WakeByAddressSingle((void*)&g_SaveWaitObject);
			}
		}
	}

	WaitForMultipleObjects(6, threadArr, true, INFINITE);

	wprintf(L"# All Threads were Terminated\n");

	return 0;
}

unsigned int WINAPI PrintProc(void* arg)
{
	std::list<int> copiedList;

	while (!g_Shutdown)
	{
		WaitOnAddress((void*)&g_PrintWaitObject, (void*)&g_PrintWaitObject, sizeof(g_PrintWaitObject), 1000);

		AcquireSRWLockShared(&g_ListLock);
		copiedList = g_List;
		ReleaseSRWLockShared(&g_ListLock);

		auto it = copiedList.begin();

		if (copiedList.empty())
		{
			continue;
		}

		wprintf(L"[");
		for (int iCnt = 0; iCnt < copiedList.size() - 1; iCnt++)
		{
			wprintf(L"%d->", *it);
			++it;
		}
		wprintf(L"%d]\n", *it);
	}

	return 0;
}

unsigned int WINAPI WorkerProc(void* arg)
{
	//DWORD threadID = GetCurrentThreadId();
	//srand(threadID);

	int i = 0;
	int base = (int)arg;

	while (!g_Shutdown)
	{
		//int randNum = rand() % 100;
		int num = base * (i + 1);
		i++;
		i = i % 9;

		AcquireSRWLockExclusive(&g_ListLock);
		g_List.push_front(num);
		ReleaseSRWLockExclusive(&g_ListLock);

		WaitOnAddress((void*)&g_WorkWaitObject, (void*)&g_WorkWaitObject, sizeof(g_WorkWaitObject), 1000);
	}

	return 0;
}

unsigned int WINAPI DeleteProc(void* arg)
{
	while (!g_Shutdown)
	{
		int randNum = rand();

		AcquireSRWLockExclusive(&g_ListLock);
		if (!g_List.empty())
		{
			g_List.pop_back();
		}
		ReleaseSRWLockExclusive(&g_ListLock);

		WaitOnAddress((void*)&g_DeleteWaitObject, (void*)&g_DeleteWaitObject, sizeof(g_DeleteWaitObject), 500);
	}

	return 0;
}

unsigned int WINAPI SaveProc(void* arg)
{
	std::list<int> copiedList;

	while (!g_Shutdown)
	{
		WaitOnAddress((void*)&g_SaveWaitObject, (void*)&g_SaveWaitObject, sizeof(g_SaveWaitObject), INFINITE);

		AcquireSRWLockShared(&g_ListLock);
		copiedList = g_List;
		ReleaseSRWLockShared(&g_ListLock);

		if (copiedList.empty())
		{
			continue;
		}

		FILE* file;

		fopen_s(&file, "MultiThread_List_Content.txt", "w");
		if (file == NULL)
		{
			wprintf(L"# File is NULL");
			break;
		}

		auto it = copiedList.begin();

		fwprintf(file, L"[");
		for (int iCnt = 0; iCnt < copiedList.size() - 1; iCnt++)
		{
			fwprintf(file, L"%d->", *it);
			++it;
		}
		fwprintf(file, L"%d]\n", *it);

		fclose(file);
	}

	return 0;
}