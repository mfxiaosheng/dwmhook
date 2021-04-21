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

std::string IDraw::string_to_utf8(const std::string& str)
{
	int nwLen = MultiByteToWideChar(CP_ACP, 0, str.c_str(), -1, NULL, 0);
	wchar_t* pwBuf = new wchar_t[nwLen + 1];
	if (!pwBuf)
		return "";
	memset(pwBuf, 0, nwLen * 2 + 2);
	MultiByteToWideChar(CP_ACP, 0, str.c_str(), str.length(), pwBuf, nwLen);

	int nLen = WideCharToMultiByte(CP_UTF8, 0, pwBuf, -1, NULL, NULL, NULL, NULL);
	char* pBuf = new char[nLen + 1];
	if (!pBuf)
		return "";
	memset(pBuf, 0, nLen + 1);
	WideCharToMultiByte(CP_UTF8, 0, pwBuf, nwLen, pBuf, nLen, NULL, NULL);

	std::string ret = pBuf;

	delete[]pwBuf;
	delete[]pBuf;

	return ret;
}

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
	Renderer* render = Renderer::GetInstance();
	ImGui::Begin("ImGui Window");


	Point point;
	point.x = 500.0;
	point.y = 100.0;

	ImU32 a = GetColor(0xFF8800FF);
	ImGui::Text("Text Test");
	ImGui::End();

	//argb
	render->BeginScene();
	render->DrawOutlinedText(font_, XorStr("DWM 测试 Test"), ImVec2(500, 100), 18.0, 0xE96A16FF, true);
	render->DrawText(font_, XorStr("DWM 测试 Test"), ImVec2(500, 200), 18.0, 0xE96A16FF, true);
	render->DrawHealth(ImVec2(100, 100), ImVec2(100, 200), 80);
	render->DrawCircleFilled(ImVec2(200, 200), 50, 0xE96A16FF);
	render->RenderText(font_,string_to_utf8("DWM 测试 Test"), ImVec2(500, 300),18.0, 0xFF00FFFF,true);
	render->EndScene();

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
bool IDraw::SharedDraw()
{
	Renderer* render = Renderer::GetInstance();
	
	for (int i = 0; i < 1000; i++)
	{
		if (shared->shared_mem_->circle_list[i].radius != 0)
		{
			if(shared->shared_mem_->circle_list[i].filled)
				render->DrawCircleFilled(shared->shared_mem_->circle_list[i].point, shared->shared_mem_->circle_list[i].radius, shared->shared_mem_->circle_list[i].rgb);
			else
				render->DrawCircle(shared->shared_mem_->circle_list[i].point, shared->shared_mem_->circle_list[i].radius, shared->shared_mem_->circle_list[i].rgb, shared->shared_mem_->circle_list[i].thickness);
		}
	}
	for (int i = 0; i < 1000; i++)
	{
		if (shared->shared_mem_->line_list[i].size != 0)
		{
			render->DrawLine(shared->shared_mem_->line_list[i].start, shared->shared_mem_->line_list[i].end, shared->shared_mem_->line_list[i].rgb, shared->shared_mem_->line_list[i].size);
		}
	}
	for (int i = 0; i < 1000; i++)
	{
		if (shared->shared_mem_->rect_list[i].rect.w!=0 && shared->shared_mem_->rect_list[i].rect.x != 0)
		{
			//TODO:绘制矩形的坐标转换
		}
	}
	for (int i = 0; i < 1000; i++)
	{
		if (shared->shared_mem_->text_list[i].size != 0 )
		{
			if (shared->shared_mem_->text_list[i].filled)
				render->RenderText(font_, shared->shared_mem_->text_list[i].text, shared->shared_mem_->text_list[i].point, shared->shared_mem_->text_list[i].size, shared->shared_mem_->text_list[i].rgb,true);
			else
				render->RenderText(font_, shared->shared_mem_->text_list[i].text, shared->shared_mem_->text_list[i].point, shared->shared_mem_->text_list[i].size, shared->shared_mem_->text_list[i].rgb, true);
		}
	}

	return true;
}


bool IDraw::DarwText(std::string text, Point point, ULONG rgb, int size, bool filled)
{
	ImVec2 vec = { point.x,point.y };

	if (filled)
	{
		//返回宽高
		//ImVec2 text_size =  ImGui::GetFont()->CalcTextSizeA(size, 0, 0, text.c_str());
		ImGuiIO& io = ImGui::GetIO();
		
	/*	ImGui::GetForegroundDrawList()->AddText(font_, size, ImVec2(point.x-1, point.y), 0xFF000000, text.c_str());
		ImGui::GetForegroundDrawList()->AddText(font_, size, ImVec2(point.x+1, point.y), 0xFF000000, text.c_str());
		ImGui::GetForegroundDrawList()->AddText(font_, size, ImVec2(point.x, point.y-1), 0xFF000000, text.c_str());
		ImGui::GetForegroundDrawList()->AddText(font_, size, ImVec2(point.x, point.y+1), 0xFF000000, text.c_str());*/


		ImVec2 textSize = font_->CalcTextSizeA(size, FLT_MAX, 0.0f, text.c_str());

		ImGui::GetForegroundDrawList()->AddText(font_, size, { (point.x - textSize.x / 2.0f) + 1.0f, (point.y + textSize.y )*0 + 1.0f }, 0xFF000000, text.c_str());
		ImGui::GetForegroundDrawList()->AddText(font_, size, { (point.x - textSize.x / 2.0f) - 1.0f, (point.y + textSize.y )*0 - 1.0f }, 0xFF000000, text.c_str());
		ImGui::GetForegroundDrawList()->AddText(font_, size, { (point.x - textSize.x / 2.0f) + 1.0f, (point.y + textSize.y )*0 - 1.0f }, 0xFF000000, text.c_str());
		ImGui::GetForegroundDrawList()->AddText(font_, size, { (point.x - textSize.x / 2.0f) - 1.0f, (point.y + textSize.y ) *0+ 1.0f }, 0xFF000000, text.c_str());
	}
	
	ImGui::GetForegroundDrawList()->AddText(font_, size, vec, rgb, text.c_str());
	ImGui::GetOverlayDrawList()->AddText(font_, size, vec, rgb, text.c_str());
	
		return true;
}
