
#include "winapi.h"

using namespace WinAPI;

string ListView::GetItemText( int iItem, int iSubItem ) const
{
	string s;
	const int len = 128;
	s.resize( len + 1 );
	s[0] = s[len] = 0;
	ListView_GetItemText( hWnd, iItem, iSubItem, &s[0], len + 1 );
	s.resize( strlen( s.data() ) );
	return s;
}
