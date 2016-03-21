
#include "winapi.h"
#include "passlparam.h"

using namespace WinAPI;

BOOL CALLBACK WinAPIDefaultDlgProc( HWND hwndDlg, UINT uMsg, WPARAM wParam, LPARAM lParam );

void ModelessDialog::Create( LPCDLGTEMPLATE hDialogTemplate, HWND hWndParent, LPARAM lParam, HINSTANCE hInstance )
{
	lpTemplate = 0;
	PASSLPARAM pass = { reinterpret_cast<LPVOID>( lParam ), this, &hWnd };
	CreateDialogIndirectParam( hInstance, hDialogTemplate, hWndParent, WinAPIDefaultDlgProc, reinterpret_cast<LPARAM>( &pass ) );
	b_created = true;
}
