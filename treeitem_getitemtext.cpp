
#include "winapi.h"

using namespace WinAPI;

string TreeItem::GetItemText() const
{
	string s;
	const int len = 128;
	s.resize( len + 1 );
	s[0] = s[len] = 0;
	TVITEM item;
	item.mask = TVIF_HANDLE | TVIF_TEXT;
	item.hItem = hitem;
	item.pszText = &s[0];
	item.cchTextMax = len;
	GetItem( &item );
	s.resize( strlen( s.data() ) );
	return s;
}
