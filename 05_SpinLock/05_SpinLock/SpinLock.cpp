#include "SpinLock.h"

void SpinLock::Lock(void)
{
	while (InterlockedExchange(&_key, 1) == 1)
	{
		while (_key == 1)
		{
			YieldProcessor();
		}
	}

	return;
}

void SpinLock::Unlock(void)
{
	_key = 0;

	return;
}