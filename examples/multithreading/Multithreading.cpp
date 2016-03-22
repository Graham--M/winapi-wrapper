
// Include main WinAPI Wrapper header
#include <winapi.h>
// Include resource header
#include "resource.h"

// Include the WinAPI Wrapper namespace
using namespace WinAPI;

// Define main window class based on a modeless dialog class and on a thread class
class MainWindow: public ModelessDialog, public Thread {
	ProgressBar progress;	// the progress bar control
	bool b_interrupt;		// task interrupt flag (user's Cancel button)
public:
	// Constructor
	MainWindow( HWND hwndParent = 0 ): b_interrupt( false ) {

		// Load common controls (for the progress bar)
		INITCOMMONCONTROLSEX icc = { sizeof(INITCOMMONCONTROLSEX), ICC_PROGRESS_CLASS };
		InitCommonControlsEx( &icc );

		// Create the thread in suspended state
		Thread::Create( CREATE_SUSPENDED );

		// Create the actual window from a dialog resource
		ModelessDialog::Create( IDD_SAMPLE, hwndParent );
	}
	// Initialize dialog
	virtual bool OnInitDialog( HWND hwndFocus, LPARAM lParam ) {

		// Save progress bar control's HWND
		progress.Assign( GetDlgItem( IDC_PROGRESS ) );

		// Run the created thread
		Resume();
		return true;
	}
	// During window destruction the message queue is stopped
	virtual void OnDestroy() {
		PostQuitMessage( 0 );
	}
	// Window exit command
	enum { ID_FINISHED = 102 };
	// Handle WM_COMMAND
	virtual void OnCommand( int nIdentifier, int nNotifyCode, HWND hwndControl ) {
		switch ( nIdentifier ) {

		// The task has been finished - close window
		case ID_FINISHED:
			Destroy();
			break;

		// The user cancelled the task - signal an interrupt
		case IDCANCEL:
			b_interrupt = true;
			break;

		}
	}
	// The actual thread
	virtual void Run() {
		progress.SetRange( 0, 100 );
		for ( int i=0; i < 100; i++ ) {

			// Simulate one step of a task (by Sleep()ing)
			Sleep( 50 );

			// Display progress
			progress.SetPos( i+1 );

			// Check if the user requested an interrupt
			if ( b_interrupt )
				break;
		}

		// Task finished - close the window
		PostMessage( WM_COMMAND, ID_FINISHED );
	}
};

// The application object using the main window class
namespace {
	Application< MainWindow, ModelessDialogAccel< MainWindow > > app;
}

USE_LIB_WINMAIN
