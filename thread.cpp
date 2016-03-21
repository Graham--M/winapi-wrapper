
#include "winapi.h"

using namespace WinAPI;

DWORD WINAPI Thread::TheProc( LPVOID lpParam )
{
	reinterpret_cast<Thread*>( lpParam )->Run();
	return 0;
}
