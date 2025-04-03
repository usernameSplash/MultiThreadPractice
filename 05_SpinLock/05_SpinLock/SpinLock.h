#pragma once

#include <Windows.h>

class SpinLock
{
public:
	SpinLock() {};
	~SpinLock() {};

public:
	void Lock(void);
	void Unlock(void);

private:
	volatile LONG _key = 0;
};