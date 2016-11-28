// Client.cpp : Defines the entry point for the application.
//

#include "stdafx.h"
#include "Resource.h"
#include <roapi.h>
#include "DDraw.h"
#include "../Megayuchi_Common/IRadialDevice.h"
#include "../Megayuchi_UTIL/WriteDebugString.h"

#define MAX_LOADSTRING 100

// Global Variables:
HINSTANCE hInst;                                // current instance
WCHAR szTitle[MAX_LOADSTRING];                  // The title bar text
WCHAR szWindowClass[MAX_LOADSTRING];            // the main window class name

// Forward declarations of functions included in this code module:
ATOM                MyRegisterClass(HINSTANCE hInstance);
BOOL                InitInstance(HINSTANCE, int);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    About(HWND, UINT, WPARAM, LPARAM);

#pragma comment(lib, "onecore.lib")
#pragma comment(lib, "ddraw.lib")
#pragma comment(lib, "dxguid.lib")
#pragma comment(linker, "/entry:wWinMainCRTStartup /subsystem:console" )

HRESULT typedef (__stdcall *CREATE_INSTANCE_FUNC)(void* ppv);
CDDraw*	g_pDDraw = nullptr;
HANDLE	g_hConsoleOut = nullptr;
IRadialDevice*	g_pRadialDevice = nullptr;
HWND g_hMainWindow = nullptr;

enum CUMSTOM_MENU_ID
{
	CUSTOM_MENU_ID_0 = 0,
	CUSTOM_MENU_ID_1 = 1
};

void OnWindowSize();
void OnDraw();
void WriteText(WCHAR* wchTxt, DWORD dwLen, DWORD Y);
BOOL Initialize();
void Cleanup();


void __stdcall RadialDeviceCallbacFocus(IRadialDevice* pRadialDevice, int MenuItemID);
void __stdcall RadialDeviceCallbacLostFocus(IRadialDevice* pRadialDevice, int MenuItemID);
void __stdcall RadialDeviceCallbackRot(IRadialDevice* pRadialDevice, int MenuItemID, double delta);
void __stdcall RadialDeviceCallbackButtonClicked(IRadialDevice* pRadialDevice, int MenuItemID);

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
	_In_opt_ HINSTANCE hPrevInstance,
	_In_ LPWSTR    lpCmdLine,
	_In_ int       nCmdShow)
{
	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);

	// TODO: Place code here.

	// Initialize global strings
	LoadStringW(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
	LoadStringW(hInstance, IDC_CLIENT, szWindowClass, MAX_LOADSTRING);
	MyRegisterClass(hInstance);

	// Perform application initialization:
	if (!InitInstance(hInstance, nCmdShow))
	{
		return FALSE;
	}

	HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_CLIENT));

	Initialize();

	BOOL	bResult = FALSE;

	MSG msg;

	// Main message loop:
	while (GetMessage(&msg, nullptr, 0, 0))
	{
		if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}
	Cleanup();

	return (int)msg.wParam;
}
BOOL Initialize()
{

	const WCHAR* wchRadialDeivceFileName = nullptr;
#ifdef WIN64
#ifdef _DEBUG
	wchRadialDeivceFileName = L"../dll/RadialDevice_x64_debug.dll";
#else
	wchRadialDeivceFileName = L"../dll/RadialDevice_x64_release.dll";
#endif
#else
#ifdef _DEBUG
	wchRadialDeivceFileName = L"../dll/RadialDevice_x86_debug.dll";
#else
	wchRadialDeivceFileName = L"../dll/RadialDevice_x86_release.dll";
#endif

#endif
	RoInitialize(RO_INIT_MULTITHREADED);

	g_hConsoleOut = GetStdHandle(STD_OUTPUT_HANDLE);

	g_pDDraw = new CDDraw;
	g_pDDraw->InitializeDDraw(g_hMainWindow);



	HMODULE	hRadialDeviceDLL = LoadLibrary(wchRadialDeivceFileName);
	if (!hRadialDeviceDLL)
	{
		DWORD dwErrCode = GetLastError();
		WriteDebugStringW(DEBUG_OUTPUT_TYPE_DEBUG_CONSOLE, L"Fail to LoadLibrary(%s) - Error Code: %u", wchRadialDeivceFileName, dwErrCode);
		__debugbreak();
	}
	CREATE_INSTANCE_FUNC pFuncCreateRaidalDevice = (CREATE_INSTANCE_FUNC)GetProcAddress(hRadialDeviceDLL, "DllCreateInstance");
	pFuncCreateRaidalDevice(&g_pRadialDevice);
	if (g_pRadialDevice->Initialize(g_hMainWindow, RadialDeviceCallbacFocus, RadialDeviceCallbacLostFocus, RadialDeviceCallbackRot, RadialDeviceCallbackButtonClicked))
	{
		wprintf_s(L"Successfully initialized.\n");
		if (g_pRadialDevice->AddMenuItem(L"icon_custom0.png", L"C0", CUSTOM_MENU_ID_0))
		{
			wprintf_s(L"Custom Menu 0 Added.\n");
		}
		if (g_pRadialDevice->AddMenuItem(L"icon_custom1.png", L"C1", CUSTOM_MENU_ID_1))
		{
			wprintf_s(L"Custom Menu 1 Added.\n");
		}
		wprintf_s(L"Press and Hold Surface Dial.\n");
	}

	OnDraw();

	return TRUE;
}
void Cleanup()
{
	if (g_pRadialDevice)
	{
		g_pRadialDevice->Release();
		g_pRadialDevice = nullptr;
	}
	if (g_pDDraw)
	{
		delete g_pDDraw;
		g_pDDraw = NULL;
	}
	RoUninitialize();
}
void OnWindowSize()
{
	if (g_pDDraw)
	{
		delete g_pDDraw;
		g_pDDraw = new CDDraw;

		g_pDDraw->InitializeDDraw(g_hMainWindow);
		g_pDDraw->Clear();

		OnDraw();
	}
}

