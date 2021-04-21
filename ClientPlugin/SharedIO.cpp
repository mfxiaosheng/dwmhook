#include "pch.h"
#include "SharedIO.h"

#include <sddl.h>
SharedIO::SharedIO()
{


	//初始化共享内存 映射到内存当中来
	hmap_ = OpenFileMappingA(FILE_MAP_READ | FILE_MAP_WRITE, NULL, "eli");


	if (hmap_ == NULL)
	{

		//AddToLog("OpenFileMappingA error %d", GetLastError());
		SECURITY_ATTRIBUTES  sa;
		sa.nLength = sizeof(SECURITY_ATTRIBUTES);
		sa.bInheritHandle = FALSE;
		/* give access to members of administrators group */
		BOOL success = ConvertStringSecurityDescriptorToSecurityDescriptorA(
			"D:(A;OICI;GA;;;BA)",
			SDDL_REVISION_1,
			&(sa.lpSecurityDescriptor),
			NULL);
		hmap_ = CreateFileMappingA(INVALID_HANDLE_VALUE, &sa,
			PAGE_READWRITE | SEC_COMMIT, 0, SHARED_SIZE, "eli");
		if (!hmap_)
		{
			//AddToLog("CreateFileMappingA error %d", GetLastError());
			return;
		}


	}

	lpdata_ = MapViewOfFile(hmap_, FILE_MAP_READ | FILE_MAP_WRITE, 0, 0, 0);
	if (lpdata_ == NULL)
	{
		//AddToLog("MapViewOfFile error %d", GetLastError());
		return;
	}
	InitSharedMem();
}

SharedIO::~SharedIO()
{

}

bool SharedIO::InitSharedMem()
{
	if (lpdata_ == NULL)
	{
		return false;
	}

	shared_mem_ = (SharedMem*)lpdata_;
	return true;
}
