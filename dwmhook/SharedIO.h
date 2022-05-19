#pragma once
#ifndef __TEST_H__
#define __TEST_H__
//Í·ÎÄ¼þ
#include <windows.h>
#include "imgui/imgui.h"
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
	float thickness;
};

struct DRect
{
	ImVec4 rect;
	ULONG rgb;
	float thickness;
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
	int line_num;
	DRect rect_list[1000];
	int rect_num;
	DCircle circle_list[1000];
	int circle_num;
	DText text_list[1000];
	int text_num;
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
#endif //__TEST_H__
