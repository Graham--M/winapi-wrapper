
#include "winapi.h"

using namespace WinAPI;

string ComboBox::GetLBText( int nIndex ) const
{
	string s;
	const int len = SendMessage( CB_GETLBTEXTLEN, nIndex );
	if ( len == CB_ERR )
		return "";
	s.resize( len + 1 );
	int len2 = SendMessage( CB_GETLBTEXT, nIndex, reinterpret_cast<LPARAM>( &s[0] ) );
	if ( len2 < 0 ) len2 = 0;
	s.resize( len2 );
	return s;
}
