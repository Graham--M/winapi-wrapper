
namespace WinAPI {

	struct PASSLPARAM {
		LPVOID lpParam;
		GenericWindow* pWindow;
		HWND* phwnd;
	};

	inline WPoint LParam2Point( LPARAM lParam )
	{
		const POINTS pts = MAKEPOINTS( lParam );
		return WPoint( pts.x, pts.y );
	}

}
