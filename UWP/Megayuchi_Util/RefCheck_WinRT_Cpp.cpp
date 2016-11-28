#include "pch.h"
#include <initguid.h>
#include <inspectable.h>
#include "RefCheck_WinRT_Cpp.h"
#include "RefCheck.h"
#include "debug_new.h"


ULONG GetRefCount_WinRT_CPP(winrt::Windows::IInspectable t)
{
	// t <- ++ on copied
	IInspectable* pObj = *(IInspectable**)&t;
	t = nullptr;

	ULONG ref_count = GetRefCount(pObj);

	return ref_count;
}