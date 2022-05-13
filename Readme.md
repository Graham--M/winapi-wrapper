# WinAPI-Wrapper for C++
*Forked from [cdragan/WinAPI-Wrapper](https://github.com/cdragan/WinAPI-Wrapper) with some minor compatability fixes and a switch to the CMAKE build system.*

This is a C++ wrapper for WINAPI to take some of the clumsiness out of programming WIN32 applications. Most classes wrap the calls to the WINAPI functions so that they keep track of the handles for you and define some sensible defaults.

Almost all the documentation in the original repo help dir still applies and is recommended reading. However a major change is that the library default `WinMain` function must now be defined in your code for compatibility with non MSVC linkers. You can easily do this using the `USE_LIB_WINMAIN` macro. For example the `sdi.cpp` example app is as below:

```C++
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
```
This example assumes the `UNICODE` macro is NOT defined for simplicity's sake. The real example supports ASCII and UNICODE chars.

All modifications are made available under the original license of the library (see [License.txt](License.txt)).