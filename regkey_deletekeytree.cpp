
#include "winapi.h"

using namespace WinAPI;

int RegKey::DeleteKeyTree( LPCTSTR lpSubKey )
{
	RegKey subkey;
	const int retval = subkey.Open( *this, lpSubKey );
	if ( retval != ERROR_SUCCESS )
		return retval;
	char name[ 256 ];
	for ( int i=0; ; i++ ) {
		DWORD name_size = 256;
		if ( RegEnumKeyEx( subkey, i, name, &name_size, 0, 0, 0, 0 ) != ERROR_SUCCESS )
			break;
		const int retval = subkey.DeleteKeyTree( name );
		if ( retval != ERROR_SUCCESS )
			return retval;
	}
	subkey.Close();
	return Delete( lpSubKey );
}
