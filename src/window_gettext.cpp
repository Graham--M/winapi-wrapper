
#include "winapi.h"

using namespace WinAPI;

tstring GenericWindow::GetText() const
{
	tstring s;
	const int len = GetWindowTextLength( hWnd );
	if ( len > 0 ) {
		s.resize( len + 1 );
		GetWindowText( hWnd, &s[0], len + 1 );
		s.resize( len );
	}
	return s;
}
