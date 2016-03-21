
#include "winapi.h"

using namespace WinAPI;

void WindowClass::Register( WNDPROC wndProc, UINT style, int cbClsExtra, int cbWndExtra, HICON hIcon, HCURSOR hCursor, HBRUSH hbrBackground, LPCTSTR lpszMenuName, HINSTANCE hInstance )
{
	static int cls_index = 0;
	cls_index++;
	wsprintf( szClassName, "Class%08X", cls_index );
	WNDCLASS wc = {
		style,
		wndProc,
		cbClsExtra,
		cbWndExtra,
		hInstance,
		hIcon,
		hCursor,
		hbrBackground,
		lpszMenuName,
		szClassName
	};
	if ( RegisterClass( &wc ) == 0 )
		szClassName[0] = 0;
}
