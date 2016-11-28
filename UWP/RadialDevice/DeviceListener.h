#pragma once

#include "../Megayuchi_Common/IRadialDevice.h"


using namespace winrt;

class CRadialDevice;
class DeviceListener
{
	CRadialDevice*	m_pRadialDevice = nullptr;
	winrt::Windows::UI::Input::RadialController m_Controller = nullptr;

	fire_and_forget AddIconToMenu(const WCHAR* wchIconFileName, const WCHAR* wchMenuName, int MenuItemID);
	void OnRotationChanged(Windows::UI::Input::RadialController sender, Windows::UI::Input::RadialControllerRotationChangedEventArgs args);
	void OnButtonClicked(Windows::UI::Input::RadialController sender, Windows::UI::Input::RadialControllerButtonClickedEventArgs args);
	void OnScreenContactStarted(Windows::UI::Input::RadialController sender, Windows::UI::Input::RadialControllerScreenContactStartedEventArgs args);
	void OnScreenContactContinued(Windows::UI::Input::RadialController sender, Windows::UI::Input::RadialControllerScreenContactContinuedEventArgs args);
	void OnScreenContactEnded(Windows::UI::Input::RadialController sender, Windows::IInspectable args);
	void OnControlAcquired(Windows::UI::Input::RadialController sender, Windows::UI::Input::RadialControllerControlAcquiredEventArgs args);
	void OnControlLost(Windows::UI::Input::RadialController sender, Windows::IInspectable args);
	void OnInvoked(Windows::UI::Input::RadialControllerMenuItem sender, Windows::IInspectable args);

	void Cleanup();
public:


	void Init(CRadialDevice* pRadialDevice);
	void AddMenuItem(const WCHAR* wchIconFileName, const WCHAR* wchMenuName, int MenuItemID);
	void RemoveAllMenuItems();

	DeviceListener();
	~DeviceListener();

};

