#pragma once
#include "includes.hpp"
#include "imgui/imgui_impl_dx11.h"
#include "imgui/imgui_impl_win32.h"
class IDraw
{
public:
	IDraw();
	~IDraw();
	bool Init(IDXGISwapChain* pDxgiSwapChain);
	void SetHwnd(HWND hwnd);
	bool Draw();
public:
	HWND hwnd_;
	bool is_init_;
	ID3D11Device* pD3DXDevice;
	ID3D11DeviceContext* pD3DXDeviceCtx;
	ID3D11RenderTargetView* mainRenderTargetView;
};

