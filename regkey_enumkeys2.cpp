
#include "winapi.h"

using namespace WinAPI;

int RegKey::EnumKeys( vector<string>* p_keys ) const
{
	p_keys->clear();
	DWORD num_sub_keys = 0;
	int retval = QueryInfo( &num_sub_keys );
	if ( retval != ERROR_SUCCESS ) return retval;
	p_keys->reserve( num_sub_keys );
	string name;
	for ( int i=num_sub_keys-1; i >= 0; i-- ) {
		retval = EnumKey( i, &name );
		if ( retval != ERROR_SUCCESS ) return retval;
		p_keys->push_back( name );
	}
	return ERROR_SUCCESS;
}
