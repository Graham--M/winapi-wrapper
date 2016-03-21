
#include "winapi.h"
#include "passlparam.h"

using namespace WinAPI;

BOOL CALLBACK WinAPIDefaultDlgProc( HWND hwndDlg, UINT uMsg, WPARAM wParam, LPARAM lParam );

void ModelessDialog::Create( int _lpTemplate, HWND hWndParent, LPARAM lParam, HINSTANCE hInstance )
{
	lpTemplate = MAKEINTRESOURCE( _lpTemplate );
	PASSLPARAM pass = { reinterpret_cast<LPVOID>( lParam ), this, &hWnd };
	CreateDialogParam( hInstance, lpTemplate, hWndParent, WinAPIDefaultDlgProc, reinterpret_cast<LPARAM>( &pass ) );
	b_created = true;
}
