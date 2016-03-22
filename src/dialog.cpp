
#include "winapi.h"
#include "passlparam.h"

using namespace WinAPI;

BOOL CALLBACK WinAPIDefaultDlgProc( HWND hwndDlg, UINT uMsg, WPARAM wParam, LPARAM lParam )
{
	Dialog* pWindow = windows_cast<Dialog*>( GetWindowLong( hwndDlg, GWL_USERDATA ) );
	if ( pWindow == NULL ) {
		if ( uMsg == WM_INITDIALOG ) {
			PASSLPARAM* pass = reinterpret_cast<PASSLPARAM*>( lParam );
			if ( pass == 0 )
				return FALSE;
			pWindow = static_cast<Dialog*>( pass->pWindow );
			if ( pWindow == 0 )
				return FALSE;
			*pass->phwnd = hwndDlg;
			lParam = reinterpret_cast<LPARAM>( pass->lpParam );
			SetWindowLong( hwndDlg, GWL_USERDATA, windows_cast<LONG>( pWindow ) );
		} else
			return FALSE;
	}
	switch ( uMsg ) {
	case WM_NCDESTROY:
		pWindow->HandleMessage( uMsg, static_cast<int>( wParam ), static_cast<int>( lParam ) );
		if ( pWindow->IsCreated() )
			Window::Delete( pWindow );
		return FALSE;
#ifndef PURE_WRAPPER
	case WM_INITDIALOG:
		return pWindow->OnInitDialog( reinterpret_cast<HWND>( wParam ), lParam );
	case WM_DESTROY:
		pWindow->OnDestroy();
		return 0;
	case WM_COMMAND:
		pWindow->OnCommand( LOWORD( wParam ), HIWORD( wParam ), reinterpret_cast<HWND>( lParam ) );
		return true;
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
#endif
	default:
		return pWindow->HandleMessage( uMsg, static_cast<int>( wParam ), static_cast<int>( lParam ) );
	}
}

Dialog::~Dialog()
{
}

int Dialog::HandleMessage( int uMsg, int wParam, int lParam )
{
	return 0;
}

#ifndef PURE_WRAPPER

void Dialog::OnClose()
{
}

void Dialog::OnDestroy()
{
}

bool Dialog::OnInitDialog( HWND hwndFocus, LPARAM lParam )
{
	return true;
}

void Dialog::OnCommand( int nIdentifier, int nNotifyCode, HWND hwndControl )
{
}

int Dialog::OnNotify( int nIdentifier, LPNMHDR pnmh )
{
	return 0;
}

void Dialog::OnMouseMove( WPoint point, int keys )
{
}

void Dialog::OnMouseDown( WPoint point, int keys, int button )
{
}

void Dialog::OnMouseUp( WPoint point, int keys, int button )
{
}

void Dialog::OnMouseDblClk( WPoint point, int keys, int button )
{
}

bool Dialog::OnCreate( LPCREATESTRUCT lpCreateStruct )
{
	return true;
}

void Dialog::OnPaint()
{
}

void Dialog::OnSize( int sizing, WSize new_size )
{
}

void Dialog::OnKeyDown( int key, int keyData )
{
}

void Dialog::OnKeyUp( int key, int keyData )
{
}

#endif
