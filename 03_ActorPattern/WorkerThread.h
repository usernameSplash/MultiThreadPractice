#pragma once

#include <list>
#include <string>
#include <cstdlib>
#include <cstdio>

#include <Windows.h>
#include <process.h>
#include <synchapi.h>
#include <conio.h>

#include "Protocol.h"
#include "RingBuffer.h"

#define TIMEOUT_PROVIDER 50
#define TIMEOUT_MONITOR 1000

static MyDataStructure::RingBuffer g_MessageQueue;
static std::list<std::wstring> g_List;
static SRWLOCK g_MessageQueueLock;
static SRWLOCK g_ListLock;

static HANDLE g_Event;

void InitializeSetting(void);

unsigned int WINAPI WorkerThread(void* arg);
unsigned int WINAPI ProviderThread(void* arg);
unsigned int WINAPI MonitorThread(void* arg);