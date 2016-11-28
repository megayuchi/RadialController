#include "stdafx.h"

#include "DeviceListener.h"
#include <windows.h>
#include <Windowsx.h>
#include <tchar.h>
#include <StrSafe.h>
#include <wrl\implements.h>
#include <wrl\module.h>
#include <wrl\event.h>
#include <roapi.h>
#include <wrl.h>
#include <map>
#include <vector>
#include <windows.foundation.h>
#include <windows.foundation.numerics.h>
#include <windows.foundation.collections.h>
#include <windows.ui.input.h>
//#include <Windows.ApplicationModel.h>
//#include <Windows.ApplicationModel.datatransfer.h>
#include <wrl\client.h>
#include <winstring.h>
#include "RadialDevice.h"
#include "../Megayuchi_UTIL/RefCheck.h"

#define RETURN_IF_FAILED(hr) { if(FAILED(hr)) return hr; }

using namespace ABI::Windows::UI::Input;
using namespace ABI::Windows::Foundation;
using namespace Microsoft::WRL;
using namespace Microsoft::WRL::Wrappers;




DeviceListener::DeviceListener()
{
}
HRESULT DeviceListener::Init(CRadialDevice* pRadialDevice,HWND hwnd)
{
	m_pRadialDevice = pRadialDevice;

	bool isRoInit = false;
	HRESULT hr = Windows::Foundation::Initialize(RO_INIT_MULTITHREADED);

	if (SUCCEEDED(hr))
	{
		WriteDebugStringW(DEBUG_OUTPUT_TYPE_DEBUG_CONSOLE, L"RO_INIT_MULTITHREADED SUCCEEDED\n");
		isRoInit = true;
	}
	else
	{
		WriteDebugStringW(DEBUG_OUTPUT_TYPE_DEBUG_CONSOLE, L"RO_INIT_MULTITHREADED FAILED\n");
	}

	if (isRoInit)
	{
		hr = RegisterForEvents(hwnd);

		RETURN_IF_FAILED(Windows::Foundation::GetActivationFactory(
			HStringReference(RuntimeClass_Windows_UI_Input_RadialControllerMenuItem).Get(),
			&m_menuItemStatics));

		if (SUCCEEDED(hr))
		{
			WriteDebugStringW(DEBUG_OUTPUT_TYPE_DEBUG_CONSOLE, L"Successfully initialized \n");
		}
		else
		{
			WriteDebugStringW(DEBUG_OUTPUT_TYPE_DEBUG_CONSOLE, L"Failed to initialize\n");
		}
		
	}
	
	return hr;
}
HRESULT DeviceListener::AddMenuItem(ComPtr<ABI::Windows::UI::Input::IRadialControllerMenuItem>* pOutMenuItem,const WCHAR* wchIconFileName, const WCHAR* wchMenuName,EventRegistrationToken* pOutToken)
{
	// Icon's filename for adding
	//const WCHAR*	ICON_FILE_NAME = L"\\icon_miku.png";
	//const WCHAR*	MENU_NAME = L"Miku";

	HRESULT	hr;
	ComPtr<IActivationFactory> pFactory;
	hr = Windows::Foundation::GetActivationFactory(Microsoft::WRL::Wrappers::HStringReference(RuntimeClass_Windows_Storage_StorageFile).Get(), &pFactory);
	if (S_OK != hr)
		__debugbreak();

	ComPtr<ABI::Windows::Storage::IStorageFileStatics> pStorageFileStatics;
	hr = pFactory.As(&pStorageFileStatics);
	if (S_OK != hr)
		__debugbreak();

	// Open the file.
	WCHAR wchCurPath[_MAX_PATH] = {};
	GetCurrentDirectory(_MAX_PATH, wchCurPath);
	
	WCHAR	wchFullPath[_MAX_PATH] = {};
	swprintf_s(wchFullPath, L"%s\\%s", wchCurPath, wchIconFileName);
	
	HStringReference path(wchFullPath);
	HStringReference menuName(wchMenuName);
	
	
	ComPtr<ABI::Windows::Foundation::__FIAsyncOperation_1_Windows__CStorage__CStorageFile_t> async;
	pStorageFileStatics->GetFileFromPathAsync(path.Get(), &async);

	struct OPEN_STREAM_RESULT
	{
		ComPtr<ABI::Windows::Storage::Streams::IRandomAccessStreamReference> streamRef;
		HRESULT	hr;
		HANDLE	hCompleteEvent;
	};

	OPEN_STREAM_RESULT	result;
	result.hCompleteEvent = CreateEvent(NULL, FALSE, FALSE, NULL);
	result.hr = S_FALSE;

	OPEN_STREAM_RESULT*	pInOutResult = &result;
	
	typedef IAsyncOperationCompletedHandler<ABI::Windows::Storage::StorageFile*> HandlerType;
	auto handler = Microsoft::WRL::Callback<HandlerType>([this, menuName, pInOutResult, pOutToken](IAsyncOperation<ABI::Windows::Storage::StorageFile*>* async, AsyncStatus status)
	{
		HRESULT		hr = S_FALSE;
		ComPtr<ABI::Windows::Storage::IStorageFile> file;
		switch (status)
		{
		case Started:
			break;
		case Completed:
		case Error:
			hr = async->GetResults(&file);
			break;
		case Canceled:
			break;
		}
		if (file.Get())
		{	
			ComPtr<IActivationFactory> pStreamRefFactory;
			const WCHAR* runtime_class_name = L"Windows.Storage.Streams.RandomAccessStreamReference";
			hr = Windows::Foundation::GetActivationFactory(Microsoft::WRL::Wrappers::HStringReference(runtime_class_name).Get(), &pStreamRefFactory);
			if (S_OK != hr)
				__debugbreak();

			ComPtr<ABI::Windows::Storage::Streams::IRandomAccessStreamReferenceStatics> pStreamRefStatic;
			hr = pStreamRefFactory.As(&pStreamRefStatic);
			if (S_OK != hr)
				__debugbreak();

			ComPtr<ABI::Windows::Storage::Streams::IRandomAccessStreamReference> streamRef;
			hr = pStreamRefStatic->CreateFromFile(file.Get(), &streamRef);
			pInOutResult->streamRef = streamRef;

		}
		pInOutResult->hr = hr;
		SetEvent(pInOutResult->hCompleteEvent);
		return hr;
	});
	async->put_Completed(handler.Get());
	
	WaitForSingleObject(pInOutResult->hCompleteEvent, INFINITE);
	CloseHandle(pInOutResult->hCompleteEvent);
	pInOutResult = nullptr;

	if (S_OK == result.hr)
	{
		ComPtr<ABI::Windows::UI::Input::IRadialControllerMenuItem> menuItem;
		AddMenuItemFromUnknownIcon(pOutMenuItem,menuName.Get(), result.streamRef, pOutToken);
	}
	return result.hr;
}

