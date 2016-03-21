
#include "winapi.h"
#include "passlparam.h"

using namespace WinAPI;

BOOL CALLBACK WinAPIDefaultDlgProc( HWND hwndDlg, UINT uMsg, WPARAM wParam, LPARAM lParam );

int Dialog::Go( HWND hwndParent, LPARAM lParam, HINSTANCE hInstance )
{
	if ( ! bModal ) return -1;
	PASSLPARAM pass = { reinterpret_cast<LPVOID>( lParam ), this, &hWnd };
	return static_cast<int>( DialogBoxParam( hInstance, lpTemplate, hwndParent, WinAPIDefaultDlgProc, reinterpret_cast<LPARAM>( &pass ) ) );
}
