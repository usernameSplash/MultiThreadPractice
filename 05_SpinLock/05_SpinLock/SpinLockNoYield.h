#pragma once

#include <Windows.h>

class SpinLockNoYield
{
public:
	SpinLockNoYield() {};
	~SpinLockNoYield() {};

public:
	void Lock(void);
	void Unlock(void);

private:
	volatile LONG _key = 0;
};