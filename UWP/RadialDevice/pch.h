#pragma once

#include "targetver.h"
/*
#ifdef _DEBUG
	#define _CRTDBG_MAP_ALLOC
	#include <crtdbg.h>
	//#define new new(_NORMAL_BLOCK, __FILE__, __LINE__) new 가 overload 된 소스코드가 존재하면 컴파일 실패. 
	// malloc을 트랙킹 하기 위해서 _CRTDBG_MAP_ALLOC을 선언해놓는다.
	// new(_NORMAL_BLOCK, __FILE__, __LINE__)는 별도 헤더파일에서 선언하고 소스코드에서 include한다.

#endif
*/
#pragma comment(lib, "windowsapp")


#include "winrt/Windows.ApplicationModel.Activation.h"
#include "winrt/Windows.Foundation.h"
#include "winrt/Windows.UI.Xaml.Controls.h"
#include "winrt/Windows.UI.Xaml.Media.h"
#include "winrt/Windows.Storage.Streams.h"
#include "winrt/Windows.Graphics.Imaging.h"
#include "winrt/Windows.Media.Ocr.h"
#include "winrt/Windows.Storage.Pickers.h"
#include "winrt/Windows.Networking.h"
#include "winrt/Windows.UI.Input.h"

////#using <Windows.winmd>
//#include <winapifamily.h>
//#include <windows.h>
//#include <memory>
//#include <Ole2.h>
//#include <initguid.h>
//#include <wrl.h>
//#include <inspectable.h>
//#include <windows.h>
//
