// RadialDevice.cpp : Defines the exported functions for the DLL application.
//

#include "stdafx.h"
#include "RadialDevice.h"
#include "DeviceListener.h"
#include "../Megayuchi_UTIL/RefCheck.h"
#include "../Megayuchi_UTIL/debug_new.h"



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

BOOL __stdcall CRadialDevice::Initialize(HWND hWnd,
	RADIAL_DEVICE_CALLBACK_FOCUS pCallbackFocus, 
		RADIAL_DEVICE_CALLBACK_LOST_FOCUS pCallbackLostFocus, 
		RADIAL_DEVICE_CALLBACK_ROT pCallbackRot, 
		RADIAL_DEVICE_CALLBACK_BUTTON_CLICKED pCallbackButtonClicked)
{
	m_pDeviceListener = new DeviceListener();
	m_pDeviceListener->Init(this,hWnd);

	m_pCallbackFoucus = pCallbackFocus;
	m_pCallbackLostFocus = pCallbackLostFocus;
	m_pCallbackRot = pCallbackRot;
	m_pCallbackButtonClicked = pCallbackButtonClicked;

	m_pMenuCallbackTable = new MENU_CALLBACK_ITEM[MAX_MENU_COUNT];

	return TRUE;
}
BOOL __stdcall CRadialDevice::AddMenuItem(const WCHAR* wchIconFileName, const WCHAR* wchMenuName,int MenuItemID)
{
	BOOL	bResult = FALSE;
	
	if (m_dwCurMenuCount >= MAX_MENU_COUNT)
		return FALSE;

	ComPtr<IRadialControllerMenuItem>	menuItem;

	MENU_CALLBACK_ITEM*	pCurItem = m_pMenuCallbackTable + m_dwCurMenuCount;
	if (S_OK == m_pDeviceListener->AddMenuItem(&menuItem, wchIconFileName, wchMenuName,&pCurItem->token))
	{
		pCurItem->menuItem = menuItem;
		pCurItem->ID = MenuItemID;
		m_dwCurMenuCount++;
		bResult = TRUE;
	}
	
	return bResult;
}
MENU_CALLBACK_ITEM* CRadialDevice::FindCallbackItem(ComPtr<IRadialControllerMenuItem> menuItem) const
{
	MENU_CALLBACK_ITEM*	pSelectedItem = nullptr;
	for (DWORD i = 0; i < m_dwCurMenuCount; i++)
	{
		if (m_pMenuCallbackTable[i].menuItem.Get() == menuItem.Get())
		{
			pSelectedItem = m_pMenuCallbackTable + i;
			break;
		}
	}
	return pSelectedItem;
}

void CRadialDevice::OnItemInvoked(ComPtr<IRadialControllerMenuItem> menuItem)
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
			m_pCallbackLostFocus(this, m_pCurSelectedItem->ID);
		}
	}
	m_pCallbackFoucus(this, pSelectedItem->ID);
	m_pCurSelectedItem = pSelectedItem;
}
void CRadialDevice::OnRotationChanged(ComPtr<IRadialControllerMenuItem> menuItem, double delta)
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
void CRadialDevice::OnButtonClicked(ComPtr<IRadialControllerMenuItem> menuItem)
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
