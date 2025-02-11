
#include "Protocol.h"

LONG g_TPSAdd = 0;
LONG g_TPSDel = 0;
LONG g_TPSSort = 0;
LONG g_TPSFind = 0;
LONG g_TPSPrint = 0;

LONG g_TPSAll = 0;
LONG g_TPSEachThread[WORKER_THREAD_NUM] = { 0, };
