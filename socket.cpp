
#include "winapi.h"

using namespace WinAPI;

namespace {

	class WindowsSockets {
		bool b_init_ok;

		WindowsSockets(): b_init_ok( false ) {
			WSADATA wsa;
			b_init_ok = WSAStartup( 0x0202, &wsa ) == 0;
		}
	public:
		~WindowsSockets() { WSACleanup(); } // made public for VS6 to compile
		static bool IsInitialized() {
			static WindowsSockets sockets;
			return sockets.b_init_ok;
		}
	};

}

Socket::Socket( bool b_udp )
: socket( empty )
{
	const int type = b_udp ? SOCK_DGRAM : SOCK_STREAM;

	if ( WindowsSockets::IsInitialized() )
		socket = static_cast<int>( ::socket( AF_INET, type, 0 ) );
	if ( socket >= 0 ) {
		int bdcast = 1;
		setsockopt( socket, SOL_SOCKET, SO_BROADCAST, reinterpret_cast<char*>( &bdcast ), sizeof( bdcast ) );
	}
}

Socket::Socket( int type, int protocol )
: socket( empty )
{
	if ( WindowsSockets::IsInitialized() )
		socket = static_cast<int>( ::socket( AF_INET, type, protocol ) );
	if ( socket >= 0 ) {
		int bdcast = 1;
		setsockopt( socket, SOL_SOCKET, SO_BROADCAST, reinterpret_cast<char*>( &bdcast ), sizeof( bdcast ) );
	}
}
