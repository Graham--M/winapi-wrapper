
#include "winapi.h"

using namespace WinAPI;

tstring Edit::GetLine( int nLine ) const
{
	tstring s;
	const int len = LineLength( LineIndex( nLine ) );
	if ( len < 1 ) return s;
	s.resize( len + 1 );
	*reinterpret_cast<unsigned short*>( &s[0] ) = static_cast<unsigned short>( len + 1 );
	const int len2 = SendMessage( EM_GETLINE, nLine, reinterpret_cast<LPARAM>( &s[0] ) );
	s.resize( len );
	return s;
}