//
//  FUNCTION: MyRegisterClass()
//
//  PURPOSE: Registers the window class.
//
ATOM MyRegisterClass(HINSTANCE hInstance)
{
	WNDCLASSEXW wcex;

	wcex.cbSize = sizeof(WNDCLASSEX);

	wcex.style = CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc = WndProc;
	wcex.cbClsExtra = 0;
	wcex.cbWndExtra = 0;
	wcex.hInstance = hInstance;
	wcex.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_CLIENT));
	wcex.hCursor = LoadCursor(nullptr, IDC_ARROW);
	wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
	wcex.lpszMenuName = MAKEINTRESOURCEW(IDC_CLIENT);
	wcex.lpszClassName = szWindowClass;
	wcex.hIconSm = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

	return RegisterClassExW(&wcex);
}

//
//   FUNCTION: InitInstance(HINSTANCE, int)
//
//   PURPOSE: Saves instance handle and creates main window
//
//   COMMENTS:
//
//        In this function, we save the instance handle in a global variable and
//        create and display the main program window.
//
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
	hInst = hInstance; // Store instance handle in our global variable

	HWND hWnd = g_hMainWindow = CreateWindowW(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, nullptr, nullptr, hInstance, nullptr);

	if (!hWnd)
	{
		return FALSE;
	}

	ShowWindow(hWnd, nCmdShow);
	UpdateWindow(hWnd);

	return TRUE;
}

//
//  FUNCTION: WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  PURPOSE:  Processes messages for the main window.
//
//  WM_COMMAND  - process the application menu
//  WM_PAINT    - Paint the main window
//  WM_DESTROY  - post a quit message and return
//
//
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
	case WM_COMMAND:
	{
		int wmId = LOWORD(wParam);
		// Parse the menu selections:
		switch (wmId)
		{
		case IDM_ABOUT:
			DialogBox(hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, About);
			break;
		case IDM_EXIT:
			DestroyWindow(hWnd);
			break;
		default:
			return DefWindowProc(hWnd, message, wParam, lParam);
		}
	}
	break;
	case WM_PAINT:
	{
		PAINTSTRUCT ps;
		HDC hdc = BeginPaint(hWnd, &ps);
		// TODO: Add any drawing code here...
		if (g_pDDraw)
		{
			OnDraw();
		}
		EndPaint(hWnd, &ps);
		break;
	}
	case WM_SIZE:
	{
		OnWindowSize();
		break;
	}
	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;
}

// Message handler for about box.
INT_PTR CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	UNREFERENCED_PARAMETER(lParam);
	switch (message)
	{
	case WM_INITDIALOG:
		return (INT_PTR)TRUE;

	case WM_COMMAND:
		if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
		{
			EndDialog(hDlg, LOWORD(wParam));
			return (INT_PTR)TRUE;
		}
		break;
	}
	return (INT_PTR)FALSE;
}



void __stdcall RadialDeviceCallbacFocus(IRadialDevice* pRadialDevice, int MenuItemID)
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
	wprintf_s(L"OnFocused %s\n", wchMenuID);
	OnDraw();
}
void __stdcall RadialDeviceCallbacLostFocus(IRadialDevice* pRadialDevice, int MenuItemID)
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
	wprintf_s(L"OnLostFocus %s\n", wchMenuID);
	OnDraw();
}