HRESULT DeviceListener::SetRotationResolution(double res)
{
	return m_controller->put_RotationResolutionInDegrees(res);
}

HRESULT DeviceListener::RegisterForEvents(HWND hwnd)
{
	RETURN_IF_FAILED(Windows::Foundation::GetActivationFactory(
		HStringReference(RuntimeClass_Windows_UI_Input_RadialController).Get(),
		&m_controllerInterop));

	RETURN_IF_FAILED(m_controllerInterop->CreateForWindow(hwnd, IID_PPV_ARGS(&m_controller)));

	// Wire events
	RETURN_IF_FAILED(m_controller->add_ScreenContactContinued(
		Callback<ITypedEventHandler<RadialController*, RadialControllerScreenContactContinuedEventArgs*>>(this, &DeviceListener::OnScreenContactContinued).Get(),
		&m_screenContactContinuedToken));

	RETURN_IF_FAILED(m_controller->add_ScreenContactStarted(
		Callback<ITypedEventHandler<RadialController*, RadialControllerScreenContactStartedEventArgs*>>(this, &DeviceListener::OnScreenContactStarted).Get(),
		&m_screenContactStartedToken));

	RETURN_IF_FAILED(m_controller->add_ScreenContactEnded(
		Callback<ITypedEventHandler<RadialController*, IInspectable*>>(this, &DeviceListener::OnScreenContactEnded).Get(),
		&m_screenContactEndedToken));

	RETURN_IF_FAILED(m_controller->add_ControlLost(
		Callback<ITypedEventHandler<RadialController*, IInspectable*>>(this, &DeviceListener::OnControlLost).Get(),
		&m_controlLostToken));

	RETURN_IF_FAILED(m_controller->add_ControlAcquired(
		Callback<ITypedEventHandler<RadialController*, RadialControllerControlAcquiredEventArgs*>>(this, &DeviceListener::OnControlAcquired).Get(),
		&m_controlAcquiredToken));

	RETURN_IF_FAILED(m_controller->add_ButtonClicked(
		Callback<ITypedEventHandler<RadialController*, RadialControllerButtonClickedEventArgs*>>(this, &DeviceListener::OnButtonClicked).Get(),
		&m_bottonClickedToken));


	RETURN_IF_FAILED(m_controller->add_RotationChanged(
		Callback<ITypedEventHandler<RadialController*, RadialControllerRotationChangedEventArgs*>>(this, &DeviceListener::OnRotationChanged).Get(),
		&m_rotatedToken));

	// Lambda callback
	RETURN_IF_FAILED(m_controller->add_ButtonClicked(
		Callback<ITypedEventHandler<RadialController*, RadialControllerButtonClickedEventArgs*>>([this]
		(IRadialController*, IRadialControllerButtonClickedEventArgs* args)
	{
		WriteDebugStringW(DEBUG_OUTPUT_TYPE_DEBUG_CONSOLE, L"ButtonClicked\n");
		return S_OK;
	}).Get(),
		&m_buttonClickedToken));

	return S_OK;
}



