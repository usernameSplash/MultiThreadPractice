#pragma comment(lib, "Synchronization.lib")

#include <Windows.h>
#include <process.h>
#include <list>
#include <synchapi.h>

#include <cstdio>
#include <conio.h>

std::list<int> g_List;
bool g_Shutdown = false;

HANDLE g_ShutdownEvent;
HANDLE g_SaveEvent;

SRWLOCK g_ListLock;

//const int g_PrintWaitObject = 0;
//const int g_WorkWaitObject = 0;
//const int g_DeleteWaitObject = 0;
//const int g_SaveWaitObject = 0;

unsigned int WINAPI PrintProc(void* arg);
unsigned int WINAPI WorkerProc(void* arg);
unsigned int WINAPI DeleteProc(void* arg);
unsigned int WINAPI SaveProc(void* arg);


int wmain(void)
{
	InitializeSRWLock(&g_ListLock);

	g_ShutdownEvent = CreateEvent(NULL, true, false, NULL);
	if (g_ShutdownEvent == NULL)
	{
		wprintf(L"# ShutDown Event Object is NULL");
		return 0;
	}

	g_SaveEvent = CreateEvent(NULL, false, false, NULL);
	if (g_SaveEvent == NULL)
	{
		wprintf(L"# Save Event Object is NULL");
		return 0;
	}

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
				//g_Shutdown = true;
				SetEvent(g_ShutdownEvent);
				break;
			}

			if (ch == 'S' || ch == 's')
			{
				//WakeByAddressSingle((void*)&g_SaveWaitObject;
				SetEvent(g_SaveEvent);
			}
		}
	}

	WaitForMultipleObjects(6, threadArr, true, INFINITE);
	CloseHandle(g_ShutdownEvent);
	CloseHandle(g_SaveEvent);

	wprintf(L"# All Threads were Terminated\n");

	return 0;
}

unsigned int WINAPI PrintProc(void* arg)
{
	std::list<int> copiedList;

	while (true /*!g_Shutdown*/)
	{
		//WaitOnAddress((void*)&g_PrintWaitObject, (void*)&g_PrintWaitObject, sizeof(g_PrintWaitObject), 1000);
		
		DWORD waitResult = WaitForSingleObject(g_ShutdownEvent, 1000);

		if (waitResult != WAIT_TIMEOUT) // or, waitResult == WAIT_OBJECT_0
		{
			break;
		}

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

	while (true /*!g_Shutdown*/)
	{
		//WaitOnAddress((void*)&g_WorkWaitObject, (void*)&g_WorkWaitObject, sizeof(g_WorkWaitObject), 1000);
		
		DWORD waitResult = WaitForSingleObject(g_ShutdownEvent, 1000);

		if (waitResult != WAIT_TIMEOUT)
		{
			break;
		}

		//int randNum = rand() % 100;
		int num = base * (i + 1);
		i++;
		i = i % 9;

		AcquireSRWLockExclusive(&g_ListLock);
		g_List.push_front(num);
		ReleaseSRWLockExclusive(&g_ListLock);

	}

	return 0;
}

unsigned int WINAPI DeleteProc(void* arg)
{
	while (true /*!g_Shutdown*/)
	{
		//WaitOnAddress((void*)&g_DeleteWaitObject, (void*)&g_DeleteWaitObject, sizeof(g_DeleteWaitObject), 333);
		
		DWORD waitResult = WaitForSingleObject(g_ShutdownEvent, 500);
		
		if (waitResult != WAIT_TIMEOUT)
		{
			break;
		}

		AcquireSRWLockExclusive(&g_ListLock);
		if (!g_List.empty())
		{
			g_List.pop_back();
		}
		ReleaseSRWLockExclusive(&g_ListLock);

	}

	return 0;
}

unsigned int WINAPI SaveProc(void* arg)
{
	std::list<int> copiedList;

	const HANDLE events[2] = { g_ShutdownEvent, g_SaveEvent };

	while (true /*!g_Shutdown*/)
	{
		//WaitOnAddress((void*)&g_SaveWaitObject, (void*)&g_SaveWaitObject, sizeof(g_SaveWaitObject), INFINITE);
		
		DWORD waitResult = WaitForMultipleObjects(2, events, false, INFINITE);

		if (waitResult == WAIT_OBJECT_0)
		{
			break;
		}

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