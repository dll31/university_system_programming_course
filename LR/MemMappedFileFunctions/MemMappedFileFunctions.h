// MemMappedFileFunctions.h : main header file for the MemMappedFileFunctions DLL
//

#pragma once

#ifndef __AFXWIN_H__
	#error "include 'pch.h' before including this file for PCH"
#endif

#include "resource.h"		// main symbols
#include <string>

// CMemMappedFileFunctionsApp
// See MemMappedFileFunctions.cpp for the implementation of this class
//

class CMemMappedFileFunctionsApp : public CWinApp
{
public:
	CMemMappedFileFunctionsApp();

// Overrides
public:
	virtual BOOL InitInstance();

	DECLARE_MESSAGE_MAP()
};


struct header
{
	int addr;
	int size;
};


struct receiveHeader
{
	header h;
	std::string str;
};