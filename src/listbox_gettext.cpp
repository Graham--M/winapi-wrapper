
#include "winapi.h"

using namespace WinAPI;

tstring ListBox::GetText( int nIndex ) const
{
	tstring s;
	const int len = SendMessage( LB_GETTEXTLEN, nIndex );
	if ( len == LB_ERR )
		return _T("");
	s.resize( len + 1 );
	int len2 = SendMessage( LB_GETTEXT, nIndex, reinterpret_cast<LPARAM>( &s[0] ) );
	if ( len2 < 0 ) len2 = 0;
	s.resize( len2 );
	return s;
}
