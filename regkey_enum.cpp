
#include "winapi.h"

using namespace WinAPI;

int RegKey::EnumKey( DWORD dwIndex, string* p_name, PFILETIME lpftLastWriteTime ) const
{
	p_name->resize( 256 );
	DWORD count = 256;
	const int retval = RegEnumKeyEx( hKey, dwIndex, &(*p_name)[0], &count, 0, 0, 0, lpftLastWriteTime );
	p_name->resize( count );
	return retval;
}
