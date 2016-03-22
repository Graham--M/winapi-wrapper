
#include "winapi.h"

using namespace WinAPI;

tstring ListView::GetItemText( int iItem, int iSubItem ) const
{
	tstring s;
	const int len = 128;
	s.resize( len + 1 );
	s[0] = s[len] = 0;
	ListView_GetItemText( hWnd, iItem, iSubItem, &s[0], len + 1 );
	s.resize( _tcslen( s.data() ) );
	return s;
}
