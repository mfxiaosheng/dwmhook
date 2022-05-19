// dllmain.cpp : 定义 DLL 应用程序的入口点。
//#include <SharedIO.h>
#include "pch.h"
#include <windows.h>
#include "SharedIO.h"
#include "inject.h"
#include "log.h"


extern "C" __declspec(dllexport)  char*  WINAPI GetSymbolSig();
extern "C" __declspec(dllexport) BOOL  WINAPI SetSymbolOffset(int offset);
extern "C" __declspec(dllexport) BOOL WINAPI DrawString(char* text, int x, int y, float size, int col, bool filled);
extern "C" __declspec(dllexport) BOOL WINAPI DrawRircle(int x,int y, float radius, int col, float thickness, bool filled);
extern "C" __declspec(dllexport) BOOL WINAPI DrawLine(int x,int y,int x1,int y1, int col, float thickness);
extern "C" __declspec(dllexport) BOOL WINAPI DrawRect(int x, int y, int w, int h, int col, float thickness);
extern "C" __declspec(dllexport) void DrawBegin();
SharedIO* shared =  NULL;

extern "C" __declspec(dllexport) char*  WINAPI  GetSymbolSig()
{
    if (shared == NULL)
        shared = new SharedIO;
    if (shared->shared_mem_ == NULL)
    {
        AddToLog("GetSymbolSig error shared_mem_ == NULL");
        return NULL;
    }
    
    return shared->shared_mem_->symbol_sig;
}

extern "C" __declspec(dllexport) BOOL WINAPI SetSymbolOffset(int offset)
{
    if (shared->shared_mem_ == NULL)
    {
        return FALSE;
    }
    shared->shared_mem_->symbol_offset = offset;
    return TRUE;
}


extern "C" __declspec(dllexport) BOOL WINAPI DrawString(char* text,int x,int y,float size,int col,bool filled)
{
    if (shared->shared_mem_ == NULL)
    {
        return FALSE;
    }

    if (shared->shared_mem_->text_num >=1000)
    {
        return FALSE;
    }
    shared->shared_mem_->text_list[shared->shared_mem_->text_num].filled = filled;
    shared->shared_mem_->text_list[shared->shared_mem_->text_num].point = ImVec2(x,y);
    shared->shared_mem_->text_list[shared->shared_mem_->text_num].rgb = col;
    shared->shared_mem_->text_list[shared->shared_mem_->text_num].size = size;
    strcpy(shared->shared_mem_->text_list[shared->shared_mem_->text_num].text, text);
    shared->shared_mem_->text_num++;
    return TRUE;
}

extern "C" __declspec(dllexport) BOOL WINAPI DrawRircle(int x, int y, float radius, int col,float thickness, bool filled)
{
    if (shared->shared_mem_ == NULL)
    {
        return FALSE;
    }

    if (shared->shared_mem_->circle_num >= 1000)
    {
        return FALSE;
    }
    shared->shared_mem_->circle_list[shared->shared_mem_->circle_num].filled = filled;
    shared->shared_mem_->circle_list[shared->shared_mem_->circle_num].point = ImVec2(x,y);
    shared->shared_mem_->circle_list[shared->shared_mem_->circle_num].rgb = col;
    shared->shared_mem_->circle_list[shared->shared_mem_->circle_num].radius = radius;
    shared->shared_mem_->circle_list[shared->shared_mem_->circle_num].thickness = thickness;
 
    shared->shared_mem_->circle_num++;
    return TRUE;
}

extern "C" __declspec(dllexport) BOOL WINAPI DrawLine(int x, int y, int x1, int y1, int col, float thickness)
{
    if (shared->shared_mem_ == NULL)
    {
        return FALSE;
    }

    if (shared->shared_mem_->line_num >= 1000)
    {
        return FALSE;
    }
    shared->shared_mem_->line_list[shared->shared_mem_->line_num].start = ImVec2(x,y);
    shared->shared_mem_->line_list[shared->shared_mem_->line_num].end = ImVec2(x1, y1);
    shared->shared_mem_->line_list[shared->shared_mem_->line_num].rgb = col;
    shared->shared_mem_->line_list[shared->shared_mem_->line_num].thickness = thickness;
  

    shared->shared_mem_->line_num++;
    return TRUE;
}
extern "C" __declspec(dllexport) void DrawBegin()
{
    if (shared->shared_mem_)
    {
       // ZeroMemory(shared->shared_mem_, sizeof(SharedMem));
        shared->shared_mem_->line_num = 0;
        shared->shared_mem_->circle_num = 0;
        shared->shared_mem_->rect_num = 0;
        shared->shared_mem_->text_num = 0;
    }
}
extern "C" __declspec(dllexport) BOOL WINAPI DrawFilledRect(int x, int y, int w, int h, int col, float thickness)
{
    if (shared->shared_mem_ == NULL)
    {
        return FALSE;
    }

    if (shared->shared_mem_->rect_num >= 1000)
    {
        return FALSE;
    }
    ImVec4 vec4(x, y, x + w, y + h);

    shared->shared_mem_->rect_list[shared->shared_mem_->rect_num].rect = vec4;
    shared->shared_mem_->rect_list[shared->shared_mem_->rect_num].rgb = col;
    shared->shared_mem_->rect_list[shared->shared_mem_->rect_num].thickness = thickness;
    shared->shared_mem_->rect_list[shared->shared_mem_->rect_num].filled = true;


    shared->shared_mem_->rect_num++;
}
extern "C" __declspec(dllexport) BOOL WINAPI DrawRect(int x,int y,int w,int h, int col, float thickness)
{
    if (shared->shared_mem_ == NULL)
    {
        return FALSE;
    }

    if (shared->shared_mem_->rect_num >= 1000)
    {
        return FALSE;
    }
    ImVec4 vec4(x,y,x+w,y+h);
    
    shared->shared_mem_->rect_list[shared->shared_mem_->rect_num].rect = vec4;
    shared->shared_mem_->rect_list[shared->shared_mem_->rect_num].rgb = col;
    shared->shared_mem_->rect_list[shared->shared_mem_->rect_num].thickness = thickness;
    shared->shared_mem_->rect_list[shared->shared_mem_->rect_num].filled = false;


    shared->shared_mem_->rect_num++;
}

extern "C" __declspec(dllexport) bool Inject(char* dll_path)
{
    if (!dll_path)
        return false;
   return inject_to_dwm(dll_path);
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

