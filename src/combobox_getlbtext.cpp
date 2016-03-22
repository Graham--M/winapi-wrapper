
#include "winapi.h"

using namespace WinAPI;

tstring ComboBox::GetLBText( int nIndex ) const
{
	tstring s;
	const int len = SendMessage( CB_GETLBTEXTLEN, nIndex );
	if ( len == CB_ERR )
		return _T("");
	s.resize( len + 1 );
	int len2 = SendMessage( CB_GETLBTEXT, nIndex, reinterpret_cast<LPARAM>( &s[0] ) );
	if ( len2 < 0 ) len2 = 0;
	s.resize( len2 );
	return s;
}
