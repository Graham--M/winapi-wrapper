
#include "winapi.h"

using namespace WinAPI;

bool Dialog::EndDialog( int nResult )
{
	if ( ! bModal ) return false;
	HWND hwndEnd = hWnd;
	hWnd = NULL;
	SetWindowLong( hwndEnd, GWL_USERDATA, 0 );
	return ::EndDialog( hwndEnd, nResult ) != FALSE;
}
