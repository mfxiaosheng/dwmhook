// test.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
//


#include <iostream>
#include <d3d11.h>
#include <d3d9.h>
#include "SymbolFile.h"
#pragma  comment(lib,"d3d11.lib")
#define LOG_FILE_PATH		"D:\\DWM_GAY.log"
int main()
{
	SymbolFile symbol_file(L"C:\\symbols\\dxgi.pdb\\013A571D9173429CA2C47576F771B5411\\dxgi.pdb");

	uint32_t offset = 0;
	symbol_file.FindSymbolByName(L"const ATL::CComContainedObject<class CDXGISwapChainDWMLegacy>::`vftable'", offset);
	
	wprintf(L"addr:%08X", symbol_file.offset_);
	return 0;
}

