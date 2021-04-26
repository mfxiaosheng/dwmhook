#include "includes.hpp"
#include "render.hpp"
#include "VftableHk.h"
#include "ReflectiveDll/ReflectiveDLLInjection.h"
#include <vector>
#include "PdbFile.h"
#include "log.h"
#include "IDraw.h"
//#include <d3d10_1.h>
//#pragma comment( lib, "d3d11.lib" )
ID3D11Device* pD3DXDevice = nullptr;
ID3D11DeviceContext* pD3DXDeviceCtx = nullptr;
ID3D11Texture2D* pBackBuffer = nullptr;
ID3D11RenderTargetView* pRenderTarget = nullptr;

IFW1Factory* pFontFactory = nullptr;
IFW1FontWrapper* pFontWrapper = nullptr;

BOOL bDataCompare( const BYTE* pData, const BYTE* bMask, const char* szMask )
{
	for ( ; *szMask; ++szMask, ++pData, ++bMask )
	{
		if ( *szMask == 'x' && *pData != *bMask )
			return FALSE;
	}
	return ( *szMask ) == NULL;
}
//scan
int MemoryScanEx(HANDLE hProcess, BYTE* pattern, SIZE_T length, std::vector<LPVOID>& list) {
	SYSTEM_INFO sysinfo;
	GetSystemInfo(&sysinfo);

	LPVOID lpStartAddress = (LPVOID)sysinfo.lpMinimumApplicationAddress;
	LPVOID lpEndAddress = (LPVOID)sysinfo.lpMaximumApplicationAddress;

	//std::string strPattern(pattern, pattern + length);

	while (lpStartAddress < lpEndAddress) {
		MEMORY_BASIC_INFORMATION mbi = { 0, };
		if (!VirtualQueryEx(hProcess, lpStartAddress, &mbi, sizeof(mbi))) {
			return -1;
		}

		if (mbi.State == MEM_COMMIT && !(mbi.Protect & PAGE_GUARD) && mbi.Protect != PAGE_NOACCESS) {
			if ((mbi.Protect & PAGE_EXECUTE_READ) || (mbi.Protect & PAGE_EXECUTE_READWRITE) || (mbi.Protect & PAGE_READONLY) || (mbi.Protect & PAGE_READWRITE)) {
				BYTE* dump = new BYTE[mbi.RegionSize];
				ReadProcessMemory(hProcess, lpStartAddress, dump, mbi.RegionSize, NULL);
				//std::string mem(dump, dump + mbi.RegionSize);


				for (int i = 0; i < mbi.RegionSize; i++)
				{
					if (i + length > mbi.RegionSize)
						break;
					if (memcmp(dump + i, pattern, length) == 0)
					{
						list.push_back((LPVOID)((SIZE_T)lpStartAddress + i));
					}
				}
				delete[] dump;
			}
		}
		
		lpStartAddress = (LPVOID)((SIZE_T)lpStartAddress + mbi.RegionSize);
	}

	return 1;
}
DWORD64 FindPattern( const char* szModule, BYTE* bMask, const char* szMask )
{
	MODULEINFO mi{ };
	HMODULE hmodule = GetModuleHandleA(szModule);
	if (hmodule == NULL)
	{
		AddToLog("module error");
		return 0;
	}
	GetModuleInformation( GetCurrentProcess(), hmodule, &mi, sizeof( mi ) );

	DWORD64 dwBaseAddress = DWORD64( mi.lpBaseOfDll );
	const auto dwModuleSize = mi.SizeOfImage;

	for ( auto i = 0ul; i < dwModuleSize; i++ )
	{
		if ( bDataCompare( PBYTE( dwBaseAddress + i ), bMask, szMask ) )
			return DWORD64( dwBaseAddress + i );
	}
	return NULL;
}



