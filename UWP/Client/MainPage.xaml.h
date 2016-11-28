//
// MainPage.xaml.h
// Declaration of the MainPage class.
//

#pragma once

#include "MainPage.g.h"

namespace Client
{

	enum CUMSTOM_MENU_ID
	{
		CUSTOM_MENU_ID_0 = 0,
		CUSTOM_MENU_ID_1 = 1
	};
	const int CUSTOM_MENU_COUNT = 2;
	/// <summary>
	/// An empty page that can be used on its own or navigated to within a Frame.
	/// </summary>
	public ref class MainPage sealed
	{
		IRadialDevice*	m_pRadialDevice = nullptr;
		int				m_iCustomMenuCount = 0;

		void WriteText(Platform::String^ text);
		void WriteTextW(const WCHAR* wchFormat, ...);
		void UpdateConsoleScrollBar();

	internal:
		void OnRadialDeviceCallbacFocus(IRadialDevice* pRadialDevice, int MenuItemID);
		void OnRadialDeviceCallbacLostFocus(IRadialDevice* pRadialDevice, int MenuItemID);
		void OnRadialDeviceCallbackRot(IRadialDevice* pRadialDevice, int MenuItemID, double delta);
		void OnRadialDeviceCallbackButtonClicked(IRadialDevice* pRadialDevice, int MenuItemID);
		void OnRadialDeviceCallbackMenuItemAdded(IRadialDevice* pRadialDevice, int MenuItemID, BOOL bSuccess);

	public:
		MainPage();

	};
}
