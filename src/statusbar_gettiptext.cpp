
#include "winapi.h"

using namespace WinAPI;

tstring StatusBar::GetTipText( int nPart ) const
{
	tstring s;
	const int len = 128;
	s.resize( len + 1 );
	s[0] = s[len] = 0;
	SendMessage( SB_GETTIPTEXT, MAKEWPARAM( nPart, len ), reinterpret_cast<LPARAM>( &s[0] ) );
	s.resize( _tcslen( s.data() ) );
	return s;
}
