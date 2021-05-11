#pragma once

#include <Windows.h>
#include <cstdio>
#include <Psapi.h>
#include <process.h>
#include <d3d11.h>
#include <dxgi.h>
#include <d3d11_1.h>
#include <intrin.h>
#include <d2d1_1.h>
#include <d2d1_2.h>
#include <d2d1_3.h>
#include <d3d11_2.h>
#include <d2d1_3helper.h>
#include <dwrite_3.h>
#include <dcomp.h>
#include <d3d11_1.h>
#include <DirectXMath.h>
#include <DirectXPackedVector.h>
#include <d3dcompiler.h>
#include "SharedIO.h"
#include "imgui/imgui_impl_dx11.h"
#include "imgui/imgui_impl_win32.h"
#include "imgui/imgui_internal.h"
#include "XorStr.h"
#define XorStr xorstr_
using namespace DirectX;

#pragma comment( lib, "dxgi" )
#pragma comment( lib, "d2d1" )
#pragma comment( lib, "d3d11" )
#pragma comment( lib, "dcomp" )
#pragma comment( lib, "dwrite" )
#pragma comment( lib, "Gdi32.lib" )
#pragma comment(lib,"Advapi32.lib")
#pragma comment(lib,"User32.lib")
#ifdef _DEBUG
#pragma comment( lib, "minhook-debug.lib" )
#else
#pragma comment( lib, "minhook.lib" )
#endif
#pragma comment( lib, "d3dcompiler.lib" )

#define SAFE_RELEASE( p )	if( p ) { p->Release(); p = nullptr; }


#include "MinHook.h"
#include "save_state.hpp"