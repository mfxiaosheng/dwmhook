// dllmain.cpp : 定义 DLL 应用程序的入口点。
//#include <SharedIO.h>
#include "pch.h"
#include <windows.h>
#include "SharedIO.h"



extern "C" __declspec(dllexport)  char*  WINAPI GetSymbolSig();
extern "C" __declspec(dllexport) BOOL  WINAPI SetSymbolOffset(int offset);
SharedIO shared;

extern "C" __declspec(dllexport) char*  WINAPI  GetSymbolSig()
{
    if (shared.shared_mem_ == NULL)
    {
        return NULL;
    }

    return shared.shared_mem_->symbol_sig;
}

extern "C" __declspec(dllexport) BOOL WINAPI SetSymbolOffset(int offset)
{
    if (shared.shared_mem_ == NULL)
    {
        return FALSE;
    }
    shared.shared_mem_->symbol_offset = offset;
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

