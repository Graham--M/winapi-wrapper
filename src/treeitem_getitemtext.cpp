
#include "winapi.h"

using namespace WinAPI;

tstring TreeItem::GetItemText() const
{
	tstring s;
	const int len = 128;
	s.resize( len + 1 );
	s[0] = s[len] = 0;
	TVITEM item;
	item.mask = TVIF_HANDLE | TVIF_TEXT;
	item.hItem = hitem;
	item.pszText = &s[0];
	item.cchTextMax = len;
	GetItem( &item );
	s.resize( _tcslen( s.data() ) );
	return s;
}
