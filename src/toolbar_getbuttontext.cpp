
#include "winapi.h"

using namespace WinAPI;

tstring Toolbar::GetButtonText( int idCommand ) const
{
	tstring s;
	const int len = 128;
	s.resize( len + 1 );
	s[0] = s[len] = 0;
	const int new_len = SendMessage( TB_GETBUTTONTEXT, idCommand, reinterpret_cast<LPARAM>( &s[0] ) );
	s.resize( new_len );
	return s;
}
