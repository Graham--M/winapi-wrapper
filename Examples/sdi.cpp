
// Include main WinAPI Wrapper header
#include <winapi.h>

// Include the WinAPI Wrapper namespace
using namespace WinAPI;

// Define main window class based on a regular window class
class MainWindow: public Window {
public:
	// Constructor creates the actual window (through CreateWindowEx)
	MainWindow() {
		Create( "Sample App" );
	}
	// During window destruction the message queue is stopped
	virtual void OnDestroy() {
		PostQuitMessage( 0 );
	}
	// Paint "Hello, World!" during WM_PAINT
	virtual void OnPaint() {
		PaintDC( *this ).TextOut( WPoint(50,50), "Hello, World!" );
	}
};

// The application object using the main window class
namespace {
	Application< MainWindow > app;
}

USE_LIB_WINMAIN
