#pragma once

#include "../Megayuchi_Common/IRadialDevice.h"


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

using namespace ABI::Windows::UI::Input;
using namespace ABI::Windows::Foundation;
using namespace Microsoft::WRL;
using namespace Microsoft::WRL::Wrappers;


struct MENU_CALLBACK_ITEM
{
	ComPtr<IRadialControllerMenuItem> menuItem;
	int		ID = -1;
	EventRegistrationToken					token;

};

const DWORD MAX_MENU_COUNT = 16;

class DeviceListener;
class CRadialDevice : public IRadialDevice
{
	ULONG				m_ulRefCount = 1;
	DeviceListener*		m_pDeviceListener = nullptr;
	MENU_CALLBACK_ITEM*	m_pMenuCallbackTable = nullptr;
	DWORD				m_dwCurMenuCount = 0;
	MENU_CALLBACK_ITEM* m_pCurSelectedItem = nullptr;

	RADIAL_DEVICE_CALLBACK_FOCUS			m_pCallbackFoucus = nullptr;
	RADIAL_DEVICE_CALLBACK_LOST_FOCUS		m_pCallbackLostFocus = nullptr;
	RADIAL_DEVICE_CALLBACK_ROT				m_pCallbackRot = nullptr;
	RADIAL_DEVICE_CALLBACK_BUTTON_CLICKED	m_pCallbackButtonClicked = nullptr;
	
	MENU_CALLBACK_ITEM* FindCallbackItem(ComPtr<IRadialControllerMenuItem> menuItem) const;
public:
	void	OnItemInvoked(ComPtr<IRadialControllerMenuItem> menuItem);
	void	OnRotationChanged(ComPtr<IRadialControllerMenuItem> menuItem, double delta);
	void	OnButtonClicked(ComPtr<IRadialControllerMenuItem> menuItem);

	// external interface
    STDMETHODIMP			QueryInterface(REFIID,void** ppv);
	STDMETHODIMP_(ULONG)	AddRef(void);
    STDMETHODIMP_(ULONG)	Release(void);

	BOOL	__stdcall Initialize(HWND hWnd,
		RADIAL_DEVICE_CALLBACK_FOCUS pCallbackFocus, 
		RADIAL_DEVICE_CALLBACK_LOST_FOCUS pCallbackLostFocus, 
		RADIAL_DEVICE_CALLBACK_ROT pCallbackRot, 
		RADIAL_DEVICE_CALLBACK_BUTTON_CLICKED pCallbackButtonClicked);
	BOOL	__stdcall AddMenuItem(const WCHAR* wchIconFileName, const WCHAR* wchMenuName,int MenuItemID);
	
	CRadialDevice();
	~CRadialDevice();
};