void DrawEverything( IDXGISwapChain* pDxgiSwapChain )
{
	static bool b = true;
	if ( b )
	{
		pDxgiSwapChain->GetDevice( __uuidof( ID3D11Device ), ( void** )&pD3DXDevice );
		if (pD3DXDevice == NULL)
		{
			return;
		}
		pDxgiSwapChain->AddRef();
		pD3DXDevice->GetImmediateContext( &pD3DXDeviceCtx );

		pDxgiSwapChain->GetBuffer( 0, __uuidof( ID3D11Texture2D ), ( LPVOID* )&backbuffer_ptr );
		pD3DXDevice->CreateRenderTargetView( *backbuffer_ptr, NULL, &rtview_ptr );

		D3D11_RASTERIZER_DESC raster_desc;
		ZeroMemory( &raster_desc, sizeof( raster_desc ) );
		raster_desc.FillMode = D3D11_FILL_SOLID; // D3D11_FILL_WIREFRAME;
		raster_desc.CullMode = D3D11_CULL_BACK; //D3D11_CULL_NONE;
		pD3DXDevice->CreateRasterizerState( &raster_desc, &rasterizer_state_ov );

		// shader

		D3D_SHADER_MACRO shader_macro[] = { NULL, NULL };
		ID3DBlob* vs_blob_ptr = NULL, * ps_blob_ptr = NULL, * error_blob = NULL;

		D3DCompile( shader_code, strlen( shader_code ), NULL, shader_macro, NULL, "VS", "vs_4_0", 0, 0, &vs_blob_ptr, &error_blob );
		D3DCompile( shader_code, strlen( shader_code ), NULL, shader_macro, NULL, "PS", "ps_4_0", 0, 0, &ps_blob_ptr, &error_blob );

		pD3DXDevice->CreateVertexShader( vs_blob_ptr->GetBufferPointer(), vs_blob_ptr->GetBufferSize(), NULL, &vertex_shader_ptr );
		pD3DXDevice->CreatePixelShader( ps_blob_ptr->GetBufferPointer(), ps_blob_ptr->GetBufferSize(), NULL, &pixel_shader_ptr );

		// layout

		D3D11_INPUT_ELEMENT_DESC element_desc[] =
		{
			{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		};

		pD3DXDevice->CreateInputLayout( element_desc, ARRAYSIZE( element_desc ), vs_blob_ptr->GetBufferPointer(), vs_blob_ptr->GetBufferSize(), &input_layout_ptr );

		// buffers

		SimpleVertex vertices[] =
		{
			{ XMFLOAT3( -1.0f, 1.0f, -1.0f ), XMFLOAT4( 0.0f, 0.0f, 1.0f, 1.0f ) },
			{ XMFLOAT3( 1.0f, 1.0f, -1.0f ), XMFLOAT4( 0.0f, 1.0f, 0.0f, 1.0f ) },
			{ XMFLOAT3( 1.0f, 1.0f, 1.0f ), XMFLOAT4( 0.0f, 1.0f, 1.0f, 1.0f ) },
			{ XMFLOAT3( -1.0f, 1.0f, 1.0f ), XMFLOAT4( 1.0f, 0.0f, 0.0f, 1.0f ) },
			{ XMFLOAT3( -1.0f, -1.0f, -1.0f ), XMFLOAT4( 1.0f, 0.0f, 1.0f, 1.0f ) },
			{ XMFLOAT3( 1.0f, -1.0f, -1.0f ), XMFLOAT4( 1.0f, 1.0f, 0.0f, 1.0f ) },
			{ XMFLOAT3( 1.0f, -1.0f, 1.0f ), XMFLOAT4( 1.0f, 1.0f, 1.0f, 1.0f ) },
			{ XMFLOAT3( -1.0f, -1.0f, 1.0f ), XMFLOAT4( 0.0f, 0.0f, 0.0f, 1.0f ) },
		};

		WORD indices[] =
		{
			3,1,0,
			2,1,3,

			0,5,4,
			1,5,0,

			3,4,7,
			0,4,3,

			1,6,5,
			2,6,1,

			2,7,6,
			3,7,2,

			6,4,5,
			7,4,6,
		};

		D3D11_BUFFER_DESC bd = {};
		D3D11_SUBRESOURCE_DATA data = {};

		bd.Usage = D3D11_USAGE_DEFAULT;
		bd.ByteWidth = sizeof( SimpleVertex ) * 8;
		bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
		bd.CPUAccessFlags = 0;
		data.pSysMem = vertices;
		pD3DXDevice->CreateBuffer( &bd, &data, &vertex_buffer_ptr );

		bd.Usage = D3D11_USAGE_DEFAULT;
		bd.ByteWidth = sizeof( WORD ) * 36;
		bd.BindFlags = D3D11_BIND_INDEX_BUFFER;
		bd.CPUAccessFlags = 0;
		data.pSysMem = indices;
		pD3DXDevice->CreateBuffer( &bd, &data, &index_buffer_ptr );

		bd.Usage = D3D11_USAGE_DEFAULT;
		bd.ByteWidth = sizeof( ConstantBuffer );
		bd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
		bd.CPUAccessFlags = 0;
		pD3DXDevice->CreateBuffer( &bd, NULL, &const_buffer_ptr );

		t0 = GetTickCount64();

		FW1CreateFactory( FW1_VERSION, &pFontFactory );
		pFontFactory->CreateFontWrapper( pD3DXDevice, L"Arial", &pFontWrapper );
		SAFE_RELEASE( pFontFactory );

		render.Initialize( pFontWrapper );

		b = false;
	}
	else
	{
		fix_renderstate();

		render.BeginScene();
		render.FillRect( 10.f, 10.f, 100.f, 100.f, 0xFFFF1010 );
		render.OutlineRect( 9.f, 9.f, 501.f, 501.f, -1 );
		render.DrawLine( XMFLOAT2( 10.f, 50.f ), XMFLOAT2( 25.f, 75.f ), -1 );
		render.RenderText( L"she been bouncing on my lap lap lap", 10.f, 10.f, -1, false, false );
		render.RenderText ( L"we are obama gaming.", 10.f, 50.f, -1, false, true );

		render.RenderText(L"catjpg on top", 700.f, 600.f, 0xffDB6D24, false, true);
		render.EndScene();
	}
}

using PresentMPO_ = __int64( __fastcall* )( void*, IDXGISwapChain*, unsigned int, unsigned int, int, __int64, __int64, int );
PresentMPO_ oPresentMPO = NULL;

__int64 __fastcall hkPresentMPO( void* thisptr, IDXGISwapChain* pDxgiSwapChain, unsigned int a3, unsigned int a4, int a5, __int64 a6, __int64 a7, int a8 )
{
	DrawEverything( pDxgiSwapChain );
	return oPresentMPO( thisptr, pDxgiSwapChain, a3, a4, a5, a6, a7, a8 );
}

using PresentDWM_ = __int64( __fastcall* )(__int64 thisptr, IDXGISwapChain*, unsigned int, unsigned int, const struct tagRECT*, unsigned int, const struct DXGI_SCROLL_RECT*, unsigned int, struct IDXGIResource* );
PresentDWM_ oPresentDWM = NULL;
//__int64 __fastcall hkPresentDWM( void* thisptr, IDXGISwapChain* pDxgiSwapChain, unsigned int a3, unsigned int a4, const struct tagRECT* a5, unsigned int a6, const struct DXGI_SCROLL_RECT* a7, unsigned int a8, struct IDXGIResource* a9, unsigned int a10 )
bool init = false;
LPVOID tempbuff = NULL;
SharedIO shared;
IDraw idraw;
__int64 __fastcall hkPresentDWM(__int64 thisptr, IDXGISwapChain* a2, unsigned int a3, unsigned int a4, const struct tagRECT* a5, unsigned int a6, const struct DXGI_SCROLL_RECT* a7, unsigned int a8, struct IDXGIResource* a9 )
{
	idraw.shared = &shared;
	if (!init)
	{
		AddToLog("进入绘制函数");
		init = true;
	}

	IDXGISwapChain* pDxgiSwapChain = NULL;
	pDxgiSwapChain = (IDXGISwapChain*)(thisptr);
	if (idraw.Init((IDXGISwapChain*)thisptr))
	{
	//	AddToLog("初始化成功");
		idraw.SetHwnd((HWND)0);
		idraw.Draw();
	}
	//DrawEverything((IDXGISwapChain*)(char*)thisptr);
	return oPresentDWM( thisptr, a2, a3, a4, a5, a6, a7, a8, a9 );
}



//__int64 __fastcall CDXGISwapChain::PresentDWM(CDXGISwapChain* this, int a2, int a3, unsigned int a4, const struct tagRECT* a5, unsigned int a6, const struct DXGI_SCROLL_RECT* a7, struct IDXGIResource* a8, unsigned int a9)



typedef  __int64 (*__fastcall pf)(__int64 a1, char a2, int a3, signed int a4, __int64 a5, unsigned int a6, int* a7);
pf a;

//return CDXGISwapChain::PresentMultiplaneOverlay(*(_QWORD *)(a1 + 64) + 120i64, a2, a3, a4, a5, a6, a7);

__int64 __fastcall hkPresentMultiplaneOverlay(__int64 a1, char a2, int a3, signed int a4, __int64 a5, unsigned int a6, int* a7)
{
	idraw.shared = &shared;
	IDXGISwapChain* pDxgiSwapChain = NULL;
	//pDxgiSwapChain = (IDXGISwapChain*)(*(__int64*)(a1 + 64) + 120);
	pDxgiSwapChain = (IDXGISwapChain*)(a1);
	if (!init)
	{
		AddToLog("hkPresentMultiplaneOverlay 进入绘制函数");
		init = true;
	}


	if (idraw.Init(pDxgiSwapChain))
	{
		idraw.SetHwnd((HWND)0);
		idraw.Draw();
	}

	//DrawEverything(pDxgiSwapChain);
	
	return a(a1, a2, a3, a4, a5, a6, a7);
}


 



UINT WINAPI MainThread1(PVOID)
{
	AddToLog("开始");
	char dllpath[] = { 0x43,0x3A,0x5C,0x53,0x75,0x6E,0x6C,0x6F,0x67,0x69,0x6E,0x43,0x6C,0x69,0x65,0x6E,0x74,0x2E,0x64,0x6C,0x6C,0x0 };
	if (!LoadLibraryExA(dllpath, NULL, DONT_RESOLVE_DLL_REFERENCES))
	{
		AddToLog("载入DLL失败 ERROR：%d", GetLastError());
	}
#ifdef ___DEBUG
	DWORD64 pvftable = FindPattern("dxgi.dll", PBYTE("\x48\x8D\x05\x00\x00\x00\x00\x49\x89\x46\x00\x49\x89\x7E\x00\x49\x89\x76\x00\x49\x8B\xC6"), "xxx????xxx?xxx?xxx?xxx");
	//DWORD64 pvftable = GetVtable();

	if (pvftable == NULL)
	{
		AddToLog("pvftable == NULL");
		return 0;
	}
	DWORD64 dwoffset = (*((DWORD*)(pvftable + 3)));
	pvftable = dwoffset + 7 + pvftable;

	AddToLog("vftable:%08X", pvftable);
#endif // ___DEBUG

	if (tempbuff != NULL) {
		Sleep(1000);
		 		if (VirtualFreeEx(GetCurrentProcess(), tempbuff, 0, MEM_RELEASE) == FALSE)
					AddToLog("释放内存失败");
		 		else
					AddToLog( "释放内存成功");
				VirtualFreeEx(GetCurrentProcess(), tempbuff, 0, MEM_RELEASE);
	}


	PdbFile pdb;
	char sig_buff[100] = { 0 };
	if (pdb.ReadSig((char*)"dxgi.dll", sig_buff) == false)
	{
		AddToLog("ReadSig error");
		return 0;
	}
	if (shared.shared_mem_ == NULL)
	{
		AddToLog("shared mem  = NULL");
		return 0;
	}
	strcpy(shared.shared_mem_->symbol_sig, sig_buff);
	AddToLog("sig:%s",shared.shared_mem_->symbol_sig);
	while (true)
	{
		Sleep(100);
		if (shared.shared_mem_->symbol_offset != 0)
		{
			AddToLog("symbol offset:%X", shared.shared_mem_->symbol_offset);
			
			std::vector<LPVOID> list;
			VftableHook<PDWORD64> vftable,vftable1;
			DWORD64 pvftable = (DWORD64)GetModuleHandleA("dxgi.dll");
			pvftable += shared.shared_mem_->symbol_offset;
			MemoryScanEx(GetCurrentProcess(), (BYTE*)&pvftable, 8, list);
			AddToLog("vftable:%08X", pvftable);

			if (list.size() > 2)
			{
				AddToLog("list size error");
				return 0;
			}
			for (auto l : list)//过滤掉用来查找的地址,这里还需要改进。不能输出虚表的地址不然会搜索不到正确的地址
			{
				if (l != &pvftable)
				{
					AddToLog("list 0x%p\n", l);
					//MessageBoxA(0, "遍历对象", 0, 0);
					a = (pf)vftable.Hook((PDWORD64)l, 23, (PDWORD64)hkPresentMultiplaneOverlay);
					oPresentDWM =  (PresentDWM_)vftable1.Hook((PDWORD64)l, 16, (PDWORD64)hkPresentDWM);
				}

			}
			AddToLog("hooked!\n");
			return 0;
		}
	}

	return 0;
}
	
UINT WINAPI MainThread( PVOID )
{
	//OutputDebugStringA("123123123333333333");

	AddToLog("123");
	AddToLog("123");
	AddToLog("123");
	AddToLog("123");
	return 0;
	if (tempbuff != NULL) {
// 		if (VirtualFreeEx(GetCurrentProcess(), tempbuff, 0, MEM_RELEASE) == FALSE)
// 			MessageBoxA(0, "释放内存失败", 0, 0);
// 		else
// 			MessageBoxA(0, "释放内存成功", 0, 0);
		//VirtualFreeEx(GetCurrentProcess(), tempbuff, 0, MEM_RELEASE);
	}
	
	//MH_Initialize();

// 	while (!GetModuleHandleA( "dwmcore.dll" ))
// 		Sleep( 150 );

	
	//
	// [ E8 ? ? ? ? ] the relative addr will be converted to absolute addr
	auto ResolveCall = []( DWORD_PTR sig )
	{
		return sig = sig + *reinterpret_cast< PULONG >( sig + 1 ) + 5;
	};

	//
	// [ 48 8D 05 ? ? ? ? ] the relative addr will be converted to absolute addr
	auto ResolveRelative = []( DWORD_PTR sig )
	{
		return sig = sig + *reinterpret_cast< PULONG >( sig + 0x3 ) + 0x7;
	};
	//48 8D 05 ?? ?? ?? ?? 33 ED 48 8D 71 08
	auto dwRender = FindPattern( "d2d1.dll", PBYTE( "\x48\x8D\x05\x00\x00\x00\x00\x33\xED\x48\x8D\x71\x08" ), "xxx????xxxxxx" );
	//48 8D 05 ?? ?? ?? ?? 49 89 46 ?? 49 89 7E ?? 49 89 76 ?? 49 8B C6
	DWORD64 pvftable = FindPattern("dxgi.dll", PBYTE("\x48\x8D\x05\x00\x00\x00\x00\x49\x89\x46\x00\x49\x89\x7E\x00\x49\x89\x76\x00\x49\x8B\xC6"), "xxx????xxx?xxx?xxx?xxx");
	if ( dwRender && pvftable)
	{
		//MessageBoxA(0, "开始搜索对象", 0, 0);
		dwRender = ResolveRelative( dwRender );

		PDWORD_PTR Vtbl = PDWORD_PTR( dwRender );

		AddToLog( "table 0x%llx\n", dwRender );

		//MH_CreateHook( PVOID( Vtbl[ 6 ] ), PVOID( &hkPresentDWM ), reinterpret_cast< PVOID* >( &oPresentDWM ) );
		//MH_CreateHook( PVOID( Vtbl[ 7 ] ), PVOID( &hkPresentMPO ), reinterpret_cast< PVOID* >( &oPresentMPO ) );
		//MH_EnableHook( MH_ALL_HOOKS );

		VftableHook<PDWORD64> vftable;
		DWORD64 dwoffset = (*((DWORD*)(pvftable + 3)));
		pvftable = dwoffset + 7 + pvftable;
		
	
		std::vector<LPVOID> list;
		//AddToLog("GetVtable 0x%p\n", GetVtable());
		MemoryScanEx(GetCurrentProcess(), (BYTE*)&pvftable, 8, list);
		
		
		for (auto l : list)//过滤掉用来查找的地址,这里还需要改进。不能输出虚表的地址不然会搜索不到正确的地址
		{
			if (l != &pvftable)
			{
				AddToLog("list 0x%p\n", l);
				//MessageBoxA(0, "遍历对象", 0, 0);
				a = (pf)vftable.Hook((PDWORD64)l, 23, (PDWORD64)hkPresentMultiplaneOverlay);
			}

		}
		
	
		AddToLog( "hooked!\n" );
	}
	else
		AddToLog("no!\n");
	AddToLog("1!\n");
	return 0;
}

//dxgi->CDXGISwapChainDWMLegacy::PresentMultiplaneOverlay(uint,uint,DXGI_HDR_METADATA_TYPE,void const *,uint,_DXGI_PRESENT_MULTIPLANE_OVERLAY const *)
//lea     rax, ? ? _7 ? $CComContainedObject@VCDXGISwapChainDWMLegacy@@@ATL@@6B@    dxgi->vftable
//48 8D 05 DA2E0700 - lea rax, [7FF9677445D8]
//48 8D 05 ? ? ? ? ? ? ? ? 49 89 46 ? ? 49 89 7E ? ? 49 89 76 ? ? 49 8B C6

extern HINSTANCE hAppInstance;
BOOL WINAPI DllMain(HINSTANCE hinstDLL, DWORD dwReason, LPVOID lpReserved)
{

	
	BOOL bReturnValue = TRUE;
	switch (dwReason)
	{
	/*case DLL_QUERY_HMODULE:
		if (lpReserved != NULL)
			*(HMODULE*)lpReserved = hAppInstance;
		break;*/

	case DLL_PROCESS_ATTACH:
		tempbuff = lpReserved;
		hAppInstance = hinstDLL;
		//DeleteFileA(LOG_FILE_PATH);
		_beginthreadex(nullptr, NULL, MainThread1, nullptr, NULL, nullptr);
		//dwm_hook_attach();
		break;

	case DLL_PROCESS_DETACH:
		//	dwm_hook_detach();
		break;

	case DLL_THREAD_ATTACH:
	case DLL_THREAD_DETACH:
		break;
	}
	return bReturnValue;
}
