
#include "winapi.h"

using namespace WinAPI;

MDIFrameWindow::~MDIFrameWindow()
{
}

bool MDIFrameWindow::CreateClient( const RECT& rc, HMENU hMenu, int idFirstChild, DWORD dwStyle, DWORD dwExStyle )
{
	if ( ! client_wnd.IsCreated() ) {
		CLIENTCREATESTRUCT ccs = { hMenu, idFirstChild };
		return client_wnd.Create( "", "MDICLIENT", dwStyle, rc, *this, 0, dwExStyle, &ccs );
	} else
		return true;
}

int MDIFrameWindow::HandleMessage( int uMsg, int wParam, int lParam )
{
	return static_cast<int>( DefFrameProc( hWnd, client_wnd, uMsg, wParam, lParam ) );
}

#ifndef PURE_WRAPPER

void MDIFrameWindow::OnClose()
{
	DefFrameProc( hWnd, client_wnd, WM_CLOSE, 0, 0 );
}

int MDIFrameWindow::OnNotify( int nIdentifier, LPNMHDR pnmh )
{
	return static_cast<int>( DefFrameProc( hWnd, client_wnd, WM_NOTIFY, nIdentifier, reinterpret_cast<LPARAM>( pnmh ) ) );
}

void MDIFrameWindow::OnCommand( int nIdentifier, int nNotifyCode, HWND hwndControl )
{
	DefFrameProc( hWnd, client_wnd, WM_COMMAND, MAKEWPARAM( nIdentifier, nNotifyCode ), windows_cast<LPARAM>( hwndControl ) );
}

void MDIFrameWindow::OnPaint()
{
	DefFrameProc( hWnd, client_wnd, WM_PAINT, 0, 0 );
}

void MDIFrameWindow::OnSize( int sizing, WSize new_size )
{
	DefFrameProc( hWnd, client_wnd, WM_SIZE, sizing, MAKELPARAM( new_size.cx, new_size.cy ) );
}

void MDIFrameWindow::OnMouseMove( WPoint point, int keys )
{
	DefFrameProc( hWnd, client_wnd, WM_MOUSEMOVE, keys, MAKELPARAM( point.x, point.y ) );
}

void MDIFrameWindow::OnMouseDown( WPoint point, int keys, int button )
{
	DefFrameProc( hWnd, client_wnd, button==left_button ? WM_LBUTTONDOWN : (button==right_button ? WM_RBUTTONDOWN : WM_MBUTTONDOWN), keys, MAKELPARAM( point.x, point.y ) );
}

void MDIFrameWindow::OnMouseUp( WPoint point, int keys, int button )
{
	DefFrameProc( hWnd, client_wnd, button==left_button ? WM_LBUTTONUP : (button==right_button ? WM_RBUTTONUP : WM_MBUTTONUP), keys, MAKELPARAM( point.x, point.y ) );
}

void MDIFrameWindow::OnMouseDblClk( WPoint point, int keys, int button )
{
	DefFrameProc( hWnd, client_wnd, button==left_button ? WM_LBUTTONDOWN : (button==right_button ? WM_RBUTTONDOWN : WM_MBUTTONDOWN), keys, MAKELPARAM( point.x, point.y ) );
}

void MDIFrameWindow::OnKeyDown( int key, int keyData )
{
	DefFrameProc( hWnd, client_wnd, WM_KEYDOWN, key, keyData );
}

void MDIFrameWindow::OnKeyUp( int key, int keyData )
{
	DefFrameProc( hWnd, client_wnd, WM_KEYUP, key, keyData );
}

#endif
