// DDraw.cpp: implementation of the CDDraw class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "DDraw.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CDDraw::CDDraw()
{
	memset(this,0,sizeof(CDDraw));

}

#include <ddraw.h>


BOOL CDDraw::InitializeDDraw(HWND hWnd)
{
	m_hWnd = hWnd;
	BOOL	bResult = FALSE;
	LPDIRECTDRAW			lpDD;

	if (DD_OK != DirectDrawCreate(NULL,&lpDD, NULL))
	{
        MessageBox(hWnd,L"Fail to Create DirectDraw",L"Error",MB_OK);
		goto lb_return;
	}
	
	if (DD_OK != lpDD->QueryInterface(IID_IDirectDraw4, (LPVOID*)&m_lpDD4))
	{
		MessageBox(hWnd,L"Fail to Create DirectDraw 4",L"Error",MB_OK);
		goto lb_return;
	}



	HRESULT hr;
	hr = m_lpDD4->SetCooperativeLevel( hWnd, DDSCL_NORMAL );
	
	if (FAILED(hr))
	{
		MessageBox(NULL,L"Failed to Set CooperativeLevel",L"ERROR",MB_OK);
		goto lb_return;
	}

	DDSURFACEDESC2 ddsd;
	ZeroMemory( &ddsd, sizeof(DDSURFACEDESC2) );
	ddsd.dwSize = sizeof(DDSURFACEDESC2);

	ddsd.dwFlags        = DDSD_CAPS;
	ddsd.ddsCaps.dwCaps = DDSCAPS_PRIMARYSURFACE;

	// Create the primary surface.
	hr = m_lpDD4->CreateSurface( &ddsd, &m_lpDDPrimary, NULL );
	if (FAILED(hr))
	{
		MessageBox(NULL,L"Failed to CreateSurface",L"ERROR",MB_OK);
		goto lb_return;
	}
	// Create a clipper object which handles all our clipping for cases when
	// our window is partially obscured by other windows. This is not needed
	// for apps running in fullscreen mode.
	LPDIRECTDRAWCLIPPER pcClipper;
	hr = lpDD->CreateClipper( 0, &pcClipper, NULL );
	if (FAILED(hr))
	{
		MessageBox(NULL,L"Failed to Create Clipper",L"ERROR",MB_OK);
		goto lb_return;
	}
	// Associate the clipper with our window. Note that, afterwards, the
	// clipper is internally referenced by the primary surface, so it is safe
	// to release our local reference to it.
	pcClipper->SetHWnd( 0, hWnd );
	m_lpDDPrimary->SetClipper( pcClipper );
	pcClipper->Release();

	ddsd.dwFlags        = DDSD_WIDTH | DDSD_HEIGHT | DDSD_CAPS;
	ddsd.ddsCaps.dwCaps = DDSCAPS_OFFSCREENPLAIN |DDSCAPS_SYSTEMMEMORY; 




	GetClientRect(hWnd,&m_rcWindow);
	m_dwWidth = m_rcWindow.right - m_rcWindow.left;
	m_dwHeight = m_rcWindow.bottom - m_rcWindow.top;
	ddsd.dwWidth  = m_dwWidth;
	ddsd.dwHeight = m_dwHeight;



	hr = m_lpDD4->CreateSurface( &ddsd, &m_lpDDBack, NULL );
	if (FAILED(hr))
	{
		MessageBox(NULL,L"Failed to Create Surface",L"ERROR",MB_OK);
		goto lb_return;
	}
	
	DDSURFACEDESC2 ddsc;
	memset(&ddsc,0,sizeof(DDSURFACEDESC2));
	ddsc.dwSize = sizeof(DDSURFACEDESC2);

	Clear();

	lpDD->Release();

	bResult = TRUE;

lb_return:
	return bResult;

}

HRESULT CDDraw::GetDC(HDC* pOutDC)
{
	HRESULT hr = m_lpDDBack->GetDC(pOutDC);
	return hr;
}
void CDDraw::ReleaseDC(HDC hDC)
{
	m_lpDDBack->ReleaseDC(hDC);
}
void CDDraw::Clear()
{
	if (m_lpDDBack)
	{
		char*	p = NULL;
		DWORD	dwPitch;
		DWORD	dwWidth,dwHeight;
		LockBackBuffer(&p,&dwWidth,&dwHeight,&dwPitch);
		for (DWORD y=0; y<dwHeight; y++)
		{
			memset(p + y*dwPitch,0,4*dwWidth);
		}
		UnlockBackBuffer();
	}
}

BOOL CDDraw::LockBackBuffer(char** ppBits,DWORD* pdwWidth,DWORD* pdwHeight,DWORD* pdwPitch)
{
	BOOL	bResult = FALSE;

	if (m_lpDDBack)
	{

		DDSURFACEDESC2 ddsc;
		memset(&ddsc,0,sizeof(DDSURFACEDESC2));
		ddsc.dwSize = sizeof(DDSURFACEDESC2);

		m_lpDDBack->Lock( NULL,&ddsc, DDLOCK_WAIT,  NULL);

		*ppBits = (char*)ddsc.lpSurface;
		*pdwPitch = ddsc.lPitch;
		*pdwWidth = ddsc.dwWidth;
		*pdwHeight = ddsc.dwHeight;
	}

	bResult = TRUE;
lb_return:
	return bResult;
}
void CDDraw::UnlockBackBuffer()
{
	if (m_lpDDBack)
		m_lpDDBack->Unlock(NULL);
}
void CDDraw::OnDraw()
{
	if (m_lpDDPrimary)
	{
		GetClientRect(m_hWnd,&m_rcWindow);
		::ClientToScreen(m_hWnd, (POINT*)&m_rcWindow.left );
	    ::ClientToScreen(m_hWnd, (POINT*)&m_rcWindow.right );

		m_lpDDPrimary->Blt(&m_rcWindow,m_lpDDBack,NULL,DDBLT_WAIT, NULL);
	}
}
void CDDraw::Cleanup()
{
	if (m_lpDDBack)
	{
		m_lpDDBack->Release();
		m_lpDDBack = NULL;
	}
	if (m_lpDDPrimary)
	{
		m_lpDDPrimary = NULL;
	}
	if (m_lpDD4)
	{
		m_lpDD4->Release();
		m_lpDD4 = NULL;
	}
}
int main()
{
//	LoadTga("test.tga");
	return 0;
}

CDDraw::~CDDraw()
{
	Cleanup();
}
