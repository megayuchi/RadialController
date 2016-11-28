#include "pch.h"

#include "RadialDevice.h"

BOOL APIENTRY DllMain(HMODULE /* hModule */, DWORD ul_reason_for_call, LPVOID /* lpReserved */)
{
    switch (ul_reason_for_call)
    {
    case DLL_PROCESS_ATTACH:
    case DLL_THREAD_ATTACH:
    case DLL_THREAD_DETACH:
    case DLL_PROCESS_DETACH:
        break;
    }
    return TRUE;
}

STDAPI DllCreateInstance(void** ppv)
{
	HRESULT hr = S_OK;
	
	CRadialDevice*	pRadialDevice = new CRadialDevice;
	if (!pRadialDevice)
	{
		hr = E_OUTOFMEMORY;
		goto lb_return;
	}
	hr = S_OK;
	*ppv = pRadialDevice;	
lb_return:
	return hr;
}