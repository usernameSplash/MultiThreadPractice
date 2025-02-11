
#include "WorkerThread.h"

void InitializeSetting(void)
{
	g_MessageQueue = MyDataStructure::RingBuffer(65535);
	g_List = std::list<std::wstring>();
	g_Event = CreateEvent(NULL, false, false, NULL);

	InitializeSRWLock(&g_MessageQueueLock);
	InitializeSRWLock(&g_ListLock);

	return;
}

unsigned int WINAPI WorkerThread(void* arg)
{
	DWORD threadID = GetCurrentThreadId();
	wprintf(L"# Worker Thread Start, ID : %d\n", threadID);

	int threadIndex = (int)arg;

	bool bShutdown = false;

	while (!bShutdown)
	{
		MSG_HEAD head;
		wchar_t buf[27];
		std::wstring recvStr;

		// need to check queue size is zero
		WaitForSingleObject(g_Event, INFINITE);
		
		AcquireSRWLockExclusive(&g_MessageQueueLock);

		g_MessageQueue.Peek((char*)&head, sizeof(MSG_HEAD));
		g_MessageQueue.Dequeue(sizeof(MSG_HEAD));

		if (head._payloadLen > 0)
		{
			g_MessageQueue.Peek((char*)buf, head._payloadLen);
			g_MessageQueue.Dequeue((size_t)head._payloadLen);
			buf[head._payloadLen / sizeof(wchar_t)] = L'\0';

			recvStr = buf;
		}

		ReleaseSRWLockExclusive(&g_MessageQueueLock);


		switch (head._type)
		{
		case eMSG_TYPE::ADD:
			{
				AcquireSRWLockExclusive(&g_ListLock);
				g_List.push_back(recvStr);
				ReleaseSRWLockExclusive(&g_ListLock);

				InterlockedIncrement(&g_TPSAdd);

				break;
			}
		case eMSG_TYPE::DEL:
			{
				AcquireSRWLockExclusive(&g_ListLock);
				if (g_List.empty())
				{
					g_List.pop_front();
				}
				ReleaseSRWLockExclusive(&g_ListLock);

				InterlockedIncrement(&g_TPSDel);

				break;
			}
		case eMSG_TYPE::SORT:
			{
				AcquireSRWLockExclusive(&g_ListLock);
				g_List.sort();
				ReleaseSRWLockExclusive(&g_ListLock);

				InterlockedIncrement(&g_TPSSort);

				break;
			}

		case eMSG_TYPE::FIND:
			{
				AcquireSRWLockShared(&g_ListLock);
				std::list<std::wstring>listCopy = g_List;
				ReleaseSRWLockShared(&g_ListLock);
				
				std::list<std::wstring>::iterator findIter = std::find(listCopy.begin(), listCopy.end(), recvStr);

				InterlockedIncrement(&g_TPSFind);
				
				break;
			}

		case eMSG_TYPE::PRINT:
			{
				AcquireSRWLockShared(&g_ListLock);
				std::list<std::wstring>listCopy = g_List;
				ReleaseSRWLockShared(&g_ListLock);

				FILE* file;
				file = _wfopen(L"WorkerThreadPrintResult", L"w");

				if (file == NULL)
				{
					wprintf(L"# File is NULL\n");
					break;
				}

				for (auto iter = listCopy.begin(); iter != listCopy.end(); ++iter)
				{
					fwprintf(file, L"%s\n", (*iter).c_str());
				}

				fclose(file);

				InterlockedIncrement(&g_TPSPrint);

				break;
			}

		case eMSG_TYPE::QUIT:
			{
				bShutdown = true;
				break;
			}
		default:
			break;
		}

		InterlockedIncrement(&g_TPSEachThread[threadIndex]);
		InterlockedIncrement(&g_TPSAll);
	}

	return 0;
}

