// test.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
//


#include <iostream>
#include <d3d11.h>
#include <d3d9.h>
#include "SymbolFile.h"
#include "SharedIO.h"
#pragma  comment(lib,"d3d11.lib")
#define LOG_FILE_PATH		"D:\\DWM_GAY.log"
int main()
{
	/*SymbolFile symbol_file(L"C:\\symbols\\dxgi.pdb\\013A571D9173429CA2C47576F771B5411\\dxgi.pdb");

	uint32_t offset = 0;
	symbol_file.FindSymbolByName(L"const ATL::CComContainedObject<class CDXGISwapChainDWMLegacy>::`vftable'", offset);

	wprintf(L"addr:%08X", symbol_file.offset_);*/

	SharedIO shared;
	printf("%d\n", PAGE_READWRITE | SEC_COMMIT);
	if (shared.InitSharedMem() == false)
	{
		printf("shared init error\n");
	}
	else
	{
		printf("shered:%08X\n", shared);
		printf("symbol sig:%s\n", shared.shared_mem_->symbol_sig);
	}
	getchar();
	return 0;
}

