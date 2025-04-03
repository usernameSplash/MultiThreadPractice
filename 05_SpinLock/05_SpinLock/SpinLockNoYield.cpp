#include "SpinLockNoYield.h"

void SpinLockNoYield::Lock(void)
{
	while (InterlockedExchange(&_key, 1) == 1)
	{
		while (_key == 1)
		{
			//YieldProcessor();
		}
	}

	return;
}

void SpinLockNoYield::Unlock(void)
{
	_key = 0;

	return;
}