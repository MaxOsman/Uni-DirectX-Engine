#pragma once

#include <string>
#include <Windows.h>

class Debug
{
public:
	void DebugNumber(int output)
	{
		char sz[1024] = { 0 };
		sprintf_s(sz, "The number is %d \n", output);
		OutputDebugStringA(sz);
	}

	void DebugString(std::string output)
	{
		char sz[1024] = { 0 };
		sprintf_s(sz, "The string is %d \n", output);
		OutputDebugStringA(sz);
	}
};