
#include "winapi.h"

using namespace WinAPI;

tstring TabControl::GetItemText( int nItem ) const
{
	tstring s;
	const int len = 128;
	s.resize( len + 1 );
	s[0] = s[len] = 0;
	TCITEM item;
	item.mask = TCIF_TEXT;
	item.pszText = &s[0];
	item.cchTextMax = len;
	GetItem( nItem, &item );
	s.resize( _tcslen( s.data() ) );
	return s;
}
