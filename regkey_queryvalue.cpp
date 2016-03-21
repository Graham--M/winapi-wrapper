
#include "winapi.h"

using namespace WinAPI;

int RegKey::QueryValue( LPCTSTR lpValueName, string* p_string, LPDWORD lpType ) const
{
	*p_string = "";
	DWORD data_size = 0;
	const int retval = QueryValueSize( lpValueName, &data_size, lpType );
	if ( retval != ERROR_SUCCESS ) return retval;
	p_string->resize( data_size );
	const int retval2 = QueryValue( lpValueName, &(*p_string)[0], &data_size );
	if ( retval2 == ERROR_SUCCESS ) p_string->resize( p_string->length() - 1 );
	return retval2;
}
