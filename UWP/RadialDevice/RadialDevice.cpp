// RadialDevice.cpp : Defines the exported functions for the DLL application.
//

#include "pch.h"
#include "DeviceListener.h"
#include "RadialDevice.h"
#include <inspectable.h>
#include "../Megayuchi_Util/RefCheck.h"
#include "../Megayuchi_UTIL/RefCheck_WinRT_CPP.h"
#include "../Megayuchi_UTIL/debug_new.h"


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


CRadialDevice::CRadialDevice()
{
#ifdef _DEBUG
	int	flag = _CRTDBG_ALLOC_MEM_DF |_CRTDBG_LEAK_CHECK_DF;
	_CrtSetDbgFlag(flag);	
#endif

}


STDMETHODIMP CRadialDevice::QueryInterface(REFIID refiid, void** ppv)
{
	
	return E_NOINTERFACE;

	
}
STDMETHODIMP_(ULONG) CRadialDevice::AddRef(void)
{
	m_ulRefCount++;
	return m_ulRefCount;
	
}
STDMETHODIMP_(ULONG) CRadialDevice::Release(void)
{
	DWORD	ref_count = --m_ulRefCount;
	if (!m_ulRefCount)
		delete this;

	return ref_count;
}

BOOL __stdcall CRadialDevice::Initialize(
		RADIAL_DEVICE_CALLBACK_FOCUS pCallbackFocus, 
		RADIAL_DEVICE_CALLBACK_LOST_FOCUS pCallbackLostFocus, 
		RADIAL_DEVICE_CALLBACK_ROT pCallbackRot, 
		RADIAL_DEVICE_CALLBACK_BUTTON_CLICKED pCallbackButtonClicked,
		RADIAL_DEVICE_CALLBACK_MENU_ITEM_ADDED pCallbackMenuItemAdded)
{
	m_pDeviceListener = new DeviceListener();
	m_pDeviceListener->Init(this);

	m_pCallbackFoucus = pCallbackFocus;
	m_pCallbackLostFocus = pCallbackLostFocus;
	m_pCallbackRot = pCallbackRot;
	m_pCallbackButtonClicked = pCallbackButtonClicked;
	m_pCallbackMenuItemAdded = pCallbackMenuItemAdded;

	m_pMenuCallbackTable = new MENU_CALLBACK_ITEM[MAX_MENU_COUNT];

	//m_pDeviceListener->AddExtMenu(L"icon_miku.png",L"Miku");
	return TRUE;
}

BOOL __stdcall CRadialDevice::AddMenuItem(const WCHAR* wchIconFileName, const WCHAR* wchMenuName, int MenuItemID)
{
	BOOL	bResult = FALSE;
	
	if (m_dwCurMenuCount >= MAX_MENU_COUNT)
		return FALSE;
		
	m_pDeviceListener->AddMenuItem(wchIconFileName, wchMenuName,MenuItemID);
	
	return TRUE;
}
void CRadialDevice::OnAddMenuItem(winrt::Windows::UI::Input::IRadialControllerMenuItem menuItem,int MenuItemID)
{
	if (menuItem)
	{		
		m_pMenuCallbackTable[m_dwCurMenuCount].menuItem = menuItem;
		m_pMenuCallbackTable[m_dwCurMenuCount].ID = MenuItemID;
		m_dwCurMenuCount++;
	}
}
void CRadialDevice::OnCompleteAddMenu(int MenuItemID, BOOL bSuccess)
{
	m_pCallbackMenuItemAdded(this, MenuItemID, bSuccess);
}

MENU_CALLBACK_ITEM* CRadialDevice::FindCallbackItem(winrt::Windows::UI::Input::IRadialControllerMenuItem menuItem) const
{
	MENU_CALLBACK_ITEM*	pSelectedItem = nullptr;
	for (DWORD i = 0; i < m_dwCurMenuCount; i++)
	{
		if (m_pMenuCallbackTable[i].menuItem == menuItem)
		{
			pSelectedItem = m_pMenuCallbackTable + i;
			break;
		}
	}
	return pSelectedItem;
}

void CRadialDevice::OnItemInvoked(winrt::Windows::UI::Input::IRadialControllerMenuItem menuItem)
{
	MENU_CALLBACK_ITEM*	pSelectedItem = FindCallbackItem(menuItem);
#ifdef _DEBUG
	if (!pSelectedItem)
		__debugbreak();
#endif
	if (m_pCurSelectedItem)
	{
		if (m_pCurSelectedItem != pSelectedItem)
		{
			m_pCallbackLostFocus(this,m_pCurSelectedItem->ID);
		}
	}
	m_pCallbackFoucus(this,pSelectedItem->ID);
	m_pCurSelectedItem = pSelectedItem;
}
void CRadialDevice::OnRotationChanged(winrt::Windows::UI::Input::IRadialControllerMenuItem menuItem, double delta)
{
	if (!m_pCurSelectedItem)
		return;

#ifdef _DEBUG
	MENU_CALLBACK_ITEM*	pSelectedItem = FindCallbackItem(menuItem);
	if (m_pCurSelectedItem != pSelectedItem)
		__debugbreak();
#endif

	m_pCallbackRot(this,m_pCurSelectedItem->ID,delta);
}
void CRadialDevice::OnButtonClicked(winrt::Windows::UI::Input::IRadialControllerMenuItem menuItem)
{
	if (!m_pCurSelectedItem)
		return;

#ifdef _DEBUG
	MENU_CALLBACK_ITEM*	pSelectedItem = FindCallbackItem(menuItem);
	if (m_pCurSelectedItem != pSelectedItem)
		__debugbreak();
#endif

	m_pCallbackButtonClicked(this,m_pCurSelectedItem->ID);

}
CRadialDevice::~CRadialDevice()
{
	if (m_pDeviceListener)
	{
		m_pDeviceListener->RemoveAllMenuItems();
	}
	if (m_pMenuCallbackTable)
	{
		delete[] m_pMenuCallbackTable;
		m_pMenuCallbackTable = nullptr;
	}
	if (m_pDeviceListener)
	{
		delete m_pDeviceListener;
		m_pDeviceListener = nullptr;
	}
	
#ifdef _DEBUG
	_ASSERT(_CrtCheckMemory());
#endif
}