void __stdcall RadialDeviceCallbackRot(IRadialDevice* pRadialDevice, int MenuItemID, double delta)
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
	wprintf_s(L"OnRotation %s - %f\n", wchMenuID, (float)delta);
	OnDraw();
}
void __stdcall RadialDeviceCallbackButtonClicked(IRadialDevice* pRadialDevice, int MenuItemID)
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
	wprintf_s(L"OnButtonClicked %s\n", wchMenuID);
	OnDraw();
}


void OnDraw()
{
	const int MAX_WIDTH = 120;
	const int MAX_HEIGHT = 40;

	size_t size = sizeof(CHAR_INFO);
	CHAR_INFO	chrInfo[MAX_WIDTH] = {};

	g_pDDraw->Clear();


	CONSOLE_SCREEN_BUFFER_INFO info = {};
	GetConsoleScreenBufferInfo(g_hConsoleOut, &info);
	int height = info.srWindow.Bottom - info.srWindow.Top;
	int width = info.srWindow.Right - info.srWindow.Left;

	width = min(width, MAX_WIDTH);
	height = min(height, MAX_HEIGHT);

	COORD		OutSize, OutPos;
	OutSize.X = width;
	OutSize.Y = 1;
	OutPos.X = 0;
	OutPos.Y = 0;

	int read_pos_bottom = info.dwCursorPosition.Y;
	int read_pos_top = read_pos_bottom - height;
	if (read_pos_top < 0)
		read_pos_top = 0;

	int read_line_count = read_pos_bottom - read_pos_top;

	for (int y = 0; y < read_line_count; y++)
	{
		SMALL_RECT	rect;
		rect.Left = 0;
		rect.Right = width;
		rect.Top = read_pos_top + y;
		rect.Bottom = rect.Top + 1;

		if (ReadConsoleOutput(g_hConsoleOut, chrInfo, OutSize, OutPos, &rect))
		{
			CHAR_INFO*	pChrEntry = chrInfo;
			WCHAR	wchTxt[MAX_WIDTH] = {};

			DWORD	dwLen = 0;
			int width_per_line = rect.Right - rect.Left;
			for (int i = 0; i < width_per_line; i++)
			{
				wchTxt[dwLen] = pChrEntry->Char.UnicodeChar;
				dwLen++;
				pChrEntry++;
			}
			wchTxt[dwLen] = 0;
			WriteText(wchTxt, dwLen, y);
		}
		else
		{
			int a = 0;
		}
	}
	g_pDDraw->OnDraw();

}
void WriteText(WCHAR* wchTxt, DWORD dwLen, DWORD Y)
{
	HDC	hDC = nullptr;

	HRESULT hr = g_pDDraw->GetDC(&hDC);
	if (S_OK != hr)
		return;

	SetBkMode(hDC, TRANSPARENT);

	RECT	textRect, texRectSide[4];
	textRect.left = 16;
	textRect.top = 16 + Y * 16;
	textRect.right = textRect.left + dwLen * 5;
	textRect.bottom = textRect.top + 16;



	texRectSide[0].left = textRect.left - 1;
	texRectSide[0].top = textRect.top - 1;
	texRectSide[0].right = textRect.right - 1;
	texRectSide[0].bottom = textRect.bottom - 1;


	texRectSide[1].left = textRect.left + 1;
	texRectSide[1].top = textRect.top - 1;
	texRectSide[1].right = textRect.right + 1;
	texRectSide[1].bottom = textRect.bottom - 1;

	texRectSide[2].left = textRect.left + 1;
	texRectSide[2].top = textRect.top + 1;
	texRectSide[2].right = textRect.right + 1;
	texRectSide[2].bottom = textRect.bottom + 1;

	texRectSide[3].left = textRect.left - 1;
	texRectSide[3].top = textRect.top + 1;
	texRectSide[3].right = textRect.right - 1;
	texRectSide[3].bottom = textRect.bottom + 1;

	SetTextColor(hDC, 0x00000000);
	for (DWORD i = 0; i < 4; i++)
	{
		DrawText(hDC, wchTxt, -1, &texRectSide[i], DT_LEFT | DT_WORDBREAK);
	}

	SetTextColor(hDC, 0x0000ff00);
	DrawText(hDC, wchTxt, -1, &textRect, DT_LEFT | DT_WORDBREAK);

	g_pDDraw->ReleaseDC(hDC);
}