unsigned int WINAPI ProviderThread(void* arg)
{	
	DWORD threadID = GetCurrentThreadId();
	wprintf(L"# Provider Thread Start, ID : %d\n", threadID);

	srand(threadID);

	const wchar_t randomStringSource[27] = L"abcdefghijklmnopqrstuvwxyz";

	while (true)
	{
		int typeNum;
		MSG_HEAD head;

		if (_kbhit())
		{
			int ch = _getch();

			if (ch == 27) // esc
			{
				for (int iCnt = 0; iCnt < WORKER_THREAD_NUM; ++iCnt)
				{
					head._type = eMSG_TYPE::QUIT;
					head._payloadLen = 0;

					AcquireSRWLockExclusive(&g_MessageQueueLock);
					g_MessageQueue.Enqueue((char*)&head, sizeof(MSG_HEAD));
					ReleaseSRWLockExclusive(&g_MessageQueueLock);

					SetEvent(g_Event);
				}

				break;
			}
		}

		typeNum = rand() % MSG_TYPE_NUM - 1; // exclude QUIT
		head._type = (eMSG_TYPE)typeNum;;
		
		switch (head._type)
		{
		case ADD:
			{
				int payloadStrLen = rand() % 27;
				head._payloadLen = payloadStrLen;

				AcquireSRWLockExclusive(&g_MessageQueueLock);
				g_MessageQueue.Enqueue((char*)&head, sizeof(MSG_HEAD));
				g_MessageQueue.Enqueue((char*)randomStringSource, sizeof(const wchar_t) * payloadStrLen);
				ReleaseSRWLockExclusive(&g_MessageQueueLock);

				SetEvent(g_Event);

				break;
			}
		case DEL:
			{
				head._payloadLen = 0;

				AcquireSRWLockExclusive(&g_MessageQueueLock);
				g_MessageQueue.Enqueue((char*)&head, sizeof(MSG_HEAD));
				ReleaseSRWLockExclusive(&g_MessageQueueLock);

				SetEvent(g_Event);

				break;
			}
		case SORT:
			{
				head._payloadLen = 0;

				AcquireSRWLockExclusive(&g_MessageQueueLock);
				g_MessageQueue.Enqueue((char*)&head, sizeof(MSG_HEAD));
				ReleaseSRWLockExclusive(&g_MessageQueueLock);

				SetEvent(g_Event);

				break;
			}
		case FIND:
			{
				int payloadStrLen = rand() % 27;
				head._payloadLen = payloadStrLen;

				AcquireSRWLockExclusive(&g_MessageQueueLock);
				g_MessageQueue.Enqueue((char*)&head, sizeof(MSG_HEAD));
				g_MessageQueue.Enqueue((char*)randomStringSource, sizeof(const wchar_t) * payloadStrLen);
				ReleaseSRWLockExclusive(&g_MessageQueueLock);

				SetEvent(g_Event);

				break;
			}
		case PRINT:
			{
				head._payloadLen = 0;

				AcquireSRWLockExclusive(&g_MessageQueueLock);
				g_MessageQueue.Enqueue((char*)&head, sizeof(MSG_HEAD));
				ReleaseSRWLockExclusive(&g_MessageQueueLock);

				SetEvent(g_Event);

				break;
			}
		case QUIT:
			break;
		default:
			break;
		}

		Sleep(TIMEOUT_PROVIDER);
	}

	return 0;
}

unsigned int WINAPI MonitorThread(void* arg)
{
	DWORD threadID = GetCurrentThreadId();
	wprintf(L"# Monitor Thread Start, ID : %d\n", threadID);

	while (true)
	{
		long tpsAdd = g_TPSAdd;
		long tpsDel = g_TPSDel;
		long tpsSort = g_TPSSort;
		long tpsFind = g_TPSFind;
		long tpsPrint = g_TPSPrint;
		long tpsThread1 = g_TPSEachThread[0];
		long tpsThread2 = g_TPSEachThread[1];
		long tpsThread3 = g_TPSEachThread[2];
		long tpsAll = g_TPSAll;

		g_TPSAdd = 0;
		g_TPSDel = 0;
		g_TPSSort = 0;
		g_TPSFind = 0;
		g_TPSPrint = 0;
		g_TPSEachThread[0] = 0;
		g_TPSEachThread[1] = 0;
		g_TPSEachThread[2] = 0;
		g_TPSAll = 0;

		wprintf(L"TPS\n\
 Add : %d\n\
 Del : %d\n\
 Sort : %d\n\
 Find : %d\n\
 Print : %d\n\n\
 Thread 1 : %d\n\
 Thread 2 : %d\n\
 Thread 3 : %d\n\
 All : %d\n\n",
			tpsAdd, tpsDel, tpsSort, tpsFind, tpsPrint, tpsThread1, tpsThread2, tpsThread3, tpsAll);

		Sleep(TIMEOUT_MONITOR);
	}

	return 0;
}