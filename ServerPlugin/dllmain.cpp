// dllmain.cpp : 定义 DLL 应用程序的入口点。
#include "pch.h"
#include "SymbolFile.h"


std::wstring ASCIIToUnicode(const char* v_szASCII)
{
	std::wstring wstrUnicode = L"";
	if (NULL == v_szASCII) {
		return wstrUnicode;
	}
	DWORD dwNum = MultiByteToWideChar(CP_ACP, 0, v_szASCII, -1, NULL, 0);
	if (ERROR_NO_UNICODE_TRANSLATION == dwNum) {
		return wstrUnicode;
	}
	if (0 == dwNum) {
		return wstrUnicode;
	}
	WCHAR* wcsUnicode = new WCHAR[dwNum + 1];
	if (wcsUnicode == NULL)
	{
		return wstrUnicode;
	}
	memset(wcsUnicode, 0, (dwNum + 1) * sizeof(WCHAR));

	MultiByteToWideChar(CP_ACP, 0, v_szASCII, -1, wcsUnicode, dwNum);
	wstrUnicode = wcsUnicode;
	if (NULL != wcsUnicode) {
		delete[] wcsUnicode;
	}
	return wstrUnicode;
}

extern "C" BOOL  __declspec(dllexport) WINAPI  FindSymbolAddr(CHAR* file_path, CHAR* symbol_name, uint32_t& out_addr);

extern "C" BOOL  WINAPI  FindSymbolAddr(CHAR* file_path,CHAR* symbol_name,uint32_t& out_addr)
{
    SymbolFile symbol_file(ASCIIToUnicode(file_path));

    if (symbol_file.FindSymbolByName(ASCIIToUnicode(symbol_name)) == false)
    {
        return FALSE;
    }

	out_addr = symbol_file.offset_;
    return TRUE;
}



BOOL APIENTRY DllMain( HMODULE hModule,
                       DWORD  ul_reason_for_call,
                       LPVOID lpReserved
                     )
{
    switch (ul_reason_for_call)
    {
    case DLL_PROCESS_ATTACH:
    case DLL_THREAD_ATTACH:
    case DLL_THREAD_DETACH:
    case DLL_PROCESS_DETACH:
        break;
    }
    return TRUE;
}






