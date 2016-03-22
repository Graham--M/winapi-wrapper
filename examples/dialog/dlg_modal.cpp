
// Include main WinAPI Wrapper header
#include <winapi.h>
// Include resource header
#include "resource.h"

// Include the WinAPI Wrapper namespace
using namespace WinAPI;

// Define main window class based on a modal dialog class
class MainWindow: public Dialog {
public:
	// Constructor sets up dialog box template
	MainWindow(): Dialog( IDD_SAMPLE ) { }
	// If IDOK or IDCANCEL comes from the dialog - end it
	virtual void OnCommand( int nIdentifier, int nNotifyCode, HWND hwndControl ) {
		switch ( nIdentifier ) {

		case IDOK:
		case IDCANCEL:
			EndDialog( nIdentifier );

		}
	}
};

// Main window function that displays the modal dialog
int WINAPI WinMain( HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nShowCmd )
{
	MainWindow dlg;
	dlg.Go();
	return 0;
}