HRESULT DeviceListener::AddMenuItemFromKnownIcon(ComPtr<ABI::Windows::UI::Input::IRadialControllerMenuItem>* pOutNewMenuItem,_In_ HSTRING itemName, _In_ RadialControllerMenuKnownIcon icon, _Out_ EventRegistrationToken* pOutToken)
{
	Microsoft::WRL::ComPtr<ABI::Windows::UI::Input::IRadialControllerMenu> menu;
	RETURN_IF_FAILED(m_controller->get_Menu(&menu));

	// Get menu items
	ComPtr<Collections::IVector<RadialControllerMenuItem*>> menuItems;
	RETURN_IF_FAILED(menu->get_Items(&menuItems));

	// Create item
	ComPtr<IRadialControllerMenuItem> menuItem;
	RETURN_IF_FAILED(m_menuItemStatics->CreateFromKnownIcon(itemName, icon, &menuItem));


	// Set Callback
	RETURN_IF_FAILED(menuItem->add_Invoked(
		Callback<ITypedEventHandler<RadialControllerMenuItem*, IInspectable*>>(this, &DeviceListener::OnItemInvoked).Get(),
		pOutToken));
	
	// Add item to menu
	RETURN_IF_FAILED(menuItems->Append(menuItem.Get()));

	// Log new item
	wchar_t message[2000];
	swprintf_s(message, 2000, L"Added %s to menu\n", WindowsGetStringRawBuffer(itemName, nullptr));
	WriteDebugStringW(DEBUG_OUTPUT_TYPE_DEBUG_CONSOLE,message);

	*pOutNewMenuItem = menuItem;

	return S_OK;
}


HRESULT DeviceListener::AddMenuItemFromUnknownIcon(ComPtr<ABI::Windows::UI::Input::IRadialControllerMenuItem>* pOutNewMenuItem,_In_ HSTRING itemName, _In_ ComPtr<ABI::Windows::Storage::Streams::IRandomAccessStreamReference> icon, _Out_ EventRegistrationToken* pOutToken)
{
	Microsoft::WRL::ComPtr<ABI::Windows::UI::Input::IRadialControllerMenu> menu;
	RETURN_IF_FAILED(m_controller->get_Menu(&menu));

	// Get menu items
	ComPtr<Collections::IVector<RadialControllerMenuItem*>> menuItems;
	RETURN_IF_FAILED(menu->get_Items(&menuItems));

	// Create item
	ComPtr<IRadialControllerMenuItem> menuItem;
	HRESULT hr = m_menuItemStatics->CreateFromIcon(itemName, icon.Get(), &menuItem);
	RETURN_IF_FAILED(hr);
	
	// Set Callback
	RETURN_IF_FAILED(menuItem->add_Invoked(
		Callback<ITypedEventHandler<RadialControllerMenuItem*, IInspectable*>>(this, &DeviceListener::OnItemInvoked).Get(),
		pOutToken));
	
	// Add item to menu
	RETURN_IF_FAILED(menuItems->Append(menuItem.Get()));

	WriteDebugStringW(DEBUG_OUTPUT_TYPE_DEBUG_CONSOLE, L"Added %s to menu\n", WindowsGetStringRawBuffer(itemName, nullptr));

	*pOutNewMenuItem = menuItem;
	return S_OK;
}

