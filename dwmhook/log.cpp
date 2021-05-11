#include "log.h"
#include <stdio.h>
#include "XorStr.h"

void AddToLog(const char* fmt, ...)
{

	va_list va;
	va_start(va, fmt);

	char buff[1024]{ };
	char* log_path =  xorstr_("D:\\code\\dwmhook-master\\x64\\Release\\DWM_GAY.log");
	vsnprintf_s(buff, sizeof(buff), fmt, va);

	va_end(va);

	FILE* f = nullptr;
	fopen_s(&f, log_path, "a");

	if (!f)
	{
		char szDst[256];
		sprintf_s(szDst, "Failed to create file %d", GetLastError());
		OutputDebugStringA(buff);
		//MessageBoxA(0, buff, 0, 0);
		return;
	}

	OutputDebugStringA(buff);
	fprintf_s(f, buff);
	fclose(f);

}