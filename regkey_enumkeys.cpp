
#include "winapi.h"

using namespace WinAPI;

int RegKey::EnumKeys( ENUMKEYS* p_keys ) const
{
	p_keys->clear();
	DWORD num_sub_keys = 0;
	int retval = QueryInfo( &num_sub_keys );
	if ( retval != ERROR_SUCCESS ) return retval;
	p_keys->reserve( num_sub_keys );
	for ( int i=num_sub_keys-1; i >= 0; i-- ) {
		ENUMKEY ek;
		ek.name.resize( 256 );
		ek.key_class.resize( 256 );
		DWORD name_len = 256;
		DWORD key_class_len = 256;
		retval = RegEnumKeyEx( hKey, i, &ek.name[0], &name_len, 0, &ek.key_class[0], &key_class_len, &ek.last_file_time );
		if ( retval != ERROR_SUCCESS ) return retval;
		p_keys->push_back( ek );
	}
	return ERROR_SUCCESS;
}
