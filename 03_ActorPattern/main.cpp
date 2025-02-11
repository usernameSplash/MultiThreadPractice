
#pragma comment(lib, "Synchronization.lib")
#pragma comment(lib, "winmm.lib")

#include <cstdio>
#include <process.h>
#include <Windows.h>
#include <synchapi.h>
#include <timeapi.h>

#include "WorkerThread.h"

int wmain(void)
{
	timeBeginPeriod(1);

	InitializeSetting();
	
	HANDLE threads[5];
	threads[0] = (HANDLE)_beginthreadex(NULL, 0, WorkerThread, (void*)0, 0, NULL);
	threads[1] = (HANDLE)_beginthreadex(NULL, 0, WorkerThread, (void*)1, 0, NULL);
	threads[2] = (HANDLE)_beginthreadex(NULL, 0, WorkerThread, (void*)2, 0, NULL);
	threads[3] = (HANDLE)_beginthreadex(NULL, 0, ProviderThread, NULL, 0, NULL);
	threads[4] = (HANDLE)_beginthreadex(NULL, 0, MonitorThread, NULL, 0, NULL);

	WaitForMultipleObjects(5, threads, TRUE, INFINITE);
	
	wprintf(L"# All Threads Were Terminated\n");

	return 0;
}