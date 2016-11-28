#pragma once

interface IRadialDevice;
typedef void(__stdcall *RADIAL_DEVICE_CALLBACK_FOCUS)(IRadialDevice* pRadialDevice, int MenuItemID);
typedef void(__stdcall *RADIAL_DEVICE_CALLBACK_LOST_FOCUS)(IRadialDevice* pRadialDevice, int MenuItemID);
typedef void(__stdcall *RADIAL_DEVICE_CALLBACK_ROT)(IRadialDevice* pRadialDevice, int MenuItemID, double delta);
typedef void(__stdcall *RADIAL_DEVICE_CALLBACK_BUTTON_CLICKED)(IRadialDevice* pRadialDevice, int MenuItemID);
typedef void(__stdcall *RADIAL_DEVICE_CALLBACK_BUTTON_CLICKED)(IRadialDevice* pRadialDevice, int MenuItemID);
typedef void(__stdcall *RADIAL_DEVICE_CALLBACK_MENU_ITEM_ADDED)(IRadialDevice* pRadialDevice, int MenuItemID, BOOL bSuccess);

interface IRadialDevice : public IUnknown
{
	virtual		BOOL __stdcall Initialize(RADIAL_DEVICE_CALLBACK_FOCUS pCallbackFocus,
		RADIAL_DEVICE_CALLBACK_LOST_FOCUS pCallbackLostFocus,
		RADIAL_DEVICE_CALLBACK_ROT pCallbackRot,
		RADIAL_DEVICE_CALLBACK_BUTTON_CLICKED pCallbackButtonClicked,
		RADIAL_DEVICE_CALLBACK_MENU_ITEM_ADDED pCallbackMenuItemAdded) = 0;
	virtual		BOOL __stdcall AddMenuItem(const WCHAR* wchIconFileName, const WCHAR* wchMenuName, int MenuItemID) = 0;
};
