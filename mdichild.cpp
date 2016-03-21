
#include "winapi.h"
#include "passlparam.h"

using namespace WinAPI;

bool MDIChildWindow::Create( LPCTSTR lpszWindowName, const MDIChildWindowClass& wndClass,
							 HWND hWndParent, const RECT& rc, HMENU hMenu, DWORD dwStyle,
							 DWORD dwExStyle, LPVOID lpParam, HINSTANCE hInstance )
{
	PASSLPARAM pass = { lpParam, this, &hWnd };
	const char* class_name = wndClass;
	// the const_cast is a VS6 backwards compatibility issue
	CreateMDIWindow( const_cast<char*>(class_name), const_cast<char*>(lpszWindowName), dwStyle, rc.left, rc.top, rc.right-rc.left, rc.bottom-rc.top, hWndParent, hInstance, reinterpret_cast<LPARAM>( &pass ) );
	b_created = GenericWindow::IsCreated();
	return b_created;
}

MDIChildWindow::~MDIChildWindow()
{
}

int MDIChildWindow::HandleMessage( int uMsg, int wParam, int lParam )
{
	return static_cast<int>( DefMDIChildProc( hWnd, uMsg, wParam, lParam ) );
}

#ifndef PURE_WRAPPER

void MDIChildWindow::OnClose()
{
	DefMDIChildProc( hWnd, WM_CLOSE, 0, 0 );
}

int MDIChildWindow::OnNotify( int nIdentifier, LPNMHDR pnmh )
{
	return static_cast<int>( DefMDIChildProc( hWnd, WM_NOTIFY, nIdentifier, reinterpret_cast<LPARAM>( pnmh ) ) );
}

void MDIChildWindow::OnPaint()
{
	DefMDIChildProc( hWnd, WM_PAINT, 0, 0 );
}

void MDIChildWindow::OnSize( int sizing, WSize new_size )
{
	DefMDIChildProc( hWnd, WM_SIZE, sizing, MAKELPARAM( new_size.cx, new_size.cy ) );
}

void MDIChildWindow::OnMouseMove( WPoint point, int keys )
{
	DefMDIChildProc( hWnd, WM_MOUSEMOVE, keys, MAKELPARAM( point.x, point.y ) );
}

void MDIChildWindow::OnMouseDown( WPoint point, int keys, int button )
{
	DefMDIChildProc( hWnd, button==left_button ? WM_LBUTTONDOWN : (button==right_button ? WM_RBUTTONDOWN : WM_MBUTTONDOWN), keys, MAKELPARAM( point.x, point.y ) );
}

void MDIChildWindow::OnMouseUp( WPoint point, int keys, int button )
{
	DefMDIChildProc( hWnd, button==left_button ? WM_LBUTTONUP : (button==right_button ? WM_RBUTTONUP : WM_MBUTTONUP), keys, MAKELPARAM( point.x, point.y ) );
}

void MDIChildWindow::OnMouseDblClk( WPoint point, int keys, int button )
{
	DefMDIChildProc( hWnd, button==left_button ? WM_LBUTTONDOWN : (button==right_button ? WM_RBUTTONDOWN : WM_MBUTTONDOWN), keys, MAKELPARAM( point.x, point.y ) );
}

void MDIChildWindow::OnKeyDown( int key, int keyData )
{
	DefMDIChildProc( hWnd, WM_KEYDOWN, key, keyData );
}

void MDIChildWindow::OnKeyUp( int key, int keyData )
{
	DefMDIChildProc( hWnd, WM_KEYUP, key, keyData );
}

#endif
