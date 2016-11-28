//
// MainPage.xaml.cpp
// Implementation of the MainPage class.
//

#include "pch.h"
#include "MainPage.xaml.h"
#include "../Megayuchi_Common/IRadialDevice.h"
#include "Util.h"

using namespace Client;

using namespace Platform;
using namespace Windows::Foundation;
using namespace Windows::Foundation::Collections;
using namespace Windows::UI::Xaml;
using namespace Windows::UI::Xaml::Controls;
using namespace Windows::UI::Xaml::Controls::Primitives;
using namespace Windows::UI::Xaml::Data;
using namespace Windows::UI::Xaml::Input;
using namespace Windows::UI::Xaml::Media;
using namespace Windows::UI::Xaml::Navigation;

// The Blank Page item template is documented at http://go.microsoft.com/fwlink/?LinkId=402352&clcid=0x409

void __stdcall RadialDeviceCallbacFocused(IRadialDevice* pRadialDevice, int MenuItemID);
void __stdcall RadialDeviceCallbacLostFocus(IRadialDevice* pRadialDevice, int MenuItemID);
void __stdcall RadialDeviceCallbackRot(IRadialDevice* pRadialDevice, int MenuItemID, double delta);
void __stdcall RadialDeviceCallbackButtonClicked(IRadialDevice* pRadialDevice, int MenuItemID);
void __stdcall RadialDeviceCallbackMenuItemAdded(IRadialDevice* pRadialDevice, int MenuItemID, BOOL bSuccess);

MainPage::MainPage()
{
	InitializeComponent();



	CPU_ARCH		arch = CPU_ARCH_X86;
	BUILD_CONFIG	config = BUILD_CONFIG_DEBUG;

	// Architecture
#if defined(ARM_ARCH)
	arch = CPU_ARCH_ARM;
#elif defined(WIN64)
	arch = CPU_ARCH_X64;
#else
	arch = CPU_ARCH_X86;
#endif

	// debug / release
#ifdef _DEBUG
	config = BUILD_CONFIG_DEBUG;
#else
	config = BUILD_CONFIG_RELEASE;
#endif

	WCHAR	wchRadialDeivceFileName[_MAX_PATH] = {};
	CreateEngineFileName(wchRadialDeivceFileName, _countof(wchRadialDeivceFileName), L"RadialDevice", arch, config);
	HMODULE	hRadialDeviceDLL = LoadPackagedLibrary(wchRadialDeivceFileName, 0);
	if (!hRadialDeviceDLL)
	{
		__debugbreak();
	}
	CREATE_INSTANCE_FUNC	pRadialDeviceCreateFunc = (CREATE_INSTANCE_FUNC)GetProcAddress(hRadialDeviceDLL, "DllCreateInstance");
	pRadialDeviceCreateFunc(&m_pRadialDevice);

	if (m_pRadialDevice->Initialize(
		RadialDeviceCallbacFocused,
		RadialDeviceCallbacLostFocus,
		RadialDeviceCallbackRot,
		RadialDeviceCallbackButtonClicked,
		RadialDeviceCallbackMenuItemAdded))
	{
		wprintf_s(L"Successfully initialized.\n");
		m_pRadialDevice->AddMenuItem(L"icon_custom0.png", L"C0", CUSTOM_MENU_ID_0);
		m_pRadialDevice->AddMenuItem(L"icon_custom1.png", L"C1", CUSTOM_MENU_ID_1);
	}
}
void MainPage::OnRadialDeviceCallbacFocus(IRadialDevice* pRadialDevice, int MenuItemID)
{
	const WCHAR*	wchMenuID = nullptr;
	switch (MenuItemID)
	{
	case CUSTOM_MENU_ID_0:
		wchMenuID = L"CUSTOM_MENU_ID_0";
		break;
	case CUSTOM_MENU_ID_1:
		wchMenuID = L"CUSTOM_MENU_ID_1";
		break;
	}
	WriteTextW(L"OnFocused %s\n", wchMenuID);
}
void MainPage::OnRadialDeviceCallbacLostFocus(IRadialDevice* pRadialDevice, int MenuItemID)
{
	const WCHAR*	wchMenuID = nullptr;
	switch (MenuItemID)
	{
	case CUSTOM_MENU_ID_0:
		wchMenuID = L"CUSTOM_MENU_ID_0";
		break;
	case CUSTOM_MENU_ID_1:
		wchMenuID = L"CUSTOM_MENU_ID_1";
		break;
	}
	WriteTextW(L"OnLostFocus %s\n", wchMenuID);
}
void MainPage::OnRadialDeviceCallbackRot(IRadialDevice* pRadialDevice, int MenuItemID, double delta)
{
	const WCHAR*	wchMenuID = nullptr;
	switch (MenuItemID)
	{
	case CUSTOM_MENU_ID_0:
		wchMenuID = L"CUSTOM_MENU_ID_0";
		break;
	case CUSTOM_MENU_ID_1:
		wchMenuID = L"CUSTOM_MENU_ID_1";
		break;
	}
	WriteTextW(L"OnRotation %s - %f\n", wchMenuID, (float)delta);
}
void MainPage::OnRadialDeviceCallbackButtonClicked(IRadialDevice* pRadialDevice, int MenuItemID)
{
	const WCHAR*	wchMenuID = nullptr;
	switch (MenuItemID)
	{
	case CUSTOM_MENU_ID_0:
		wchMenuID = L"CUSTOM_MENU_ID_0";
		break;
	case CUSTOM_MENU_ID_1:
		wchMenuID = L"CUSTOM_MENU_ID_1";
		break;
	}
	WriteTextW(L"OnButtonClicked %s\n", wchMenuID);
}
void MainPage::OnRadialDeviceCallbackMenuItemAdded(IRadialDevice* pRadialDevice, int MenuItemID, BOOL bSuccess)
{
	const WCHAR*	wchMenuID = nullptr;
	switch (MenuItemID)
	{
	case CUSTOM_MENU_ID_0:
		wchMenuID = L"CUSTOM_MENU_ID_0";
		break;
	case CUSTOM_MENU_ID_1:
		wchMenuID = L"CUSTOM_MENU_ID_1";
		break;
	}
	WCHAR	wchTxt[256] = {};
	if (bSuccess)
	{
		swprintf_s(wchTxt, L"Successfully added %s.\n", wchMenuID);
	}
	else
	{
		swprintf_s(wchTxt, L"Failed to add %s.\n", wchMenuID);
	}
	WriteTextW(wchTxt);

	if (bSuccess)
	{
		m_iCustomMenuCount++;
		if (m_iCustomMenuCount == CUSTOM_MENU_COUNT)
		{
			WriteTextW(L"Press and Hold Surface Dial.\n");
		}
	}
	
}

