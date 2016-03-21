
#include "winapi.h"

using namespace WinAPI;

string ComboBoxEx::GetItemText( int nItem ) const
{
	string s;
	const int len = 128;
	s.resize( len + 1 );
	s[0] = s[len] = 0;
	COMBOBOXEXITEM item;
	item.mask = CBEIF_TEXT;
	item.iItem = nItem;
	item.pszText = &s[0];
	item.cchTextMax = len;
	GetItem( &item );
	s.resize( strlen( s.data() ) );
	return s;
}
