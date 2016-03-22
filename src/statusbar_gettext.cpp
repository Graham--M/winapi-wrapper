
#include "winapi.h"

using namespace WinAPI;

tstring StatusBar::GetText( int nPart ) const
{
	tstring s;
	const int len = LOWORD( SendMessage( SB_GETTEXTLENGTH, nPart ) );
	s.resize( len + 1 );
	s[0] = s[len] = 0;
	SendMessage( SB_GETTEXT, nPart, reinterpret_cast<LPARAM>( &s[0] ) );
	s.resize( _tcslen( s.data() ) );
	return s;
}
