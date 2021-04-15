#include "IDraw.h"

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
		is_init_ = true;
	}
	return true;
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
	ImGui::Text("Text Test");
	ImGui::End();

	ImGui::Render();

	pD3DXDeviceCtx->OMSetRenderTargets(1, &mainRenderTargetView, NULL);
	ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
	return true;
}
