
#include "winapi.h"

#ifndef MINGW
#include <crtdbg.h>
#endif

namespace WinAPI {

//
// Default module handle. The old code used a hardcoded value assuming it
// wouldn't change for the main executable but unfortunatly it eventually
// did.
//
HINSTANCE g_hInstance = GetModuleHandle(NULL);

//
// Defining WinMain here complicates builds with g++ because unresolved symbols
// are only searched for in libraries linked after where they appear. Because
// the CRT is supposed to call WinMain, it can't see a symbol defined here.
//
// Instead, use the macro - USE_LIB_WINMAIN - to define WinMain in your exe
// code if you don't want to define your own. You had to write: 
//   Application< MainWindow > app;
// anyway so this isn't a major change.
//
int WINAPI Lib_WinMain( HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nShowCmd )
{
#ifndef MINGW
	_CrtSetDbgFlag( _CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF );
#endif
	ApplicationBase* p_application = ApplicationBase::p_application;
	if ( p_application != 0 )
		p_application->Run();
	return 0;
}

} // namespace WinAPI
