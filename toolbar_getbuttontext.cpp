
#include "winapi.h"

using namespace WinAPI;

string Toolbar::GetButtonText( int idCommand ) const
{
	string s;
	const int len = 128;
	s.resize( len + 1 );
	s[0] = s[len] = 0;
	const int new_len = SendMessage( TB_GETBUTTONTEXT, idCommand, reinterpret_cast<LPARAM>( &s[0] ) );
	s.resize( new_len );
	return s;
}
