#include "pch.h"
#include "Util.h"

DWORD CreateEngineFileName(WCHAR* wchOutFileName,DWORD dwMaxBufferCount,WCHAR* wchName,CPU_ARCH arch,BUILD_CONFIG config)
{
	
	
	WCHAR*	wchOS = NULL;
	WCHAR*	wchArch = NULL;
	WCHAR*	wchConfig = NULL;

	switch(arch)
	{
	case CPU_ARCH_X86:
		wchArch = L"_x86";
		break;
	case CPU_ARCH_X64:
		wchArch = L"_x64";
		break;
	case CPU_ARCH_ARM:
		wchArch = L"_arm";
		break;
	}

	switch(config)
	{
	case BUILD_CONFIG_DEBUG:
		wchConfig = L"_debug";
		break;
	case BUILD_CONFIG_RELEASE:
		wchConfig = L"_release";
		break;

	}
	if (wchName)
	{
		wcscpy_s(wchOutFileName,dwMaxBufferCount,wchName);
	}
	if (wchOS)
	{
		wcscat_s(wchOutFileName,dwMaxBufferCount,wchOS);
	}
	if (wchArch)
	{
		wcscat_s(wchOutFileName,dwMaxBufferCount,wchArch);
	}
	if (wchConfig)
	{
		wcscat_s(wchOutFileName,dwMaxBufferCount,wchConfig);
	}
	wcscat_s(wchOutFileName,dwMaxBufferCount,L".dll");
	DWORD	dwLen = (DWORD)wcslen(wchOutFileName);

	return dwLen;
}