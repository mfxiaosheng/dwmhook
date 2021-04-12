#pragma once
#include <string.h>
#include <string>
#include <dia2.h>


// Tags returned by Dia
const wchar_t* const rgTags[] =
{
  L"(SymTagNull)",                     // SymTagNull
  L"Executable (Global)",              // SymTagExe
  L"Compiland",                        // SymTagCompiland
  L"CompilandDetails",                 // SymTagCompilandDetails
  L"CompilandEnv",                     // SymTagCompilandEnv
  L"Function",                         // SymTagFunction
  L"Block",                            // SymTagBlock
  L"Data",                             // SymTagData
  L"Annotation",                       // SymTagAnnotation
  L"Label",                            // SymTagLabel
  L"PublicSymbol",                     // SymTagPublicSymbol
  L"UserDefinedType",                  // SymTagUDT
  L"Enum",                             // SymTagEnum
  L"FunctionType",                     // SymTagFunctionType
  L"PointerType",                      // SymTagPointerType
  L"ArrayType",                        // SymTagArrayType
  L"BaseType",                         // SymTagBaseType
  L"Typedef",                          // SymTagTypedef
  L"BaseClass",                        // SymTagBaseClass
  L"Friend",                           // SymTagFriend
  L"FunctionArgType",                  // SymTagFunctionArgType
  L"FuncDebugStart",                   // SymTagFuncDebugStart
  L"FuncDebugEnd",                     // SymTagFuncDebugEnd
  L"UsingNamespace",                   // SymTagUsingNamespace
  L"VTableShape",                      // SymTagVTableShape
  L"VTable",                           // SymTagVTable
  L"Custom",                           // SymTagCustom
  L"Thunk",                            // SymTagThunk
  L"CustomType",                       // SymTagCustomType
  L"ManagedType",                      // SymTagManagedType
  L"Dimension",                        // SymTagDimension
  L"CallSite",                         // SymTagCallSite
  L"InlineSite",                       // SymTagInlineSite
  L"BaseInterface",                    // SymTagBaseInterface
  L"VectorType",                       // SymTagVectorType
  L"MatrixType",                       // SymTagMatrixType
  L"HLSLType",                         // SymTagHLSLType
  L"Caller",                           // SymTagCaller,
  L"Callee",                           // SymTagCallee,
  L"Export",                           // SymTagExport,
  L"HeapAllocationSite",               // SymTagHeapAllocationSite
  L"CoffGroup",                        // SymTagCoffGroup
  L"Inlinee",                          // SymTagInlinee
};
class SymbolFile
{
public:
	SymbolFile();
	~SymbolFile();


	bool LoadDataFromPdb(const wchar_t* szFilename, IDiaDataSource** ppSource, IDiaSession** ppSession, IDiaSymbol** ppGlobal);
	bool DumpAllPublics(IDiaSymbol* pGlobal);
	void PrintPublicSymbol(IDiaSymbol* pSymbol);
	void Cleanup();
	SymbolFile(std::wstring file_path);
	bool FindSymbolByName(std::wstring name, uint32_t& offset);
	bool FindNameCallBack(IDiaSymbol* pSymbol);
public:
	const wchar_t* g_szFilename;
	IDiaDataSource* g_pDiaDataSource;
	IDiaSession* g_pDiaSession;
	IDiaSymbol* pSymbol;
	DWORD g_dwMachineType = CV_CFL_80386;
	std::wstring file_path_;
	uint32_t offset_;
	std::wstring find_name_;
};

