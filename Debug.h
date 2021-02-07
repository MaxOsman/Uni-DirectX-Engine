#pragma once

#include <string>
#include <Windows.h>

class Debug
{
public:
	void DebugNumber(float output)
	{
		char sz[1024] = { 0 };
		sprintf_s(sz, "\n", output);
		OutputDebugStringA(sz);
	}

	void DebugString(std::string output)
	{
		OutputDebugStringA(output.c_str());
	}
};