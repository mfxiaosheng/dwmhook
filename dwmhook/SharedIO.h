#pragma once
#include <windows.h>

#define  SHARED_SIZE 4096*100

struct Point
{
	int x;
	int y;
};

struct DLine
{
	Point start;
	Point end;
	ULONG rgb;
};

struct DLine
{
	Point start;
	Point end;
	ULONG rgb;
};












struct SharedMem
{
	char symbol_sig[100];
	int symbol_offset;
};

class SharedIO
{
public:
	SharedIO();
	~SharedIO();

	bool InitSharedMem();
	SharedMem* shared_mem_;

public:
	HANDLE hmap_;
	LPVOID lpdata_;
};

