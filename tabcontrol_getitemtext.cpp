
#include "winapi.h"

using namespace WinAPI;

string TabControl::GetItemText( int nItem ) const
{
	string s;
	const int len = 128;
	s.resize( len + 1 );
	s[0] = s[len] = 0;
	TCITEM item;
	item.mask = TCIF_TEXT;
	item.pszText = &s[0];
	item.cchTextMax = len;
	GetItem( nItem, &item );
	s.resize( strlen( s.data() ) );
	return s;
}
