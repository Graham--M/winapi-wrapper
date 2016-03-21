
#include "winapi.h"

using namespace WinAPI;

string ListBox::GetText( int nIndex ) const
{
	string s;
	const int len = SendMessage( LB_GETTEXTLEN, nIndex );
	if ( len == LB_ERR )
		return "";
	s.resize( len + 1 );
	int len2 = SendMessage( LB_GETTEXT, nIndex, reinterpret_cast<LPARAM>( &s[0] ) );
	if ( len2 < 0 ) len2 = 0;
	s.resize( len2 );
	return s;
}
