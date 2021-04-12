#include "pch.h"
#include "SymbolFile.h"
#include <dia2.h>
#include "Callback.h"
#include <shlwapi.h>
#pragma comment(lib,"shlwapi.lib")
#pragma comment(lib,"Ole32.lib")
#pragma comment(lib,"OleAut32.lib")

#define ___DEBUG 0
#if ___DEBUG
#define wprintf
#endif // ___DEBUG


SymbolFile::SymbolFile()
{

}

SymbolFile::SymbolFile(std::wstring file_path)
{
	this->file_path_ = file_path;
	this->g_szFilename = this->file_path_.c_str();
}



bool SymbolFile::FindSymbolByName(std::wstring name)
{
	this->find_name_ = name;
	this->offset_ = 0;

	if (!LoadDataFromPdb(g_szFilename, &g_pDiaDataSource, &g_pDiaSession, &pSymbol)) 
	{
		return false;
	}


	if (!DumpAllPublics(this->pSymbol))
	{
		return false;
	}
	Cleanup();
	return true;
}

bool SymbolFile::FindNameCallBack(IDiaSymbol* pSymbol)
{
	DWORD dwSymTag;
	DWORD dwRVA;
	DWORD dwSeg;
	DWORD dwOff;
	BSTR bstrName;

	if (pSymbol->get_symTag(&dwSymTag) != S_OK) {
		return false;
	}

	if (pSymbol->get_relativeVirtualAddress(&dwRVA) != S_OK) {
		dwRVA = 0xFFFFFFFF;
	}

	pSymbol->get_addressSection(&dwSeg);
	pSymbol->get_addressOffset(&dwOff);

	wprintf(L"%s: [%08X][%04X:%08X] ", rgTags[dwSymTag], dwRVA, dwSeg, dwOff);

	if (dwSymTag == SymTagThunk) {
		if (pSymbol->get_name(&bstrName) == S_OK) {
			wprintf(L"%s\n", bstrName);
			if (StrStrIW(bstrName, this->find_name_.c_str()) > 0)
			{
				this->offset_ = dwRVA;
				wprintf(L"FindName:%s OffSet:%08X\n", this->find_name_.c_str(), dwRVA);
				SysFreeString(bstrName);
				return true;
			}


			SysFreeString(bstrName);
		}

		else {
			if (pSymbol->get_targetRelativeVirtualAddress(&dwRVA) != S_OK) {
				dwRVA = 0xFFFFFFFF;
			}

			pSymbol->get_targetSection(&dwSeg);
			pSymbol->get_targetOffset(&dwOff);

			wprintf(L"target -> [%08X][%04X:%08X]\n", dwRVA, dwSeg, dwOff);
		}
	}

	else {
		// must be a function or a data symbol

		BSTR bstrUndname;

		if (pSymbol->get_name(&bstrName) == S_OK) {
			if (pSymbol->get_undecoratedName(&bstrUndname) == S_OK) {
				wprintf(L"%s(%s)\n", bstrName, bstrUndname);

				if (StrStrIW(bstrUndname, this->find_name_.c_str()) > 0)
				{
					this->offset_ = dwRVA;
					wprintf(L"FindName:%s OffSet:%08X\n", bstrUndname, dwRVA);
					SysFreeString(bstrUndname);
					return true;
				}
				SysFreeString(bstrUndname);
			}

			else {
				wprintf(L"%s\n", bstrName);
			}
			if (StrStrIW(bstrName, this->find_name_.c_str()) > 0)
			{
				this->offset_ = dwRVA;
				wprintf(L"FindName:%s OffSet:%08X\n", this->find_name_.c_str(), dwRVA);
				SysFreeString(bstrName);
				return true;
			}
			SysFreeString(bstrName);
		}
	}

	return false;
}

SymbolFile::~SymbolFile()
{
	Cleanup();
}

