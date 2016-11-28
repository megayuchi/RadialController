// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently, but
// are changed infrequently
//

#pragma once

#include "targetver.h"



#include <stdlib.h>
/*
#ifdef _DEBUG
	#define _CRTDBG_MAP_ALLOC
	#include <crtdbg.h>
	#define new new(_NORMAL_BLOCK, __FILE__, __LINE__)
#endif
*/

#include <intrin.h>
#include <Ole2.h>
#include <io.h>
#include <initguid.h>

#include "../Megayuchi_Common/IRadialDevice.h"
#include "../Megayuchi_UTIL/WriteDebugString.h"
#include "../Megayuchi_UTIL/RefCheck.h"


#define WIN32_LEAN_AND_MEAN             // Exclude rarely-used stuff from Windows headers
// Windows Header Files:
#include <windows.h>



// TODO: reference additional headers your program requires here
#include <wrl\implements.h>
#include <wrl\module.h>
#include <wrl\event.h>
#include <roapi.h>
#include <wrl.h>
#include <wrl\client.h>
#include <windows.foundation.h>
#include <windows.foundation.numerics.h>
#include <windows.foundation.collections.h>
#include <windows.ui.input.h>
#include <Windows.ApplicationModel.h>
#include <Windows.ApplicationModel.datatransfer.h>
#include <RadialControllerInterop.h>

