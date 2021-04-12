// dllmain.cpp : 定义 DLL 应用程序的入口点。
#include "pch.h"
#include "SymbolFile.h"


extern "C" BOOL  __declspec(dllexport) WINAPI  FindSymbolAddr(WCHAR* file_path, WCHAR* symbol_name, uint32_t& out_addr);

extern "C" BOOL  WINAPI  FindSymbolAddr(WCHAR* file_path,WCHAR* symbol_name,uint32_t& out_addr)
{
    SymbolFile symbol_file(file_path);

    if (symbol_file.FindSymbolByName(symbol_name, out_addr) == false)
    {
        return FALSE;
    }
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