void MainPage::WriteText(Platform::String^ text)
{
#ifdef _DEBUG
	OutputBox->Text += text;

	UpdateConsoleScrollBar();
#endif
}

void MainPage::WriteTextW(const WCHAR* wchFormat,...)
{
	va_list argptr;
	WCHAR cBuf[2048];

	va_start( argptr, wchFormat );
	vswprintf_s( cBuf,2048, wchFormat, argptr );
	va_end( argptr );

	String^ str = ref new String(cBuf);
	WriteText(str);

}

void MainPage::UpdateConsoleScrollBar()
{
	auto height = OutputTextScrollViewer->ScrollableHeight;
	auto width = OutputTextScrollViewer->ScrollableWidth;

	OutputTextScrollViewer->ChangeView(width, height, 1.0f);
}

void __stdcall RadialDeviceCallbacFocused(IRadialDevice* pRadialDevice, int MenuItemID)
{
	auto content = dynamic_cast<Windows::UI::Xaml::Controls::Frame^>(Window::Current->Content)->Content;
	MainPage^ mainPage = dynamic_cast<MainPage^>(content);
	mainPage->OnRadialDeviceCallbacFocus(pRadialDevice, MenuItemID);
}
void __stdcall RadialDeviceCallbacLostFocus(IRadialDevice* pRadialDevice, int MenuItemID)
{
	auto content = dynamic_cast<Windows::UI::Xaml::Controls::Frame^>(Window::Current->Content)->Content;
	MainPage^ mainPage = dynamic_cast<MainPage^>(content);
	mainPage->OnRadialDeviceCallbacLostFocus(pRadialDevice, MenuItemID);
}
void __stdcall RadialDeviceCallbackRot(IRadialDevice* pRadialDevice, int MenuItemID, double delta)
{
	auto content = dynamic_cast<Windows::UI::Xaml::Controls::Frame^>(Window::Current->Content)->Content;
	MainPage^ mainPage = dynamic_cast<MainPage^>(content);
	mainPage->OnRadialDeviceCallbackRot(pRadialDevice, MenuItemID, delta);
}
void __stdcall RadialDeviceCallbackButtonClicked(IRadialDevice* pRadialDevice, int MenuItemID)
{
	auto content = dynamic_cast<Windows::UI::Xaml::Controls::Frame^>(Window::Current->Content)->Content;
	MainPage^ mainPage = dynamic_cast<MainPage^>(content);
	mainPage->OnRadialDeviceCallbackButtonClicked(pRadialDevice, MenuItemID);
}
void __stdcall RadialDeviceCallbackMenuItemAdded(IRadialDevice* pRadialDevice, int MenuItemID, BOOL bSuccess)
{
	auto content = dynamic_cast<Windows::UI::Xaml::Controls::Frame^>(Window::Current->Content)->Content;
	MainPage^ mainPage = dynamic_cast<MainPage^>(content);
	mainPage->OnRadialDeviceCallbackMenuItemAdded(pRadialDevice, MenuItemID, bSuccess);
}
