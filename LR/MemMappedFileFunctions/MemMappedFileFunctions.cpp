// MemMappedFileFunctions.cpp : Defines the initialization routines for the DLL.
//

#include "pch.h"
#include "framework.h"
#include "MemMappedFileFunctions.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

//
//TODO: If this DLL is dynamically linked against the MFC DLLs,
//		any functions exported from this DLL which call into
//		MFC must have the AFX_MANAGE_STATE macro added at the
//		very beginning of the function.
//
//		For example:
//
//		extern "C" BOOL PASCAL EXPORT ExportedFunction()
//		{
//			AFX_MANAGE_STATE(AfxGetStaticModuleState());
//			// normal function body here
//		}
//
//		It is very important that this macro appear in each
//		function, prior to any calls into MFC.  This means that
//		it must appear as the first statement within the
//		function, even before any object variable declarations
//		as their constructors may generate calls into the MFC
//		DLL.
//
//		Please see MFC Technical Notes 33 and 58 for additional
//		details.
//

// CMemMappedFileFunctionsApp

BEGIN_MESSAGE_MAP(CMemMappedFileFunctionsApp, CWinApp)
END_MESSAGE_MAP()


// CMemMappedFileFunctionsApp construction

CMemMappedFileFunctionsApp::CMemMappedFileFunctionsApp()
{
	// TODO: add construction code here,
	// Place all significant initialization in InitInstance
}


// The one and only CMemMappedFileFunctionsApp object

CMemMappedFileFunctionsApp theApp;


// CMemMappedFileFunctionsApp initialization

BOOL CMemMappedFileFunctionsApp::InitInstance()
{
	CWinApp::InitInstance();

	return TRUE;
}


struct header
{
    int addr;
    int size;
};


extern "C"
{
    __declspec(dllexport) void __stdcall mapsend(int addr, const char* str)
    {
        AFX_MANAGE_STATE(AfxGetStaticModuleState());

        header h = { addr, strlen(str) + 1 };
        HANDLE hFile = CreateFile("file.dat", GENERIC_READ | GENERIC_WRITE, FILE_SHARE_WRITE | FILE_SHARE_READ, NULL, OPEN_ALWAYS, 0, 0);
        HANDLE hFileMap = CreateFileMapping(hFile, NULL, PAGE_READWRITE, 0, h.size + sizeof(header), NULL);
        char* buff = (char*)MapViewOfFile(hFileMap, FILE_MAP_ALL_ACCESS, 0, 0, h.size + sizeof(header));

        memcpy(buff, &h, sizeof(header));
        memcpy(buff + sizeof(header), str, h.size);


        UnmapViewOfFile(buff);
        CloseHandle(hFileMap);
        CloseHandle(hFile);

    }
}

