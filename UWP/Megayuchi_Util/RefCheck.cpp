#include "pch.h"
#include <initguid.h>
#include "RefCheck.h"

ULONG GetRefCount(IUnknown* pInterface)
{
	pInterface->AddRef();
	ULONG	ref_count = pInterface->Release();

	return ref_count;
}