HRESULT DeviceListener::OnItemInvoked(_In_ IRadialControllerMenuItem* item, _In_ IInspectable* args)
{
	if (item)
	{
		ComPtr<ABI::Windows::UI::Input::IRadialControllerMenuItem> menuItem = item;
		m_pRadialDevice->OnItemInvoked(menuItem);
	
		HSTRING str;
		item->get_DisplayText(&str);
		WriteDebugStringW(DEBUG_OUTPUT_TYPE_CONSOLE,L"ItemInvoked %s\n", WindowsGetStringRawBuffer(str, nullptr));
	}
	else
	{
		int a = 0;
	}

	return S_OK;
}

HRESULT DeviceListener::OnScreenContactContinued(_In_ IRadialController* /*sender*/, _In_ IRadialControllerScreenContactContinuedEventArgs* args)
{

	return S_OK;
}

HRESULT DeviceListener::OnScreenContactStarted(_In_ IRadialController* /*sender*/, _In_ IRadialControllerScreenContactStartedEventArgs* args)
{

	return S_OK;
}

HRESULT DeviceListener::OnScreenContactEnded(_In_ IRadialController* sender, _In_ IInspectable* args)
{
	WriteDebugStringW(DEBUG_OUTPUT_TYPE_DEBUG_CONSOLE, L"ScreenContactEnded\n");

	
	return S_OK;
}

HRESULT DeviceListener::OnControlLost(_In_ IRadialController* sender, _In_ IInspectable* args)
{
	WriteDebugStringW(DEBUG_OUTPUT_TYPE_DEBUG_CONSOLE, L"ControlLost\n");

	return S_OK;
}

HRESULT DeviceListener::OnControlAcquired(_In_ IRadialController* sender, _In_ IRadialControllerControlAcquiredEventArgs* args)
{
	WriteDebugStringW(DEBUG_OUTPUT_TYPE_DEBUG_CONSOLE,L"ControlAcquired\n");

	return S_OK;
}
HRESULT DeviceListener::OnButtonClicked(_In_ IRadialController* sender, _In_ IRadialControllerButtonClickedEventArgs* args)
{
	ComPtr<ABI::Windows::UI::Input::IRadialControllerMenu> menu;
	ComPtr<ABI::Windows::UI::Input::IRadialControllerMenuItem> menuItem;
	sender->get_Menu(&menu);
	
	HRESULT hr = menu->GetSelectedMenuItem(&menuItem);
	
	m_pRadialDevice->OnButtonClicked(menuItem);
	
	WriteDebugStringW(DEBUG_OUTPUT_TYPE_DEBUG_CONSOLE, L"Button Clicked\n");

	return S_OK;
}
HRESULT DeviceListener::OnRotationChanged(_In_ IRadialController* sender, _In_ IRadialControllerRotationChangedEventArgs* args)
{
	double delta = 0;
	
	ComPtr<ABI::Windows::UI::Input::IRadialControllerMenu> menu;
	ComPtr<ABI::Windows::UI::Input::IRadialControllerMenuItem> menuItem;
	sender->get_Menu(&menu);
	
	HRESULT hr = menu->GetSelectedMenuItem(&menuItem);
	
	args->get_RotationDeltaInDegrees(&delta);
	
	m_pRadialDevice->OnRotationChanged(menuItem, delta);
	
	WriteDebugStringW(DEBUG_OUTPUT_TYPE_DEBUG_CONSOLE, L"RotationChanged delta=%lf\n", delta);
	
	return S_OK;
}

template<typename TContactArgs>
HRESULT DeviceListener::LogContact(_In_ TContactArgs* args)
{
	// Get contact
	ComPtr<IRadialControllerScreenContact> contact;
	RETURN_IF_FAILED(args->get_Contact(&contact));

	if (contact != nullptr)
	{
		// Get contact info
		Point position;
		Rect bounds;
		RETURN_IF_FAILED(contact->get_Position(&position));
		RETURN_IF_FAILED(contact->get_Bounds(&bounds));

		// Log contact info
		wchar_t message[2000];
		swprintf_s(message, 2000, L"\t Postion X=%lf, Y=%lf & \n \t Bounds Height=%lf, Width=%lf, X=%lf, Y=%lf\n",
			position.X, position.Y,
			bounds.Height, bounds.Width, bounds.X, bounds.Y);

		WriteDebugStringW(DEBUG_OUTPUT_TYPE_DEBUG_CONSOLE, message);
	}

	return S_OK;
}
DeviceListener::~DeviceListener()
{
	m_controllerInterop = nullptr;
	m_controller = nullptr;
	m_menuItemStatics = nullptr;

	RoUninitialize();
}