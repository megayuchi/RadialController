#include "pch.h"
#include "DeviceListener.h"
#include "RadialDevice.h"

#include "winrt/base.h"

#include "../Megayuchi_Util/RefCheck_WinRT_CPP.h"
#include "../Megayuchi_Util/debug_new.h"


using namespace winrt;

using namespace Windows::ApplicationModel::Activation;
using namespace Windows::Foundation;
using namespace Windows::UI;
using namespace Windows::UI::Xaml;
using namespace Windows::UI::Xaml::Controls;
using namespace Windows::UI::Xaml::Media;
using namespace Windows::Storage;
using namespace Windows::Storage::Streams;
using namespace Windows::Graphics::Imaging;
using namespace Windows::Media::Ocr;
using namespace Windows::Storage::Pickers;


DeviceListener::DeviceListener()
{
}
fire_and_forget DeviceListener::AddIconToMenu(const WCHAR* wchIconFileName, const WCHAR* wchMenuName, int MenuItemID)
{
	Windows::ApplicationModel::Package package = Windows::ApplicationModel::Package::Current();
	Windows::Storage::StorageFolder folder = package.InstalledLocation();
	auto t = folder.Path();

	WCHAR wchCurPath[_MAX_PATH] = {};
	GetCurrentDirectory(_MAX_PATH, wchCurPath);

	WCHAR	wchFullPath[_MAX_PATH] = {};
	swprintf_s(wchFullPath, L"%s\\%s", wchCurPath, wchIconFileName);

	auto fullPath = hstring(wchFullPath);
	auto menuName = hstring(wchMenuName);

	BOOL	bResult = FALSE;

	winrt::Windows::Storage::StorageFile file = co_await winrt::Windows::Storage::StorageFile::GetFileFromPathAsync(fullPath);
	if (file)
	{
		auto streamRef = winrt::Windows::Storage::Streams::RandomAccessStreamReference::CreateFromFile(file);
		auto menuItem = Windows::UI::Input::RadialControllerMenuItem::CreateFromIcon(menuName, streamRef);
		if (menuItem)
		{
			Windows::UI::Input::IRadialControllerMenu menu = m_Controller.Menu();
			auto items = menu.Items();
			items.Append(menuItem);

			menuItem.Invoked(TypedEventHandler<Windows::UI::Input::RadialControllerMenuItem, Windows::IInspectable>(this, &DeviceListener::OnInvoked));
			m_pRadialDevice->OnAddMenuItem(menuItem, MenuItemID);
			bResult = TRUE;
		}
	}
	m_pRadialDevice->OnCompleteAddMenu(MenuItemID, bResult);
}
void DeviceListener::Init(CRadialDevice* pRadialDevice)
{

	m_pRadialDevice = pRadialDevice;
	m_Controller = winrt::Windows::UI::Input::RadialController::CreateForCurrentView();
	m_Controller.RotationResolutionInDegrees(10);

	m_Controller.RotationChanged(TypedEventHandler<Windows::UI::Input::RadialController, Windows::UI::Input::RadialControllerRotationChangedEventArgs>(this, &DeviceListener::OnRotationChanged));
	m_Controller.ButtonClicked(TypedEventHandler<Windows::UI::Input::RadialController, Windows::UI::Input::RadialControllerButtonClickedEventArgs>(this, &DeviceListener::OnButtonClicked));
	m_Controller.ScreenContactStarted(TypedEventHandler<Windows::UI::Input::RadialController, Windows::UI::Input::RadialControllerScreenContactStartedEventArgs>(this, &DeviceListener::OnScreenContactStarted));
	m_Controller.ScreenContactContinued(TypedEventHandler<Windows::UI::Input::RadialController, Windows::UI::Input::RadialControllerScreenContactContinuedEventArgs>(this, &DeviceListener::OnScreenContactContinued));
	m_Controller.ScreenContactEnded(TypedEventHandler<Windows::UI::Input::RadialController, Windows::IInspectable>(this, &DeviceListener::OnScreenContactEnded));
	m_Controller.ControlAcquired(TypedEventHandler<Windows::UI::Input::RadialController, Windows::UI::Input::RadialControllerControlAcquiredEventArgs>(this, &DeviceListener::OnControlAcquired));
	m_Controller.ControlLost(TypedEventHandler<Windows::UI::Input::RadialController, Windows::IInspectable>(this, &DeviceListener::OnControlLost));
}
void DeviceListener::AddMenuItem(const WCHAR* wchIconFileName, const WCHAR* wchMenuName, int MenuItemID)
{
	AddIconToMenu(wchIconFileName, wchMenuName, MenuItemID);

}
void DeviceListener::OnInvoked(Windows::UI::Input::RadialControllerMenuItem sender, Windows::IInspectable args)
{
	m_pRadialDevice->OnItemInvoked(sender);
}

void DeviceListener::OnRotationChanged(Windows::UI::Input::RadialController sender, Windows::UI::Input::RadialControllerRotationChangedEventArgs args)
{
	auto menu = sender.Menu();
	auto menuItem = menu.GetSelectedMenuItem();

	double delta = 0;
	args->get_RotationDeltaInDegrees(&delta);

	m_pRadialDevice->OnRotationChanged(menuItem, delta);

}
void DeviceListener::OnButtonClicked(Windows::UI::Input::RadialController sender, Windows::UI::Input::RadialControllerButtonClickedEventArgs args)
{
	auto menu = sender.Menu();
	auto menuItem = menu.GetSelectedMenuItem();

	m_pRadialDevice->OnButtonClicked(menuItem);
}
void DeviceListener::OnScreenContactStarted(Windows::UI::Input::RadialController sender, Windows::UI::Input::RadialControllerScreenContactStartedEventArgs args)
{
	auto contact = args.Contact();

	int a = 0;
}
void DeviceListener::OnScreenContactContinued(Windows::UI::Input::RadialController sender, Windows::UI::Input::RadialControllerScreenContactContinuedEventArgs args)
{
	auto contact = args.Contact();

	int a = 0;
}
void DeviceListener::OnScreenContactEnded(Windows::UI::Input::RadialController sender, Windows::IInspectable args)
{
	int a = 0;
}
void DeviceListener::OnControlAcquired(Windows::UI::Input::RadialController sender, Windows::UI::Input::RadialControllerControlAcquiredEventArgs args)
{

	int a = 0;
}
void DeviceListener::OnControlLost(Windows::UI::Input::RadialController sender, Windows::IInspectable args)
{
	int a = 0;
}
void DeviceListener::RemoveAllMenuItems()
{
	auto menu = m_Controller.Menu();
	auto items = menu.Items();
	items.Clear();
	items = nullptr;
	menu = nullptr;
}
void DeviceListener::Cleanup()
{
	RemoveAllMenuItems();

	m_Controller = nullptr;
}
DeviceListener::~DeviceListener()
{
	Cleanup();
}