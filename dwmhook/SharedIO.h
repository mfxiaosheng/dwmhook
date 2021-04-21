#pragma once
#include <windows.h>
#include "includes.hpp"
#define  SHARED_SIZE 4096*100

 struct Point
{
	float x;
	float y;
};

 struct DLine
{
	ImVec2 start;
	ImVec2 end;
	ULONG rgb;
	int size;
};

 struct DRect
{
	ImVec4 rect;
	ULONG rgb;
	int thickness;
	bool filled;
};

  struct DCircle
 {
	 ImVec2 point;
	 int radius;
	 ULONG rgb;
	 float thickness;
	 bool filled;
 };
  struct DText
 {
	 ImVec2 point;
	 char text[255];
	 ULONG rgb;
	 float size;
	 bool filled;
 };

struct SharedMem
{
	char symbol_sig[100];
	int symbol_offset;

	DLine line_list[1000];
	DRect rect_list[1000];
	DCircle circle_list[1000];
	DText text_list[1000];
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

