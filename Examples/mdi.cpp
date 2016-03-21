
// Include main WinAPI Wrapper header
#include <winapi.h>

// Include the WinAPI Wrapper namespace
using namespace WinAPI;

// Define MDI child window class
class DocumentWindow: public MDIChildWindow {
public:
	// Constructor creates the actual window
	DocumentWindow( HWND hwndParent ) {
		Create( "Document", hwndParent );
	}
	// Paint "Hello, World!" during WM_PAINT
	virtual void OnPaint() {
		PaintDC( *this ).TextOut( WPoint(50,50), "Hello, World!" );
	}
};

// Define main window class based on a MDI frame window class
class MainWindow: public MDIFrameWindow {
public:
	// Constructor creates the actual window (through CreateWindowEx)
	MainWindow() {
		Create( "Sample App" );
	}
	// Command that creates MDI children
	enum { ID_CREATE = 101 };
	// During window creation, create the client window and a sample child
	virtual bool OnCreate( LPCREATESTRUCT lpCreateStruct = 0 ) {
		// Create menu bar
		Menu menubar;
		menubar.Create();
		Menu submenu;
		submenu.Create();
		submenu.Append( "&Create", ID_CREATE );
		menubar.Append( "&Window", submenu );
		SetMenu( menubar );

		// Create client window and an MDI child
		CreateClient( GetClientRect(), submenu );
		new DocumentWindow( GetClientWindow() );
		return true;
	}
	// During window destruction the message queue is stopped
	virtual void OnDestroy() {
		PostQuitMessage( 0 );
	}
	// WM_COMMAND
	virtual void OnCommand( int nIdentifier, int nNotifyCode, HWND hwndControl ) {
		switch ( nIdentifier ) {

		// Create a new document window
		case ID_CREATE:
			new DocumentWindow( GetClientWindow() );
			break;

		// Default WM_COMMAND processing (for MDI commands)
		default:
			MDIFrameWindow::OnCommand( nIdentifier, nNotifyCode, hwndControl );

		}
	}
};

// The application object using the main window class
namespace {
	Application< MainWindow, MDIAccel< MainWindow > > app;
}

USE_LIB_WINMAIN
