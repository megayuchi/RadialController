#pragma once

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

class CRadialDevice;
class DeviceListener
{
	CRadialDevice*	m_pRadialDevice = nullptr;
	
    Microsoft::WRL::ComPtr<IRadialControllerInterop> m_controllerInterop;
    Microsoft::WRL::ComPtr<ABI::Windows::UI::Input::IRadialController> m_controller;
    Microsoft::WRL::ComPtr<ABI::Windows::UI::Input::IRadialControllerMenuItemStatics> m_menuItemStatics;

	EventRegistrationToken m_bottonClickedToken;
    EventRegistrationToken m_rotatedToken;
    EventRegistrationToken m_screenContactStartedToken;
    EventRegistrationToken m_screenContactEndedToken;
    EventRegistrationToken m_screenContactContinuedToken;
    EventRegistrationToken m_controlLostToken;
    EventRegistrationToken m_controlAcquiredToken;
    EventRegistrationToken m_buttonClickedToken;
        
    HRESULT RegisterForEvents(HWND hwnd);
	HRESULT AddMenuItemFromKnownIcon(ComPtr<ABI::Windows::UI::Input::IRadialControllerMenuItem>* pOutNewMenuItem, _In_ HSTRING itemName, _In_ RadialControllerMenuKnownIcon icon, _Out_ EventRegistrationToken* pOutToken);
	HRESULT AddMenuItemFromUnknownIcon(ComPtr<ABI::Windows::UI::Input::IRadialControllerMenuItem>* pOutNewMenuItem, _In_ HSTRING itemName, _In_ ComPtr<ABI::Windows::Storage::Streams::IRandomAccessStreamReference> icon, _Out_ EventRegistrationToken* pOutToken);
    HRESULT OnItemInvoked(_In_ ABI::Windows::UI::Input::IRadialControllerMenuItem* item, _In_ IInspectable* args);

    HRESULT OnScreenContactContinued(_In_ ABI::Windows::UI::Input::IRadialController*, _In_ ABI::Windows::UI::Input::IRadialControllerScreenContactContinuedEventArgs* args);
    HRESULT OnScreenContactStarted(_In_ ABI::Windows::UI::Input::IRadialController*, _In_ ABI::Windows::UI::Input::IRadialControllerScreenContactStartedEventArgs* args);
    HRESULT OnScreenContactEnded(_In_ ABI::Windows::UI::Input::IRadialController* /*sender*/, _In_ IInspectable* args);
    HRESULT OnControlLost(_In_ ABI::Windows::UI::Input::IRadialController* sender, _In_ IInspectable* args);
    HRESULT OnControlAcquired(_In_ ABI::Windows::UI::Input::IRadialController* sender, _In_ ABI::Windows::UI::Input::IRadialControllerControlAcquiredEventArgs* args);
    HRESULT OnRotationChanged(_In_ ABI::Windows::UI::Input::IRadialController* sender, _In_ ABI::Windows::UI::Input::IRadialControllerRotationChangedEventArgs* args);
	HRESULT OnButtonClicked(_In_ ABI::Windows::UI::Input::IRadialController* sender, _In_ ABI::Windows::UI::Input::IRadialControllerButtonClickedEventArgs* args);
       
    DWORD _threadId = 0;
	
public:
    DeviceListener();
	~DeviceListener();

    HRESULT Init(CRadialDevice* pRadialDevice,HWND hwnd);
    HRESULT SetRotationResolution(double res);
	HRESULT AddMenuItem(ComPtr<ABI::Windows::UI::Input::IRadialControllerMenuItem>* pOutMenuItem, const WCHAR* wchIconFileName, const WCHAR* wchMenuName, EventRegistrationToken* pOutToken);;
	

private:
	
};

