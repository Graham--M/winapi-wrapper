
#include "winapi.h"

using namespace WinAPI;

string Menu::GetString( int id, bool b_command ) const
{
	const int MAX_CHARS = 256;
	string s( MAX_CHARS, '\0' );
	const int num_chars = GetMenuString( hMenu, id, &s[0], MAX_CHARS, b_command?MF_BYCOMMAND:MF_BYPOSITION );
	s.resize( num_chars );
	return s;
}
