#pragma once

#include "../Megayuchi_Common/IRadialDevice.h"

struct MENU_CALLBACK_ITEM
{
	winrt::Windows::UI::Input::IRadialControllerMenuItem menuItem;
	int		ID;
};

const DWORD MAX_MENU_COUNT = 16;


class DeviceListener;
class CRadialDevice : public IRadialDevice
{
	ULONG				m_ulRefCount = 1;
	DeviceListener*		m_pDeviceListener = nullptr;
	MENU_CALLBACK_ITEM*	m_pMenuCallbackTable;
	DWORD				m_dwCurMenuCount = 0;
	MENU_CALLBACK_ITEM* m_pCurSelectedItem = nullptr;

	RADIAL_DEVICE_CALLBACK_FOCUS			m_pCallbackFoucus = nullptr;
	RADIAL_DEVICE_CALLBACK_LOST_FOCUS		m_pCallbackLostFocus = nullptr;
	RADIAL_DEVICE_CALLBACK_ROT				m_pCallbackRot = nullptr;
	RADIAL_DEVICE_CALLBACK_BUTTON_CLICKED	m_pCallbackButtonClicked = nullptr;
	RADIAL_DEVICE_CALLBACK_MENU_ITEM_ADDED	m_pCallbackMenuItemAdded = nullptr;

	MENU_CALLBACK_ITEM* FindCallbackItem(winrt::Windows::UI::Input::IRadialControllerMenuItem menuItem) const;

public:
	void	OnItemInvoked(winrt::Windows::UI::Input::IRadialControllerMenuItem menuItem);
	void	OnRotationChanged(winrt::Windows::UI::Input::IRadialControllerMenuItem menuItem, double delta);
	void	OnButtonClicked(winrt::Windows::UI::Input::IRadialControllerMenuItem menuItem);
	void	OnAddMenuItem(winrt::Windows::UI::Input::IRadialControllerMenuItem menuItem, int MenuItemID);
	void	OnCompleteAddMenu(int MenuItemID,BOOL bSuccess);

	STDMETHODIMP			QueryInterface(REFIID, void** ppv);
	STDMETHODIMP_(ULONG)	AddRef(void);
	STDMETHODIMP_(ULONG)	Release(void);

	BOOL	__stdcall Initialize(RADIAL_DEVICE_CALLBACK_FOCUS pCallbackFocus,
		RADIAL_DEVICE_CALLBACK_LOST_FOCUS pCallbackLostFocus,
		RADIAL_DEVICE_CALLBACK_ROT pCallbackRot,
		RADIAL_DEVICE_CALLBACK_BUTTON_CLICKED pCallbackButtonClicked,
		RADIAL_DEVICE_CALLBACK_MENU_ITEM_ADDED pCallbackMenuItemAdded);
	BOOL	__stdcall AddMenuItem(const WCHAR* wchIconFileName, const WCHAR* wchMenuName, int MenuItemID);

	CRadialDevice();
	~CRadialDevice();
};

