
#include "winapi.h"

using namespace WinAPI;

WindowClass::WindowClass( WNDPROC wndProc, UINT style, int cbClsExtra, int cbWndExtra, HICON hIcon, HCURSOR hCursor, HBRUSH hbrBackground, LPCTSTR lpszMenuName, HINSTANCE hInstance )
{
	Register( wndProc, style, cbClsExtra, cbWndExtra, hIcon, hCursor, hbrBackground, lpszMenuName, hInstance );
}
