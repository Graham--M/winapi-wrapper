
#include "winapi.h"
#include "passlparam.h"

using namespace WinAPI;

bool Window::Create( LPCTSTR lpszWindowName, const WindowClass& wndClass,
					 DWORD dwStyle, const RECT& rc, HWND hWndParent, HMENU hMenu,
					 DWORD dwExStyle, LPVOID lpParam, HINSTANCE hInstance )
{
	PASSLPARAM pass = { lpParam, this, &hWnd };
	GenericWindow::Create( lpszWindowName, wndClass, dwStyle, rc, hWndParent, hMenu, dwExStyle, reinterpret_cast<LPVOID>( &pass ), hInstance );
	b_created = GenericWindow::IsCreated();
	return b_created;
}

Window::~Window()
{
	b_created = false;
	if ( hWnd != NULL )
		SetWindowLong( hWnd, GWL_USERDATA, 0 );
}

int Window::HandleMessage( int uMsg, int wParam, int lParam )
{
	return static_cast<int>( DefWindowProc( hWnd, uMsg, wParam, lParam ) );
}

#ifndef PURE_WRAPPER

bool Window::OnCreate( LPCREATESTRUCT lpCreateStruct )
{
	return true;
}

void Window::OnClose()
{
	DefWindowProc( hWnd, WM_CLOSE, 0, 0 );
}

void Window::OnDestroy()
{
}

void Window::OnCommand( int nIdentifier, int nNotifyCode, HWND hwndControl )
{
}

int Window::OnNotify( int nIdentifier, LPNMHDR pnmh )
{
	return static_cast<int>( DefWindowProc( hWnd, WM_NOTIFY, nIdentifier, reinterpret_cast<LPARAM>( pnmh ) ) );
}

void Window::OnPaint()
{
	DefWindowProc( hWnd, WM_PAINT, 0, 0 );
}

void Window::OnSize( int sizing, WSize new_size )
{
	DefWindowProc( hWnd, WM_SIZE, sizing, MAKELPARAM( new_size.cx, new_size.cy ) );
}

void Window::OnMouseMove( WPoint point, int keys )
{
	DefWindowProc( hWnd, WM_MOUSEMOVE, keys, MAKELPARAM( point.x, point.y ) );
}

void Window::OnMouseDown( WPoint point, int keys, int button )
{
	DefWindowProc( hWnd, button==left_button ? WM_LBUTTONDOWN : (button==right_button ? WM_RBUTTONDOWN : WM_MBUTTONDOWN), keys, MAKELPARAM( point.x, point.y ) );
}

void Window::OnMouseUp( WPoint point, int keys, int button )
{
	DefWindowProc( hWnd, button==left_button ? WM_LBUTTONUP : (button==right_button ? WM_RBUTTONUP : WM_MBUTTONUP), keys, MAKELPARAM( point.x, point.y ) );
}

void Window::OnMouseDblClk( WPoint point, int keys, int button )
{
	DefWindowProc( hWnd, button==left_button ? WM_LBUTTONDOWN : (button==right_button ? WM_RBUTTONDOWN : WM_MBUTTONDOWN), keys, MAKELPARAM( point.x, point.y ) );
}

void Window::OnKeyDown( int key, int keyData )
{
	DefWindowProc( hWnd, WM_KEYDOWN, key, keyData );
}

void Window::OnKeyUp( int key, int keyData )
{
	DefWindowProc( hWnd, WM_KEYUP, key, keyData );
}

#endif
