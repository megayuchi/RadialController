#pragma once


enum CPU_ARCH
{
	CPU_ARCH_X86,
	CPU_ARCH_X64,
	CPU_ARCH_ARM
};

enum BUILD_CONFIG
{
	BUILD_CONFIG_DEBUG,
	BUILD_CONFIG_RELEASE
};

DWORD CreateEngineFileName(WCHAR* wchOutFileName, DWORD dwMaxBufferCount, WCHAR* wchName, CPU_ARCH arch, BUILD_CONFIG config);

HRESULT typedef (__stdcall *CREATE_INSTANCE_FUNC)(void* ppv);