////////////////////////////////////////////////////////////
// Create an IDiaData source and open a PDB file
//
bool SymbolFile::LoadDataFromPdb(
	const wchar_t* szFilename,
	IDiaDataSource** ppSource,
	IDiaSession** ppSession,
	IDiaSymbol** ppGlobal)
{
	wchar_t wszExt[MAX_PATH];
	const wchar_t* wszSearchPath = L"C:\\symbols"; // Alternate path to search for debug data
	DWORD dwMachType = 0;

	HRESULT hr = CoInitialize(NULL);

	// Obtain access to the provider

	hr = CoCreateInstance(__uuidof(DiaSource),
		NULL,
		CLSCTX_INPROC_SERVER,
		__uuidof(IDiaDataSource),
		(void**)ppSource);

	if (FAILED(hr)) {
		wprintf(L"CoCreateInstance failed - HRESULT = %08X\n", hr);

		return false;
	}

	_wsplitpath_s(szFilename, NULL, 0, NULL, 0, NULL, 0, wszExt, MAX_PATH);

	if (!_wcsicmp(wszExt, L".pdb")) {
		// Open and prepare a program database (.pdb) file as a debug data source

		hr = (*ppSource)->loadDataFromPdb(szFilename);

		if (FAILED(hr)) {
			wprintf(L"loadDataFromPdb failed - HRESULT = %08X\n", hr);

			return false;
		}
	}

	else {
		CCallback callback; // Receives callbacks from the DIA symbol locating procedure,
							// thus enabling a user interface to report on the progress of
							// the location attempt. The client application may optionally
							// provide a reference to its own implementation of this
							// virtual base class to the IDiaDataSource::loadDataForExe method.
		callback.AddRef();

		// Open and prepare the debug data associated with the executable

		hr = (*ppSource)->loadDataForExe(szFilename, wszSearchPath, &callback);

		if (FAILED(hr)) {
			wprintf(L"loadDataForExe failed - HRESULT = %08X\n", hr);

			return false;
		}
	}

	// Open a session for querying symbols

	hr = (*ppSource)->openSession(ppSession);

	if (FAILED(hr)) {
		wprintf(L"openSession failed - HRESULT = %08X\n", hr);

		return false;
	}

	// Retrieve a reference to the global scope

	hr = (*ppSession)->get_globalScope(ppGlobal);

	if (hr != S_OK) {
		wprintf(L"get_globalScope failed\n");

		return false;
	}

	// Set Machine type for getting correct register names

	if ((*ppGlobal)->get_machineType(&dwMachType) == S_OK) {
		switch (dwMachType) {
		case IMAGE_FILE_MACHINE_I386: g_dwMachineType = CV_CFL_80386; break;
		case IMAGE_FILE_MACHINE_IA64: g_dwMachineType = CV_CFL_IA64; break;
		case IMAGE_FILE_MACHINE_AMD64: g_dwMachineType = CV_CFL_AMD64; break;
		}
	}

	return true;
}
////////////////////////////////////////////////////////////
// Dump all the public symbols - SymTagPublicSymbol
//
bool SymbolFile::DumpAllPublics(IDiaSymbol* pGlobal)
{
	wprintf(L"\n\n*** PUBLICS\n\n");

	// Retrieve all the public symbols

	IDiaEnumSymbols* pEnumSymbols;

	if (FAILED(pGlobal->findChildren(SymTagPublicSymbol, NULL, nsNone, &pEnumSymbols))) {
		return false;
	}

	IDiaSymbol* pSymbol;
	ULONG celt = 0;

	while (SUCCEEDED(pEnumSymbols->Next(1, &pSymbol, &celt)) && (celt == 1)) {
		/*PrintPublicSymbol(pSymbol);*/
		if (FindNameCallBack(pSymbol) == true)
		{
			pSymbol->Release();
			pEnumSymbols->Release();
			return true;
		}
		pSymbol->Release();
	}

	pEnumSymbols->Release();

	putwchar(L'\n');

	return true;
}


////////////////////////////////////////////////////////////
// Print a public symbol info: name, VA, RVA, SEG:OFF
//
void SymbolFile::PrintPublicSymbol(IDiaSymbol* pSymbol)
{
	DWORD dwSymTag;
	DWORD dwRVA;
	DWORD dwSeg;
	DWORD dwOff;
	BSTR bstrName;

	if (pSymbol->get_symTag(&dwSymTag) != S_OK) {
		return;
	}

	if (pSymbol->get_relativeVirtualAddress(&dwRVA) != S_OK) {
		dwRVA = 0xFFFFFFFF;
	}

	pSymbol->get_addressSection(&dwSeg);
	pSymbol->get_addressOffset(&dwOff);

	wprintf(L"%s: [%08X][%04X:%08X] ", rgTags[dwSymTag], dwRVA, dwSeg, dwOff);

	if (dwSymTag == SymTagThunk) {
		if (pSymbol->get_name(&bstrName) == S_OK) {
			wprintf(L"%s\n", bstrName);
			if (StrCmpW(bstrName, L"") == 0)
			{

				return;
			}
			
		
			SysFreeString(bstrName);
		}

		else {
			if (pSymbol->get_targetRelativeVirtualAddress(&dwRVA) != S_OK) {
				dwRVA = 0xFFFFFFFF;
			}

			pSymbol->get_targetSection(&dwSeg);
			pSymbol->get_targetOffset(&dwOff);
			
			wprintf(L"target -> [%08X][%04X:%08X]\n", dwRVA, dwSeg, dwOff);
		}
	}

	else {
		// must be a function or a data symbol

		BSTR bstrUndname;

		if (pSymbol->get_name(&bstrName) == S_OK) {
			if (pSymbol->get_undecoratedName(&bstrUndname) == S_OK) {
				wprintf(L"%s(%s)\n", bstrName, bstrUndname);

				SysFreeString(bstrUndname);
			}

			else {
				wprintf(L"%s\n", bstrName);
			}

			SysFreeString(bstrName);
		}
	}
}
////////////////////////////////////////////////////////////
// Release DIA objects and CoUninitialize
//
void  SymbolFile::Cleanup()
{
	if (pSymbol) {
		pSymbol->Release();
		pSymbol = NULL;
	}

	if (g_pDiaSession) {
		g_pDiaSession->Release();
		g_pDiaSession = NULL;
	}

	CoUninitialize();
}