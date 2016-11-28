#pragma once

enum DEBUG_OUTPUT_TYPE
{
	DEBUG_OUTPUT_TYPE_NULL,
	DEBUG_OUTPUT_TYPE_CONSOLE,
	DEBUG_OUTPUT_TYPE_DEBUG_CONSOLE
};

void WriteDebugStringW(DEBUG_OUTPUT_TYPE type,WCHAR* wchFormat,...);
void WriteDebugStringA(DEBUG_OUTPUT_TYPE type,char* szFormat,...);
