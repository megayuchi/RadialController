// DDraw.h: interface for the CDDraw class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_DDRAW_H__627A510B_7641_4053_B222_0C0951603602__INCLUDED_)
#define AFX_DDRAW_H__627A510B_7641_4053_B222_0C0951603602__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <ddraw.h>

class CDDraw  
{
	LPDIRECTDRAW4			m_lpDD4;
	LPDIRECTDRAWSURFACE4	m_lpDDPrimary;
	LPDIRECTDRAWSURFACE4	m_lpDDBack;
	
	DWORD					m_dwWidth;
	DWORD					m_dwHeight;
	RECT					m_rcWindow;
	HWND					m_hWnd;
	
	

public:
	BOOL				InitializeDDraw(HWND hWnd);
	BOOL				LockBackBuffer(char** ppBits,DWORD* pdwWidth,DWORD* pdwHeight,DWORD* pdwPitch);
	void				UnlockBackBuffer();
	
	HRESULT				GetDC(HDC* pOutDC);
	void				ReleaseDC(HDC hDC);
	void				Clear();
	void				OnDraw();
	void				Cleanup();
	CDDraw();
	~CDDraw();

};

#endif // !defined(AFX_DDRAW_H__627A510B_7641_4053_B222_0C0951603602__INCLUDED_)
