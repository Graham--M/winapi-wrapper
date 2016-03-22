
#include "winapi.h"

using namespace WinAPI;

tstring ComboBoxEx::GetItemText( int nItem ) const
{
	tstring s;
	const int len = 128;
	s.resize( len + 1 );
	s[0] = s[len] = 0;
	COMBOBOXEXITEM item;
	item.mask = CBEIF_TEXT;
	item.iItem = nItem;
	item.pszText = &s[0];
	item.cchTextMax = len;
	GetItem( &item );
	s.resize( _tcslen( s.data() ) );
	return s;
}
