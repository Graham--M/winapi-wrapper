
// Include main WinAPI Wrapper header
#include <winapi.h>
// Include resource header
#include "resource.h"

// Include the WinAPI Wrapper namespace
using namespace WinAPI;

// Define main window class based on a modeless dialog class
class MainWindow: public ModelessDialog {
public:
	// Constructor creates the actual window from a dialog resource
	MainWindow( HWND hwndParent = 0 ) {
		Create( IDD_SAMPLE, hwndParent );
	}
	// During window destruction the message queue is stopped
	virtual void OnDestroy() {
		PostQuitMessage( 0 );
	}
	// If IDOK or IDCANCEL comes from the dialog - end it
	virtual void OnCommand( int nIdentifier, int nNotifyCode, HWND hwndControl ) {
		switch ( nIdentifier ) {

		case IDOK:
		case IDCANCEL:
			Destroy();

		}
	}
};

// The application object using the main window class
namespace {
	Application< MainWindow, ModelessDialogAccel< MainWindow > > app;
}

USE_LIB_WINMAIN
