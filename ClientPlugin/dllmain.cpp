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


extern "C" __declspec(dllexport) BOOL WINAPI DrawText(char* text,ImVec2& point,float size,int col,bool filled)
{
    if (shared.shared_mem_ == NULL)
    {
        return FALSE;
    }

    if (shared.shared_mem_->text_num >=1000)
    {
        return FALSE;
    }
    shared.shared_mem_->text_list[shared.shared_mem_->text_num].filled = filled;
    shared.shared_mem_->text_list[shared.shared_mem_->text_num].point = point;
    shared.shared_mem_->text_list[shared.shared_mem_->text_num].rgb = col;
    shared.shared_mem_->text_list[shared.shared_mem_->text_num].size = size;
    strcpy(shared.shared_mem_->text_list[shared.shared_mem_->text_num].text, text);
    shared.shared_mem_->text_num++;
    return TRUE;
}

extern "C" __declspec(dllexport) BOOL WINAPI DrawRircle(ImVec2 & point, float radius, int col,float thickness, bool filled)
{
    if (shared.shared_mem_ == NULL)
    {
        return FALSE;
    }

    if (shared.shared_mem_->circle_num >= 1000)
    {
        return FALSE;
    }
    shared.shared_mem_->circle_list[shared.shared_mem_->circle_num].filled = filled;
    shared.shared_mem_->circle_list[shared.shared_mem_->circle_num].point = point;
    shared.shared_mem_->circle_list[shared.shared_mem_->circle_num].rgb = col;
    shared.shared_mem_->circle_list[shared.shared_mem_->circle_num].radius = radius;
    shared.shared_mem_->circle_list[shared.shared_mem_->circle_num].thickness = thickness;
 
    shared.shared_mem_->circle_num++;
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

