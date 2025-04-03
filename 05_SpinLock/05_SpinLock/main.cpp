
#include "SpinLock.h"
#include "SpinLockNoYield.h"
#include <cstdio>
#include <Windows.h>
#include <process.h>

#pragma comment(lib, "winmm.lib")

unsigned __int64 g_var = 0;

unsigned int WINAPI AddProcSpin(void* arg);
unsigned int WINAPI SubProcSpin(void* arg);
unsigned int WINAPI AddProcSRW(void* arg);
unsigned int WINAPI SubProcSRW(void* arg);
unsigned int WINAPI AddProcNoYield(void* arg);
unsigned int WINAPI SubProcNoYield(void* arg);

#define THREAD_NUM 8
#define LOOP_NUM 10000000
#define WASTE_LOOP_NUM 100

SpinLock g_spinLock;
SRWLOCK g_srwLock;
SpinLockNoYield g_noYieldLock;

#define TEST_WITH_SRW
//#define TEST_WITH_NOYIELD

int wmain(void)
{
	HANDLE threads[THREAD_NUM];

	timeBeginPeriod(1);

	LARGE_INTEGER freq;
	QueryPerformanceFrequency(&freq);

	LARGE_INTEGER start;
	LARGE_INTEGER end;

	QueryPerformanceCounter(&start);
	
	for (int iCnt = 0; iCnt < THREAD_NUM / 2; ++iCnt)
	{
		threads[iCnt] = (HANDLE)_beginthreadex(NULL, 0, AddProcSpin, NULL, 0, NULL);
	}

	for (int iCnt = THREAD_NUM / 2; iCnt < THREAD_NUM; ++iCnt)
	{
		threads[iCnt] = (HANDLE)_beginthreadex(NULL, 0, SubProcSpin, NULL, 0, NULL);
	}

	WaitForMultipleObjects(THREAD_NUM, threads, TRUE, INFINITE);
	QueryPerformanceCounter(&end);

	wprintf(L"Var : %lld\n", g_var);
	wprintf(L"time : %lf\n", (end.QuadPart - start.QuadPart) / (double)freq.QuadPart);

	for (int iCnt = 0; iCnt < THREAD_NUM; ++iCnt)
	{
		CloseHandle(threads[iCnt]);
	}

#ifdef TEST_WITH_SRW
	QueryPerformanceCounter(&start);

	for (int iCnt = 0; iCnt < THREAD_NUM / 2; ++iCnt)
	{
		threads[iCnt] = (HANDLE)_beginthreadex(NULL, 0, AddProcSRW, NULL, 0, NULL);
	}

	for (int iCnt = THREAD_NUM / 2; iCnt < THREAD_NUM; ++iCnt)
	{
		threads[iCnt] = (HANDLE)_beginthreadex(NULL, 0, SubProcSRW, NULL, 0, NULL);
	}

	WaitForMultipleObjects(THREAD_NUM, threads, TRUE, INFINITE);
	QueryPerformanceCounter(&end);

	wprintf(L"Var : %lld\n", g_var);
	wprintf(L"time : %lf\n", (end.QuadPart - start.QuadPart) / (double)freq.QuadPart);

	for (int iCnt = 0; iCnt < THREAD_NUM; ++iCnt)
	{
		CloseHandle(threads[iCnt]);
	}
#endif

#ifdef TEST_WITH_NOYIELD
	QueryPerformanceCounter(&start);

	for (int iCnt = 0; iCnt < THREAD_NUM / 2; ++iCnt)
	{
		threads[iCnt] = (HANDLE)_beginthreadex(NULL, 0, AddProcNoYield, NULL, 0, NULL);
	}

	for (int iCnt = THREAD_NUM / 2; iCnt < THREAD_NUM; ++iCnt)
	{
		threads[iCnt] = (HANDLE)_beginthreadex(NULL, 0, SubProcNoYield, NULL, 0, NULL);
	}

	WaitForMultipleObjects(THREAD_NUM, threads, TRUE, INFINITE);
	QueryPerformanceCounter(&end);

	wprintf(L"Var : %lld\n", g_var);
	wprintf(L"time : %lf\n", (end.QuadPart - start.QuadPart) / (double)freq.QuadPart);

	for (int iCnt = 0; iCnt < THREAD_NUM; ++iCnt)
	{
		CloseHandle(threads[iCnt]);
	}
#endif
	return 0;
}

unsigned int WINAPI AddProcSpin(void* arg)
{
	for (int iCnt = 0; iCnt < LOOP_NUM; ++iCnt)
	{
		g_spinLock.Lock();
		g_var++;

		for (int j = 0; j < WASTE_LOOP_NUM; ++j)
		{

		}
		g_spinLock.Unlock();
	}

	return 0;
}

unsigned int WINAPI SubProcSpin(void* arg)
{
	for (int iCnt = 0; iCnt < LOOP_NUM; ++iCnt)
	{
		g_spinLock.Lock();
		g_var--;

		for (int j = 0; j < WASTE_LOOP_NUM; ++j)
		{

		}
		g_spinLock.Unlock();
	}

	return 0;
}

unsigned int WINAPI AddProcSRW(void* arg)
{
	for (int iCnt = 0; iCnt < LOOP_NUM; ++iCnt)
	{
		AcquireSRWLockExclusive(&g_srwLock);
		g_var++;

		for (int j = 0; j < WASTE_LOOP_NUM; ++j)
		{

		}
		ReleaseSRWLockExclusive(&g_srwLock);
	}

	return 0;
}

unsigned int WINAPI SubProcSRW(void* arg)
{
	for (int iCnt = 0; iCnt < LOOP_NUM; ++iCnt)
	{
		AcquireSRWLockExclusive(&g_srwLock);
		g_var--;

		for (int j = 0; j < WASTE_LOOP_NUM; ++j)
		{

		}
		ReleaseSRWLockExclusive(&g_srwLock);
	}

	return 0;
}

unsigned int WINAPI AddProcNoYield(void* arg)
{
	for (int iCnt = 0; iCnt < LOOP_NUM; ++iCnt)
	{
		g_noYieldLock.Lock();
		g_var++;

		for (int j = 0; j < WASTE_LOOP_NUM; ++j)
		{

		}
		g_noYieldLock.Unlock();
	}

	return 0;
}

unsigned int WINAPI SubProcNoYield(void* arg)
{
	for (int iCnt = 0; iCnt < LOOP_NUM; ++iCnt)
	{
		g_noYieldLock.Lock();
		g_var--;

		for (int j = 0; j < WASTE_LOOP_NUM; ++j)
		{

		}
		g_noYieldLock.Unlock();
	}

	return 0;
}