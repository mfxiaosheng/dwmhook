#pragma once
#include "includes.hpp"
#include <string.h>
#include <string>
class IDraw
{
public:
	IDraw();
	~IDraw();
	bool Init(IDXGISwapChain* pDxgiSwapChain);
	bool SetFont(char* font_path);
//	bool SetFont(void* font_data);
	void SetHwnd(HWND hwnd);
	bool Draw();
	ImU32 GetColor(ULONG color);
	bool DarwText(std::string text,Point point,ULONG rgb,int size,bool filled);
public:
	//SharedIO shared;
	HWND hwnd_;
	bool is_init_;
	ID3D11Device* pD3DXDevice;
	ID3D11DeviceContext* pD3DXDeviceCtx;
	ID3D11RenderTargetView* mainRenderTargetView;
public:
	ImFont* font_;
};

