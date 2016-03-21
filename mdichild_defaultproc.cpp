
#include "winapi.h"
#include "passlparam.h"

using namespace WinAPI;

namespace {

LRESULT CALLBACK MDIChildWndProc( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam )
{
	Window* pWindow = windows_cast<Window*>( GetWindowLong( hWnd, GWL_USERDATA ) );
	bool b_def = true;
	do {
		if ( pWindow == 0 || *pWindow != hWnd ) {
			if ( uMsg == WM_NCCREATE ) {
				LPCREATESTRUCT lpCreate = reinterpret_cast<LPCREATESTRUCT>( lParam );
				LPMDICREATESTRUCT lpMDICreate = reinterpret_cast<LPMDICREATESTRUCT>( lpCreate->lpCreateParams );
				PASSLPARAM* pass = reinterpret_cast<PASSLPARAM*>( lpMDICreate->lParam );
				if ( pass == 0 )
					break;
				pWindow = static_cast<Window*>( pass->pWindow );
				if ( pWindow == 0 )
					break;
				*pass->phwnd = hWnd;
				lpCreate->lpCreateParams = pass->lpParam;
				SetWindowLong( hWnd, GWL_USERDATA, windows_cast<LONG>( pWindow ) );
			} else
				break;
		}
		b_def = false;
	} while ( false );
	if ( b_def ) return DefMDIChildProc( hWnd, uMsg, wParam, lParam );
	switch ( uMsg ) {
	case WM_NCDESTROY:
		pWindow->HandleMessage( uMsg, static_cast<int>( wParam ), static_cast<int>( lParam ) );
		if ( pWindow->IsCreated() )
			Window::Delete( pWindow );
		return 0;
#ifndef PURE_WRAPPER
	case WM_CREATE:
		return pWindow->OnCreate( reinterpret_cast<LPCREATESTRUCT>( lParam ) ) ? 0 : -1;
	case WM_DESTROY:
		pWindow->OnDestroy();
		return 0;
	case WM_CLOSE:
		pWindow->OnClose();
		return 0;
	case WM_COMMAND:
		pWindow->OnCommand( LOWORD( wParam ), HIWORD( wParam ), reinterpret_cast<HWND>( lParam ) );
		return 0;
	case WM_NOTIFY:
		return pWindow->OnNotify( static_cast<int>( wParam ), reinterpret_cast<LPNMHDR>( lParam ) );
	case WM_PAINT:
		pWindow->OnPaint();
		return 0;
	case WM_SIZE:
		pWindow->OnSize( static_cast<int>( wParam ), WSize( LOWORD( lParam ), HIWORD( lParam ) ) );
		return 0;
	case WM_MOUSEMOVE:
		pWindow->OnMouseMove( LParam2Point( lParam ), static_cast<int>( wParam ) );
		return FALSE;
	case WM_LBUTTONDOWN:
		pWindow->OnMouseDown( LParam2Point( lParam ), static_cast<int>( wParam ), Window::left_button );
		return FALSE;
	case WM_LBUTTONUP:
		pWindow->OnMouseUp( LParam2Point( lParam ), static_cast<int>( wParam ), Window::left_button );
		return FALSE;
	case WM_RBUTTONDOWN:
		pWindow->OnMouseDown( LParam2Point( lParam ), static_cast<int>( wParam ), Window::right_button );
		return FALSE;
	case WM_RBUTTONUP:
		pWindow->OnMouseUp( LParam2Point( lParam ), static_cast<int>( wParam ), Window::right_button );
		return FALSE;
	case WM_MBUTTONDOWN:
		pWindow->OnMouseDown( LParam2Point( lParam ), static_cast<int>( wParam ), Window::middle_button );
		return FALSE;
	case WM_MBUTTONUP:
		pWindow->OnMouseUp( LParam2Point( lParam ), static_cast<int>( wParam ), Window::middle_button );
		return FALSE;
	case WM_LBUTTONDBLCLK:
		pWindow->OnMouseDblClk( LParam2Point( lParam ), static_cast<int>( wParam ), Window::left_button );
		return FALSE;
	case WM_RBUTTONDBLCLK:
		pWindow->OnMouseDblClk( LParam2Point( lParam ), static_cast<int>( wParam ), Window::right_button );
		return FALSE;
	case WM_MBUTTONDBLCLK:
		pWindow->OnMouseDblClk( LParam2Point( lParam ), static_cast<int>( wParam ), Window::middle_button );
		return FALSE;
	case WM_KEYDOWN:
		pWindow->OnKeyDown( static_cast<int>( wParam ), static_cast<int>( lParam ) );
		return FALSE;
	case WM_KEYUP:
		pWindow->OnKeyUp( static_cast<int>( wParam ), static_cast<int>( lParam ) );
		return FALSE;
#endif
	default:
		return pWindow->HandleMessage( uMsg, static_cast<int>( wParam ), static_cast<int>( lParam ) );
	}
}

} // namespace

MDIChildWindowClass::MDIChildWindowClass( UINT style, int cbClsExtra, int cbWndExtra, HICON hIcon, HCURSOR hCursor, HBRUSH hbrBackground, LPCTSTR lpszMenuName, HINSTANCE hInstance )
: WindowClass( MDIChildWndProc, style, cbClsExtra, cbWndExtra, hIcon, hCursor, hbrBackground, lpszMenuName, hInstance )
{
}
