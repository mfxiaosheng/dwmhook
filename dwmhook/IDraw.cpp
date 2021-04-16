#include "IDraw.h"
#include "log.h"
IDraw::IDraw()
{
		//ImGui_ImplDX11_Init(g_hWnd, g_pd3dDevice, g_pd3dContext);
	is_init_ = false;
	pD3DXDevice = NULL;
	pD3DXDeviceCtx = NULL;
}

IDraw::~IDraw()
{

}

bool IDraw::Init(IDXGISwapChain* pDxgiSwapChain)
{
	if (is_init_)
		return true;

	HWND window;

	if (SUCCEEDED(pDxgiSwapChain->GetDevice(__uuidof(ID3D11Device), (void**)&pD3DXDevice)))
	{
		pD3DXDevice->GetImmediateContext(&pD3DXDeviceCtx);
		DXGI_SWAP_CHAIN_DESC sd;
		pDxgiSwapChain->GetDesc(&sd);
		window = sd.OutputWindow;
		ID3D11Texture2D* pBackBuffer;
		pDxgiSwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID*)&pBackBuffer);
		if (pBackBuffer == NULL)
		{
			return false;
		}
		pD3DXDevice->CreateRenderTargetView(pBackBuffer, NULL, &mainRenderTargetView);
		pBackBuffer->Release();
		//oWndProc = (WNDPROC)SetWindowLongPtr(window, GWLP_WNDPROC, (LONG_PTR)WndProc);
		//InitImGui();

		ImGui::CreateContext();
		ImGuiIO& io = ImGui::GetIO();
		io.ConfigFlags = ImGuiConfigFlags_NoMouseCursorChange;
		ImGui_ImplWin32_Init(window);
		ImGui_ImplDX11_Init(pD3DXDevice, pD3DXDeviceCtx);
		if (!SetFont((char*)"c:\\Windows\\Fonts\\simhei.ttf"))
			AddToLog("初始化字体失败");
		
		is_init_ = true;
	}
	return true;
}

//bool IDraw::SetFont(void* font_data)
//{
//	ImGuiIO& io = ImGui::GetIO();
//
//	//io.Fonts->AddFontFromMemoryTTF(font_data, sizeof(font_data), 14.0f, nullptr, io.Fonts->GetGlyphRangesCyrillic());
//	font_ = io.Fonts->AddFontFromMemoryTTF(font_data, sizeof(font_data), 32.0f, nullptr, io.Fonts->GetGlyphRangesCyrillic());
//
//	return font_ ? true : false;
//}

bool IDraw::SetFont(char* font_path)
{
	ImGuiIO& io = ImGui::GetIO();

	io.Fonts->AddFontFromFileTTF("c:\\Windows\\Fonts\\simhei.ttf", 14.0f, NULL, io.Fonts->GetGlyphRangesChineseFull());

	font_ = io.Fonts->AddFontFromFileTTF("c:\\Windows\\Fonts\\simhei.ttf", 18.0f, NULL, io.Fonts->GetGlyphRangesChineseFull());
	return font_ ? true : false;
}

void IDraw::SetHwnd(HWND hwnd)
{
	this->hwnd_ = hwnd;
}

bool IDraw::Draw()
{
	
	ImGui_ImplDX11_NewFrame();
	ImGui_ImplWin32_NewFrame(this->hwnd_);
	ImGui::NewFrame();

	ImGui::Begin("ImGui Window");


	Point point;
	point.x = 500.0;
	point.y = 100.0;

	ImU32 a = GetColor(0xE96A16FF);
	this->DarwText("Test Test", point, a, 15, true);


	ImGui::Text("Text Test");
	ImGui::End();

	ImGui::Render();
	

	pD3DXDeviceCtx->OMSetRenderTargets(1, &mainRenderTargetView, NULL);
	ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
	return true;
}

ImU32 IDraw::GetColor(ULONG color)
{
	float a = (color >> 24) & 0xff;
	float r = (color >> 16) & 0xff;
	float g = (color >> 8) & 0xff;
	float b = (color) & 0xff;
	
	return ImGui::GetColorU32({ r / 255.0f, g / 255.0f, b / 255.0f, a / 255.0f });
}

//if (!this->font)
//return;
//
//if (center)
//{
//	FW1_RECTF nullRect = { 0.f, 0.f, 0.f, 0.f };
//	FW1_RECTF rect = this->font->MeasureString(txt, L"Arial", 17.0f, &nullRect, FW1_NOWORDWRAP);
//
//	auto v = XMFLOAT2{ rect.Right, rect.Bottom };
//
//	x -= v.x / 2.f;
//}
//
//if (shadow)
//{
//	this->font->DrawString(pD3DXDeviceCtx, txt, 17.0f, x - 1, y, 0xFF000000, FW1_RESTORESTATE);
//	this->font->DrawString(pD3DXDeviceCtx, txt, 17.0f, x + 1, y, 0xFF000000, FW1_RESTORESTATE);
//	this->font->DrawString(pD3DXDeviceCtx, txt, 17.0f, x, y - 1, 0xFF000000, FW1_RESTORESTATE);
//	this->font->DrawString(pD3DXDeviceCtx, txt, 17.0f, x, y + 1, 0xFF000000, FW1_RESTORESTATE);
//}
//
//this->font->DrawString(pD3DXDeviceCtx, txt, 17.0f, x, y, color, FW1_RESTORESTATE);
bool IDraw::DarwText(std::string text, Point point, ULONG rgb, int size, bool filled)
{
	ImVec2 vec = { point.x,point.y };

	if (filled)
	{
		//返回宽高
		//ImVec2 text_size =  ImGui::GetFont()->CalcTextSizeA(size, 0, 0, text.c_str());
		ImGuiIO& io = ImGui::GetIO();
		
		ImGui::GetForegroundDrawList()->AddText(font_, size, ImVec2(point.x-1, point.y), 0xFF000000, text.c_str());
		ImGui::GetForegroundDrawList()->AddText(font_, size, ImVec2(point.x+1, point.y), 0xFF000000, text.c_str());
		ImGui::GetForegroundDrawList()->AddText(font_, size, ImVec2(point.x, point.y-1), 0xFF000000, text.c_str());
		ImGui::GetForegroundDrawList()->AddText(font_, size, ImVec2(point.x, point.y+1), 0xFF000000, text.c_str());
	}
	
	ImGui::GetForegroundDrawList()->AddText(font_, size, vec, rgb, text.c_str());
	ImGui::GetOverlayDrawList()->AddText(font_, size, vec, rgb, text.c_str());
	
		return true;
}
