
/**********************************************************************************************
*
*  This file is part of the WinAPI Wrapper library.
*
*  Copyright (c) 2001-2003 Chris Dragan. All rights reserved.
*
*  Permission is hereby granted, free of charge, to any person obtaining a copy
*  of this software and associated documentation files (the "Software"),
*  to deal in the Software without restriction, including without limitation
*  the rights to use, copy, modify, merge, publish, distribute, sublicense,
*  and/or sell copies of the Software, and to permit persons to whom
*  the Software is furnished to do so, subject to the following conditions:
*
*  The above copyright notice and this permission notice shall be included
*  in all copies or substantial portions of the Software.
*
*  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
*  EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES
*  OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
*  IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM,
*  DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR
*  OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR
*  THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*
*  Current Version: 1.05.01
*  Version Date: 5.12.2003
*
**********************************************************************************************/

// Configuration (must be defined before including this header):
//   SKIP_STL - don't use STL (functions using STL are not available)
//   PURE_WRAPPER - don't use any extra methods for windows and dialogs

// Include this header only once
#ifndef MINGW
#pragma once
#endif

#ifndef _WIN32_IE
#define _WIN32_IE 0x0400
#endif
#ifndef _WIN32_WINNT
#define _WIN32_WINNT 0x0400
#endif

// Wrapper version
#define __WINAPI_WRAPPER_VERSION 0x0105

// Include Windows headers
#if defined(_MSC_VER) && (_MSC_VER >= 1300)
#include <winsock2.h>
#endif
#include <windows.h>
#include <commctrl.h>
#include <commdlg.h>
#include <shlobj.h>
#include <Tchar.h>

// Undefine Windows macros colliding with STL names
#undef min
#undef max

// Include STL
#include <cassert>
#ifndef SKIP_STL
#if defined(_MSC_VER) && (_MSC_VER < 1300)
#pragma warning( disable: 4786 )
#endif
#include <string>
#include <vector>
#endif


namespace WinAPI {

// Defined module handle - Default value for hInstance when not supplied
extern HINSTANCE g_hInstance;
#ifndef __MODULE_HANDLE
#define __MODULE_HANDLE g_hInstance
#endif

#ifndef SKIP_STL
using std::string;
using std::vector;
typedef std::basic_string<TCHAR> tstring;
#endif

/**********************************************************************************************
* windows_cast - for avoiding annoying warnings during compilation (64-bit compatibility)
**********************************************************************************************/

template< class Out, class In >
inline Out windows_cast( In x )
{
	union {
		In x;
		Out y;
	} convert;
	convert.y = 0;
	convert.x = x;
	return convert.y;
}

/**********************************************************************************************
* NonCopyAble - prevents derived objects from being copied
**********************************************************************************************/

class NonCopyAble {
protected:
	NonCopyAble() { }
	~NonCopyAble() { }
private:
	NonCopyAble( const NonCopyAble& );
	NonCopyAble& operator=( const NonCopyAble& );
};

/**********************************************************************************************
* File
**********************************************************************************************/

class File: public NonCopyAble {
	HANDLE hFile;
public:
	File(): hFile( INVALID_HANDLE_VALUE ) { }
	File( LPCTSTR lpszFileName,
		DWORD dwCreationDisposition = OPEN_EXISTING,
		DWORD dwDesiredAccess = GENERIC_READ | GENERIC_WRITE,
		DWORD dwFlagsAndAttributes = FILE_ATTRIBUTE_ARCHIVE | FILE_FLAG_RANDOM_ACCESS,
		DWORD dwShareMode = FILE_SHARE_READ,
		HANDLE hTemplateFile = 0,
		LPSECURITY_ATTRIBUTES lpSecurityAttributes = 0 );
	~File() {
		Close();
	}
	operator HANDLE() const {
		return hFile;
	}
	bool Open( LPCTSTR lpszFileName,
			DWORD dwCreationDisposition = OPEN_EXISTING,
			DWORD dwDesiredAccess = GENERIC_READ | GENERIC_WRITE,
			DWORD dwFlagsAndAttributes = FILE_ATTRIBUTE_ARCHIVE | FILE_FLAG_RANDOM_ACCESS,
			DWORD dwShareMode = FILE_SHARE_READ,
			HANDLE hTemplateFile = 0,
			LPSECURITY_ATTRIBUTES lpSecurityAttributes = 0 );
	bool SetTo( LPCTSTR lpszFileName,
			DWORD dwCreationDisposition = OPEN_EXISTING,
			DWORD dwDesiredAccess = GENERIC_READ | GENERIC_WRITE,
			DWORD dwFlagsAndAttributes = FILE_ATTRIBUTE_ARCHIVE | FILE_FLAG_RANDOM_ACCESS,
			DWORD dwShareMode = FILE_SHARE_READ,
			HANDLE hTemplateFile = 0,
			LPSECURITY_ATTRIBUTES lpSecurityAttributes = 0 ) {
		return Open( lpszFileName, dwCreationDisposition, dwDesiredAccess, dwFlagsAndAttributes, dwShareMode, hTemplateFile, lpSecurityAttributes );
	}
	bool IsOpen() const {
		return hFile != INVALID_HANDLE_VALUE;
	}
	void Close();
	DWORD Read( LPVOID lpBuffer, DWORD nNumberOfBytesToRead, LPOVERLAPPED lpOverlapped = 0 ) const;
	DWORD Write( LPCVOID lpBuffer, DWORD nNumberOfBytesToWrite, LPOVERLAPPED lpOverlapped = 0 );
	template< typename T >
	bool ReadVar( T* p_variable, LPOVERLAPPED lpOverlapped = 0 ) const {
		return sizeof(T) == Read( p_variable, sizeof(T), lpOverlapped );
	}
	template< typename T >
	bool WriteVar( const T& variable, LPOVERLAPPED lpOverlapped = 0 ) {
		return sizeof(T) == Write( &variable, sizeof(T), lpOverlapped );
	}
#ifndef SKIP_STL
	template< typename T >
	bool ReadVec( vector<T>* p_vec, LPOVERLAPPED lpOverlapped = 0 ) const {
		const size_t len = sizeof(T) * p_vec->size();
		return len == Read( &(*p_vec)[0], len, lpOverlapped );
	}
	template< typename T >
	bool WriteVec( const vector<T>& vec, LPOVERLAPPED lpOverlapped = 0 ) {
		const size_t len = sizeof(T) * vec->size();
		return len == Read( &vec[0], len, lpOverlapped );
	}
#endif
	int GetSize() const {
		return GetFileSize( hFile, 0 );
	}
	int Seek( int nDistanceToMove, DWORD dwMoveMethod = FILE_BEGIN ) const {
		return static_cast<int>( SetFilePointer( hFile, static_cast<LONG>( nDistanceToMove ), 0, dwMoveMethod ) );
	}
	int GetPosition() const {
		return Seek( 0, FILE_CURRENT );
	}
	bool Truncate() {
		return SetEndOfFile( hFile ) != 0;
	}
	bool Flush() const {
		return FlushFileBuffers( hFile ) != 0;
	}
};

inline File::File( LPCTSTR lpszFileName, DWORD dwCreationDisposition, DWORD dwDesiredAccess, DWORD dwFlagsAndAttributes, DWORD dwShareMode, HANDLE hTemplateFile, LPSECURITY_ATTRIBUTES lpSecurityAttributes )
	: hFile( INVALID_HANDLE_VALUE )
{
	Open( lpszFileName, dwCreationDisposition, dwDesiredAccess, dwFlagsAndAttributes,
		dwShareMode, hTemplateFile, lpSecurityAttributes );
}

inline bool File::Open( LPCTSTR lpszFileName, DWORD dwCreationDisposition, DWORD dwDesiredAccess, DWORD dwFlagsAndAttributes, DWORD dwShareMode, HANDLE hTemplateFile, LPSECURITY_ATTRIBUTES lpSecurityAttributes )
{
	Close();
	hFile = CreateFile( lpszFileName, dwDesiredAccess, dwShareMode, lpSecurityAttributes,
		dwCreationDisposition, dwFlagsAndAttributes, hTemplateFile );
	return hFile != INVALID_HANDLE_VALUE;
}

inline void File::Close()
{
	if ( IsOpen() ) {
		CloseHandle( hFile );
		hFile = INVALID_HANDLE_VALUE;
	}
}

inline DWORD File::Read( LPVOID lpBuffer, DWORD nNumberOfBytesToRead, LPOVERLAPPED lpOverlapped ) const
{
	DWORD dwNumRead;
	if ( ! ReadFile( hFile, lpBuffer, nNumberOfBytesToRead, &dwNumRead, lpOverlapped ) )
		return DWORD(-1);
	return dwNumRead;
}

inline DWORD File::Write( LPCVOID lpBuffer, DWORD nNumberOfBytesToWrite, LPOVERLAPPED lpOverlapped )
{
	DWORD dwNumWrit;
	if ( ! WriteFile( hFile, lpBuffer, nNumberOfBytesToWrite, &dwNumWrit, lpOverlapped ) )
		return DWORD(-1);
	return dwNumWrit;
}

/**********************************************************************************************
* FindFile
**********************************************************************************************/

class FindFile: public NonCopyAble {
	HANDLE hFindFile;
	mutable WIN32_FIND_DATA data;
public:
	FindFile( LPCTSTR lpFileName = _T("*.*") ): hFindFile( INVALID_HANDLE_VALUE ) {
		hFindFile = FindFirstFile( lpFileName, &data );
	}
	~FindFile() {
		Close();
	}
	operator HANDLE() const {
		return hFindFile;
	}
	bool IsOpen() const {
		return hFindFile != INVALID_HANDLE_VALUE;
	}
	bool FindNext() const {
		return FindNextFile( hFindFile, &data ) != 0;
	}
	const WIN32_FIND_DATA& GetFindData() const {
		return data;
	}
	bool SetTo( LPCTSTR lpFileName = _T("*.*") ) {
		Close();
		hFindFile = FindFirstFile( lpFileName, &data );
		return IsOpen();
	}
	void Close() {
		if ( hFindFile != INVALID_HANDLE_VALUE )
			FindClose( hFindFile );
	}

	class iterator: public WIN32_FIND_DATA {
		HANDLE hFindFile;
	public:
		iterator( const WIN32_FIND_DATA& fd, HANDLE hFindFile ): WIN32_FIND_DATA( fd ), hFindFile( hFindFile ) { }
		iterator(): hFindFile( INVALID_HANDLE_VALUE ) { cFileName[0] = 0; }
		void operator++() {
			if ( FindNextFile( hFindFile, this ) != TRUE )
				cFileName[0] = 0;
		}
		void operator--() {
			if ( FindNextFile( hFindFile, this ) != TRUE )
				cFileName[0] = 0;
		}
		bool operator==( const iterator& fd ) const {
			return cFileName[0] == fd.cFileName[0];
		}
		bool operator!=( const iterator& fd ) const {
			return cFileName[0] != fd.cFileName[0];
		}
	};
	iterator begin() const { return iterator( data, hFindFile ); }
	iterator end() const { return iterator(); }

	enum FINDTYPE { ALL, FILES, DIRECTORIES };

	template< class T >
	void ForEach( T& handler, FINDTYPE ft = ALL ) {
		if ( IsOpen() ) do {
			const bool b_directory = (data.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) != 0;
			if ( ft == ALL )
				handler( data );
			else if ( ft == FILES ) {
				if ( ! b_directory ) handler( data );
			} else
				if ( b_directory ) handler( data );
		} while ( FindNext() );
	}

	template< class T >
	static void ForEach( LPCTSTR lpFileName, T& handler, FINDTYPE ft = ALL ) {
		FindFile finder( lpFileName );
		finder.ForEach( handler, ft );
	}

#ifndef SKIP_STL
	class StringHandler {
		vector<tstring>& filenames;
	public:
		StringHandler( vector<tstring>& filenames ): filenames( filenames ) { }
		void operator()( const WIN32_FIND_DATA& fd ) { filenames.push_back( tstring( fd.cFileName ) ); }
	};

	class FindDataHandler {
		vector<WIN32_FIND_DATA>& files;
	public:
		FindDataHandler( vector<WIN32_FIND_DATA>& files ): files( files ) { }
		void operator()( const WIN32_FIND_DATA& fd ) { files.push_back( fd ); }
	};

	void GetAll( vector<tstring>* p_files, FINDTYPE ft = ALL ) {
		assert( p_files != 0 );
		p_files->clear();
		StringHandler handler( *p_files );
		ForEach( handler, ft );
	}
	void GetAll( vector<WIN32_FIND_DATA>* p_files, FINDTYPE ft = ALL ) {
		assert( p_files != 0 );
		p_files->clear();
		FindDataHandler handler( *p_files );
		ForEach( handler, ft );
	}
	static void GetAll( LPCTSTR lpFileName, vector<tstring>* p_files, FINDTYPE ft = ALL ) {
		FindFile finder( lpFileName );
		finder.GetAll( p_files, ft );
	}
	static void GetAll( LPCTSTR lpFileName, vector<WIN32_FIND_DATA>* p_files, FINDTYPE ft = ALL ) {
		FindFile finder( lpFileName );
		finder.GetAll( p_files, ft );
	}
#endif
};

/**********************************************************************************************
* FileMapping
**********************************************************************************************/

class FileMapping: public NonCopyAble {
	HANDLE hFile;
	HANDLE hMapping;
public:
	FileMapping(): hFile( INVALID_HANDLE_VALUE ), hMapping( NULL ) { }
	FileMapping( File& file, DWORD flProtect = PAGE_READWRITE, DWORD dwMaximumSize = 0, LPCTSTR lpName = 0, LPSECURITY_ATTRIBUTES lpAttributes = 0 ): hFile( INVALID_HANDLE_VALUE ), hMapping( NULL ) {
		SetTo( file, flProtect, dwMaximumSize, lpName, lpAttributes );
	}
	~FileMapping() {
		Close();
	}
	bool IsOpen() const {
		return hMapping != NULL;
	}
	bool Open( File& file, DWORD flProtect = PAGE_READWRITE, DWORD dwMaximumSize = 0, LPCTSTR lpName = 0, LPSECURITY_ATTRIBUTES lpAttributes = 0 );
	bool SetTo( File& file, DWORD flProtect = PAGE_READWRITE, DWORD dwMaximumSize = 0, LPCTSTR lpName = 0, LPSECURITY_ATTRIBUTES lpAttributes = 0 ) {
		return Open( file, flProtect, dwMaximumSize, lpName, lpAttributes );
	}
	void Close();
	operator HANDLE() const {
		return hMapping;
	}

	class Map: public NonCopyAble {
		HANDLE hMapping;
		void* address;
		SIZE_T size;
	public:
		Map( FileMapping& mapping, DWORD dwDesiredAccess = FILE_MAP_ALL_ACCESS, DWORD dwFileOffset = 0, SIZE_T dwNumBytesToMap = 0 ): hMapping( mapping ), size( dwNumBytesToMap ), address( 0 ) {
			address = MapViewOfFile( hMapping, dwDesiredAccess, 0, dwFileOffset, dwNumBytesToMap );
		}
		~Map() {
			if ( address != 0 ) UnmapViewOfFile( address );
		}
		operator void*() const { return address; }
		operator char*() const { return static_cast<char*>( address ); }
		char& operator[]( size_t offset ) const { return static_cast<char*>( address )[ offset ]; }
		int GetSize() const { return static_cast<int>( size ); }
		int Size() const { return static_cast<int>( size ); }
	};
};

inline void FileMapping::Close()
{
	if ( IsOpen() ) {
		CloseHandle( hMapping );
		hMapping = NULL;
	}
}

inline bool FileMapping::Open( File& file, DWORD flProtect, DWORD dwMaximumSize, LPCTSTR lpName, LPSECURITY_ATTRIBUTES lpAttributes )
{
	Close();
	if ( ! file.IsOpen() )
		return false;
	hFile = file;
	hMapping = CreateFileMapping( hFile, lpAttributes, flProtect, 0, dwMaximumSize, lpName );
	return hMapping != NULL;
}

/**********************************************************************************************
* WPoint
**********************************************************************************************/

class WPoint: public POINT {
public:
	WPoint() { x = y = 0; }
	WPoint( int _x, int _y ) {
		x = _x, y = _y;
	}
	WPoint( const POINT& pt ) {
		x = pt.x, y = pt.y;
	}
	WPoint operator+( const POINT& p ) const {
		return WPoint( x+p.x, y+p.y );
	}
	WPoint operator-( const POINT& p ) const {
		return WPoint( x-p.x, y-p.y );
	}
	WPoint operator-() const {
		return WPoint( -x, -y );
	}
	WPoint& operator+=( const POINT& p ) {
		x += p.x, y += p.y; return *this;
	}
	WPoint& operator-=( const POINT& p ) {
		x -= p.x, y -= p.y; return *this;
	}
	bool operator==( const POINT& p ) const {
		return x == p.x && y == p.y;
	}
	bool operator!=( const POINT& p ) const {
		return x != p.x || y != p.y;
	}
};

/**********************************************************************************************
* WSize
**********************************************************************************************/

class WSize: public SIZE {
public:
	WSize() { cx = cy = 0; }
	WSize( int width, int height ) {
		cx = width, cy = height;
	}
	WSize( const SIZE& size ) {
		cx = size.cx, cy = size.cy;
	}
	WSize operator-() const {
		return WSize( -cx, -cy );
	}
	WSize operator-( const SIZE& s ) const {
		return WSize( cx - s.cx, cy - s.cy );
	}
	WSize operator+( const SIZE& s ) const {
		return WSize( cx + s.cx, cy + s.cy );
	}
	WSize& operator-=( const SIZE& s ) {
		cx -= s.cx, cy -= s.cy; return *this;
	}
	WSize& operator+=( const SIZE& s ) {
		cx += s.cx, cy += s.cy; return *this;
	}
	bool operator==( const SIZE& s ) const {
		return cx == s.cx && cy == s.cy;
	}
	bool operator!=( const SIZE& s ) const {
		return cx != s.cx || cy != s.cy;
	}
};

/**********************************************************************************************
* WRect
**********************************************************************************************/

class WRect: public RECT {
public:
	WRect() { }
	WRect( const RECT& rc ) {
		left = rc.left, top = rc.top;
		right = rc.right, bottom = rc.bottom;
	}
	WRect( const POINT& p1, const POINT& p2 ) {
		left = p1.x, top = p1.y;
		right = p2.x, bottom = p2.y;
	}
	WRect( const POINT& p1, int width, int height ) {
		left = p1.x, top = p1.y;
		right = p1.x+width, bottom = p1.y+height;
	}
	WRect( const POINT& p1, const SIZE& size ) {
		left = p1.x, top = p1.y;
		right = p1.x+size.cx, bottom = p1.y+size.cy;
	}
	WRect( int _left, int _top, int _right, int _bottom ) {
		left = _left, top = _top;
		right = _right, bottom = _bottom;
	}
	bool operator==( const RECT& r ) const {
		return left == r.left && top == r.top && right == r.right && bottom == r.bottom;
	}
	bool operator!=( const RECT& r ) const {
		return left != r.left || top != r.top || right != r.right || bottom != r.bottom;
	}
	WRect operator&( const RECT& r ) const {
		return WRect( left < r.left ? r.left : left, top < r.top ? r.top : top, right > r.right ? r.right : right, bottom > r.bottom ? r.bottom : bottom );
	}
	WRect operator|( const RECT& r ) const {
		return WRect( left < r.left ? left : r.left, top < r.top ? top : r.top, right > r.right ? right : r.right, bottom > r.bottom ? bottom : r.bottom );
	}
	int Width() const {
		return right - left;
	}
	int Height() const {
		return bottom - top;
	}
	WSize Size() const {
		return WSize( Width(), Height() );
	}
	WSize GetSize() const {
		return WSize( Width(), Height() );
	}
	void SetSize( int cx, int cy ) {
		right = left + cx, bottom = top + cy;
	}
	void SetSize( const SIZE& size ) {
		right = left + size.cx, bottom = top + size.cy;
	}
	bool Contains( const POINT& p ) const {
		return (p.x >= left) && (p.x < right) && (p.y >= top) && (p.y < bottom);
	}
	bool Contains( const RECT& r ) const {
		return left <= r.left && top <= r.top && right >= r.right && bottom >= r.bottom;
	}
	bool IsValid() const {
		return left < right && top < bottom;
	}
	bool Intersects( const RECT& r ) const {
		return left < r.right && right > r.left && top < r.bottom && bottom > r.top;
	}
	WPoint LeftTop() {
		return WPoint( left, top );
	}
	WPoint LeftBottom() {
		return WPoint( left, bottom );
	}
	WPoint RightTop() {
		return WPoint( right, top );
	}
	WPoint RightBottom() {
		return WPoint( right, bottom );
	}
	void InsetBy( int x, int y ) {
		left += x, top += y, right -= x, bottom -= y;
	}
	void OffsetBy( int x, int y ) {
		left += x, top += y, right += x, bottom += y;
	}
	void OffsetTo( int x, int y ) {
		right += x - left, bottom += y - top; left = x, top = y;
	}
	void InsetBy( const POINT& p ) {
		InsetBy( p.x, p.y );
	}
	void OffsetBy( const POINT& p ) {
		OffsetBy( p.x, p.y );
	}
	void OffsetTo( const POINT& p ) {
		OffsetTo( p.x, p.y );
	}
	WRect& InsetBySelf( int x, int y ) {
		InsetBy( x, y ); return *this;
	}
	WRect& OffsetBySelf( int x, int y ) {
		OffsetBy( x, y ); return *this;
	}
	WRect& OffsetToSelf( int x, int y ) {
		OffsetTo( x, y ); return *this;
	}
	WRect& InsetBySelf( const POINT& p ) {
		InsetBy( p ); return *this;
	}
	WRect& OffsetBySelf( const POINT& p ) {
		OffsetBy( p ); return *this;
	}
	WRect& OffsetToSelf( const POINT& p ) {
		OffsetTo( p ); return *this;
	}
	WRect InsetByCopy( int x, int y ) const {
		WRect r = *this; return r.InsetBySelf( x, y );
	}
	WRect OffsetByCopy( int x, int y ) const {
		WRect r = *this; return r.OffsetBySelf( x, y );
	}
	WRect OffsetToCopy( int x, int y ) const {
		WRect r = *this; return r.OffsetToSelf( x, y );
	}
	WRect InsetByCopy( const POINT& p ) const {
		WRect r = *this; return r.InsetBySelf( p );
	}
	WRect OffsetByCopy( const POINT& p ) const {
		WRect r = *this; return r.OffsetBySelf( p );
	}
	WRect OffsetToCopy( const POINT& p ) const {
		WRect r = *this; return r.OffsetToSelf( p );
	}
};

/**********************************************************************************************
* FileTime
**********************************************************************************************/

class FileTime: public FILETIME {
public:
	FileTime() { }
	FileTime( FILETIME ft ) { *static_cast<FILETIME*>(this) = ft; }
	FileTime( const SYSTEMTIME& st ) {
		SystemTimeToFileTime( &st, this );
	}
	operator SYSTEMTIME() const {
		SYSTEMTIME st;
		FileTimeToSystemTime( this, &st );
		return st;
	}
	FileTime& operator=( const SYSTEMTIME& st ) {
		SystemTimeToFileTime( &st, this );
		return *this;
	}
	void ConvertToLocal() {
		FILETIME ft;
		FileTimeToLocalFileTime( this, &ft );
		*static_cast<FILETIME*>(this) = ft;
	}
	void ConvertToUTC() {
		FILETIME ft;
		LocalFileTimeToFileTime( this, &ft );
		*static_cast<FILETIME*>(this) = ft;
	}
	void ObtainSystemTime() {
		GetSystemTimeAsFileTime( this );
	}
	void ObtainLocalTime() {
		SYSTEMTIME st;
		::GetLocalTime( &st );
		*this = st;
	}
#ifdef SKIP_STL
	int FormatTime( TCHAR* lpTimeStr, int cchTimeStrLen, DWORD dwFlags = 0, const TCHAR* lpFormat = 0, LCID locale = LOCALE_USER_DEFAULT ) const {
		const SYSTEMTIME st = *this;
		return GetTimeFormat( locale, dwFlags, &st, lpFormat, lpTimeStr, cchTimeStrLen );
	}
	static int FormatTime( const SYSTEMTIME& st, TCHAR* lpTimeStr, int cchTimeStrLen, DWORD dwFlags = 0, const TCHAR* lpFormat = 0, LCID locale = LOCALE_USER_DEFAULT ) {
		return GetTimeFormat( locale, dwFlags, &st, lpFormat, lpTimeStr, cchTimeStrLen );
	}
	int FormatDate( TCHAR* lpDateStr, int cchDateStrLen, DWORD dwFlags = 0, const TCHAR* lpFormat = 0, LCID locale = LOCALE_USER_DEFAULT ) const {
		const SYSTEMTIME st = *this;
		return GetDateFormat( locale, dwFlags, &st, lpFormat, lpDateStr, cchDateStrLen );
	}
	static int FormatDate( const SYSTEMTIME& st, TCHAR* lpDateStr, int cchDateStrLen, DWORD dwFlags = 0, const TCHAR* lpFormat = 0, LCID locale = LOCALE_USER_DEFAULT ) {
		return GetDateFormat( locale, dwFlags, &st, lpFormat, lpDateStr, cchDateStrLen );
	}
#else
	tstring FormatTime( DWORD dwFlags = 0, const TCHAR* lpFormat = 0, LCID locale = LOCALE_USER_DEFAULT ) const {
		TCHAR s[64];
		const SYSTEMTIME st = *this;
		const int count = GetTimeFormat( locale, dwFlags, &st, lpFormat, s, 64 ) - 1;
		if ( count <= 0 ) return _T("");
		return tstring( s, count );
	}
	static tstring FormatTime( const SYSTEMTIME& st, DWORD dwFlags = 0, const TCHAR* lpFormat = 0, LCID locale = LOCALE_USER_DEFAULT ) {
		TCHAR s[64];
		const int count = GetTimeFormat( locale, dwFlags, &st, lpFormat, s, 64 ) - 1;
		if ( count <= 0 ) return _T("");
		return tstring( s, count );
	}
	tstring FormatDate( DWORD dwFlags = 0, const TCHAR* lpFormat = 0, LCID locale = LOCALE_USER_DEFAULT ) const {
		TCHAR s[64];
		const SYSTEMTIME st = *this;
		const int count = GetDateFormat( locale, dwFlags, &st, lpFormat, s, 64 ) - 1;
		if ( count <= 0 ) return _T("");
		return tstring( s, count );
	}
	static tstring FormatDate( const SYSTEMTIME& st, DWORD dwFlags = 0, const TCHAR* lpFormat = 0, LCID locale = LOCALE_USER_DEFAULT ) {
		TCHAR s[64];
		const int count = GetDateFormat( locale, dwFlags, &st, lpFormat, s, 64 ) - 1;
		if ( count <= 0 ) return _T("");
		return tstring( s, count );
	}
#endif
	bool operator==( FILETIME ft ) const {
		return CompareFileTime( this, &ft ) == 0;
	}
	bool operator!=( FILETIME ft ) const {
		return CompareFileTime( this, &ft ) != 0;
	}
	bool operator<( FILETIME ft ) const {
		return CompareFileTime( this, &ft ) < 0;
	}
	bool operator>( FILETIME ft ) const {
		return CompareFileTime( this, &ft ) > 0;
	}
	bool operator<=( FILETIME ft ) const {
		return CompareFileTime( this, &ft ) <= 0;
	}
	bool operator>=( FILETIME ft ) const {
		return CompareFileTime( this, &ft ) >= 0;
	}
};

/**********************************************************************************************
* Address
**********************************************************************************************/

class Address {
	in_addr address;
	int port;
public:
	enum {
		any			= 0,
		loopback	= 0x0100007F, // 127.0.0.1
		broadcast	= 0xFFFFFFFF  // 255.255.255.255
	};

	Address( int port, in_addr address ): address( address ), port ( port ) { }
	Address( int port = 0, int addr = any );
	Address( int port, const char* name );
	Address( const sockaddr& sa );
	bool IsValid() const { return port > 0 && port < 65536; }
	bool operator==( Address addr ) const { return address.S_un.S_addr == addr.address.S_un.S_addr && port == addr.port; }
	bool operator!=( Address addr ) const { return address.S_un.S_addr != addr.address.S_un.S_addr || port != addr.port; }
	void SetPort( int p ) { port = p; }
	in_addr GetAddress() const { return address; }
	int GetPort() const { return port; }
	sockaddr GetSockAddr() const;
#ifndef SKIP_STL
	string GetName() const;
	string GetIP() const { return inet_ntoa( address ); }
#else
	const char* GetName() const;
	const char* GetIP() const { return inet_ntoa( address ); }
#endif
};

inline Address::Address( int port, int addr )
: port( port )
{
	address.S_un.S_addr = addr;
}

inline Address::Address( int port, const char* name )
: port( port )
{
	address.S_un.S_addr = any;
	const hostent* hostname = gethostbyname( name );
	if ( hostname )
		address = *reinterpret_cast<in_addr*>( hostname->h_addr );
}

inline Address::Address( const sockaddr& sa )
: port( 0 )
{
	address.S_un.S_addr = any;
	const sockaddr_in& sa_in = reinterpret_cast<const sockaddr_in&>( sa );
	address = sa_in.sin_addr;
	port = ntohs( sa_in.sin_port );
}

inline sockaddr Address::GetSockAddr() const
{
	sockaddr_in sa;
	sa.sin_family = AF_INET;
	sa.sin_port = htons( port );
	sa.sin_addr = address;
	for ( int i=0; i<sizeof( sa.sin_zero ); i++ )
		reinterpret_cast<char*>( &sa.sin_zero )[i] = 0;
	return *reinterpret_cast<sockaddr*>( &sa );
}

#ifndef SKIP_STL
inline string Address::GetName() const
{
	const hostent* hostname = gethostbyaddr( reinterpret_cast<const char*>( &address ), sizeof(address), AF_INET );
	return hostname->h_name;
}
#else
inline const char* Address::GetName() const
{
	const hostent* hostname = gethostbyaddr( reinterpret_cast<const char*>( &address ), sizeof(address), AF_INET );
	return hostname->h_name;
}
#endif

/**********************************************************************************************
* Socket
**********************************************************************************************/

class Socket: public NonCopyAble {
	int socket;
public:
	enum { empty = -1 };

	// Construct AF_INET, SOCK_STREAM or SOCK_DGRAM
	Socket( bool b_udp = false ); // b_udp ? SOCK_DGRAM : SOCK_STREAM
	// Construct any type of AF_INET socket
	Socket( int type, int protocol );
	// Keep already existing socket
	Socket( int socket ): socket( socket ) { }
	// Copy constructor
	Socket( Socket& s ): socket( empty ) { *this = s; }
	// Assignment operator
	Socket& operator=( Socket& s );
	// Close socket on destruction
	~Socket() { Close(); }
	// Close socket
	void Close();
	// Check if the socket exists
	bool IsValid() const { return socket >= 0; }
	// Get last error
	static int GetError() { return errno; }
	// Bind local address to the socket
	bool Bind( Address addr ) const;
	bool Bind( int port ) const { return Bind( Address( port ) ); }
	// Connect to a remote address
	bool Connect( Address addr ) const;
	// Returns address of the peer to which the socket is connected
	Address GetPeerName() const;
	// Returns local address of the socket
	Address GetSockName() const;
	// Puts the socket in a listen state in order to accept incoming connections
	bool Listen( int acceptance_count = SOMAXCONN ) const;
	// Accepts an incoming connection and returns a socket with the connection
	Socket Accept( Address* paddr = 0 ) const;
	// Waits until anything's ready to be *read* (used before Accept() and Receive*() on blocking sockets)
	bool Select( int timeout_ms = 0 ) const;
	// Send a buffer of data (on a connected socket)
	int Send( const void* buf, int size ) const;
	// Receives a buffer of data (on a connected socket)
	int Receive( void* buf, int size ) const;
	// Send a buffer of data (connection-less)
	int SendTo( const void* buf, int size, Address addr ) const;
	// Receives a buffer of data (connection-less)
	int ReceiveFrom( void* buf, int size, Address* p_addr = 0 ) const;
	// Puts the socket in a blocking or non-blocking state
	bool SetBlocking( bool block = true ) const;
	// Sets an option on the socket
	bool SetOpt( int optname, const char* optval, int optlen, int level = SOL_SOCKET );
	template< class T >
	bool SetOpt( int optname, const T& optval, int level = SOL_SOCKET ) {
		return SetOpt( optname, reinterpret_cast<const char*>( &optval ), sizeof(optval), level );
	}
	// Controls I/O mode of the socket
	bool IOCtl( int command, unsigned long* arg );
};

inline Socket& Socket::operator=( Socket& s )
{
	Close();
	socket = s.socket;
	s.socket = empty;
	return *this;
}

inline void Socket::Close()
{
	if ( IsValid() ) {
		shutdown( socket, 2 ); // 2==SD_BOTH
		closesocket( socket );
		socket = empty;
	}
}

inline bool Socket::Bind( Address addr ) const
{
	const sockaddr sa = addr.GetSockAddr();
	return bind( socket, &sa, sizeof( sa ) ) == 0;
}

inline bool Socket::Connect( Address addr ) const
{
	const sockaddr sa = addr.GetSockAddr();
	return connect( socket, &sa, sizeof( sa ) ) == 0;
}

inline Address Socket::GetPeerName() const
{
	sockaddr sa;
	int sa_size = sizeof( sa );
	getpeername( socket, &sa, &sa_size );
	return Address( sa );
}

inline Address Socket::GetSockName() const
{
	sockaddr sa;
	int sa_size = sizeof( sa );
	::getsockname( socket, &sa, &sa_size );
	return Address( sa );
}

inline bool Socket::Listen( int acceptance_count ) const
{
	return listen( socket, acceptance_count ) == 0;
}

inline Socket Socket::Accept( Address* paddr ) const
{
	Socket newsocket( empty );
	sockaddr sa;
	int sa_size = sizeof( sa );
	newsocket.socket = static_cast<int>( ::accept( socket, &sa, &sa_size ) );
	if ( paddr ) *paddr = Address( sa );
	return newsocket;
}

inline bool Socket::Select( int timeout_ms ) const
{
	timeval tv;
	tv.tv_sec = timeout_ms / 1000;
	tv.tv_usec = ( timeout_ms % 1000 ) * 1000;

	fd_set fds;
	FD_ZERO( &fds );
	FD_SET( (unsigned)socket, &fds );

	return select( 32, &fds, 0, 0, timeout_ms ? &tv : 0 ) > 0;
}

inline int Socket::Send( const void* buf, int size ) const
{
	return send( socket, reinterpret_cast<const char*>( buf ), size, 0 );
}

inline int Socket::Receive( void* buf, int size ) const
{
	return recv( socket, reinterpret_cast<char*>( buf ), size, 0 );
}

inline int Socket::SendTo( const void* buf, int size, Address addr ) const
{
	sockaddr sa = addr.GetSockAddr();
	return sendto( socket, reinterpret_cast<const char*>( buf ), size, 0, &sa, sizeof( sa ) );
}

inline int Socket::ReceiveFrom( void* buf, int size, Address* p_addr ) const
{
	sockaddr sa;
	int sa_size = sizeof( sa );
	int ret = recvfrom( socket, reinterpret_cast<char*>( buf ), size, 0, &sa, &sa_size );
	if ( p_addr ) *p_addr = Address( sa );
	return ret;
}

inline bool Socket::SetBlocking( bool block ) const
{
	unsigned long blk;
	return ioctlsocket( socket, FIONBIO, &blk ) == 0;
}

inline bool Socket::SetOpt( int optname, const char* optval, int optlen, int level )
{
	return setsockopt( socket, level, optname, optval, optlen ) == 0;
}

inline bool Socket::IOCtl( int command, unsigned long* arg )
{
	return ioctlsocket( socket, command, arg ) == 0;
}

/**********************************************************************************************
* Menu
**********************************************************************************************/

class Menu {
	HMENU hMenu;
public:
	Menu(): hMenu( 0 ) { }
	Menu( HMENU hMenu ): hMenu( hMenu ) { }
	~Menu() { hMenu = 0; }
	operator HMENU() const {
		return hMenu;
	}
	bool Load( int identifier, HINSTANCE hInstance = __MODULE_HANDLE ) {
		return (hMenu = LoadMenu( hInstance, MAKEINTRESOURCE(identifier) )) != NULL;
	}
	bool AppendGeneric( LPCTSTR lpNewItem, int id, int uFlags ) const {
		return AppendMenu( hMenu, uFlags, id, lpNewItem ) != FALSE;
	}
	bool Append( HBITMAP hBitmap, int id, int uFlags = 0 ) const {
		return AppendGeneric( windows_cast<LPCTSTR>( hBitmap ), id, MF_BITMAP | uFlags );
	}
	bool Append( LPCTSTR lpszString, int id, int uFlags = 0 ) const {
		return AppendGeneric( lpszString, id, MF_STRING | uFlags );
	}
	bool AppendSeparator() const {
		return AppendGeneric( 0, 0, MF_SEPARATOR );
	}
	bool Append( LPCTSTR lpszString, HMENU hMenu, int uFlags = 0 ) const {
		return AppendGeneric( lpszString, windows_cast<int>( hMenu ), uFlags | MF_POPUP | MF_STRING | uFlags );
	}
	bool Append( HBITMAP hBitmap, HMENU hMenu, int uFlags = 0 ) const {
		return AppendGeneric( windows_cast<LPCTSTR>( hBitmap ), windows_cast<int>( hMenu ), uFlags | MF_POPUP | MF_STRING | uFlags );
	}
	DWORD CheckItem( int pos, int id, bool bCheck = true, bool b_command = true ) const {
		return CheckMenuItem( hMenu, id, (bCheck?MF_CHECKED:MF_UNCHECKED) | (b_command?MF_BYCOMMAND:MF_BYPOSITION) );
	}
	bool CheckRadioItem( int idFirst, int idLast, int idCheck, bool b_command = true ) const {
		return CheckMenuRadioItem( hMenu, idFirst, idLast, idCheck, b_command?MF_BYCOMMAND:MF_BYPOSITION ) != FALSE;
	}
	bool Create() {
		return (hMenu = CreateMenu()) != NULL;
	}
	bool CreatePopupMenu() {
		return (hMenu = ::CreatePopupMenu()) != NULL;
	}
	bool Delete( int id, bool b_command = true ) const {
		return DeleteMenu( hMenu, id, b_command?MF_BYCOMMAND:MF_BYPOSITION ) != FALSE;
	}
	bool Destroy() {
		const bool b_ret = DestroyMenu( hMenu ) != FALSE;
		hMenu = 0;
		return b_ret;
	}
	bool EnableItem( int id, int enable, bool b_command = true ) const {
		return EnableMenuItem( hMenu, id, enable | (b_command?MF_BYCOMMAND:MF_BYPOSITION) ) != FALSE;
	}
	int GetDefaultItem( int flags = 0, bool b_command = true ) const {
		return GetMenuDefaultItem( hMenu, b_command?FALSE:TRUE, flags );
	}
#if (WINVER >= 0x0500)
	bool GetInfo( LPMENUINFO lpcmi ) const {
		return GetMenuInfo( hMenu, lpcmi ) != FALSE;
	}
#endif
	int GetItemCount() const {
		return GetMenuItemCount( hMenu );
	}
	int GetItemID( int pos ) const {
		return GetMenuItemID( hMenu, pos );
	}
	bool GetItemInfo( int id, LPMENUITEMINFO lpmii, bool b_command = true ) const {
		return GetMenuItemInfo( hMenu, id, b_command?FALSE:TRUE, lpmii ) != FALSE;
	}
	int GetState( int id, bool b_command = true ) const {
		return GetMenuState( hMenu, id, b_command?MF_BYCOMMAND:MF_BYPOSITION );
	}
#ifndef SKIP_STL
	tstring GetString( int id, bool b_command = true ) const;
#else
	int GetString( int id, LPTSTR lpString, int max_chars, bool b_command = true ) const {
		return GetMenuString( hMenu, id, lpString, max_chars, b_command?MF_BYCOMMAND:MF_BYPOSITION );
	}
#endif
	Menu GetSubMenu( int pos ) const {
		return ::GetSubMenu( hMenu, pos );
	}
	bool InsertGeneric( int pos, LPCTSTR lpNewItem, int id, int uFlags, bool b_command = true ) const {
		return InsertMenu( hMenu, pos, uFlags | (b_command?MF_BYCOMMAND:MF_BYPOSITION), id, lpNewItem ) != FALSE;
	}
	bool Insert( int pos, HBITMAP hBitmap, int id, int uFlags = 0, bool b_command = true ) const {
		return InsertGeneric( pos, windows_cast<LPCTSTR>( hBitmap ), id, MF_BITMAP | uFlags, b_command );
	}
	bool Insert( int pos, LPCTSTR lpszString, int id, int uFlags = 0, bool b_command = true ) const {
		return InsertGeneric( pos, lpszString, id, MF_STRING | uFlags, b_command );
	}
	bool InsertSeparator( int pos, bool b_command = true ) const {
		return InsertGeneric( pos, 0, 0, MF_SEPARATOR, b_command );
	}
	bool Insert( int pos, HBITMAP hBitmap, HMENU hMenu, int uFlags = 0, bool b_command = true ) const {
		return InsertGeneric( pos, windows_cast<LPCTSTR>( hBitmap ), windows_cast<int>( hMenu ), MF_BITMAP | MF_POPUP  | uFlags, b_command );
	}
	bool Insert( int pos, LPCTSTR lpszString, HMENU hMenu, int uFlags = 0, bool b_command = true ) const {
		return InsertGeneric( pos, lpszString, windows_cast<int>( hMenu ), MF_STRING | MF_POPUP | uFlags, b_command );
	}
	bool IsMenu() const {
		return ::IsMenu( hMenu ) != FALSE;
	}
	bool Remove( int id, bool b_command = true ) const {
		return RemoveMenu( hMenu, id, b_command?MF_BYCOMMAND:MF_BYPOSITION ) != FALSE;
	}
	bool SetDefaultItem( int id, bool b_command = true ) const {
		return SetMenuDefaultItem( hMenu, id, b_command?FALSE:TRUE ) != FALSE;
	}
#if (WINVER >= 0x0500)
	bool SetInfo( LPCMENUINFO lpcmi ) const {
		return SetMenuInfo( hMenu, lpcmi ) != FALSE;
	}
#endif
	bool SetItemBitmaps( int id, HBITMAP hbmUnchecked, HBITMAP hbmChecked, bool b_command = true ) const {
		return SetMenuItemBitmaps( hMenu, id, b_command?MF_BYCOMMAND:MF_BYPOSITION, hbmUnchecked, hbmChecked ) != FALSE;
	}
	bool SetItemInfo( int id, LPCMENUITEMINFO lpmii, bool b_command = true ) const {
		return SetMenuItemInfo( hMenu, id, b_command?FALSE:TRUE, lpmii ) != FALSE;
	}
	int TrackPopupMenu( const POINT& pos, HWND hWnd, int flags = TPM_LEFTALIGN | TPM_TOPALIGN | TPM_RIGHTBUTTON ) const {
		return ::TrackPopupMenu( hMenu, flags, pos.x, pos.y, 0, hWnd, 0 );
	}
	int TrackPopupMenu( const POINT& pos, HWND hWnd, LPTPMPARAMS params, int flags = TPM_LEFTALIGN | TPM_TOPALIGN | TPM_RIGHTBUTTON ) const {
		return ::TrackPopupMenuEx( hMenu, flags, pos.x, pos.y, hWnd, params );
	}
};

/**********************************************************************************************
* WindowClass
**********************************************************************************************/

class WindowClass: public NonCopyAble {
	TCHAR szClassName[16];
public:
	WindowClass( UINT style = CS_HREDRAW | CS_VREDRAW,
		int cbClsExtra = 0,
		int cbWndExtra = 0,
		HICON hIcon = LoadIcon( 0, IDI_APPLICATION ),
		HCURSOR hCursor = LoadCursor( 0, IDC_ARROW ),
		HBRUSH hbrBackground = reinterpret_cast<HBRUSH>( COLOR_WINDOW+1 ),
		LPCTSTR lpszMenuName = 0,
		HINSTANCE hInstance = __MODULE_HANDLE );
	WindowClass( HICON hIcon,
		HBRUSH hbrBackground = reinterpret_cast<HBRUSH>( COLOR_WINDOW+1 ),
		LPCTSTR lpszMenuName = 0,
		UINT style = CS_HREDRAW | CS_VREDRAW,
		HCURSOR hCursor = LoadCursor( 0, IDC_ARROW ),
		HINSTANCE hInstance = __MODULE_HANDLE );
	bool IsRegistered() const {
		return szClassName[0] != 0;
	}
	operator LPCTSTR() const {
		return szClassName;
	}
protected:
	WindowClass( WNDPROC wndProc, UINT style, int cbClsExtra, int cbWndExtra, HICON hIcon, HCURSOR hCursor, HBRUSH hbrBackground, LPCTSTR lpszMenuName, HINSTANCE hInstance = __MODULE_HANDLE );
private:
	void Register( WNDPROC wndProc, UINT style, int cbClsExtra, int cbWndExtra, HICON hIcon, HCURSOR hCursor, HBRUSH hbrBackground, LPCTSTR lpszMenuName, HINSTANCE hInstance );
};

extern WindowClass wndClassDefault;

/**********************************************************************************************
* GenericWindow
**********************************************************************************************/

class Window;

class GenericWindow {
protected:
	HWND hWnd;
public:
	GenericWindow(): hWnd( NULL ) { }
	GenericWindow( HWND hWnd ): hWnd( hWnd ) { }
	~GenericWindow() { hWnd = 0; }
	operator HWND() const {
		return hWnd;
	}
	bool Create( LPCTSTR lpszWindowName,
		LPCTSTR lpszClassName,
		DWORD dwStyle = WS_OVERLAPPEDWINDOW,
		const RECT& rc = WRect( WPoint( CW_USEDEFAULT, 0 ), CW_USEDEFAULT, 0 ),
		HWND hWndParent = 0,
		HMENU hMenu = 0,
		DWORD dwExStyle = 0,
		LPVOID lpParam = 0,
		HINSTANCE hInstance = __MODULE_HANDLE );
	bool IsCreated() const {
		return hWnd != NULL;
	}
	bool Show( int nCmdShow = SW_SHOWNORMAL ) const {
		return ShowWindow( hWnd, nCmdShow ) != FALSE;
	}
	bool ShowAsync( int nCmdShow = SW_SHOWNORMAL ) const {
		return ShowWindowAsync( hWnd, nCmdShow ) != FALSE;
	}
	bool Update() const {
		return UpdateWindow( hWnd ) != FALSE;
	}
	bool Redraw( UINT flags = RDW_INVALIDATE ) const {
		return RedrawWindow( hWnd, 0, 0, flags ) != FALSE;
	}
	bool Redraw( const RECT& rcUpdate, UINT flags = RDW_INVALIDATE ) const {
		return RedrawWindow( hWnd, &rcUpdate, 0, flags ) != FALSE;
	}
	bool Redraw( HRGN hrgnUpdate, UINT flags = RDW_INVALIDATE ) const {
		return RedrawWindow( hWnd, 0, hrgnUpdate, flags ) != FALSE;
	}
	bool Invalidate( bool bErase = true ) const {
		return ::InvalidateRect( hWnd, 0, bErase ) != FALSE;
	}
	bool InvalidateRect( const RECT& rc, bool bErase = true ) const {
		return ::InvalidateRect( hWnd, &rc, bErase ) != FALSE;
	}
	bool InvalidateRgn( HRGN hrgn, bool bErase = true ) const {
		return ::InvalidateRgn( hWnd, hrgn, bErase ) != FALSE;
	}
	bool ValidateRect( const RECT& rc ) const {
		return ::ValidateRect( hWnd, &rc ) != FALSE;
	}
	bool ValidateRgn( HRGN hrgn ) const {
		return ::ValidateRgn( hWnd, hrgn ) != FALSE;
	}
	bool Destroy();
	bool BringToTop() const {
		return BringWindowToTop( hWnd ) != FALSE;
	}
	bool SetForeground() const {
		return SetForegroundWindow( hWnd ) != FALSE;
	}
	GenericWindow ChildWindowFromPoint( const POINT& pt, UINT uFlags = CWP_ALL ) const {
		return ChildWindowFromPointEx( hWnd, pt, uFlags );
	}
	GenericWindow GetLastActivePopup() const {
		return ::GetLastActivePopup( hWnd );
	}
	GenericWindow GetNext() const {
		return GetWindow( hWnd, GW_HWNDNEXT );
	}
	GenericWindow GetPrev() const {
		return GetWindow( hWnd, GW_HWNDPREV );
	}
	GenericWindow GetFirst() const {
		return GetWindow( hWnd, GW_HWNDFIRST );
	}
	GenericWindow GetLast() const {
		return GetWindow( hWnd, GW_HWNDLAST );
	}
	GenericWindow GetChild() const {
		return GetWindow( hWnd, GW_CHILD );
	}
	GenericWindow GetTop() const {
		return GetTopWindow( hWnd );
	}
	GenericWindow GetParent() const {
		return ::GetParent( hWnd );
	}
	GenericWindow SetParent( HWND hwndNewParent ) const {
		return ::SetParent( hWnd, hwndNewParent );
	}
	bool Close() const {
		return CloseWindow( hWnd ) != FALSE;
	}
	bool IsIconic() const {
		return ::IsIconic( hWnd ) != FALSE;
	}
	bool Open() const {
		return OpenIcon( hWnd ) != FALSE;
	}
	bool Enable( bool bEnable = true ) const {
		return EnableWindow( hWnd, bEnable ) != FALSE;
	}
	bool IsEnabled() const {
		return IsWindowEnabled( hWnd ) != FALSE;
	}
	bool ShowOwnedPopups( bool bShow = true ) const {
		return ::ShowOwnedPopups( hWnd, bShow ) != FALSE;
	}
	bool GetClientRect( RECT* lpRect ) const {
		return ::GetClientRect( hWnd, lpRect ) != FALSE;
	}
	WRect GetClientRect() const {
		WRect rc( 0, 0, 0, 0 ); GetClientRect( &rc ); return rc;
	}
	bool GetWindowRect( RECT* lpRect ) const {
		return ::GetWindowRect( hWnd, lpRect ) != FALSE;
	}
	WRect GetWindowRect() const {
		WRect rc( 0, 0, 0, 0 ); GetWindowRect( &rc ); return rc;
	}
	bool IsChild( HWND hwndParent ) const {
		return ::IsChild( hwndParent, hWnd ) != FALSE;
	}
	bool IsWindow() const {
		return hWnd != NULL && ::IsWindow( hWnd ) != FALSE;
	}
	bool IsVisible() const {
		return IsWindowVisible( hWnd ) != FALSE;
	}
	bool IsZoomed() const {
		return ::IsZoomed( hWnd ) != FALSE;
	}
	bool GetPlacement( WINDOWPLACEMENT* lpwndpl ) const {
		return GetWindowPlacement( hWnd, lpwndpl ) != FALSE;
	}
	bool SetPlacement( const WINDOWPLACEMENT* lpwndpl ) const {
		return SetWindowPlacement( hWnd, lpwndpl ) != FALSE;
	}
	int GetTextLength() const {
		return GetWindowTextLength( hWnd ); 
	}
#ifndef SKIP_STL
	tstring GetText() const;
#else
	int GetText( TCHAR* buf, int buf_size ) const {
		return GetWindowText( hWnd, buf, buf_size );
	}
#endif
	bool SetText( LPCTSTR lpString ) const {
		return SetWindowText( hWnd, lpString ) != FALSE;
	}
	bool Move( const POINT& pt, int width, int height, bool bRepaint = true ) const {
		return MoveWindow( hWnd, pt.x, pt.y, width, height, bRepaint ) != FALSE;
	}
	bool Move( const RECT& rc, bool bRepaint = true ) const {
		return MoveWindow( hWnd, rc.left, rc.top, rc.right-rc.left, rc.bottom-rc.top, bRepaint ) != FALSE;
	}
	bool SetPos( const POINT& pt ) const {
		return SetWindowPos( hWnd, 0, pt.x, pt.y, 0, 0, SWP_ASYNCWINDOWPOS | SWP_NOACTIVATE | SWP_NOOWNERZORDER | SWP_NOSIZE | SWP_NOZORDER ) != FALSE;
	}
	bool SetPos( int width, int height ) const {
		return SetWindowPos( hWnd, 0, 0, 0, width, height, SWP_ASYNCWINDOWPOS | SWP_NOACTIVATE | SWP_NOMOVE | SWP_NOOWNERZORDER | SWP_NOZORDER ) != FALSE;
	}
	bool SetPos( const RECT& rc ) const {
		return SetWindowPos( hWnd, 0, rc.left, rc.top, rc.right-rc.left, rc.bottom-rc.top, SWP_ASYNCWINDOWPOS | SWP_NOACTIVATE | SWP_NOOWNERZORDER | SWP_NOZORDER ) != FALSE;
	}
	bool SetPos( HWND hwndAfter ) const {
		return SetWindowPos( hWnd, hwndAfter, 0, 0, 0, 0, SWP_ASYNCWINDOWPOS | SWP_NOMOVE | SWP_NOSIZE ) != FALSE;
	}
	bool SetPos( HWND hwndAfter, const RECT& rc, UINT uFlags ) const {
		return SetWindowPos( hWnd, hwndAfter, rc.left, rc.top, rc.right-rc.left, rc.bottom-rc.top, uFlags ) != FALSE;
	}
	bool PostMessage( UINT uMsg, WPARAM wParam = 0, LPARAM lParam = 0 ) const {
		return ::PostMessage( hWnd, uMsg, wParam, lParam ) != FALSE;
	}
	int SendMessage( UINT uMsg, WPARAM wParam = 0, LPARAM lParam = 0 ) const {
		return static_cast<int>( ::SendMessage( hWnd, uMsg, wParam, lParam ) );
	}
	bool SendMessageTimeout( UINT uMsg, WPARAM wParam, LPARAM lParam, UINT uFlags, UINT uTimeout, LPDWORD lpdwResult ) const {
		return ::SendMessageTimeout( hWnd, uMsg, wParam, lParam, uFlags, uTimeout, lpdwResult ) != FALSE;
	}
	int MessageBox( LPCTSTR lpText, LPCTSTR lpCaption, UINT uType = MB_OK | MB_ICONASTERISK ) const {
		return ::MessageBox( hWnd, lpText, lpCaption, uType );
	}
	int SendDlgItemMessage( int nIDDlgItem, UINT uMsg, WPARAM wParam, LPARAM lParam ) const {
		return static_cast<int>( ::SendDlgItemMessage( hWnd, nIDDlgItem, uMsg, wParam, lParam ) );
	}
	int GetDlgCtrlID() const {
		return ::GetDlgCtrlID( hWnd );
	}
	GenericWindow GetDlgItem( int nIDDlgItem ) const {
		return ::GetDlgItem( hWnd, nIDDlgItem );
	}
	GenericWindow GetNextDlgGroupItem( HWND hCtl ) const {
		return ::GetNextDlgGroupItem( hWnd, hCtl, FALSE );
	}
	GenericWindow GetPrevDlgGroupItem( HWND hCtl ) const {
		return ::GetNextDlgGroupItem( hWnd, hCtl, TRUE );
	}
	GenericWindow GetNextDlgTabItem( HWND hCtl ) const {
		return ::GetNextDlgTabItem( hWnd, hCtl, FALSE );
	}
	GenericWindow GetPrevDlgTabItem( HWND hCtl ) const {
		return ::GetNextDlgTabItem( hWnd, hCtl, TRUE );
	}
	int GetDlgItemInt( int nIDDlgItem, bool* pbTranslated = 0 ) const;
	unsigned GetDlgItemUInt( int nIDDlgItem, bool* pbTranslated = 0 ) const;
	bool SetDlgItemInt( int nIDDlgItem, int value ) const {
		return ::SetDlgItemInt( hWnd, nIDDlgItem, value, TRUE ) != FALSE;
	}
	bool SetDlgItemInt( int nIDDlgItem, unsigned value ) const {
		return ::SetDlgItemInt( hWnd, nIDDlgItem, value, FALSE ) != FALSE;
	}
	int GetDlgItemText( int nIDDlgItem, LPTSTR lpString, int nMaxCount ) const {
		return ::GetDlgItemText( hWnd, nIDDlgItem, lpString, nMaxCount );
	}
	bool SetDlgItemText( int nIDDlgItem, LPCTSTR lpString ) const {
		return ::SetDlgItemText( hWnd, nIDDlgItem, lpString ) != FALSE;
	}
	bool MapDialogRect( RECT* lpRect ) const {
		return ::MapDialogRect( hWnd, lpRect ) != FALSE;
	}
	bool CheckDlgButton( int nIDButton, UINT uCheck = BST_CHECKED ) const {
		return ::CheckDlgButton( hWnd, nIDButton, uCheck ) != FALSE;
	}
	bool CheckRadioButton( int nIDFirst, int nIDLast, int nIDCheck ) const {
		return ::CheckRadioButton( hWnd, nIDFirst, nIDLast, nIDCheck ) != FALSE;
	}
	int IsDlgButtonChecked( int nIDButton ) const {
		return ::IsDlgButtonChecked( hWnd, nIDButton );
	}
	bool EnableScrollBar( UINT wSBFlags, UINT wArrows ) const {
		return ::EnableScrollBar( hWnd, wSBFlags, wArrows = ESB_ENABLE_BOTH ) != FALSE;
	}
	bool GetScrollInfo( int fnBar, LPSCROLLINFO lpsi ) const {
		return ::GetScrollInfo( hWnd, fnBar, lpsi ) != FALSE;
	}
	int SetScrollInfo( int fnBar, LPSCROLLINFO lpsi, bool bRedraw = true ) const {
		return ::SetScrollInfo( hWnd, fnBar, lpsi, bRedraw );
	}
	bool ShowScrollBar( int wBar, bool bShow = true ) const {
		return ::ShowScrollBar( hWnd, wBar, bShow ) != FALSE;
	}
	bool Scroll( int dx, int dy, UINT uFlags, LPCRECT rcScroll = 0, LPCRECT rcClip = 0, HRGN hrgnUpdate = 0, LPRECT rcUpdate = 0 ) const {
		return ::ScrollWindowEx( hWnd, dx, dy, rcScroll, rcClip, hrgnUpdate, rcUpdate, uFlags ) != FALSE;
	}
	void SetFont( HFONT hFont, bool bRedraw = true ) const {
		SendMessage( WM_SETFONT, reinterpret_cast<WPARAM>( hFont ), bRedraw );
	}
	HFONT GetFont() const {
		return windows_cast<HFONT>( SendMessage( WM_GETFONT ) );
	}
	HICON SetIcon( HICON hIcon, WPARAM type = ICON_BIG ) const {
		return windows_cast<HICON>( SendMessage( WM_SETICON, type, reinterpret_cast<LPARAM>( hIcon ) ) );
	}
	HICON GetIcon( WPARAM type = ICON_BIG ) const {
		return windows_cast<HICON>( SendMessage( WM_GETICON, type ) );
	}
	bool ScreenToClient( POINT& pt ) const {
		return ::ScreenToClient( hWnd, &pt ) != FALSE;
	}
	bool ClientToScreen( POINT& pt ) const {
		return ::ClientToScreen( hWnd, &pt ) != FALSE;
	}
	bool ScreenToClient( RECT& rc ) const {
		return ScreenToClient( *reinterpret_cast<POINT*>( &rc.left ) )
			&& ScreenToClient( *reinterpret_cast<POINT*>( &rc.right ) );
	}
	bool ClientToScreen( RECT& rc ) const {
		return ClientToScreen( *reinterpret_cast<POINT*>( &rc.left ) )
			&& ClientToScreen( *reinterpret_cast<POINT*>( &rc.right ) );
	}
	GenericWindow SetFocus() const {
		return ::SetFocus( hWnd );
	}
	GenericWindow SetActive() const {
		return SetActiveWindow( hWnd );
	}
	GenericWindow SetCapture() const {
		return ::SetCapture( hWnd );
	}
	WRect GetWindowRectOnParent() const {
		WRect rc = GetWindowRect();
		GetParent().ScreenToClient( rc );
		return rc;
	}
	Menu GetMenu() const {
		return ::GetMenu( hWnd );
	}
	bool DrawMenuBar() const {
		return ::DrawMenuBar( hWnd ) != FALSE;
	}
	Menu GetSystemMenu( bool b_revert = false ) const {
		return ::GetSystemMenu( hWnd, b_revert );
	}
	bool SetMenu( HMENU hMenu ) const {
		return ::SetMenu( hWnd, hMenu ) != FALSE;
	}
	bool DrawAnimatedRects( int idAni, const RECT& rcFrom, const RECT& rcTo ) const {
		return ::DrawAnimatedRects( hWnd, idAni, &rcFrom, &rcTo ) != FALSE;
	}
	template< class T >
	T* GetObject() {
		return dynamic_cast< T* >( windows_cast< Window* >( GetWindowLong( hWnd, GWL_USERDATA ) ) );
	}
};

inline bool GenericWindow::Create( LPCTSTR lpszWindowName, LPCTSTR lpszClassName, DWORD dwStyle, const RECT& rc, HWND hWndParent, HMENU hMenu, DWORD dwExStyle, LPVOID lpParam, HINSTANCE hInstance )
{
	if ( IsCreated() )
		return true;
	HWND hwndTmp = CreateWindowEx( dwExStyle, lpszClassName, lpszWindowName, dwStyle,
			rc.left, rc.top, rc.right-rc.left, rc.bottom-rc.top, hWndParent, hMenu, hInstance, lpParam );
	if ( hwndTmp != NULL )
		hWnd = hwndTmp;
	return hwndTmp != NULL;
}

inline bool GenericWindow::Destroy()
{
	if ( DestroyWindow( hWnd ) ) {
		hWnd = NULL;
		return true;
	} else
		return false;
}

inline int GenericWindow::GetDlgItemInt( int nIDDlgItem, bool* pbTranslated ) const
{
	BOOL bTranslated = FALSE;
	int ret = ::GetDlgItemInt( hWnd, nIDDlgItem, &bTranslated, TRUE );
	if ( pbTranslated != 0)
		*pbTranslated = bTranslated != FALSE;
	return ret;
}

inline unsigned GenericWindow::GetDlgItemUInt( int nIDDlgItem, bool* pbTranslated ) const
{
	BOOL bTranslated = FALSE;
	unsigned ret = ::GetDlgItemInt( hWnd, nIDDlgItem, &bTranslated, FALSE );
	if ( pbTranslated != 0)
		*pbTranslated = bTranslated != FALSE;
	return ret;
}

/**********************************************************************************************
* System windows
**********************************************************************************************/

class DesktopWindow: public GenericWindow {
public:
	DesktopWindow(): GenericWindow( GetDesktopWindow() ) { }
};

class ForegroundWindow: public GenericWindow {
public:
	ForegroundWindow(): GenericWindow( GetForegroundWindow() ) { }
};

/**********************************************************************************************
* Window
**********************************************************************************************/

class Window: public GenericWindow, public NonCopyAble {
protected:
	virtual ~Window();
	bool Destroy() {
		return DestroyWindow( hWnd ) != FALSE;
	}
	bool b_created; // prevents deleting during creating the window
public:
	Window(): b_created( false ) { }
	static void Delete( Window* pWindow ) { delete pWindow; }
	bool Create( LPCTSTR lpszWindowName,
				 const WindowClass& wndClass,
				 DWORD dwStyle = WS_OVERLAPPEDWINDOW,
				 const RECT& rc = WRect( WPoint( CW_USEDEFAULT, 0 ), CW_USEDEFAULT, 0 ),
				 HWND hWndParent = 0,
				 HMENU hMenu = 0,
				 DWORD dwExStyle = 0,
				 LPVOID lpParam = 0,
				 HINSTANCE hInstance = __MODULE_HANDLE );
	bool Create( LPCTSTR lpszWindowName,
				 DWORD dwStyle = WS_OVERLAPPEDWINDOW,
				 const RECT& rc = WRect( WPoint( CW_USEDEFAULT, 0 ), CW_USEDEFAULT, 0 ),
				 HWND hWndParent = 0,
				 HMENU hMenu = 0,
				 DWORD dwExStyle = 0,
				 LPVOID lpParam = 0,
				 HINSTANCE hInstance = __MODULE_HANDLE ) {
		return Create( lpszWindowName, wndClassDefault, dwStyle, rc, hWndParent, hMenu, dwExStyle, lpParam, hInstance );
	}
	bool IsCreated() const {
		return b_created;
	}
	virtual int HandleMessage( int uMsg, int wParam, int lParam );
#ifndef PURE_WRAPPER
	virtual bool OnCreate( LPCREATESTRUCT lpCreateStruct = 0 );
	virtual void OnClose();
	virtual void OnDestroy();
	virtual void OnCommand( int nIdentifier, int nNotifyCode, HWND hwndControl );
	virtual int OnNotify( int nIdentifier, LPNMHDR pnmh );
	virtual void OnPaint();
	virtual void OnSize( int sizing, WSize new_size );
	virtual void OnMouseMove( WPoint point, int keys );
	virtual void OnMouseDown( WPoint point, int keys, int button );
	virtual void OnMouseUp( WPoint point, int keys, int button );
	virtual void OnMouseDblClk( WPoint point, int keys, int button );
	virtual void OnKeyDown( int key, int keyData );
	virtual void OnKeyUp( int key, int keyData );

	// Mouse button indices passed to OnMouseXXX()
	enum {
		left_button,
		right_button,
		middle_button
	};
#endif
};

/**********************************************************************************************
* Dialog
**********************************************************************************************/

// In opposite to Window, Dialog never receives WM_CREATE, WM_KEYDOWN or WM_KEYUP.
// In addition to that, a modal dialog never receives WM_PAINT.
// A dialog receives WM_INITDIALOG (OnInitDialog()) instead of WM_CREATE (OnCreate()).
// Dialog class is modal, ModelessDialog class is modeless.
// A modeless dialog behaves like a window, except that it must call Destroy()
// instead of EndDialog() to close itself.

class Dialog: public Window {
protected:
	LPCTSTR lpTemplate;
	const bool bModal;

	virtual ~Dialog();
	bool EndDialog( int nResult = 0 );
	Dialog( bool bModal ): bModal( bModal ) { }
public:
	// Modal dialog
	Dialog(): lpTemplate( 0 ), bModal( true ) { }
	Dialog( LPCTSTR lpTemplate ): lpTemplate( lpTemplate ), bModal( true ) { }
	Dialog( int lpTemplate ): lpTemplate( MAKEINTRESOURCE( lpTemplate ) ), bModal( true ) { }
	int Go( HWND hwndParent = 0, LPARAM lParam = 0, HINSTANCE hInstance = __MODULE_HANDLE );
	int Go( LPCDLGTEMPLATE hDialogTemplate, HWND hwndParent = 0, LPARAM lParam = 0, HINSTANCE hInstance = __MODULE_HANDLE );
#ifndef PURE_WRAPPER
	// New handler
	virtual bool OnInitDialog( HWND hwndFocus, LPARAM lParam );
#endif
	// Empty implementations for dialogs
	virtual int HandleMessage( int uMsg, int wParam, int lParam );
#ifndef PURE_WRAPPER
	virtual void OnDestroy();
	virtual void OnCommand( int nIdentifier, int nNotifyCode, HWND hwndControl );
	virtual int OnNotify( int nIdentifier, LPNMHDR pnmh );
	virtual void OnPaint();
	virtual void OnSize( int sizing, WSize new_size );
	virtual void OnMouseMove( WPoint point, int keys );
	virtual void OnMouseDown( WPoint point, int keys, int button );
	virtual void OnMouseUp( WPoint point, int keys, int button );
	virtual void OnMouseDblClk( WPoint point, int keys, int button );

	// Unused Handlers
private:
	virtual bool OnCreate( LPCREATESTRUCT lpCreateStruct = 0 );
	virtual void OnClose();
	virtual void OnKeyDown( int key, int keyData );
	virtual void OnKeyUp( int key, int keyData );
#endif
};

class ModelessDialog: public Dialog {
protected:
	ModelessDialog(): Dialog( false ) { }
	void Create( LPCTSTR lpTemplate, HWND hWndParent, LPARAM lParam = 0, HINSTANCE hInstance = __MODULE_HANDLE );
	void Create( int lpTemplate, HWND hWndParent, LPARAM lParam = 0, HINSTANCE hInstance = __MODULE_HANDLE );
	void Create( LPCDLGTEMPLATE hDialogTemplate, HWND hWndParent, LPARAM lParam = 0, HINSTANCE hInstance = __MODULE_HANDLE );
};

/**********************************************************************************************
* MDIFrameWindow
**********************************************************************************************/

// MDIFrameWindow is the frame window for a MDI application.
// The only difference between it and Window is that during OnCreate(),
// one must call CreateClient() in order to create the client window.

class MDIFrameWindow: public Window {
	GenericWindow client_wnd;
protected:
	virtual ~MDIFrameWindow();
public:
	MDIFrameWindow() { }
	bool CreateClient( const RECT& rc = WRect(0,0,0,0), HMENU hMenu = 0, int idFirstChild = 10001, DWORD dwStyle = WS_CHILD | WS_VISIBLE | WS_CLIPCHILDREN | WS_VSCROLL | WS_HSCROLL, DWORD dwExStyle = WS_EX_CLIENTEDGE );
	GenericWindow GetClientWindow() const { return client_wnd; }
	virtual int HandleMessage( int uMsg, int wParam, int lParam );
#ifndef PURE_WRAPPER
	virtual void OnClose();
	virtual int OnNotify( int nIdentifier, LPNMHDR pnmh );
	virtual void OnCommand( int nIdentifier, int nNotifyCode, HWND hwndControl );
	virtual void OnPaint();
	virtual void OnSize( int sizing, WSize new_size );
	virtual void OnMouseMove( WPoint point, int keys );
	virtual void OnMouseDown( WPoint point, int keys, int button );
	virtual void OnMouseUp( WPoint point, int keys, int button );
	virtual void OnMouseDblClk( WPoint point, int keys, int button );
	virtual void OnKeyDown( int key, int keyData );
	virtual void OnKeyUp( int key, int keyData );
#endif
	void MDIActivate( HWND hwndChild ) const {
		client_wnd.SendMessage( WM_MDIACTIVATE, reinterpret_cast<WPARAM>( hwndChild ) );
	}
	bool MDICascade( WPARAM flags = 0 ) const {
		return client_wnd.SendMessage( WM_MDICASCADE, flags ) != FALSE;
	}
	GenericWindow MDICreate( LPMDICREATESTRUCT pmcs ) const {
		return windows_cast<HWND>( client_wnd.SendMessage( WM_MDICREATE, 0, windows_cast<LPARAM>( pmcs ) ) );
	}
	void MDIDestroy( HWND hwndChild ) const {
		client_wnd.SendMessage( WM_MDIDESTROY, reinterpret_cast<WPARAM>( hwndChild ) );
	}
	GenericWindow MDIGetActive() const {
		return windows_cast<HWND>( client_wnd.SendMessage( WM_MDIGETACTIVE ) );
	}
	GenericWindow MDIGetActive( bool* pb_maximized ) const {
		BOOL b_maximized = *pb_maximized ? TRUE : FALSE;
		const GenericWindow wnd = windows_cast<HWND>( client_wnd.SendMessage( WM_MDIGETACTIVE, 0, reinterpret_cast<LPARAM>(&b_maximized) ) );
		*pb_maximized = b_maximized != FALSE;
		return wnd;
	}
	void MDIIconArrange() const {
		client_wnd.SendMessage( WM_MDIICONARRANGE );
	}
	void MDIMaximize( HWND hwndChild ) const {
		client_wnd.SendMessage( WM_MDIMAXIMIZE, reinterpret_cast<WPARAM>( hwndChild ) );
	}
	void MDINext( HWND hwndChild = 0 ) const {
		client_wnd.SendMessage( WM_MDINEXT, reinterpret_cast<WPARAM>( hwndChild ), 0 );
	}
	void MDIPrevious( HWND hwndChild = 0 ) const {
		client_wnd.SendMessage( WM_MDINEXT, reinterpret_cast<WPARAM>( hwndChild ), 1 );
	}
	Menu MDIRefreshMenu() const {
		return windows_cast<HMENU>( client_wnd.SendMessage( WM_MDIREFRESHMENU ) );
	}
	void MDIRestore( HWND hwndChild ) const {
		client_wnd.SendMessage( WM_MDIRESTORE, reinterpret_cast<WPARAM>( hwndChild ) );
	}
	Menu MDISetMenu( HMENU hmenuFrameWindow = 0, HMENU hmenuWindow = 0 ) const {
		return windows_cast<HMENU>( client_wnd.SendMessage( WM_MDISETMENU, reinterpret_cast<WPARAM>( hmenuFrameWindow ), reinterpret_cast<LPARAM>( hmenuWindow ) ) );
	}
	bool MDITile( WPARAM flags = MDITILE_HORIZONTAL ) const {
		return client_wnd.SendMessage( WM_MDITILE, flags ) != FALSE;
	}
};

/**********************************************************************************************
* MDIChildWindowClass
**********************************************************************************************/

class MDIChildWindowClass: public WindowClass {
public:
	MDIChildWindowClass( UINT style = CS_HREDRAW | CS_VREDRAW,
		int cbClsExtra = 0,
		int cbWndExtra = 0,
		HICON hIcon = LoadIcon( 0, IDI_APPLICATION ),
		HCURSOR hCursor = LoadCursor( 0, IDC_ARROW ),
		HBRUSH hbrBackground = reinterpret_cast<HBRUSH>( COLOR_WINDOW+1 ),
		LPCTSTR lpszMenuName = 0,
		HINSTANCE hInstance = __MODULE_HANDLE );
};

extern MDIChildWindowClass mdiChildWndClassDefault;

/**********************************************************************************************
* MDIChildWindow
**********************************************************************************************/

class MDIChildWindow: public Window {
protected:
	virtual ~MDIChildWindow();
	bool Destroy() {
		return GetParent().SendMessage( WM_MDIDESTROY, reinterpret_cast<WPARAM>( static_cast<HWND>( *this ) ) ) != 0;
	}
public:
	MDIChildWindow() { }
	enum { WS_MDICHILD = MDIS_ALLCHILDSTYLES | WS_CLIPCHILDREN };
	bool Create( LPCTSTR lpszWindowName,
				 const MDIChildWindowClass& wndClass,
				 HWND hWndParent = 0,
				 const RECT& rc = WRect( WPoint( CW_USEDEFAULT, 0 ), CW_USEDEFAULT, 0 ),
				 HMENU hMenu = 0,
				 DWORD dwStyle = WS_MDICHILD,
				 DWORD dwExStyle = 0,
				 LPVOID lpParam = 0,
				 HINSTANCE hInstance = __MODULE_HANDLE );
	bool Create( LPCTSTR lpszWindowName,
				 HWND hWndParent = 0,
				 const RECT& rc = WRect( WPoint( CW_USEDEFAULT, 0 ), CW_USEDEFAULT, 0 ),
				 HMENU hMenu = 0,
				 DWORD dwStyle = WS_MDICHILD,
				 DWORD dwExStyle = 0,
				 LPVOID lpParam = 0,
				 HINSTANCE hInstance = __MODULE_HANDLE ) {
		return Create( lpszWindowName, mdiChildWndClassDefault, hWndParent, rc, hMenu, dwStyle, dwExStyle | WS_EX_MDICHILD, lpParam, hInstance );
	}
	virtual int HandleMessage( int uMsg, int wParam, int lParam );
#ifndef PURE_WRAPPER
	virtual void OnClose();
	virtual int OnNotify( int nIdentifier, LPNMHDR pnmh );
	virtual void OnPaint();
	virtual void OnSize( int sizing, WSize new_size );
	virtual void OnMouseMove( WPoint point, int keys );
	virtual void OnMouseDown( WPoint point, int keys, int button );
	virtual void OnMouseUp( WPoint point, int keys, int button );
	virtual void OnMouseDblClk( WPoint point, int keys, int button );
	virtual void OnKeyDown( int key, int keyData );
	virtual void OnKeyUp( int key, int keyData );
#endif
};

/**********************************************************************************************
* MessageQueue
**********************************************************************************************/

class MessageQueue: public MSG, public NonCopyAble {
public:
	MessageQueue() { }
	MessageQueue( const MSG& msg ) { *static_cast<MSG*>(this) = msg; }
	bool GetMessage( HWND hWnd = 0, UINT wMsgFilterMin = 0, UINT wMsgFilterMax = 0 ) {
		return ::GetMessage( this, hWnd, wMsgFilterMin, wMsgFilterMax ) != FALSE;
	}
	bool PeekMessage( UINT wRemoveMsg = PM_REMOVE, HWND hWnd = 0, UINT wMsgFilterMin = 0, UINT wMsgFilterMax = 0 ) {
		return ::PeekMessage( this, hWnd, wMsgFilterMin, wMsgFilterMax, wRemoveMsg ) != FALSE;
	}
	bool TranslateMessage() {
		return ::TranslateMessage( this ) != FALSE;
	}
	int DispatchMessage() {
		return static_cast<int>( ::DispatchMessage( this ) );
	}
	bool TranslateMDISysAccel( HWND hWndClient ) {
		return ::TranslateMDISysAccel( hWndClient, this ) != FALSE;
	}
	bool TranslateAccelerator( HWND hWnd, HACCEL hAccTable ) {
		return ::TranslateAccelerator( hWnd, hAccTable, this ) != FALSE;
	}
	bool IsDialogMessage( HWND hWnd ) {
		return ::IsDialogMessage( hWnd, this ) != FALSE;
	}
};

/**********************************************************************************************
* RegKey
**********************************************************************************************/

class RegKey: public NonCopyAble {
	HKEY hKey;
public:
	RegKey(): hKey( 0 ) { }
	RegKey( HKEY hKey ): hKey( hKey ) { }
	RegKey( HKEY hMainKey, LPCTSTR lpSubKey, DWORD samDesired = KEY_ALL_ACCESS ): hKey( 0 ) {
		Open( hMainKey, lpSubKey, samDesired );
	}
	~RegKey() { Close(); }
	operator HKEY() const { return hKey; }
	int Open( HKEY hMainKey, LPCTSTR lpSubKey, DWORD samDesired = KEY_ALL_ACCESS ) {
		Close();
		return RegOpenKeyEx( hMainKey, lpSubKey, 0, samDesired, &hKey );
	}
	int OpenUser( LPCTSTR lpSubKey, DWORD samDesired = KEY_ALL_ACCESS ) {
		return Open( HKEY_CURRENT_USER, lpSubKey, samDesired );
	}
	int OpenMachine( LPCTSTR lpSubKey, DWORD samDesired = KEY_ALL_ACCESS ) {
		return Open( HKEY_LOCAL_MACHINE, lpSubKey, samDesired );
	}
	int Close() {
		int retval = ERROR_SUCCESS;
		if ( hKey != 0 ) retval = RegCloseKey( hKey ), hKey = 0;
		return retval;
	}
	int Create( HKEY hMainKey, LPCTSTR lpSubKey, DWORD samDesired = KEY_ALL_ACCESS, LPDWORD lpdwDisposition = 0, DWORD dwOptions = 0, LPSECURITY_ATTRIBUTES lpSecurityAttributes = 0 ) {
		Close();
		return RegCreateKeyEx( hMainKey, lpSubKey, 0, NULL, dwOptions, samDesired, lpSecurityAttributes, &hKey, lpdwDisposition );
	}
	int CreateUser( LPCTSTR lpSubKey, DWORD samDesired = KEY_ALL_ACCESS, LPDWORD lpdwDisposition = 0, DWORD dwOptions = 0, LPSECURITY_ATTRIBUTES lpSecurityAttributes = 0 ) {
		return Create( HKEY_CURRENT_USER, lpSubKey, samDesired, lpdwDisposition, dwOptions, lpSecurityAttributes );
	}
	int CreateMachine( LPCTSTR lpSubKey, DWORD samDesired = KEY_ALL_ACCESS, LPDWORD lpdwDisposition = 0, DWORD dwOptions = 0, LPSECURITY_ATTRIBUTES lpSecurityAttributes = 0 ) {
		return Create( HKEY_LOCAL_MACHINE, lpSubKey, samDesired, lpdwDisposition, dwOptions, lpSecurityAttributes );
	}
	int Delete( LPCTSTR lpSubKey ) {
		const int retval = RegDeleteKey( hKey, lpSubKey );
		hKey = 0;
		return retval;
	}
	int DeleteKeyTree( LPCTSTR lpSubKey );
	int DeleteValue( LPCTSTR lpValueName ) const {
		return RegDeleteValue( hKey, lpValueName );
	}
#ifdef SKIP_STL
	int EnumKey( DWORD dwIndex, LPTSTR lpName, LPDWORD lpcName, PFILETIME lpftLastWriteTime = 0 ) const {
		return RegEnumKeyEx( hKey, dwIndex, lpName, lpcName, 0, 0, 0, lpftLastWriteTime );
	}
#else
	int EnumKey( DWORD dwIndex, tstring* p_name, PFILETIME lpftLastWriteTime = 0 ) const;
	struct ENUMKEY {
		tstring name;
		tstring key_class;
		FILETIME last_file_time;
	};
	typedef vector< ENUMKEY > ENUMKEYS;
	int EnumKeys( ENUMKEYS* p_keys ) const;
	int EnumKeys( vector<tstring>* p_keys ) const;
#endif
	int QueryInfo( LPDWORD lpcSubKeys, LPDWORD lpcbMaxSubKeyLen = 0, LPDWORD lpcValues = 0, LPDWORD lpcbMaxValueNameLen = 0, LPDWORD lpcbMaxValueLen = 0, LPDWORD lpcbSecurityDescriptor = 0, PFILETIME lpftLastWriteTime = 0 ) const {
		return RegQueryInfoKey( hKey, 0, 0, 0, lpcSubKeys, lpcbMaxSubKeyLen, 0, lpcValues, lpcbMaxValueNameLen, lpcbMaxValueLen, lpcbSecurityDescriptor, lpftLastWriteTime );
	}
	int Flush() const {
		return RegFlushKey( hKey );
	}
	int Load( LPCTSTR lpSubKey, LPCTSTR lpszFilename ) const {
		return RegLoadKey( hKey, lpSubKey, lpszFilename );
	}
	int NotifyChangeValueAsync( HANDLE hEvent, DWORD dwNotifyFilter = REG_NOTIFY_CHANGE_NAME | REG_NOTIFY_CHANGE_ATTRIBUTES | REG_NOTIFY_CHANGE_LAST_SET, bool bWatchSubTree = true ) const {
		return RegNotifyChangeKeyValue( hKey, bWatchSubTree, dwNotifyFilter, hEvent, TRUE );
	}
	int NotifyChangeValue( DWORD dwNotifyFilter = REG_NOTIFY_CHANGE_NAME | REG_NOTIFY_CHANGE_ATTRIBUTES | REG_NOTIFY_CHANGE_LAST_SET, bool bWatchSubTree = true ) const {
		return RegNotifyChangeKeyValue( hKey, bWatchSubTree, dwNotifyFilter, 0, FALSE );
	}
	int QueryValueSize( LPCTSTR lpValueName, LPDWORD lpcbData, LPDWORD lpType = 0 ) const {
		return RegQueryValueEx( hKey, lpValueName, 0, lpType, 0, lpcbData );
	}
	int QueryValue( LPCTSTR lpValueName, void* lpData, LPDWORD lpcbData, LPDWORD lpType = 0 ) const {
		return RegQueryValueEx( hKey, lpValueName, 0, lpType, static_cast<LPBYTE>( lpData ), lpcbData );
	}
	template< typename T >
	int QueryValue( LPCTSTR lpValueName, T* lpValue ) const {
		DWORD data_size = sizeof( T );
		return QueryValue( lpValueName, lpValue, &data_size );
	}
#ifndef SKIP_STL
	int QueryValue( LPCTSTR lpValueName, tstring* p_string, LPDWORD lpType = 0 ) const;
#endif
	int Replace( LPCTSTR lpSubKey, LPCTSTR lpNewFile, LPCTSTR lpOldFile ) const {
		return RegReplaceKey( hKey, lpSubKey, lpNewFile, lpOldFile );
	}
	int Restore( LPCTSTR lpFile, DWORD dwFlags = 0 ) const {
		return RegRestoreKey( hKey, lpFile, dwFlags );
	}
	int Save( LPCTSTR lpFile, LPSECURITY_ATTRIBUTES lpSecurityAttributes = 0 ) const {
		return RegSaveKey( hKey, lpFile, lpSecurityAttributes );
	}
	int SetValue( LPCTSTR lpValueName, DWORD dwType, const void* lpData, DWORD cbData ) const {
		return RegSetValueEx( hKey, lpValueName, 0, dwType, static_cast<const BYTE*>( lpData ), cbData );
	}
	int SetValue( LPCTSTR lpValueName, const char* str ) const {
		return SetValue( lpValueName, REG_SZ, str, static_cast<DWORD>( strlen( str ) + 1 ) );
	}
	int SetValue( LPCTSTR lpValueName, int value ) const {
		return SetValue( lpValueName, REG_DWORD, &value, sizeof( int ) );
	}
	int SetValue( LPCTSTR lpValueName, unsigned value ) const {
		return SetValue( lpValueName, REG_DWORD, &value, sizeof( unsigned ) );
	}
	int SetValue( LPCTSTR lpValueName, float value ) const {
		return SetValue( lpValueName, REG_DWORD, &value, sizeof( float ) );
	}
	template< typename T >
	int SetValue( LPCTSTR lpValueName, const T& value ) const {
		return SetValue( lpValueName, REG_BINARY, &value, sizeof( T ) );
	}
};

/**********************************************************************************************
* DC
**********************************************************************************************/

class DC {
protected:
	HDC hdc;
public:
	DC( HDC hdc ): hdc( hdc ) { }
	bool IsValid() const {
		return hdc != NULL;
	}
	operator HDC() const {
		return hdc;
	}
	bool Cancel() const {
		return CancelDC( hdc ) != FALSE;
	}
	COLORREF SetPixel( const POINT& point, COLORREF color ) {
		return ::SetPixel( hdc, point.x, point.y, color );
	}
	bool PatBlt( const RECT& rc, DWORD dwROP = PATCOPY ) const {
		return ::PatBlt( hdc, rc.left, rc.right, rc.right-rc.left, rc.bottom-rc.top, dwROP ) != FALSE;
	}
	bool BitBlt( const RECT& rcDest, HDC src, const POINT& ptSrc = WPoint( 0, 0 ), DWORD dwROP = SRCCOPY ) const {
		return ::BitBlt( hdc, rcDest.left, rcDest.top, rcDest.right-rcDest.left, rcDest.bottom-rcDest.top, src, ptSrc.x, ptSrc.y, dwROP ) != FALSE;
	}
	bool StretchBlt( const RECT& rcDest, HDC src, const RECT& rcSrc, DWORD dwROP = SRCCOPY ) const {
		return ::StretchBlt( hdc, rcDest.left, rcDest.top, rcDest.right-rcDest.left, rcDest.bottom-rcDest.top, src, rcSrc.left, rcSrc.top, rcSrc.right-rcSrc.left, rcSrc.bottom-rcSrc.top, dwROP ) != FALSE;
	}
	int FillRect( const RECT& rc, HBRUSH hbr ) const {
		return ::FillRect( hdc, &rc, hbr );
	}
	int FrameRect( const RECT& rc, HBRUSH hbr ) const {
		return ::FrameRect( hdc, &rc, hbr );
	}
	int InvertRect( const RECT& rc ) const {
		return ::InvertRect( hdc, &rc );
	}
	bool Ellipse( const RECT& rc ) const {
		return ::Ellipse( hdc, rc.left, rc.top, rc.right, rc.bottom ) != FALSE;
	}
	bool Rectangle( const RECT& rc ) const {
		return ::Rectangle( hdc, rc.left, rc.top, rc.right, rc.bottom ) != FALSE;
	}
	bool RoundRect( const RECT& rc, int nWidth = 1, int nHeight = 1 ) const {
		return ::RoundRect( hdc, rc.left, rc.top, rc.right, rc.bottom, nWidth, nHeight ) != FALSE;
	}
	bool Chord( const RECT& rc, const POINT& ptRadial1, const POINT& ptRadial2 ) const {
		return ::Chord( hdc, rc.left, rc.top, rc.right, rc.bottom, ptRadial1.x, ptRadial1.y, ptRadial2.x, ptRadial2.y ) != FALSE;
	}
	bool Pie( const RECT& rc, const POINT& ptRadial1, const POINT& ptRadial2 ) const {
		return ::Pie( hdc, rc.left, rc.top, rc.right, rc.bottom, ptRadial1.x, ptRadial1.y, ptRadial2.x, ptRadial2.y ) != FALSE;
	}
	bool Polygon( const POINT* pts, int nCount ) const {
		return ::Polygon( hdc, pts, nCount ) != FALSE;
	}
	bool PolyPolygon( const POINT* pts, const int* lpPolyCounts, int nCount ) const {
		return ::PolyPolygon( hdc, pts, lpPolyCounts, nCount ) != FALSE;
	}
	bool Arc( const RECT& rc, const POINT& ptRadial1, const POINT& ptRadial2 ) const {
		return ::Arc( hdc, rc.left, rc.top, rc.right, rc.bottom, ptRadial1.x, ptRadial1.y, ptRadial2.x, ptRadial2.y ) != FALSE;
	}
	bool AngleArc( int X, int Y, DWORD dwRadius, FLOAT eStartAngle, FLOAT eSweepAngle ) const {
		return ::AngleArc( hdc, X, Y, dwRadius, eStartAngle, eSweepAngle ) != FALSE;
	}
	int SetArcDirection( int ArcDirection ) const {
		return ::SetArcDirection( hdc, ArcDirection );
	}
	bool MoveTo( const POINT& pt ) const {
		return MoveToEx( hdc, pt.x, pt.y, 0 ) != FALSE;
	}
	bool MoveTo( const POINT& pt, POINT* ptOld ) const {
		return MoveToEx( hdc, pt.x, pt.y, ptOld ) != FALSE;
	}
	bool LineTo( const POINT& pt ) const {
		return ::LineTo( hdc, pt.x, pt.y ) != FALSE;
	}
	bool Polyline( const POINT* pts, int nCount ) const {
		return ::Polyline( hdc, pts, nCount ) != FALSE;
	}
	bool PolylineTo( const POINT* pts, int nCount ) const {
		return ::PolylineTo( hdc, pts, nCount ) != FALSE;
	}
	bool PolyBezier( const POINT* pts, int nCount ) const {
		return ::PolyBezier( hdc, pts, nCount ) != FALSE;
	}
	bool PolyBezierTo( const POINT* pts, int nCount ) const {
		return ::PolyBezierTo( hdc, pts, nCount ) != FALSE;
	}
	bool PolyDraw( const POINT* pts, const BYTE* lpbTypes, int nCount ) const {
		return ::PolyDraw( hdc, pts, lpbTypes, nCount ) != FALSE;
	}
	COLORREF SetBkColor( COLORREF crColor ) const {
		return ::SetBkColor( hdc, crColor );
	}
	int SetBkMode( int iBkMode ) const {
		return ::SetBkMode( hdc, iBkMode );
	}
	COLORREF GetBkColor() const {
		return ::GetBkColor( hdc );
	}
	int GetBkMode() const {
		return ::GetBkMode( hdc );
	}
	bool InvertRgn( HRGN hrgn ) const {
		return ::InvertRgn( hdc, hrgn ) != FALSE;
	}
	bool PaintRgn( HRGN hrgn ) const {
		return ::PaintRgn( hdc, hrgn ) != FALSE;
	}
	bool FillRgn( HRGN hrgn, HBRUSH hbr ) const {
		return ::FillRgn( hdc, hrgn, hbr ) != FALSE;
	}
	bool FrameRgn( HRGN hrgn, HBRUSH hbr, int nWidth = 1, int nHeight = 1 ) const {
		return ::FrameRgn( hdc, hrgn, hbr, nWidth, nHeight ) != FALSE;
	}
	int DrawText( LPCTSTR lpString, int nCount, RECT& rc, UINT uFormat = DT_LEFT | DT_TOP ) const {
		return ::DrawText( hdc, lpString, nCount, &rc, uFormat );
	}
	int DrawText( LPCTSTR lpString, RECT& rc, UINT uFormat = DT_LEFT | DT_TOP ) const {
		return DrawText( lpString, static_cast<int>( _tcslen(lpString) ), rc, uFormat );
	}
#ifndef SKIP_STL
	int DrawText( const tstring& str, RECT& rc, UINT uFormat = DT_LEFT | DT_TOP ) const {
		return DrawText( str.data(), static_cast<int>( str.length() ), rc, uFormat );
	}
#endif
	bool TextOut( const POINT& pt, LPCTSTR lpString, int nCount ) const {
		return ::TextOut( hdc, pt.x, pt.y, lpString, nCount ) != FALSE;
	}
	bool TextOut( const POINT& pt, LPCTSTR lpString ) const {
		return TextOut( pt, lpString, static_cast<int>( _tcslen(lpString) ) );
	}
#ifndef SKIP_STL
	bool TextOut( const POINT& pt, const tstring& str ) const {
		return TextOut( pt, str.data(), static_cast<int>( str.length() ) );
	}
#endif
	COLORREF SetTextColor( COLORREF crColor ) const {
		return ::SetTextColor( hdc, crColor );
	}
	COLORREF GetTextColor() const {
		return ::GetTextColor( hdc );
	}
	UINT SetTextAlign( UINT fMode ) const {
		return ::SetTextAlign( hdc, fMode );
	}
	UINT GetTextAlign() const {
		return ::GetTextAlign( hdc );
	}
	bool GetTextExtentPoint( LPCTSTR lpString, int nCount, int nMaxExtent, SIZE* pSize, LPINT lpnFit = 0, LPINT alpDx = 0 ) const {
		return GetTextExtentExPoint( hdc, lpString, nCount, nMaxExtent, lpnFit, alpDx, pSize ) != FALSE;
	}
	bool GetTextExtentPoint32( LPCTSTR lpString, int nCount, SIZE* pSize ) const {
		return ::GetTextExtentPoint32( hdc, lpString, nCount, pSize ) != FALSE;
	}
	bool GetTextMetrics( LPTEXTMETRIC lptm ) const {
		return ::GetTextMetrics( hdc, lptm ) != FALSE;
	}
	int SetStretchBltMode( int stretch_mode ) const {
		return ::SetStretchBltMode( hdc, stretch_mode );
	}
	bool SetBrushOrg( const POINT& pos, LPPOINT ppt = 0 ) const {
		return ::SetBrushOrgEx( hdc, pos.x, pos.y, ppt ) != FALSE;
	}
	bool DrawCaption( HWND hWnd, const RECT& rc, UINT uFlags = DC_ACTIVE ) const {
		return ::DrawCaption( hWnd, hdc, &rc, uFlags ) != FALSE;
	}
	bool DrawEdge( RECT* prc, UINT edge, UINT uFlags = BF_RECT ) const {
		return ::DrawEdge( hdc, prc, edge, uFlags ) != FALSE;
	}
	bool DrawFocusRect( const RECT& rc ) const {
		return ::DrawFocusRect( hdc, &rc ) != FALSE;
	}
	bool DrawFrameControl( RECT* prc, UINT uType, UINT uState ) const {
		return ::DrawFrameControl( hdc, prc, uType, uState ) != FALSE;
	}
};

/**********************************************************************************************
* Device contexts
**********************************************************************************************/

class CompatibleDC: public DC, public NonCopyAble {
public:
	CompatibleDC( HDC hdc ): DC( CreateCompatibleDC( hdc ) ) { }
	~CompatibleDC() {
		if ( IsValid() ) DeleteDC( hdc );
	}
};

class ClientDC: public DC, public NonCopyAble {
	HWND hWnd;
public:
	ClientDC( HWND hWnd ): DC( NULL ), hWnd( hWnd ) {
		hdc = GetDC( hWnd );
	}
	~ClientDC() {
		if ( IsValid() ) ReleaseDC( hWnd, hdc );
	}
};

class WindowDC: public DC, public NonCopyAble {
	HWND hWnd;
public:
	WindowDC( HWND hWnd ): DC( NULL ), hWnd( hWnd ) {
		hdc = GetWindowDC( hWnd );
	}
	~WindowDC() {
		if ( IsValid() ) ReleaseDC( hWnd, hdc );
	}
};

class DCEx: public DC, public NonCopyAble {
	HWND hWnd;
public:
	DCEx( HWND hWnd, HRGN rgn, int flags ): DC( NULL ), hWnd( hWnd ) {
		hdc = GetDCEx( hWnd, rgn, flags );
	}
	~DCEx() {
		if ( IsValid() ) ReleaseDC( hWnd, hdc );
	}
};

class PaintDC: public DC, public NonCopyAble {
	HWND hWnd;
	PAINTSTRUCT ps;
public:
	PaintDC( HWND hWnd ): DC( NULL ), hWnd( hWnd ) {
		hdc = BeginPaint( hWnd, &ps );
	}
	~PaintDC() {
		if ( IsValid() ) EndPaint( hWnd, &ps );
	}
	bool RequiresErase() const {
		return ps.fErase != FALSE;
	}
	WRect PaintRect() const {
		return ps.rcPaint;
	}
};

class SelectObj: private DC, public NonCopyAble {
	HGDIOBJ hobj;
public:
	SelectObj( HDC hdc, HGDIOBJ hobjNew ): DC( hdc ), hobj( NULL ) {
		hobj = ::SelectObject( hdc, hobjNew );
	}
	~SelectObj() {
		Deselect();
	}
	void Select( HGDIOBJ hobjNew ) {
		if ( hobj != NULL ) ::SelectObject( hdc, hobj );
		hobj = ::SelectObject( hdc, hobjNew );
	}
	void Deselect() { if ( hobj != NULL ) ::SelectObject( hdc, hobj ), hobj = NULL; }
};

/**********************************************************************************************
* GDIObject
**********************************************************************************************/

class GDIObject {
protected:
	HGDIOBJ hgdiobj;
public:
	GDIObject(): hgdiobj( 0 ) { }
	GDIObject( HGDIOBJ hgdiobj ): hgdiobj( hgdiobj ) { }
	bool IsValid() const {
		return hgdiobj != NULL;
	}
	operator HGDIOBJ() const {
		return hgdiobj;
	}
	bool GetBitmap( BITMAP* p_bitmap ) const {
		return GetObject( sizeof(BITMAP), p_bitmap );
	}
	bool GetDIBSection( DIBSECTION* p_dib_section ) const {
		return GetObject( sizeof(DIBSECTION), p_dib_section );
	}
	bool GetPaletteSize( WORD* p_count ) const {
		return GetObject( sizeof(WORD), p_count );
	}
	bool GetExtLogPen( EXTLOGPEN* p_ext_log_pen ) const {
		return GetObject( sizeof(EXTLOGPEN), p_ext_log_pen );
	}
	bool GetLogPen( LOGPEN* p_log_pen ) const {
		return GetObject( sizeof(LOGPEN), p_log_pen );
	}
	bool GetLogBrush( LOGBRUSH* p_log_brush ) const {
		return GetObject( sizeof(LOGBRUSH), p_log_brush );
	}
	bool GetLogFont( LOGFONT* p_log_font ) const {
		return GetObject( sizeof(LOGFONT), p_log_font );
	}
private:
	bool GetObject( int num_bytes, LPVOID lpvObject ) const {
		const int num_bytes_req = ::GetObject( hgdiobj, 0, 0 );
		if ( num_bytes_req != num_bytes ) return false;
		return ::GetObject( hgdiobj, num_bytes, lpvObject ) == num_bytes;
	}
};

/**********************************************************************************************
* StockObject
**********************************************************************************************/

class StockObject: public GDIObject {
public:
	StockObject( int fnObject ): GDIObject( GetStockObject( fnObject ) ) { }
	operator HBRUSH() const { return static_cast<HBRUSH>( hgdiobj ); }
	operator HPEN() const { return static_cast<HPEN>( hgdiobj ); }
	operator HFONT() const { return static_cast<HFONT>( hgdiobj ); }
	operator HPALETTE() const { return static_cast<HPALETTE>( hgdiobj ); }
};

/**********************************************************************************************
* GDIObjectBase
**********************************************************************************************/

class GDIObjectBase: public GDIObject, public NonCopyAble {
protected:
	GDIObjectBase() { }
	~GDIObjectBase() { Release(); }
private:
	void Release() { if ( hgdiobj != NULL ) DeleteObject( hgdiobj ); }
public:
	void Set( HGDIOBJ hobj ) { Release(); hgdiobj = hobj; }
};

/**********************************************************************************************
* Pen
**********************************************************************************************/

class Pen: public GDIObjectBase {
public:
	Pen() { }
	Pen( COLORREF crColor, int nWidth = 0, int fnPenStyle = PS_SOLID ) {
		hgdiobj = ::CreatePen( fnPenStyle, nWidth, crColor );
	}
	operator HPEN() const {
		return static_cast<HPEN>( hgdiobj );
	}
};

/**********************************************************************************************
* Brush
**********************************************************************************************/

class Brush: public GDIObjectBase {
public:
	Brush() { }
	Brush( COLORREF crColor ) {
		hgdiobj = ::CreateSolidBrush( crColor );
	}
	Brush( COLORREF crColor, int fnStyle ) {
		hgdiobj = ::CreateHatchBrush( fnStyle, crColor );
	}
	Brush( COLORREF crColor, int fnStyle, LONG lbHatch ) {
		LOGBRUSH lb = { fnStyle, crColor, lbHatch };
		hgdiobj = ::CreateBrushIndirect( &lb );
	}
	Brush( HBITMAP hbmp ) {
		hgdiobj = ::CreatePatternBrush( hbmp );
	}
	operator HBRUSH() const {
		return static_cast<HBRUSH>( hgdiobj );
	}
};

/**********************************************************************************************
* Bitmap
**********************************************************************************************/

class Bitmap: public GDIObjectBase {
public:
	Bitmap() { }
	Bitmap( int nWidth, int nHeight, UINT cBitsPerPixel, const void* lpvBits ) {
		hgdiobj = CreateBitmap( nWidth, nHeight, 1, cBitsPerPixel, lpvBits );
	}
	Bitmap( HDC hdc, int nWidth, int nHeight ) {
		hgdiobj = CreateCompatibleBitmap( hdc, nWidth, nHeight );
	}
	Bitmap( HDC hdc, const BITMAPINFOHEADER* lpbmih, const void* lpbInit, const BITMAPINFO* lpbmi, UINT fuUsage = DIB_RGB_COLORS ) {
		hgdiobj = CreateDIBitmap( hdc, lpbmih, CBM_INIT, lpbInit, lpbmi, fuUsage );
	}
	operator HBITMAP() const {
		return static_cast<HBITMAP>( hgdiobj );
	}
	int SetDIBits( HDC hdc, UINT uStartScan, UINT cScanLines, const void* lpvBits, const BITMAPINFO* lpbmi, UINT fuColorUse = DIB_RGB_COLORS ) {
		return ::SetDIBits( hdc, *this, uStartScan, cScanLines, lpvBits, lpbmi, fuColorUse );
	}
	int GetDIBits( HDC hdc, UINT uStartScan, UINT cScanLines, LPVOID lpvBits, LPBITMAPINFO lpbi, UINT uUsage = DIB_RGB_COLORS ) const {
		return ::GetDIBits( hdc, *this, uStartScan, cScanLines, lpvBits, lpbi, uUsage );
	}
	bool Load( int resource_id, HINSTANCE hInstance = __MODULE_HANDLE ) {
		Set( ::LoadBitmap( hInstance, MAKEINTRESOURCE(resource_id) ) );
		return IsValid();
	}
};

/**********************************************************************************************
* Region
**********************************************************************************************/

class Region: public GDIObjectBase {
public:
	Region() { }
	Region( const RECT& rc ) {
		hgdiobj = CreateRectRgnIndirect( &rc );
	}
	Region( const POINT* lppt, int cPoints, int fnPolyFillMode ) {
		hgdiobj = CreatePolygonRgn( lppt, cPoints, fnPolyFillMode );
	}
	Region( const XFORM* lpXForm, DWORD dwCount, const RGNDATA* pRgnData ) {
		hgdiobj = ExtCreateRegion( lpXForm, dwCount, pRgnData );
	}
	operator HRGN() const {
		return static_cast<HRGN>( hgdiobj );
	}
	bool SetRect( const RECT& rc ) const {
		return SetRectRgn( static_cast<HRGN>( hgdiobj ), rc.left, rc.top, rc.right, rc.bottom ) != FALSE;
	}
	bool IsPtInside( int X, int Y ) const {
		return ::PtInRegion( static_cast<HRGN>( hgdiobj ), X, Y ) != FALSE;
	}
	int GetData( DWORD dwCount, LPRGNDATA lpRgnData ) const {
		return ::GetRegionData( static_cast<HRGN>( hgdiobj ), dwCount, lpRgnData );
	}
	int OffsetBy( const POINT& nOffset ) {
		return ::OffsetRgn( static_cast<HRGN>( hgdiobj ), nOffset.x, nOffset.y );
	}
	int GetBox( RECT* p_rect ) {
		return ::GetRgnBox( static_cast<HRGN>( hgdiobj ), p_rect );
	}
};

/**********************************************************************************************
* Font
**********************************************************************************************/

class Font: public GDIObjectBase {
public:
	Font( LPCTSTR lpszFace = 0,
		 int nHeight = -10,
		 int fnWeight = FW_DONTCARE,
		 DWORD fdwItalic = FALSE,
		 DWORD fdwUnderline = FALSE,
		 DWORD fdwStrikeOut = FALSE,
		 int nWidth = 0,
		 int nEscapement = 0,
		 int nOrientation = 0,
		 DWORD fdwCharSet = DEFAULT_CHARSET,
		 DWORD fdwOutputPrecision = OUT_DEFAULT_PRECIS,
		 DWORD fdwClipPrecision = CLIP_DEFAULT_PRECIS,
		 DWORD fdwQuality = DEFAULT_QUALITY,
		 DWORD fdwPitchAndFamily = DEFAULT_PITCH | FF_DONTCARE ) {
		hgdiobj = CreateFont( nHeight, nWidth, nEscapement, nOrientation,
			fnWeight, fdwItalic, fdwUnderline, fdwStrikeOut,
			fdwCharSet, fdwOutputPrecision, fdwClipPrecision,
			fdwQuality, fdwPitchAndFamily, lpszFace );
	}
	Font( const LOGFONT& lf ) {
		hgdiobj = CreateFontIndirect( &lf );
	}
	operator HFONT() const {
		return static_cast<HFONT>( hgdiobj );
	}
};

/**********************************************************************************************
* Control
**********************************************************************************************/

class Control: public GenericWindow {
public:
	Control( HWND hwnd ): GenericWindow( hwnd ) { }
	void Assign( HWND hwndControl ) {
		hWnd = hwndControl;
	}
	bool Create( LPCTSTR lpszClassName, LPCTSTR lpszText, const RECT& rc, HWND hwndParent, int id, DWORD dwStyle, DWORD dwExStyle = 0 ) {
		return GenericWindow::Create( lpszText, lpszClassName, dwStyle | WS_CHILD | WS_VISIBLE, rc, hwndParent, windows_cast<HMENU>( id ), dwExStyle );
	}
};

/**********************************************************************************************
* Button
**********************************************************************************************/

class Button: public Control {
public:
	Button(): Control( 0 ) { }
	Button( HWND hwnd ): Control( hwnd ) { }
	bool Create( LPCTSTR lpszText, const RECT& rc, HWND hwndParent, int id, DWORD dwStyle = BS_PUSHBUTTON, DWORD dwExStyle = 0 ) {
		return Control::Create( _T("BUTTON"), lpszText, rc, hwndParent, id, dwStyle, dwExStyle );
	}
	void Click() const {
		SendMessage( BM_CLICK );
	}
	int GetCheck() const {
		return SendMessage( BM_GETCHECK );
	}
	void SetCheck( int fCheck ) const {
		SendMessage( BM_SETCHECK, fCheck );
	}
	int GetState() const {
		return SendMessage( BM_GETSTATE );
	}
	void SetState( int fState ) const {
		SendMessage( BM_SETSTATE, fState );
	}
	void SetStyle( DWORD dwStyle, bool bRedraw ) const {
		SendMessage( BM_SETSTYLE, LOWORD(dwStyle), bRedraw );
	}
	HBITMAP GetBitmap() const {
		return windows_cast<HBITMAP>( SendMessage( BM_GETIMAGE, IMAGE_BITMAP ) );
	}
	HICON GetIcon() const {
		return windows_cast<HICON>( SendMessage( BM_GETIMAGE, IMAGE_ICON ) );
	}
	void SetBitmap( HBITMAP hBitmap ) const {
		SendMessage( BM_SETIMAGE, IMAGE_BITMAP, reinterpret_cast<LPARAM>( hBitmap ) );
	}
	void SetIcon( HICON hIcon ) const {
		SendMessage( BM_SETIMAGE, IMAGE_ICON, reinterpret_cast<LPARAM>( hIcon ) );
	}
};

class DefPushButton: public Button {
public:
	DefPushButton() { }
	bool Create( LPCTSTR lpszText, const RECT& rc, HWND hwndParent, int id, DWORD dwStyle = 0, DWORD dwExStyle = 0 ) {
		return Button::Create( lpszText, rc, hwndParent, id, BS_DEFPUSHBUTTON | dwStyle, dwExStyle );
	}
};

class CheckBox: public Button {
public:
	CheckBox() { }
	CheckBox( HWND hwnd ): Button( hwnd ) { }
	bool Create( LPCTSTR lpszText, const RECT& rc, HWND hwndParent, int id, DWORD dwStyle = 0, DWORD dwExStyle = 0 ) {
		return Button::Create( lpszText, rc, hwndParent, id, BS_AUTOCHECKBOX | dwStyle, dwExStyle );
	}
};

class RadioButton: public Button {
public:
	RadioButton() { }
	RadioButton( HWND hwnd ): Button( hwnd ) { }
	bool Create( LPCTSTR lpszText, const RECT& rc, HWND hwndParent, int id, DWORD dwStyle = 0, DWORD dwExStyle = 0 ) {
		return Button::Create( lpszText, rc, hwndParent, id, BS_AUTORADIOBUTTON | dwStyle, dwExStyle );
	}
};

class ThreeState: public Button {
public:
	ThreeState() { }
	ThreeState( HWND hwnd ): Button( hwnd ) { }
	bool Create( LPCTSTR lpszText, const RECT& rc, HWND hwndParent, int id, DWORD dwStyle = 0, DWORD dwExStyle = 0 ) {
		return Button::Create( lpszText, rc, hwndParent, id, BS_AUTO3STATE | dwStyle, dwExStyle );
	}
};

class GroupBox: public Button {
public:
	GroupBox() { }
	GroupBox( HWND hwnd ): Button( hwnd ) { }
	bool Create( LPCTSTR lpszText, const RECT& rc, HWND hwndParent, int id, DWORD dwStyle = 0, DWORD dwExStyle = 0 ) {
		return Button::Create( lpszText, rc, hwndParent, id, BS_GROUPBOX | dwStyle, dwExStyle );
	}
};

/**********************************************************************************************
* ComboBox
**********************************************************************************************/

class ComboBox: public Control {
public:
	ComboBox(): Control( 0 ) { }
	ComboBox( HWND hwnd ): Control( hwnd ) { }
	bool Create( const RECT& rc, HWND hwndParent, int id, DWORD dwStyle = CBS_DROPDOWN | CBS_AUTOHSCROLL, DWORD dwExStyle = 0 ) {
		return Control::Create( _T("COMBOBOX"), _T(""), rc, hwndParent, id, dwStyle, dwExStyle );
	}
	int AddString( LPCTSTR lpString ) const {
		return SendMessage( CB_ADDSTRING, 0, reinterpret_cast<LPARAM>( lpString ) );
	}
#if (WINVER >= 0x0400)
	int InitStorage( int nNumItems, int nItemStringSize ) const {
		return int( SendMessage( CB_INITSTORAGE, nNumItems, nItemStringSize ) );
	}
#endif
	int InsertString( LPCTSTR lpString, int nIndex = 0 ) const {
		return SendMessage( CB_INSERTSTRING, nIndex, reinterpret_cast<LPARAM>( lpString ) );
	}
	int DeleteString( int nIndex ) const {
		return SendMessage( CB_DELETESTRING, nIndex );
	}
	void ResetContent() const {
		SendMessage( CB_RESETCONTENT );
	}
	int FindString( LPCTSTR lpString, int nIndexStart = -1 ) const {
		return SendMessage( CB_FINDSTRING, nIndexStart, reinterpret_cast<LPARAM>( lpString ) );
	}
	int FindStringExact( LPCTSTR lpString, int nIndexStart = -1 ) const {
		return SendMessage( CB_FINDSTRINGEXACT, nIndexStart, reinterpret_cast<LPARAM>( lpString ) );
	}
	int Dir( LPCTSTR lpFilename, int nAttrs = DDL_ARCHIVE | DDL_DIRECTORY ) const {
		return SendMessage( CB_DIR, nAttrs, reinterpret_cast<LPARAM>( lpFilename ) );
	}
	int GetCount() const {
		return SendMessage( CB_GETCOUNT );
	}
	int GetCurSel() const {
		return SendMessage( CB_GETCURSEL );
	}
	int SetCurSel( int nIndex ) const {
		return SendMessage( CB_SETCURSEL, nIndex );
	}
	int SelectString( LPCTSTR lpString, int nIndexStart = -1 ) const {
		return SendMessage( CB_SELECTSTRING, nIndexStart, reinterpret_cast<LPARAM>( lpString ) );
	}
	void LimitText( int cchLimit ) const {
		SendMessage( CB_LIMITTEXT, cchLimit );
	}
	bool GetDroppedControlRect( RECT* lpRect ) const {
		return SendMessage( CB_GETDROPPEDCONTROLRECT, 0, reinterpret_cast<LPARAM>( lpRect ) ) != 0;
	}
	bool GetDroppedState() const {
		return SendMessage( CB_GETDROPPEDSTATE ) != 0;
	}
	int GetDroppedWidth() const {
		return SendMessage( CB_GETDROPPEDWIDTH );
	}
	int SetDroppedWidth( int nWidth ) const {
		return SendMessage( CB_SETDROPPEDWIDTH, nWidth );
	}
	void GetEditSel( int* start, int* end ) const {
		SendMessage( CB_GETEDITSEL, reinterpret_cast<WPARAM>( start ), reinterpret_cast<LPARAM>( end ) );
	}
	void SetEditSel( int start, int end ) const {
		SendMessage( CB_SETEDITSEL, 0, MAKELPARAM( start, end ) );
	}
	bool GetExtendedUI() const {
		return SendMessage( CB_GETEXTENDEDUI ) != 0;
	}
	bool SetExtendedUI( bool bExtended ) const {
		return SendMessage( CB_SETEXTENDEDUI, bExtended ) != CB_ERR;
	}
	int GetHorizontalExtent() const {
		return SendMessage( CB_GETHORIZONTALEXTENT );
	}
	void SetHorizontalExtent( int extent ) const {
		SendMessage( CB_SETHORIZONTALEXTENT, extent );
	}
	int GetItemData( int nIndex ) const {
		return SendMessage( CB_GETITEMDATA, nIndex );
	}
	bool SetItemData( int nIndex, int data ) const {
		return SendMessage( CB_SETITEMDATA, nIndex, data ) != CB_ERR;
	}
	int GetItemHeight( int nIndex ) const {
		return SendMessage( CB_GETITEMHEIGHT, nIndex );
	}
	bool SetItemHeight( int nIndex, int nHeight ) const {
		return SendMessage( CB_SETITEMHEIGHT, nIndex, nHeight ) != CB_ERR;
	}
	int GetLBTextLength( int nIndex ) const {
		return SendMessage( CB_GETLBTEXTLEN, nIndex );
	}
#ifndef SKIP_STL
	tstring GetLBText( int nIndex ) const;
#else
	int GetLBText( int nIndex, char* buf ) const {
		return SendMessage( CB_GETLBTEXT, nIndex, reinterpret_cast<LPARAM>(buf) );
	}
#endif
	int GetLocale() const {
		return SendMessage( CB_GETLOCALE );
	}
	int SetLocale( int locale ) const {
		return SendMessage( CB_SETLOCALE, locale );
	}
	int GetTopIndex() const {
		return SendMessage( CB_GETTOPINDEX );
	}
	bool SetTopIndex( int nIndex ) const {
		return SendMessage( CB_SETTOPINDEX, nIndex ) == 0;
	}
	void ShowDropDown( bool bShow ) const {
		SendMessage( CB_SHOWDROPDOWN, bShow );
	}
#if (WINVER >= 0x0500)
	bool GetComboBoxInfo( PCOMBOBOXINFO pcbi ) const {
		return ::GetComboBoxInfo( hWnd, pcbi ) != 0;
	}
#endif
};

/**********************************************************************************************
* Edit
**********************************************************************************************/

class Edit: public Control {
public:
	Edit(): Control( 0 ) { }
	Edit( HWND hwnd ): Control( hwnd ) { }
	bool Create( LPCTSTR lpszText, const RECT& rc, HWND hwndParent, int id, DWORD dwStyle = ES_LEFT | ES_AUTOHSCROLL, DWORD dwExStyle = 0 ) {
		return Control::Create( _T("EDIT"), lpszText, rc, hwndParent, id, dwStyle, dwExStyle );
	}
	bool CanUndo() const {
		return SendMessage( EM_CANUNDO ) != FALSE;
	}
	LPARAM CharFromPos( const POINT& pt ) const {
		return SendMessage( EM_CHARFROMPOS, 0, MAKELPARAM( pt.x, pt.y ) );
	}
	void EmptyUndoBuffer() const {
		SendMessage( EM_EMPTYUNDOBUFFER );
	}
	void FormatLines( bool bAddEOL ) const {
		SendMessage( EM_FMTLINES, bAddEOL );
	}
	int GetFirstVisibleLine() const {
		return SendMessage( EM_GETFIRSTVISIBLELINE );
	}
	HLOCAL GetHandle() const {
		return windows_cast<HLOCAL>( SendMessage( EM_GETHANDLE ) );
	}
	int GetLimitText() const {
		return SendMessage( EM_GETLIMITTEXT );
	}
#ifndef SKIP_STL
	tstring GetLine( int nLine ) const;
#else
	int GetLine( int nLine, char* buf ) const {
		return SendMessage( EM_GETLINE, nLine, reinterpret_cast<LPARAM>(buf) );
	}
#endif
	int GetLineCount() const {
		return SendMessage( EM_GETLINECOUNT );
	}
	int GetMargins() const {
		return SendMessage( EM_GETMARGINS );
	}
	bool GetModify() const {
		return SendMessage( EM_GETMODIFY ) != FALSE;
	}
	int GetPasswordChar() const {
		return SendMessage( EM_GETPASSWORDCHAR );
	}
	void GetRect( RECT* lpRect ) const {
		SendMessage( EM_GETRECT, 0, reinterpret_cast<LPARAM>( lpRect ) );
	}
	void GetSel( int* start, int* end ) const {
		SendMessage( EM_GETSEL, reinterpret_cast<WPARAM>( start ), reinterpret_cast<LPARAM>( end ) );
	}
	int GetThumb() const {
		return SendMessage( EM_GETTHUMB );
	}
	void LimitText( int max ) const {
		SendMessage( EM_LIMITTEXT, max );
	}
	int LineFromChar( int nCharIndex ) const {
		return SendMessage( EM_LINEFROMCHAR, nCharIndex );
	}
	int LineIndex( int nLine ) const {
		return SendMessage( EM_LINEINDEX, nLine );
	}
	int LineLength( int nCharIndex ) const {
		return SendMessage( EM_LINELENGTH, nCharIndex );
	}
	bool LineScroll( int dx, int dy ) const {
		return SendMessage( EM_LINESCROLL, dx, dy ) != FALSE;
	}
	LPARAM PosFromChar( int nCharIndex ) const {
		return SendMessage( EM_POSFROMCHAR, nCharIndex );
	}
	void ReplaceSel( LPCTSTR lpString, bool bCanUndo = true ) const {
		SendMessage( EM_REPLACESEL, bCanUndo, reinterpret_cast<LPARAM>( lpString ) );
	}
	bool Scroll( int nAction ) const {
		return SendMessage( EM_SCROLL, nAction, 0 ) != FALSE;
	}
	void ScrollCaret() const {
		SendMessage( EM_SCROLLCARET );
	}
	void SetHandle( HLOCAL local ) const {
		SendMessage( EM_SETHANDLE, reinterpret_cast<WPARAM>( local ) );
	}
	void SetLimitText( int max ) const {
		SendMessage( EM_SETLIMITTEXT, max );
	}
	void SetMargins( int fMargin, int nLeft, int nRight ) const {
		SendMessage( EM_SETMARGINS, fMargin, MAKELONG( nLeft, nRight ) );
	}
	void SetModify( bool bModified ) const {
		SendMessage( EM_SETMODIFY, bModified );
	}
	void SetPasswordChar( int pwd ) const {
		SendMessage( EM_SETPASSWORDCHAR, pwd );
	}
	bool SetReadOnly( bool bReadOnly = true ) const {
		return SendMessage( EM_SETREADONLY, bReadOnly ) != 0;
	}
	void SetRect( const RECT& rc ) const {
		SendMessage( EM_SETRECT, 0, reinterpret_cast<LPARAM>( &rc ) );
	}
	void SetRectNP( const RECT& rc ) const {
		SendMessage( EM_SETRECTNP, 0, reinterpret_cast<LPARAM>( &rc ) );
	}
	void SetSel( int start, int end ) const {
		SendMessage( EM_SETSEL, start, end );
	}
	bool SetTabStops( int* lpTabs, int nCount ) const {
		return SendMessage( EM_SETTABSTOPS, nCount, reinterpret_cast<LPARAM>( lpTabs ) ) != FALSE;
	}
	bool Undo() const {
		return SendMessage( EM_UNDO ) != FALSE;
	}
};

/**********************************************************************************************
* ListBox
**********************************************************************************************/

class ListBox: public Control {
public:
	ListBox(): Control( 0 ) { }
	ListBox( HWND hwnd ): Control( hwnd ) { }
	bool Create( const RECT& rc, HWND hwndParent, int id, DWORD dwStyle = 0, DWORD dwExStyle = 0 ) {
		return Control::Create( _T("LISTBOX"), _T(""), rc, hwndParent, id, dwStyle, dwExStyle );
	}
	int AddFile( LPCTSTR lpFilename ) const {
		return SendMessage( LB_ADDFILE, 0, reinterpret_cast<LPARAM>( lpFilename ) );
	}
	int AddString( LPCTSTR lpString ) const {
		return SendMessage( LB_ADDSTRING, 0, reinterpret_cast<LPARAM>( lpString ) );
	}
#if (WINVER >= 0x0400)
	int InitStorage( int nNumItems, int nItemStringSize ) const {
		return int( SendMessage( LB_INITSTORAGE, nNumItems, nItemStringSize ) );
	}
#endif
	int InsertString( LPCTSTR lpString, int nIndex = 0 ) const {
		return SendMessage( LB_INSERTSTRING, nIndex, reinterpret_cast<LPARAM>( lpString ) );
	}
	int DeleteString( int nIndex ) const {
		return SendMessage( LB_DELETESTRING, nIndex );
	}
	void ResetContent() const {
		SendMessage( LB_RESETCONTENT );
	}
	int FindString( LPCTSTR lpString, int nIndexStart = -1 ) const {
		return SendMessage( LB_FINDSTRING, nIndexStart, reinterpret_cast<LPARAM>( lpString ) );
	}
	int FindStringExact( LPCTSTR lpString, int nIndexStart = -1 ) const {
		return SendMessage( LB_FINDSTRINGEXACT, nIndexStart, reinterpret_cast<LPARAM>( lpString ) );
	}
	int Dir( LPCTSTR lpFilename, int nAttrs = DDL_ARCHIVE | DDL_DIRECTORY ) const {
		return SendMessage( LB_DIR, nAttrs, reinterpret_cast<LPARAM>( lpFilename ) );
	}
	int GetCount() const {
		return SendMessage( LB_GETCOUNT );
	}
	int GetCurSel() const {
		return SendMessage( LB_GETCURSEL );
	}
	int SetCurSel( int nIndex ) const {
		return SendMessage( LB_SETCURSEL, nIndex );
	}
	int SelectString( LPCTSTR lpString, int nIndexStart = -1 ) const {
		return SendMessage( LB_SELECTSTRING, nIndexStart, reinterpret_cast<LPARAM>( lpString ) );
	}
	int GetHorizontalExtent() const {
		return SendMessage( LB_GETHORIZONTALEXTENT );
	}
	void SetHorizontalExtent( int extent ) const {
		SendMessage( LB_SETHORIZONTALEXTENT, extent );
	}
	int GetItemData( int nIndex ) const {
		return SendMessage( LB_GETITEMDATA, nIndex );
	}
	bool SetItemData( int nIndex, int data ) const {
		return SendMessage( LB_SETITEMDATA, nIndex, data ) != LB_ERR;
	}
	int GetItemHeight( int nIndex ) const {
		return SendMessage( LB_GETITEMHEIGHT, nIndex );
	}
	bool SetItemHeight( int nIndex, int nHeight ) const {
		return SendMessage( LB_SETITEMHEIGHT, nIndex, nHeight ) != LB_ERR;
	}
	int GetTextLen( int nIndex ) const {
		return SendMessage( LB_GETTEXTLEN, nIndex );
	}
#ifndef SKIP_STL
	tstring GetText( int nIndex ) const;
#else
	int GetText( int nIndex, char* buf ) const {
		return SendMessage( LB_GETTEXT, nIndex, reinterpret_cast<LPARAM>(buf) );
	}
#endif
	int GetLocale() const {
		return SendMessage( LB_GETLOCALE );
	}
	int SetLocale( int locale ) const {
		return SendMessage( LB_SETLOCALE, locale );
	}
	int GetTopIndex() const {
		return SendMessage( LB_GETTOPINDEX );
	}
	bool SetTopIndex( int nIndex ) const {
		return SendMessage( LB_SETTOPINDEX, nIndex ) == 0;
	}
	int GetAnchorIndex() const {
		return SendMessage( LB_GETANCHORINDEX );
	}
	bool SetAnchorIndex( int nIndex ) const {
		return SendMessage( LB_SETANCHORINDEX, nIndex ) == 0;
	}
	int GetCaretIndex() const {
		return SendMessage( LB_GETCARETINDEX );
	}
	bool SetCaretIndex( int nIndex, bool bScroll = true ) const {
		return SendMessage( LB_SETCARETINDEX, nIndex, bScroll ) != LB_ERR;
	}
	bool GetItemRect( int nIndex, RECT* lpRect ) const {
		return SendMessage( LB_GETITEMRECT, nIndex, reinterpret_cast<LPARAM>( lpRect ) ) != LB_ERR;
	}
	bool GetSel( int nIndex ) const {
		return static_cast<int>( SendMessage( LB_GETSEL, nIndex ) ) > 0;
	}
	bool SetSel( int nIndex, bool bSelected = true ) const {
		return SendMessage( LB_SETSEL, bSelected, nIndex ) != LB_ERR;
	}
	int GetSelCount() const {
		return SendMessage( LB_GETSELCOUNT );
	}
	bool GetSelItems( int* lpItems, int nCount ) const {
		return SendMessage( LB_GETSELITEMS, nCount, reinterpret_cast<LPARAM>( lpItems ) ) != LB_ERR;
	}
	int ItemFromPoint( const POINT& pt ) const {
		return SendMessage( LB_ITEMFROMPOINT, 0, MAKELPARAM( pt.x, pt.y ) );
	}
	bool SelItemRange( int first, int last, bool bSelected = true ) const {
		return SendMessage( LB_SELITEMRANGE, bSelected, MAKELPARAM( first, last ) ) != LB_ERR;
	}
	void SetColumnWidth( int nWidth ) const {
		SendMessage( LB_SETCOLUMNWIDTH, nWidth );
	}
	bool SetCount( int nCount ) const {
		return SendMessage( LB_SETCOUNT, nCount ) != LB_ERR;
	}
	bool SetTabStops( int* lpTabs, int nCount ) const {
		return SendMessage( LB_SETTABSTOPS, nCount, reinterpret_cast<LPARAM>( lpTabs ) ) != FALSE;
	}
	bool MakeDragList() const {
		return ::MakeDragList( hWnd ) != FALSE;
	}
};

/**********************************************************************************************
* Static
**********************************************************************************************/

class Static: public Control {
public:
	Static(): Control( 0 ) { }
	Static( HWND hwnd ): Control( hwnd ) { }
	bool Create( LPCTSTR lpszText, const RECT& rc, HWND hwndParent, int id = -1, DWORD dwStyle = SS_LEFT, DWORD dwExStyle = 0 ) {
		return Control::Create( _T("STATIC"), lpszText, rc, hwndParent, id, dwStyle, dwExStyle );
	}
	HICON GetIcon() const {
		return windows_cast<HICON>( SendMessage( STM_GETICON ) );
	}
	HBITMAP GetBitmap() const {
		return windows_cast<HBITMAP>( SendMessage( STM_GETIMAGE, IMAGE_BITMAP ) );
	}
	HCURSOR GetCursor() const {
		return windows_cast<HCURSOR>( SendMessage( STM_GETIMAGE, IMAGE_CURSOR ) );
	}
	HICON SetIcon( HICON hicon ) const {
		return windows_cast<HICON>( SendMessage( STM_SETICON, reinterpret_cast<WPARAM>( hicon ) ) );
	}
	HBITMAP SetBitmap( HBITMAP hbitmap ) const {
		return windows_cast<HBITMAP>( SendMessage( STM_SETIMAGE, IMAGE_BITMAP, reinterpret_cast<LPARAM>( hbitmap ) ) );
	}
	HCURSOR SetCursor( HCURSOR hcursor) const {
		return windows_cast<HCURSOR>( SendMessage( STM_SETIMAGE, IMAGE_CURSOR, reinterpret_cast<LPARAM>( hcursor ) ) );
	}
};

/**********************************************************************************************
* ScrollBar
**********************************************************************************************/

class ScrollBar: public Control {
public:
	ScrollBar(): Control( 0 ) { }
	ScrollBar( HWND hwnd ): Control( hwnd ) { }
	bool Create( const RECT& rc, HWND hwndParent, int id, DWORD dwStyle = SBS_HORZ, DWORD dwExStyle = 0 ) {
		return Control::Create( _T("SCROLLBAR"), _T(""), rc, hwndParent, id, dwStyle, dwExStyle );
	}
	bool EnableArrows( int fArrows ) const {
		return SendMessage( SBM_ENABLE_ARROWS, fArrows ) != FALSE;
	}
	int GetPos() const {
		return SendMessage( SBM_GETPOS );
	}
	int SetPos( int nPos, bool bRedraw = true ) const {
		return SendMessage( SBM_SETPOS, nPos, bRedraw );
	}
	void GetRange( int* min, int* max ) const {
		SendMessage( SBM_GETRANGE, reinterpret_cast<WPARAM>( min ), reinterpret_cast<LPARAM>( max ) );
	}
	int SetRange( int min, int max ) const {
		return SendMessage( SBM_SETRANGE, min, max );
	}
	int SetRangeRedraw( int min, int max ) const {
		return SendMessage( SBM_SETRANGEREDRAW, min, max );
	}
	bool GetScrollInfo( SCROLLINFO* lpSI ) const {
		return SendMessage( SBM_GETSCROLLINFO, 0, reinterpret_cast<LPARAM>( lpSI ) ) != FALSE;
	}
	bool SetScrollInfo( SCROLLINFO* lpSI, bool bRedraw = true ) const {
		return SendMessage( SBM_SETSCROLLINFO, bRedraw, reinterpret_cast<LPARAM>( lpSI ) ) != FALSE;
	}
};

/**********************************************************************************************
* ImageList
**********************************************************************************************/

class ImageList {
	HIMAGELIST himl;
public:
	ImageList(): himl( 0 ) { }
	ImageList( HIMAGELIST himl ): himl( himl ) { }
	operator HIMAGELIST() const {
		return himl;
	}
	bool Create( const SIZE& dimensions, UINT flags = ILC_COLOR, int cInitial = 0, int cGrow = 4 ) {
		return (himl = ImageList_Create( dimensions.cx, dimensions.cy, flags, cInitial, cGrow ) ) != 0;
	}
#ifndef MINGW
	ImageList Duplicate() const {
		return ImageList_Duplicate( himl );
	}
#endif
	bool Destroy() {
		HIMAGELIST hold = himl; himl = 0;
		return ImageList_Destroy( hold ) != FALSE;
	}
	bool LoadImage( LPCTSTR pszFilename, int cx, int cGrow, COLORREF crMask = CLR_NONE, UINT uFlags = LR_LOADTRANSPARENT, UINT uType = IMAGE_BITMAP ) {
		return (himl = ImageList_LoadImage( 0, pszFilename, cx, cGrow, crMask, uType, uFlags | LR_LOADFROMFILE ) ) != 0;
	}
	bool LoadImage( int resource, int cx, int cGrow, COLORREF crMask = CLR_NONE, UINT uFlags = LR_LOADTRANSPARENT, UINT uType = IMAGE_BITMAP, HINSTANCE hInstance = __MODULE_HANDLE ) {
		return (himl = ImageList_LoadImage( hInstance, MAKEINTRESOURCE( resource ), cx, cGrow, crMask, uType, uFlags ) ) != 0;
	}
	int Add( HBITMAP hbmImage, HBITMAP hbmMask ) const {
		return ImageList_Add( himl, hbmImage, hbmMask );
	}
	int AddMasked( HBITMAP hbmImage, COLORREF crMask ) const {
		return ImageList_AddMasked( himl, hbmImage, crMask );
	}
	bool Replace( int i, HBITMAP hbmImage, HBITMAP hbmMask ) const {
		return ImageList_Replace( himl, i, hbmImage, hbmMask ) != FALSE;
	}
	int AddIcon( HICON hicon ) const {
		return ImageList_AddIcon( himl, hicon );
	}
	bool Replace( int i, HICON hicon ) const {
		return ImageList_ReplaceIcon( himl, i, hicon ) != -1;
	}
	bool Remove( int i = -1 ) const {
		return ImageList_Remove( himl, i ) != FALSE;
	}
	bool SetIconSize( const SIZE& dimensions ) const {
		return ImageList_SetIconSize( himl, dimensions.cx, dimensions.cy ) != FALSE;
	}
	bool GetIconSize( SIZE* size ) const {
		return ImageList_GetIconSize( himl, reinterpret_cast<int*>( &(size->cx) ), reinterpret_cast<int*>( &(size->cy) ) ) != FALSE;
	}
	HICON GetIcon( int i, UINT flags = 0 ) const {
		return ImageList_GetIcon( himl, i, flags );
	}
	bool Copy( int iDst, ImageList imlSrc, int iSrc, UINT uFlags = ILCF_MOVE ) const {
		return ImageList_Copy( himl, iDst, imlSrc, iSrc, uFlags ) != FALSE;
	}
	bool Draw( int i, HDC hdcDst, const POINT& pt, UINT fStyle = 0 ) const {
		return ImageList_Draw( himl, i, hdcDst, pt.x, pt.y, fStyle ) != FALSE;
	}
	bool Draw( int i, HDC hdcDst, const POINT& pt, const POINT& dim, COLORREF rgbBk, COLORREF rgbFg, UINT fStyle = 0 ) const {
		return ImageList_DrawEx( himl, i, hdcDst, pt.x, pt.y, dim.x, dim.y, rgbBk, rgbFg, fStyle ) != FALSE;
	}
	static bool Draw( IMAGELISTDRAWPARAMS* pimldp ) {
		return ImageList_DrawIndirect( pimldp ) != FALSE;
	}
	int GetImageCount() const {
		return ImageList_GetImageCount( himl );
	}
	bool SetImageCount( int nNewCount ) const {
		return ImageList_SetImageCount( himl, nNewCount ) != FALSE;
	}
	bool GetImageInfo( int i, IMAGEINFO* pImageInfo ) const {
		return ImageList_GetImageInfo( himl, i, pImageInfo ) != FALSE;
	}
	bool Merge( ImageList iml1, int i1, ImageList iml2, int i2, const POINT& pos ) {
		return (himl = ImageList_Merge( iml1, i1, iml2, i2, pos.x, pos.y ) ) != 0;
	}
	bool SetOverlayImage( int iImage, int iOverlay ) const {
		return ImageList_SetOverlayImage( himl, iImage, iOverlay ) != FALSE;
	}
	bool BeginDrag( int iTrack, const POINT& pt ) const {
		return ImageList_BeginDrag( himl, iTrack, pt.x, pt.y ) != FALSE;
	}
	static bool DragEnter( HWND hwndLock, const POINT& pt ) {
		return ImageList_DragEnter( hwndLock, pt.x, pt.y ) != FALSE;
	}
	static bool DragLeave( HWND hwndLock ) {
		return ImageList_DragLeave( hwndLock ) != FALSE;
	}
	static bool DragMove( const POINT& pt ) {
		return ImageList_DragMove( pt.x, pt.y ) != FALSE;
	}
	static bool DragShowNolock( bool bShow ) {
		return ImageList_DragShowNolock( bShow ) != FALSE;
	}
	static void EndDrag() {
		ImageList_EndDrag();
	}
	bool SetDragCursorImage( int iDrag, const POINT& hotspot ) const {
		return ImageList_SetDragCursorImage( himl, iDrag, hotspot.x, hotspot.y ) != FALSE;
	}
	static ImageList GetDragImage( POINT* ppt, POINT* pptHotspot ) {
		return ImageList_GetDragImage( ppt, pptHotspot );
	}
	COLORREF SetBkColor( COLORREF clrBk ) const {
		return ImageList_SetBkColor( himl, clrBk );
	}
	COLORREF GetBkColor() const {
		return ImageList_GetBkColor( himl );
	}
	//  bool Read( LPSTREAM pstm ) {
	//    return (himl = ImageList_Read( pstm ) ) != 0;
	//  }
	//  bool Write( LPSTREAM pstm ) const {
	//    return ImageList_Write( himl, pstm ) != FALSE;
	//  }
};

/**********************************************************************************************
* AutoImageList
**********************************************************************************************/

class AutoImageList: public ImageList, public NonCopyAble {
public:
	~AutoImageList() { Destroy(); }
};

/**********************************************************************************************
* HeaderControl
**********************************************************************************************/
#ifndef MINGW
class HeaderControl: public Control {
public:
	HeaderControl(): Control( 0 ) { }
	HeaderControl( HWND hwnd ): Control( hwnd ) { }
	bool Create( const RECT& rc, HWND hwndParent, int id, DWORD dwStyle = HDS_HORZ | HDS_BUTTONS | HDS_DRAGDROP | HDS_FULLDRAG, DWORD dwExStyle = 0 ) {
		return Control::Create( WC_HEADER, _T(""), rc, hwndParent, id, dwStyle, dwExStyle );
	}
	ImageList CreateDragImage( int nIndex ) const {
		return Header_CreateDragImage( hWnd, nIndex );
	}
	bool DeleteItem( int nIndex ) const {
		return Header_DeleteItem( hWnd, nIndex ) != FALSE;
	}
	ImageList GetImageList() const {
		return Header_GetImageList( hWnd );
	}
	bool GetItem( int nIndex, HDITEM* phdi ) const {
		return Header_GetItem( hWnd, nIndex, phdi ) != FALSE;
	}
	int GetItemCount() const {
		return Header_GetItemCount( hWnd );
	}
	bool GetItemRect( int nIndex, RECT* lpRect ) const {
		return Header_GetItemRect( hWnd, nIndex, lpRect ) != FALSE;
	}
	bool GetOrderArray( int* lpiArray, int nSize ) const {
		return Header_GetOrderArray( hWnd, nSize, lpiArray ) != FALSE;
	}
	bool GetUnicodeFormat() const {
		return Header_GetUnicodeFormat( hWnd ) != FALSE;
	}
	bool InsertItem( int nIndex, const HDITEM& hdi ) const {
		return Header_InsertItem( hWnd, nIndex, &hdi ) != FALSE;
	}
	bool InsertItem( int nIndex, LPCTSTR lpszText, int width = 90, int fmt = HDF_LEFT | HDF_STRING, LPARAM lParam = 0 ) const {
		HDITEM item;
		ZeroMemory( &item, sizeof( item ) );
		item.mask = HDI_TEXT | HDI_FORMAT | HDI_WIDTH | HDI_LPARAM;
		item.pszText = const_cast<LPTSTR>( lpszText );
		item.cxy = width;
		item.fmt = fmt;
		item.lParam = lParam;
		return InsertItem( nIndex, item );
	}
	bool Layout( RECT* lpRect, WINDOWPOS* lpWndPos ) const {
		HDLAYOUT layout = { lpRect, lpWndPos };
		return Header_Layout( hWnd, &layout ) != FALSE;
	}
	int OrderToIndex( int nOrder ) const {
		return Header_OrderToIndex( hWnd, nOrder );
	}
	int SetHotDivider( const POINT& pt ) const {
		return Header_SetHotDivider( hWnd, TRUE, MAKELPARAM( pt.x, pt.y ) );
	}
	int SetHotDivider( int nIndex ) const {
		return Header_SetHotDivider( hWnd, FALSE, nIndex );
	}
	ImageList SetImageList( HIMAGELIST himl ) const {
		return Header_SetImageList( hWnd, himl );
	}
	bool SetItem( int nIndex, const HDITEM& hdi ) const {
		return Header_SetItem( hWnd, nIndex, &hdi ) != FALSE;
	}
	bool SetItemText( int nIndex, LPCTSTR lpszText ) const {
		HDITEM item;
		ZeroMemory( &item, sizeof( item ) );
		item.mask = HDI_TEXT;
		item.pszText = const_cast<LPTSTR>( lpszText );
		return SetItem( nIndex, item );
	}
	bool SetItemFormat( int nIndex, int fmt = HDF_LEFT | HDF_STRING ) const {
		HDITEM item;
		ZeroMemory( &item, sizeof( item ) );
		item.mask = HDI_FORMAT;
		item.fmt = fmt;
		return SetItem( nIndex, item );
	}
	bool SetOrderArray( int* lpiArray, int nSize ) const {
		return Header_SetOrderArray( hWnd, nSize, lpiArray ) != FALSE;
	}
	bool SetUnicodeFormat( bool bUnicode ) const {
		return Header_SetUnicodeFormat( hWnd, bUnicode ) != FALSE;
	}
};
#endif
/**********************************************************************************************
* HotKey
**********************************************************************************************/

class HotKey: public Control {
public:
	HotKey(): Control( 0 ) { }
	HotKey( HWND hwnd ): Control( hwnd ) { }
	bool Create( const RECT& rc, HWND hwndParent, int id, DWORD dwStyle = 0, DWORD dwExStyle = 0 ) {
		return Control::Create( HOTKEY_CLASS, _T(""), rc, hwndParent, id, dwStyle, dwExStyle );
	}
	int GetHotKey() const {
		return SendMessage( HKM_GETHOTKEY ) & 0xFF;
	}
	int GetHotKeyModifier() const {
		return (SendMessage( HKM_GETHOTKEY ) & 0xFF00)>>8;
	}
	void SetHotKey( int hkey, int modifier ) const {
		SendMessage( HKM_SETHOTKEY, MAKEWORD(hkey,modifier) );
	}
	void SetRules( int combo, int modifier = 0 ) const {
		SendMessage( HKM_SETRULES, combo, modifier );
	}
};

/**********************************************************************************************
* ToolTip
**********************************************************************************************/

class ToolTip: public Control {
public:
	ToolTip(): Control( 0 ) { }
	ToolTip( HWND hwnd ): Control( hwnd ) { }
	bool Create( HWND hwnd, DWORD dwStyle = TTS_NOPREFIX | TTS_ALWAYSTIP, DWORD dwExStyle = WS_EX_TOPMOST ) {
		const bool b_ok = GenericWindow::Create( 0, TOOLTIPS_CLASS, WS_POPUP | dwStyle,
			WRect( WPoint( CW_USEDEFAULT, 0 ), CW_USEDEFAULT, 0 ), hwnd, 0, dwExStyle );
		if ( b_ok )
			SetPos( HWND_TOPMOST );
		return b_ok;
	}
	void Activate( bool b_activate ) const {
		SendMessage( TTM_ACTIVATE, b_activate );
	}
	bool AddTool( const TOOLINFO* pToolInfo ) const {
		return SendMessage( TTM_ADDTOOL, 0, reinterpret_cast<LPARAM>( pToolInfo ) ) != FALSE;
	}
	bool AddTool( HWND hwndTool, LPCTSTR pszText, UINT uFlags = TTF_SUBCLASS, HWND hwnd = 0, HINSTANCE hInstance =0 ) const {
		TOOLINFO info;
		ZeroMemory( &info, sizeof( info ) );
		info.cbSize = sizeof( info );
		info.uFlags = uFlags | TTF_IDISHWND;
		info.hwnd = hwnd;
		info.uId = windows_cast<UINT>( hwndTool );
		info.hinst = hInstance;
		info.lpszText = const_cast<TCHAR*>( pszText );
		return AddTool( &info );
	}
	bool AddTool( UINT uId, HWND hwnd, const RECT& rc, LPCTSTR pszText, UINT uFlags = 0, HINSTANCE hInstance = 0 ) const {
		TOOLINFO info;
		ZeroMemory( &info, sizeof( info ) );
		info.cbSize = sizeof( info );
		info.uFlags = uFlags & ~TTF_IDISHWND;
		info.hwnd = hwnd;
		info.rect = rc;
		info.uId = uId;
		info.hinst = hInstance;
		info.lpszText = const_cast<TCHAR*>( pszText );
		return AddTool( &info );
	}
	void DelTool( TOOLINFO* pToolInfo ) const {
		SendMessage( TTM_DELTOOL, 0, reinterpret_cast<LPARAM>( pToolInfo ) );
	}
	void DelTool( HWND hwndTool, HWND hwnd = 0 ) const {
		TOOLINFO info;
		ZeroMemory( &info, sizeof( info ) );
		info.cbSize = sizeof( info );
		info.hwnd = hwnd;
		info.uId = windows_cast<UINT>( hwndTool );
		DelTool( &info );
	}
	void DelTool( UINT uId, HWND hwnd ) const {
		TOOLINFO info;
		ZeroMemory( &info, sizeof( info ) );
		info.cbSize = sizeof( info );
		info.hwnd = hwnd;
		info.uId = uId;
		DelTool( &info );
	}
	bool EnumTools( UINT iTool, TOOLINFO* pToolInfo ) const {
		return SendMessage( TTM_ENUMTOOLS, iTool, reinterpret_cast<LPARAM>( pToolInfo ) ) != FALSE;
	}
	bool GetCurrentTool( TOOLINFO* pToolInfo ) const {
		return SendMessage( TTM_GETCURRENTTOOL, 0, reinterpret_cast<LPARAM>( pToolInfo ) ) != FALSE;
	}
	void GetMargin( RECT* pRect ) const {
		SendMessage( TTM_GETMARGIN, 0, reinterpret_cast<LPARAM>( pRect ) );
	}
	int GetMaxTipWidth() const {
		return SendMessage( TTM_GETMAXTIPWIDTH );
	}
	COLORREF GetTipBkColor() const {
		return SendMessage( TTM_GETTIPBKCOLOR );
	}
	COLORREF GetTipTextColor() const {
		return SendMessage( TTM_GETTIPTEXTCOLOR );
	}
	int GetToolCount() const {
		return SendMessage( TTM_GETTOOLCOUNT );
	}
	bool GetToolInfo( TOOLINFO* pToolInfo ) const {
		return SendMessage( TTM_GETTOOLINFO, 0, reinterpret_cast<LPARAM>( pToolInfo ) ) != FALSE;
	}
	bool HitTest( LPHITTESTINFO* pHti ) const {
		return SendMessage( TTM_HITTEST, 0, reinterpret_cast<LPARAM>( pHti ) ) != FALSE;
	}
	void NewToolRect( TOOLINFO* pToolInfo ) const {
		SendMessage( TTM_NEWTOOLRECT, 0, reinterpret_cast<LPARAM>( pToolInfo ) );
	}
	void Pop() const {
		SendMessage( TTM_POP );
	}
	void RelayEvent( MSG* pMsg ) const {
		SendMessage( TTM_RELAYEVENT, 0, reinterpret_cast<LPARAM>( pMsg ) );
	}
	void SetDelayTime( DWORD dwDuration, int time ) const {
		SendMessage( TTM_SETDELAYTIME, dwDuration, MAKELONG( time, 0 ) );
	}
	void SetMargin( const RECT& pRect ) const {
		SendMessage( TTM_SETMARGIN, 0, reinterpret_cast<LPARAM>( &pRect ) );
	}
	int SetMaxTipWidth( int width ) const {
		return SendMessage( TTM_SETMAXTIPWIDTH, 0, width );
	}
	void SetTipBkColor( COLORREF clr ) const {
		SendMessage( TTM_SETTIPBKCOLOR, clr );
	}
	void SetTipTextColor( COLORREF clr ) const {
		SendMessage( TTM_SETTIPTEXTCOLOR, clr );
	}
	void SetToolInfo( TOOLINFO* pToolInfo ) const {
		SendMessage( TTM_SETTOOLINFO, 0, reinterpret_cast<LPARAM>( pToolInfo ) );
	}
#if (_WIN32_IE >= 0x0500)
	void SetTitle( const char* lpszTitle, int icon = 0 ) const {
		SendMessage( TTM_SETTITLE, icon, reinterpret_cast<LPARAM>( lpszTitle ) );
	}
#endif
	void TrackActivate( TOOLINFO* pToolInfo, bool b_activate = true ) const {
		SendMessage( TTM_TRACKACTIVATE, b_activate, reinterpret_cast<LPARAM>( pToolInfo ) );
	}
	void TrackPosition( int x, int y ) const {
		SendMessage( TTM_TRACKPOSITION, 0, MAKELONG( x, y ) );
	}
	void Update() const {
		SendMessage( TTM_UPDATE );
	}
	void UpdateTipText( TOOLINFO* pToolInfo ) const {
		SendMessage( TTM_UPDATETIPTEXT, 0, reinterpret_cast<LPARAM>( pToolInfo ) );
	}
	void UpdateTipText( HWND hwndTool, LPCTSTR pszText, HWND hwnd = 0, HINSTANCE hInstance = 0 ) const {
		TOOLINFO info;
		ZeroMemory( &info, sizeof( info ) );
		info.cbSize = sizeof( info );
		info.hwnd = hwnd;
		info.uId = windows_cast<UINT>( hwndTool );
		info.hinst = hInstance;
		info.lpszText = const_cast<TCHAR*>( pszText );
		UpdateTipText( &info );
	}
	void UpdateTipText( UINT uId, HWND hwnd, LPCTSTR pszText, HINSTANCE hInstance = 0 ) const {
		TOOLINFO info;
		ZeroMemory( &info, sizeof( info ) );
		info.cbSize = sizeof( info );
		info.hwnd = hwnd;
		info.uId = uId;
		info.hinst = hInstance;
		info.lpszText = const_cast<TCHAR*>( pszText );
		UpdateTipText( &info );
	}
	GenericWindow WindowFromPoint( const POINT& pt ) const {
		return windows_cast<HWND>( SendMessage( TTM_WINDOWFROMPOINT, 0, reinterpret_cast<LPARAM>( &pt ) ) );
	}
};

/**********************************************************************************************
* ListView
**********************************************************************************************/

class ListView: public Control {
public:
	ListView(): Control( 0 ) { }
	ListView( HWND hwnd ): Control( hwnd ) { }
	bool Create( const RECT& rc, HWND hwndParent, int id, DWORD dwStyle = LVS_REPORT | LVS_SINGLESEL, DWORD dwExStyle = 0 ) {
		return Control::Create( WC_LISTVIEW, _T(""), rc, hwndParent, id, dwStyle, dwExStyle );
	}
	WPoint ApproximateViewRect( const POINT& pt, int iCount ) const {
		DWORD ret = ListView_ApproximateViewRect( hWnd, pt.x, pt.y, iCount );
		return WPoint( LOWORD( ret ), HIWORD( ret ) );
	}
	bool Arrange( UINT code = LVA_DEFAULT ) const {
		return ListView_Arrange( hWnd, code ) != FALSE;
	}
	ImageList CreateDragImage( int iItem, POINT* lpptUpLeft ) const {
		return ListView_CreateDragImage( hWnd, iItem, lpptUpLeft );
	}
	bool DeleteAllItems() const {
		return ListView_DeleteAllItems( hWnd ) != FALSE;
	}
	bool DeleteColumn( int col ) const {
		return ListView_DeleteColumn( hWnd, col ) != FALSE;
	}
	bool DeleteItem( int iItem ) const {
		return ListView_DeleteItem( hWnd, iItem ) != FALSE;
	}
	Edit EditLabel( int iItem ) const {
		return ListView_EditLabel( hWnd, iItem );
	}
	bool EnsureVisible( int i, bool bPartialOK = false ) const {
		return ListView_EnsureVisible( hWnd, i, bPartialOK ) != FALSE;
	}
	int FindItem( int iStart, const LVFINDINFO* plvfi ) const {
		return ListView_FindItem( hWnd, iStart, plvfi );
	}
	COLORREF GetBkColor() const {
		return ListView_GetBkColor( hWnd );
	}
	bool GetBkImage( LVBKIMAGE* plvbki ) const {
		return ListView_GetBkImage( hWnd, plvbki ) != FALSE;
	}
	int GetCallbackMask() const {
		return ListView_GetCallbackMask( hWnd );
	}
#ifndef MINGW
	bool GetCheckState( int iIndex ) const {
		return ListView_GetCheckState( hWnd, iIndex ) != FALSE;
	}
#endif
	bool GetColumn( int iCol, LVCOLUMN* pcol ) const {
		return ListView_GetColumn( hWnd, iCol, pcol ) != FALSE;
	}
	bool GetColumnOrderArray( int* lpiArray, int iCount ) const {
		return ListView_GetColumnOrderArray( hWnd, iCount, lpiArray ) != FALSE;
	}
	int GetColumnWidth( int iCol ) const {
		return ListView_GetColumnWidth( hWnd, iCol );
	}
	int GetCountPerPage() const {
		return ListView_GetCountPerPage( hWnd );
	}
	Edit GetEditControl() const {
		return ListView_GetEditControl( hWnd );
	}
	int GetExtendedListViewStyle() const {
		return ListView_GetExtendedListViewStyle( hWnd );
	}
#ifndef MINGW
	HeaderControl GetHeader() const {
		return ListView_GetHeader( hWnd );
	}
#endif
	HCURSOR GetHotCursor() const {
		return ListView_GetHotCursor( hWnd );
	}
	int GetHotItem() const {
		return ListView_GetHotItem( hWnd );
	}
	int GetHoverTime() const {
		return SendMessage( LVM_GETHOVERTIME );
	}
	ImageList GetImageList( int iImageList ) const {
		return ListView_GetImageList( hWnd, iImageList );
	}
#ifndef MINGW
	bool GetISearchString( LPSTR lpsz ) const {
		return ListView_GetISearchString( hWnd, lpsz ) != FALSE;
	}
#endif
	bool GetItem( LVITEM* pitem ) const {
		return ListView_GetItem( hWnd, pitem ) != FALSE;
	}
	int GetItemImage( int nItem, int nSubItem = 0 ) const {
		LVITEM item;
		ZeroMemory( &item, sizeof( item ) );
		item.iItem = nItem;
		item.iSubItem = nSubItem;
		item.mask = LVIF_IMAGE;
		item.iImage = 0;
		GetItem( &item );
		return item.iImage;
	}
	int GetItemParam( int nItem, int nSubItem = 0 ) const {
		LVITEM item;
		ZeroMemory( &item, sizeof( item ) );
		item.iItem = nItem;
		item.iSubItem = nSubItem;
		item.mask = LVIF_PARAM;
		item.lParam = 0;
		GetItem( &item );
		return static_cast<int>( item.lParam );
	}
	int GetItemCount() const {
		return ListView_GetItemCount( hWnd );
	}
	bool GetItemPosition( int i, POINT* ppt ) const {
		return ListView_GetItemPosition( hWnd, i, ppt ) != FALSE;
	}
	bool GetItemRect( int i, RECT* prc, int code = LVIR_BOUNDS ) const {
		return ListView_GetItemRect( hWnd, i, prc, code ) != FALSE;
	}
	int GetItemSpacing( bool bSmall = true ) const {
		return ListView_GetItemSpacing( hWnd, bSmall );
	}
	int GetItemState( int i, int mask ) const {
		return ListView_GetItemState( hWnd, i, mask );
	}
#ifndef SKIP_STL
	tstring GetItemText( int iItem, int iSubItem = 0 ) const;
#else
	void GetItemText( int iItem, int iSubItem, char* buf, int buf_size ) const {
		ListView_GetItemText( hWnd, iItem, iSubItem, buf, buf_size );
	}
#endif
	int GetNextItem( int iStart, int flags = LVNI_ALL ) const {
		return ListView_GetNextItem( hWnd, iStart, flags );
	}
#ifndef MINGW
	bool GetNumberOfWorkAreas( UINT* lpuWorkAreas ) const {
		return ListView_GetNumberOfWorkAreas( hWnd, lpuWorkAreas ) != FALSE;
	}
#endif
	bool GetOrigin( POINT* lpptOrg ) const {
		return ListView_GetOrigin( hWnd, lpptOrg ) != FALSE;
	}
	int GetSelectedCount() const {
		return ListView_GetSelectedCount( hWnd );
	}
	int GetSelectionMark() const {
		return ListView_GetSelectionMark( hWnd );
	}
	int GetStringWidth( LPCSTR psz ) const {
		return ListView_GetStringWidth( hWnd, psz );
	}
	bool GetSubItemRect( RECT* prc, int iItem, int iSubItem = 0, int code = LVIR_BOUNDS ) const {
		return ListView_GetSubItemRect( hWnd, iItem, iSubItem, code, prc ) != FALSE;
	}
	COLORREF GetTextBkColor() const {
		return ListView_GetTextBkColor( hWnd );
	}
	COLORREF GetTextColor() const {
		return ListView_GetTextColor( hWnd );
	}
	ToolTip GetToolTips() const {
		return windows_cast<HWND>( SendMessage( LVM_GETTOOLTIPS ) );
	}
	int GetTopIndex() const {
		return ListView_GetTopIndex( hWnd );
	}
#ifndef MINGW
	bool GetUnicodeFormat() const {
		return ListView_GetUnicodeFormat( hWnd ) != FALSE;
	}
#endif
	bool GetViewRect( RECT* prc ) const {
		return ListView_GetViewRect( hWnd, prc ) != FALSE;
	}
	void GetWorkAreas( RECT* prc, int nWorkAreas ) const {
		ListView_GetWorkAreas( hWnd, nWorkAreas, prc );
	}
	int HitTest( LVHITTESTINFO* pinfo ) const {
		return ListView_HitTest( hWnd, pinfo );
	}
	int InsertColumn( int iCol, const LVCOLUMN& col ) const {
		return ListView_InsertColumn( hWnd, iCol, &col );
	}
	int InsertColumn( int nColumn, LPCTSTR pszText, int width = 90, int fmt = LVCFMT_LEFT ) const {
		LV_COLUMN column;
		ZeroMemory( &column, sizeof( column ) );
		column.mask = LVCF_FMT | LVCF_WIDTH | LVCF_TEXT | LVCF_SUBITEM;
		column.fmt = fmt;
		column.cx = width;
		column.pszText = const_cast<TCHAR*>( pszText );
		column.iSubItem = nColumn;
		return InsertColumn( nColumn, column );
	}
	int InsertItem( const LVITEM& item ) const {
		return ListView_InsertItem( hWnd, &item );
	}
	int InsertItem( LPCTSTR pszText, int nItem = -1 ) const {
		LVITEM item;
		ZeroMemory( &item, sizeof( item ) );
		item.mask = LVIF_TEXT;
		item.iItem = nItem;
		item.iSubItem = 0;
		item.pszText = const_cast<TCHAR*>( pszText );
		return InsertItem( item );
	}
	int InsertItem( int nItem, LPCTSTR pszText, int nImage, int lParam = 0 ) const {
		LVITEM item;
		ZeroMemory( &item, sizeof( item ) );
		item.mask = LVIF_TEXT | LVIF_IMAGE | LVIF_PARAM;
		item.iItem = nItem;
		item.iSubItem = 0;
		item.pszText = const_cast<TCHAR*>( pszText );
		item.iImage = nImage;
		item.lParam = lParam;
		return InsertItem( item );
	}
#ifndef SKIP_STL
	int InsertItem( const vector<tstring>& vSubItems, int nImage, int lParam = 0 ) const {
		if ( vSubItems.empty() ) return -1;
		const int nItem = InsertItem( GetItemCount(), vSubItems[0].c_str(), nImage, lParam );
		typedef vector<tstring> VS;
		VS::const_iterator p = vSubItems.begin();
		for ( int nSubItem=1; ++p != vSubItems.end(); nSubItem++ )
			SetItem( nItem, nSubItem, p->c_str() );
		return nItem;
	}
#endif
	bool RedrawItems( int iFirst, int iLast ) const {
		return ListView_RedrawItems( hWnd, iFirst, iLast ) != FALSE;
	}
	bool Scroll( const POINT& delta ) const {
		return ListView_Scroll( hWnd, delta.x, delta.y ) != FALSE;
	}
	bool SetBkColor( COLORREF clrBk ) const {
		return ListView_SetBkColor( hWnd, clrBk ) != FALSE;
	}
	bool SetBkImage( LVBKIMAGE* plvbki ) const {
		return ListView_SetBkImage( hWnd, plvbki ) != FALSE;
	}
	bool SetCallbackMask( int mask ) const {
		return ListView_SetCallbackMask( hWnd, mask ) != FALSE;
	}
	bool SetColumn( int iCol, const LVCOLUMN& col ) const {
		return ListView_SetColumn( hWnd, iCol, &col ) != FALSE;
	}
	bool SetColumn( int iCol, LPCTSTR pszText, int width = 90, int fmt = LVCFMT_LEFT ) const {
		LV_COLUMN column;
		ZeroMemory( &column, sizeof( column ) );
		column.mask = LVCF_FMT | LVCF_WIDTH | LVCF_TEXT | LVCF_SUBITEM;
		column.fmt = fmt;
		column.cx = width;
		column.pszText = const_cast<TCHAR*>( pszText );
		column.iSubItem = iCol;
		return SetColumn( iCol, column );
	}
	bool SetColumnOrderArray( int* lpiArray, int iCount ) const {
		return ListView_SetColumnOrderArray( hWnd, iCount, lpiArray ) != FALSE;
	}
	bool SetColumnWidth( int iCol, int width ) const {
		return ListView_SetColumnWidth( hWnd, iCol, width ) != FALSE;
	}
	void SetExtendedListViewStyle( int dwExStyle ) const {
		ListView_SetExtendedListViewStyle( hWnd, dwExStyle );
	}
	void SetExtendedListViewStyle( int dwExMask, int dwExStyle ) const {
		ListView_SetExtendedListViewStyleEx( hWnd, dwExMask, dwExStyle );
	}
	HCURSOR SetHotCursor( HCURSOR hCursor ) const {
		return ListView_SetHotCursor( hWnd, hCursor );
	}
	int SetHotItem( int nIndex ) const {
		return ListView_SetHotItem( hWnd, nIndex );
	}
	int SetHoverTime( int new_time ) const {
		return SendMessage( LVM_SETHOVERTIME, 0, new_time );
	}
	WPoint SetIconSpacing( const POINT& spacing ) const {
		DWORD ret = ListView_SetIconSpacing( hWnd, spacing.x, spacing.y );
		return WPoint( LOWORD( ret ), HIWORD( ret ) );
	}
	ImageList SetImageList( HIMAGELIST himl, int iImageList ) const {
		return ListView_SetImageList( hWnd, himl, iImageList );
	}
	bool SetItem( const LVITEM& item ) const {
		return ListView_SetItem( hWnd, &item ) != FALSE;
	}
	bool SetItem( int nItem, int nSubItem, LPCTSTR pszText ) const {
		LVITEM item;
		ZeroMemory( &item, sizeof( item ) );
		item.mask = LVIF_TEXT;
		item.iItem = nItem;
		item.iSubItem = nSubItem;
		item.pszText = const_cast<TCHAR*>( pszText );
		return SetItem( item );
	}
	bool SetItem( int nItem, int nSubItem, LPCTSTR pszText, int nImage ) const {
		LVITEM item;
		ZeroMemory( &item, sizeof( item ) );
		item.mask = LVIF_TEXT | LVIF_IMAGE;
		item.iItem = nItem;
		item.iSubItem = nSubItem;
		item.pszText = const_cast<TCHAR*>( pszText );
		item.iImage = nImage;
		return SetItem( item );
	}
	bool SetItem( int nItem, int nSubItem, LPCTSTR pszText, int nImage, int lParam ) const {
		LVITEM item;
		item.mask = LVIF_TEXT | LVIF_IMAGE | LVIF_PARAM;
		item.iItem = nItem;
		item.iSubItem = nSubItem;
		item.pszText = const_cast<TCHAR*>( pszText );
		item.iImage = nImage;
		item.lParam = lParam;
		return SetItem( item );
	}
#ifndef SKIP_STL
	bool SetItem( int nItem, const vector<tstring>& vSubItems, int nImage, int lParam = 0 ) const {
		if ( vSubItems.empty() ) return false;
		typedef vector<tstring> VS;
		VS::const_iterator p = vSubItems.begin();
		for ( int nSubItem=0 ; p != vSubItems.end(); p++, nSubItem++ )
			if ( nSubItem == 0 ) {
				if ( ! SetItem( nItem, 0, p->c_str(), nImage, lParam ) )
					return false;
			} else
				if ( ! SetItem( nItem, nSubItem, p->c_str() ) )
					return false;
		return true;
	}
#endif
	void SetItemCount( int cItems ) const {
		ListView_SetItemCount( hWnd, cItems );
	}
#ifndef MINGW
	void SetItemCount( int cItems, int flags ) const {
		ListView_SetItemCountEx( hWnd, cItems, flags );
	}
#endif
	bool SetItemPosition( int i, const POINT& pos ) const {
		return ListView_SetItemPosition( hWnd, i, pos.x, pos.y ) != FALSE;
	}
	void SetItemPosition32( int i, const POINT& pos ) const {
		ListView_SetItemPosition32( hWnd, i, pos.x, pos.y );
	}
	bool SetItemState( int i, int state, int mask ) const {
		LVITEM item; item.state = state; item.stateMask = mask;
		return SendMessage( LVM_SETITEMSTATE, i, reinterpret_cast<LPARAM>( &item ) ) != FALSE;
	}
	void SetItemText( LPCTSTR pszText, int nItem, int iSubItem = 0 ) const {
		ListView_SetItemText( hWnd, nItem, iSubItem, const_cast<TCHAR*>( pszText ) );
	}
	int SetSelectionMark( int nIndex ) const {
		return ListView_SetSelectionMark( hWnd, nIndex );
	}
	bool SetTextBkColor( COLORREF clrText ) const {
		return ListView_SetTextBkColor( hWnd, clrText ) != FALSE;
	}
	bool SetTextColor( COLORREF clrText ) const {
		return ListView_SetTextColor( hWnd, clrText ) != FALSE;
	}
	ToolTip SetToolTips( HWND hwndToolTips ) const {
		return windows_cast<HWND>( SendMessage( LVM_SETTOOLTIPS, 0, reinterpret_cast<LPARAM>( hwndToolTips ) ) );
	}
#ifndef MINGW
	bool SetUnicodeFormat( bool bUnicode = true ) const {
		return ListView_SetUnicodeFormat( hWnd, bUnicode ) != FALSE;
	}
#endif
	void SetWorkAreas( RECT* prc, int nWorkAreas ) const {
		ListView_SetWorkAreas( hWnd, nWorkAreas, prc );
	}
	bool SortItems( PFNLVCOMPARE pfnCompare, LPARAM lParamSort ) const {
		return ListView_SortItems( hWnd, pfnCompare, lParamSort ) != FALSE;
	}
#if (WINVER >= 0x0500)
	bool SortItemsEx( PFNLVCOMPARE pfnCompare, LPARAM lParamSort ) const {
		return ListView_SortItemsEx( hWnd, pfnCompare, lParamSort ) != FALSE;
	}
#endif
	int SubItemHitTest( LVHITTESTINFO* pInfo ) const {
		return ListView_SubItemHitTest( hWnd, pInfo );
	}
	bool Update( int nItem ) const {
		return ListView_Update( hWnd, nItem ) != FALSE;
	}
};

/**********************************************************************************************
* ProgressBar
**********************************************************************************************/

class ProgressBar: public Control {
public:
	ProgressBar(): Control( 0 ) { }
	ProgressBar( HWND hwnd ): Control( hwnd ) { }
	bool Create( const RECT& rc, HWND hwndParent, int id, DWORD dwStyle = 0, DWORD dwExStyle = 0 ) {
		return Control::Create( PROGRESS_CLASS, _T(""), rc, hwndParent, id, dwStyle, dwExStyle );
	}
	int DeltaPos( int nIncrement ) const {
		return SendMessage( PBM_DELTAPOS, nIncrement );
	}
	int GetPos() const {
		return SendMessage( PBM_GETPOS );
	}
	int GetRangeLow() const {
		return SendMessage( PBM_GETRANGE, TRUE );
	}
	int GetRangeHigh() const {
		return SendMessage( PBM_GETRANGE, FALSE );
	}
	COLORREF SetBarColor( COLORREF cr = CLR_DEFAULT ) const {
		return static_cast<COLORREF>( SendMessage( PBM_SETBARCOLOR, 0, static_cast<LPARAM>( cr ) ) );
	}
	COLORREF SetBkColor( COLORREF cr = CLR_DEFAULT ) const {
		return static_cast<COLORREF>( SendMessage( PBM_SETBKCOLOR, 0, static_cast<LPARAM>( cr ) ) );
	}
	int SetPos( int nNewPos ) const {
		return SendMessage( PBM_SETPOS, nNewPos );
	}
	bool SetRange( int nMin, int nMax ) const {
		return SendMessage( PBM_SETRANGE, 0, MAKELPARAM( nMin, nMax ) ) != 0;
	}
	int SetStep( int nStepInc ) const {
		return SendMessage( PBM_SETSTEP, nStepInc );
	}
	int StepIt() const {
		return SendMessage( PBM_STEPIT );
	}
};

/**********************************************************************************************
* Rebar
**********************************************************************************************/

class Rebar: public Control {
public:
	Rebar(): Control( 0 ) { }
	Rebar( HWND hwnd ): Control( hwnd ) { }
	bool Create( HWND hwndParent, int id = -1, DWORD dwStyle = WS_CLIPSIBLINGS | WS_CLIPCHILDREN | RBS_VARHEIGHT | CCS_NODIVIDER, DWORD dwExStyle = 0 ) {
		return Control::Create( REBARCLASSNAME, _T(""), WRect(0,0,0,0), hwndParent, id, dwStyle, dwExStyle );
	}
	void BeginDrag( int nBand, int nPos ) const {
		SendMessage( RB_BEGINDRAG, nBand, nPos );
	}
	bool DeleteBand( int nBand ) const {
		return SendMessage( RB_DELETEBAND, nBand ) != 0;
	}
	void DragMove( int nPos ) const {
		SendMessage( RB_DRAGMOVE, 0, nPos );
	}
	void EndDrag() const {
		SendMessage( RB_ENDDRAG );
	}
	WRect GetBandBorders( int nBand ) const {
		WRect rc;
		SendMessage( RB_GETBANDBORDERS, nBand, reinterpret_cast<LPARAM>( &rc ) );
		return rc;
	}
	int GetBandCount() const {
		return SendMessage( RB_GETBANDCOUNT );
	}
	bool GetBandInfo( int nBand, LPREBARBANDINFO lprbbi ) const {
		return SendMessage( RB_GETBANDINFO, nBand, reinterpret_cast<LPARAM>( lprbbi ) ) != 0;
	}
	int GetBarHeight() const {
		return SendMessage( RB_GETBARHEIGHT );
	}
	bool GetBarInfo( LPREBARINFO lprbi ) const {
		return SendMessage( RB_GETBARINFO, 0, reinterpret_cast<LPARAM>( lprbi ) ) != 0;
	}
	COLORREF GetBkColor() const {
		return static_cast<COLORREF>( SendMessage( RB_GETBKCOLOR ) );
	}
//	void GetDropTarget( IDropTarget** ppDropTarget ) const {
//		SendMessage( RB_GETDROPTARGET, 0, reinterpret_cast<LPARAM>( ppDropTarget ) );
//	}
	bool GetColorScheme( LPCOLORSCHEME lpcs ) const {
		return SendMessage( RB_GETCOLORSCHEME, 0, reinterpret_cast<LPARAM>( lpcs ) ) != 0;
	}
	HPALETTE GetPalette() const {
		return windows_cast<HPALETTE>( SendMessage( RB_GETPALETTE ) );
	}
	bool GetRect( int nBand, RECT* prc ) const {
		return SendMessage( RB_GETRECT, nBand, reinterpret_cast<LPARAM>( prc ) ) != 0;
	}
	int GetRowCount() const {
		return SendMessage( RB_GETROWCOUNT );
	}
	int GetRowHeight( int nRow ) const {
		return SendMessage( RB_GETROWHEIGHT, nRow );
	}
	COLORREF GetTextColor() const {
		return static_cast<COLORREF>( SendMessage( RB_GETTEXTCOLOR ) );
	}
	ToolTip GetToolTips() const {
		return windows_cast<HWND>( SendMessage( RB_GETTOOLTIPS ) );
	}
	bool GetUnicodeFormat() const {
		return SendMessage( RB_GETUNICODEFORMAT ) != 0;
	}
	int HitTest( LPRBHITTESTINFO lprbht ) const {
		return SendMessage( RB_HITTEST, 0, reinterpret_cast<LPARAM>( lprbht ) );
	}
	int IDToIndex( int nBand ) const {
		return SendMessage( RB_IDTOINDEX, nBand );
	}
	int InsertBand( const REBARBANDINFO& rbbi, int nIndex = -1 ) const {
		return SendMessage( RB_INSERTBAND, nIndex, reinterpret_cast<LPARAM>( &rbbi ) );
	}
	int InsertBand( HWND hwndChild, const SIZE& size, UINT fStyle = 0, int nIndex = -1 ) const {
		REBARBANDINFO rbi;
		ZeroMemory( &rbi, sizeof( rbi ) );
		rbi.cbSize = sizeof( rbi );
		rbi.fMask = RBBIM_STYLE | RBBIM_CHILD | RBBIM_CHILDSIZE | RBBIM_SIZE | RBBIM_IDEALSIZE;
		rbi.fStyle = fStyle;
		rbi.hwndChild = hwndChild;
		rbi.cyMinChild = size.cy;
		rbi.cx = rbi.cxMinChild = rbi.cxIdeal = size.cx + 4;
#if (_WIN32_IE >= 0x0500)
		if ( (fStyle & RBBS_USECHEVRON) != 0 ) rbi.cxMinChild = size.cy;
#endif
		return InsertBand( rbi, nIndex );
	}
	int InsertBand( HWND hwndChild, UINT fStyle = 0, int nIndex = -1 ) const {
		return InsertBand( hwndChild, GenericWindow(hwndChild).GetClientRect().GetSize(), fStyle, nIndex );
	}
	int InsertBand( HWND hwndChild, const SIZE& size, LPCTSTR lpszText, UINT fStyle = 0, int nIndex = -1 ) const {
		REBARBANDINFO rbi;
		ZeroMemory( &rbi, sizeof( rbi ) );
		rbi.cbSize = sizeof( rbi );
		rbi.fMask = RBBIM_TEXT | RBBIM_STYLE | RBBIM_CHILD | RBBIM_CHILDSIZE | RBBIM_SIZE | RBBIM_IDEALSIZE;
		rbi.fStyle = fStyle;
		rbi.lpText = const_cast<LPTSTR>( lpszText );
		rbi.hwndChild = hwndChild;
		rbi.cyMinChild = size.cy;
		rbi.cx = rbi.cxMinChild = rbi.cxIdeal = size.cx + 4;
#if (_WIN32_IE >= 0x0500)
		if ( (fStyle & RBBS_USECHEVRON) != 0 ) rbi.cxMinChild = size.cy;
#endif
		return InsertBand( rbi, nIndex );
	}
	int InsertBand( HWND hwndChild, LPCTSTR lpszText, UINT fStyle = 0, int nIndex = -1 ) const {
		return InsertBand( hwndChild, GenericWindow(hwndChild).GetClientRect().GetSize(), lpszText, fStyle, nIndex );
	}
	void MaximizeBand( int nBand, bool bIdeal = true ) const {
		SendMessage( RB_MAXIMIZEBAND, nBand, bIdeal );
	}
	void MinimizeBand( int nBand ) const {
		SendMessage( RB_MINIMIZEBAND, nBand );
	}
	bool MoveBand( int nFrom, int nTo ) const {
		return SendMessage( RB_MOVEBAND, nFrom, nTo ) != 0;
	}
	bool SetBandInfo( int nBand, const REBARBANDINFO& rbbi ) const {
		return SendMessage( RB_SETBANDINFO, nBand, reinterpret_cast<LPARAM>( &rbbi ) ) != 0;
	}
	bool SetBarInfo( const REBARINFO& rbi ) const {
		return SendMessage( RB_SETBARINFO, 0, reinterpret_cast<LPARAM>( &rbi ) ) != 0;
	}
	bool SetBarInfo( HIMAGELIST himl = 0 ) const {
		REBARINFO ri = { sizeof( REBARINFO ), RBIM_IMAGELIST, himl };
		return SetBarInfo( ri );
	}
	COLORREF SetBkColor( COLORREF clr ) const {
		return static_cast<COLORREF>( SendMessage( RB_SETBKCOLOR, 0, static_cast<LPARAM>( clr ) ) );
	}
	void SetColorScheme( LPCOLORSCHEME lpcs ) const {
		SendMessage( RB_SETCOLORSCHEME, 0, reinterpret_cast<LPARAM>( lpcs ) );
	}
	HPALETTE SetPalette( HPALETTE pal ) const {
		return windows_cast<HPALETTE>( SendMessage( RB_GETPALETTE, 0, reinterpret_cast<LPARAM>( pal ) ) );
	}
	GenericWindow SetParent( HWND hwndParent ) const {
		return windows_cast<HWND>( SendMessage( RB_SETPARENT, reinterpret_cast<LPARAM>( hwndParent ) ) );
	}
	COLORREF SetTextColor( COLORREF clr ) const {
		return static_cast<COLORREF>( SendMessage( RB_GETTEXTCOLOR, 0, static_cast<LPARAM>( clr ) ) );
	}
	void SetToolTips( HWND hwndToolTip ) const {
		SendMessage ( RB_SETTOOLTIPS, reinterpret_cast<LPARAM>( hwndToolTip ) );
	}
	bool SetUnicodeFormat( bool bUnicode ) const {
		return SendMessage( RB_SETUNICODEFORMAT, bUnicode ) != 0;
	}
	void ShowBand( int nBand, bool bShow = true ) const {
		SendMessage( RB_SHOWBAND, nBand, bShow );
	}
	bool SizeToRect( RECT* prc ) const {
		return SendMessage( RB_SIZETORECT, 0, reinterpret_cast<LPARAM>( prc ) ) != 0;
	}
};

/**********************************************************************************************
* StatusBar
**********************************************************************************************/

class StatusBar: public Control {
public:
	StatusBar(): Control( 0 ) { }
	StatusBar( HWND hwnd ): Control( hwnd ) { }
	bool Create( HWND hwndParent, int id = -1, DWORD dwStyle = SBS_SIZEGRIP, DWORD dwExStyle = 0 ) {
		return Control::Create( STATUSCLASSNAME, _T(""), WRect(0,0,0,0), hwndParent, id, dwStyle, dwExStyle );
	}
	bool GetBorders( int* borders ) const {
		return SendMessage( SB_GETBORDERS, 0, reinterpret_cast<LPARAM>( borders ) ) != 0;
	}
	HICON GetIcon( int nPart ) const {
		return windows_cast<HICON>( SendMessage( SB_GETICON, nPart ) );
	}
	int GetParts( int* coords, int nParts ) const {
		return SendMessage( SB_GETPARTS, nParts, reinterpret_cast<LPARAM>( coords ) );
	}
	bool GetRect( int nPart, RECT* prc ) const {
		return SendMessage( SB_GETRECT, nPart, reinterpret_cast<LPARAM>( prc ) ) != 0;
	}
	int GetPartType( int nPart ) const {
		return HIWORD( SendMessage( SB_GETTEXTLENGTH, nPart ) );
	}
	int GetTextLen( int nPart ) const {
		return LOWORD( SendMessage( SB_GETTEXTLENGTH, nPart ) );
	}
#ifndef SKIP_STL
	tstring GetText( int nPart ) const;
	tstring GetTipText( int nPart ) const;
#else
	int GetText( int nPart, TCHAR* buf ) const {
		return SendMessage( SB_GETTEXT, nPart, reinterpret_cast<LPARAM>( buf ) );
	}
	void GetTipText( int nPart, TCHAR* buf, int buf_size ) const {
		SendMessage( SB_GETTIPTEXT, MAKEWPARAM( nPart, buf_size ), reinterpret_cast<LPARAM>( buf ) );
	}
#endif
	bool GetUnicodeFormat() const {
		return SendMessage( SB_GETUNICODEFORMAT ) != 0;
	}
	bool IsSimple() const {
		return SendMessage( SB_ISSIMPLE ) != 0;
	}
	COLORREF SetBkColor( COLORREF clr ) const {
		return static_cast<COLORREF>( SendMessage( SB_SETBKCOLOR, 0, static_cast<LPARAM>( clr ) ) );
	}
	HICON SetIcon( int nPart, HICON hicon ) const {
		return windows_cast<HICON>( SendMessage( SB_GETICON, nPart, reinterpret_cast<LPARAM>( hicon ) ) );
	}
	void SetMinHeight( int nMinHeight ) const {
		SendMessage( SB_SETMINHEIGHT, nMinHeight );
	}
	bool SetParts( int* nWidths, int nParts ) const {
		return SendMessage( SB_SETPARTS, nParts, reinterpret_cast<LPARAM>( nWidths ) ) != 0;
	}
	bool SetText( int nPart, LPCTSTR pszText, int nType = 0 ) const {
		return SendMessage( SB_SETTEXT, nPart | nType, reinterpret_cast<LPARAM>( pszText ) ) != 0;
	}
	void SetTipText( int nPart, LPCTSTR pszToolTipText ) const {
		SendMessage( SB_SETTIPTEXT, nPart, reinterpret_cast<LPARAM>( pszToolTipText ) );
	}
	bool SetUnicodeFormat( bool bUnicode ) const {
		return SendMessage( SB_SETUNICODEFORMAT, bUnicode ) != 0;
	}
	bool Simple( bool bSimple = true ) const {
		return SendMessage( SB_SIMPLE, bSimple ) != 0;
	}
};

/**********************************************************************************************
* TabControl
**********************************************************************************************/

class TabControl: public Control {
public:
	TabControl(): Control( 0 ) { }
	TabControl( HWND hwnd ): Control( hwnd ) { }
	bool Create( const RECT& rc, HWND hwndParent, int id = -1, DWORD dwStyle = TCS_MULTILINE | TCS_TOOLTIPS, DWORD dwExStyle = 0 ) {
		return Control::Create( WC_TABCONTROL, _T(""), rc, hwndParent, id, dwStyle, dwExStyle );
	}
	void AdjustRect( RECT* prc, bool bLarger ) const {
		TabCtrl_AdjustRect( hWnd, bLarger, prc );
	}
	bool DeleteAllItems() const {
		return TabCtrl_DeleteAllItems( hWnd ) != FALSE;
	}
	bool DeleteItem( int nItem ) const {
		return TabCtrl_DeleteItem( hWnd, nItem ) != FALSE;
	}
	void DeselectAll( bool bExcludeFocus = false ) const {
		TabCtrl_DeselectAll( hWnd, bExcludeFocus );
	}
	int GetCurFocus() const {
		return TabCtrl_GetCurFocus( hWnd );
	}
	int GetCurSel() const {
		return TabCtrl_GetCurSel( hWnd );
	}
	int GetExtendedStyle() const {
		return TabCtrl_GetExtendedStyle( hWnd );
	}
#ifndef MINGW
	ImageList GetImageList() const {
		return TabCtrl_GetImageList( hWnd );
	}
#endif
	bool GetItem( int nItem, LPTCITEM pitem ) const {
		return TabCtrl_GetItem( hWnd, nItem, pitem ) != FALSE;
	}
#ifndef SKIP_STL
	tstring GetItemText( int nItem ) const;
#endif
	int GetItemImage( int nItem ) const {
		TCITEM item;
		item.mask = TCIF_IMAGE;
		item.iImage = 0;
		GetItem( nItem, &item );
		return item.iImage;
	}
	int GetItemParam( int nItem ) const {
		TCITEM item;
		item.mask = TCIF_PARAM;
		item.lParam = 0;
		GetItem( nItem, &item );
		return static_cast<int>( item.lParam );
	}
	int GetItemState( int nItem ) const {
		TCITEM item;
		item.mask = TCIF_STATE;
		item.dwState = 0;
		item.dwStateMask = (DWORD)-1;
		GetItem( nItem, &item );
		return item.dwState;
	}
	int GetItemCount() const {
		return TabCtrl_GetItemCount( hWnd );
	}
	bool GetItemRect( int nItem, RECT* prc ) const {
		return TabCtrl_GetItemRect( hWnd, nItem, prc ) != FALSE;
	}
	int GetRowCount() const {
		return TabCtrl_GetRowCount( hWnd );
	}
	ToolTip GetToolTips() const {
		return TabCtrl_GetToolTips( hWnd );
	}
	bool GetUnicodeFormat() const {
		return TabCtrl_GetUnicodeFormat( hWnd ) != FALSE;
	}
	bool HighlightItem( int nItem, bool bHighlight = true ) const {
		return TabCtrl_HighlightItem( hWnd, nItem, bHighlight ) != FALSE;
	}
	int HitTest( LPTCHITTESTINFO pinfo ) const {
		return TabCtrl_HitTest( hWnd, pinfo );
	}
	int InsertItem( int nItem, const TCITEM& item ) const {
		return TabCtrl_InsertItem( hWnd, nItem, &item );
	}
	int InsertItem( int nItem, LPCTSTR pszText, int nImage = -1, int lParam = 0 ) const {
		TCITEM item;
		item.mask = TCIF_TEXT | TCIF_IMAGE | TCIF_PARAM;
		item.pszText = const_cast<TCHAR*>( pszText );
		item.iImage = nImage;
		item.lParam = lParam;
		return InsertItem( nItem, item );
	}
	void RemoveImage( int nImage ) const {
		TabCtrl_RemoveImage( hWnd, nImage );
	}
	void SetCurFocus( int nItem ) const {
		TabCtrl_SetCurFocus( hWnd, nItem );
	}
	int SetCurSel( int nItem ) const {
		return TabCtrl_SetCurSel( hWnd, nItem );
	}
	int SetExtendedStyle( int nExStyle ) const {
		return TabCtrl_SetExtendedStyle( hWnd, nExStyle );
	}
	bool SetImageList( HIMAGELIST himl ) const {
		return TabCtrl_SetImageList( hWnd, himl ) != FALSE;
	}
	bool SetItem( int nItem, const TCITEM& item ) const {
		return TabCtrl_SetItem( hWnd, nItem, &item ) != FALSE;
	}
	bool SetItem( int nItem, LPCTSTR pszText, int nImage = -1, int lParam = 0 ) const {
		TCITEM item;
		item.mask = TCIF_TEXT | TCIF_IMAGE | TCIF_PARAM;
		item.pszText = const_cast<TCHAR*>( pszText );
		item.iImage = nImage;
		item.lParam = lParam;
		return SetItem( nItem, item );
	}
	bool SetItemState( int nItem, int nState, int nStateMask ) const {
		TCITEM item;
		item.mask = TCIF_STATE;
		item.dwState = nState;
		item.dwStateMask = nStateMask;
		return SetItem( nItem, item );
	}
	bool SetItemExtra( int cb ) const {
		return TabCtrl_SetItemExtra( hWnd, cb ) != FALSE;
	}
	WSize SetItemSize( int width, int height ) const {
		int size = TabCtrl_SetItemSize( hWnd, width, height );
		return WSize( LOWORD( size ), HIWORD( size ) );
	}
	int SetMinTabWidth( int width ) const {
		return TabCtrl_SetMinTabWidth( hWnd, width );
	}
	void SetPadding( int width, int height ) const {
		TabCtrl_SetPadding( hWnd, width, height );
	}
	void SetToolTips( HWND hwndTT ) const {
		TabCtrl_SetToolTips( hWnd, hwndTT );
	}
	bool SetUnicodeFormat( bool bUnicode ) const {
		return TabCtrl_SetUnicodeFormat( hWnd, bUnicode ) != FALSE;
	}
};

/**********************************************************************************************
* Toolbar
**********************************************************************************************/

class Toolbar: public Control {
public:
	Toolbar(): Control( 0 ) { }
	Toolbar( HWND hwnd ): Control( hwnd ) { }
	bool Create( HWND hwndParent,
				 int id = -1,
				 DWORD dwStyle = TBSTYLE_FLAT | TBSTYLE_TOOLTIPS,
				 int nBitmaps = 0,
				 int nBitmapID = 0,
				 LPCTBBUTTON lpButtons = NULL,
				 int nButtons = 0,
				 WSize nButtonSize = WSize( 0, 0 ),
				 WSize nBitmapSize = WSize( 0, 0 ),
				 int nStructSize = sizeof( TBBUTTON ),
				 HINSTANCE hInstance = __MODULE_HANDLE ) {
		hWnd = CreateToolbarEx( hwndParent,
			dwStyle | WS_CHILD | WS_VISIBLE, id,
			nBitmaps, hInstance, nBitmapID, lpButtons, nButtons,
			nButtonSize.cx, nButtonSize.cy, nBitmapSize.cx,
			nBitmapSize.cy, nStructSize );
		return IsCreated();
	}
	int AddBitmap( int nButtons, int id, HINSTANCE hinst = __MODULE_HANDLE ) const {
		TBADDBITMAP tbab = { hinst, id };
		return SendMessage( TB_ADDBITMAP, nButtons, reinterpret_cast<LPARAM>( &tbab ) );
	}
	bool AddButtons( LPTBBUTTON lpButtons, int nButtons ) const {
		return SendMessage( TB_ADDBUTTONS, nButtons, reinterpret_cast<LPARAM>( lpButtons ) ) != 0;
	}
	bool AddButton( int nBitmap, int nCommand, int nState = TBSTATE_ENABLED, int nStyle = TBSTYLE_BUTTON, int nData = 0, int nString = -1 ) {
		TBBUTTON button = { nBitmap, nCommand, nState, nStyle,
#if defined(_WIN32) | defined(_WIN64)
		{0},
#endif
		nData, nString };
		return AddButtons( &button, 1 );
	}
	bool AddSeparator() {
		TBBUTTON button = { 0, 0, TBSTATE_ENABLED, TBSTYLE_SEP, 0, 0 };
		return AddButtons( &button, 1 );
	}
	int AddString( int idString, HINSTANCE hinst = __MODULE_HANDLE ) const {
		return SendMessage( TB_ADDSTRING, reinterpret_cast<WPARAM>( hinst ), idString );
	}
	int AddStrings( const char* strings ) const {
		return SendMessage( TB_ADDSTRING, 0, reinterpret_cast<LPARAM>( strings ) );
	}
	void AutoSize() const {
		SendMessage( TB_AUTOSIZE );
	}
	int ButtonCount() const {
		return SendMessage( TB_BUTTONCOUNT );
	}
	void ButtonStructSize( int cb ) const {
		SendMessage( TB_BUTTONSTRUCTSIZE, cb );
	}
	bool ChangeBitmap( int idCommand, int nBitmap ) const {
		return SendMessage( TB_CHANGEBITMAP, idCommand, nBitmap ) != 0;
	}
	bool CheckButton( int idCommand, bool bCheck = true ) const {
		return SendMessage( TB_CHECKBUTTON, idCommand, bCheck ) != 0;
	}
	int CommandToIndex( int idCommand ) const {
		return SendMessage( TB_COMMANDTOINDEX, idCommand );
	}
	void Customize() const {
		SendMessage( TB_CUSTOMIZE );
	}
	bool DeleteButton( int nButton ) const {
		return SendMessage( TB_DELETEBUTTON, nButton ) != 0;
	}
	bool EnableButton( int idCommand, bool bEnable = true ) const {
		return SendMessage( TB_ENABLEBUTTON, idCommand, bEnable ) != 0;
	}
	bool GetAnchorHighlight() const {
		return SendMessage( TB_GETANCHORHIGHLIGHT ) != 0;
	}
	int GetBitmap( int idCommand ) const {
		return SendMessage( TB_GETBITMAP, idCommand );
	}
	int GetBitmapFlags() const {
		return SendMessage( TB_GETBITMAPFLAGS );
	}
	bool GetButton( int nButton, LPTBBUTTON lpButton ) const {
		return SendMessage( TB_GETBUTTON, nButton, reinterpret_cast<LPARAM>( lpButton ) ) != 0;
	}
	int GetButtonInfo( int idCommand, LPTBBUTTONINFO lptbbi ) const {
		return SendMessage( TB_GETBUTTONINFO, idCommand, reinterpret_cast<LPARAM>( lptbbi ) );
	}
	WSize GetButtonSize() const {
		const DWORD s = SendMessage( TB_GETBUTTONSIZE );
		return WSize( LOWORD( s ), HIWORD( s ) );
	}
#ifndef SKIP_STL
	tstring GetButtonText( int idCommand ) const;
#else
	int GetButtonText( int idCommand, TCHAR* buf ) const {
		return SendMessage( TB_GETBUTTONTEXT, idCommand, reinterpret_cast<LPARAM>( buf ) );
	}
#endif
	bool GetColorScheme( LPCOLORSCHEME lpcs ) const {
		return SendMessage( TB_GETCOLORSCHEME, 0, reinterpret_cast<LPARAM>( lpcs ) ) != 0;
	}
	ImageList GetDisabledImageList() const {
		return windows_cast<HIMAGELIST>( SendMessage( TB_GETDISABLEDIMAGELIST ) );
	}
	int GetExtendedStyle() const {
		return SendMessage( TB_GETEXTENDEDSTYLE );
	}
	ImageList GetHotImageList() const {
		return windows_cast<HIMAGELIST>( SendMessage( TB_GETHOTIMAGELIST ) );
	}
	int GetHotItem() const {
		return SendMessage( TB_GETHOTITEM );
	}
	ImageList GetImageList() const {
		return windows_cast<HIMAGELIST>( SendMessage( TB_GETIMAGELIST ) );
	}
	void GetInsertMark( LPTBINSERTMARK lptbim ) const {
		SendMessage( TB_GETINSERTMARK, 0, reinterpret_cast<LPARAM>( lptbim ) );
	}
	COLORREF GetInsertMarkColor() const {
		return static_cast<COLORREF>( SendMessage( TB_GETINSERTMARKCOLOR ) );
	}
	bool GetItemRect( int nButton, RECT* prc ) const {
		return SendMessage( TB_GETITEMRECT, nButton, reinterpret_cast<LPARAM>( prc ) ) != 0;
	}
	bool GetMaxSize( SIZE* pSize ) const {
		return SendMessage( TB_GETMAXSIZE, 0, reinterpret_cast<LPARAM>( pSize ) ) != 0;
	}
	WSize GetMaxSize() const {
		WSize size( 0, 0 );
		GetMaxSize( &size );
		return size;
	}
	WSize GetPadding() const {
		const int p = SendMessage( TB_GETPADDING );
		return WSize( LOWORD( p ), HIWORD( p ) );
	}
	bool GetRect( int nButton, RECT* prc ) const {
		return SendMessage( TB_GETRECT, nButton, reinterpret_cast<LPARAM>( prc ) ) != 0;
	}
	int GetRows() const {
		return SendMessage( TB_GETROWS );
	}
	int GetState( int idCommand ) const {
		return SendMessage( TB_GETSTATE, idCommand );
	}
	int GetStyle() const {
		return SendMessage( TB_GETSTYLE );
	}
	int GetTextRows() const {
		return SendMessage( TB_GETTEXTROWS );
	}
	ToolTip GetToolTips() const {
		return windows_cast<HWND>( SendMessage( TB_GETTOOLTIPS ) );
	}
	bool GetUnicodeFormat() const {
		return SendMessage( TB_GETUNICODEFORMAT ) != 0;
	}
	bool HideButton( int idCommand, bool bShow = false ) const {
		return SendMessage( TB_HIDEBUTTON, idCommand, bShow ) != 0;
	}
	int HitTest( POINT* ppt ) const {
		return SendMessage( TB_HITTEST, 0, reinterpret_cast<LPARAM>( ppt ) );
	}
	bool Indeterminate( int idCommand, bool bIndeterminate = true ) const {
		return SendMessage( TB_INDETERMINATE, idCommand, bIndeterminate ) != 0;
	}
	bool InsertButton( int nButton, LPTBBUTTON lpButton ) const {
		return SendMessage( TB_INSERTBUTTON, nButton, reinterpret_cast<LPARAM>( lpButton ) ) != 0;
	}
	bool InsertButton( int nButton, int nBitmap, int nCommand, int nState = TBSTATE_ENABLED, int nStyle = TBSTYLE_BUTTON, int nData = 0, int nString = -1 ) {
		TBBUTTON button = { nBitmap, nCommand, nState, nStyle, nData, nString };
		return InsertButton( nButton, &button );
	}
	bool InsertMarkHitTest( const POINT& pt, LPTBINSERTMARK lptbim ) const {
		return SendMessage( TB_INSERTMARKHITTEST, reinterpret_cast<WPARAM>( &pt ), reinterpret_cast<LPARAM>( lptbim ) ) != 0;
	}
	bool IsButtonChecked( int idCommand ) const {
		return SendMessage( TB_ISBUTTONCHECKED, idCommand ) != 0;
	}
	bool IsButtonEnabled( int idCommand ) const {
		return SendMessage( TB_ISBUTTONENABLED, idCommand ) != 0;
	}
	bool IsButtonHidden( int idCommand ) const {
		return SendMessage( TB_ISBUTTONHIDDEN, idCommand ) != 0;
	}
	bool IsButtonHighlighted( int idCommand ) const {
		return SendMessage( TB_ISBUTTONHIGHLIGHTED, idCommand ) != 0;
	}
	bool IsButtonIndeterminate( int idCommand ) const {
		return SendMessage( TB_ISBUTTONINDETERMINATE, idCommand ) != 0;
	}
	bool IsButtonPressed( int idCommand ) const {
		return SendMessage( TB_ISBUTTONPRESSED, idCommand ) != 0;
	}
	int LoadImages( int idBitmap, HINSTANCE hinst = __MODULE_HANDLE ) const {
		return SendMessage( TB_LOADIMAGES, idBitmap, reinterpret_cast<LPARAM>( hinst ) );
	}
	bool MapAccelerator( char chAccel, int* pidCommand ) const {
		return SendMessage( TB_MAPACCELERATOR, chAccel, reinterpret_cast<LPARAM>( pidCommand ) ) != 0;
	}
#ifndef MINGW
	bool MarkButton( int idCommand, bool bHighlight = true ) const {
		return SendMessage( TB_MARKBUTTON, idCommand, bHighlight ) != 0;
	}
#endif
	bool MoveButton( int nOldPos, int nNewPos ) const {
		return SendMessage( TB_MOVEBUTTON, nOldPos, nNewPos ) != 0;
	}
	bool PressButton( int idCommand, bool bPress = true ) const {
		return SendMessage( TB_PRESSBUTTON, idCommand, bPress ) != 0;
	}
	bool ReplaceBitmap( LPTBREPLACEBITMAP ptbrb ) const {
		return SendMessage( TB_REPLACEBITMAP, 0, reinterpret_cast<LPARAM>( ptbrb ) ) != 0;
	}
	void Restore( TBSAVEPARAMS* ptbsp ) const {
		SendMessage( TB_SAVERESTORE, FALSE, reinterpret_cast<LPARAM>( ptbsp ) );
	}
	void Save( TBSAVEPARAMS* ptbsp ) const {
		SendMessage( TB_SAVERESTORE, TRUE, reinterpret_cast<LPARAM>( ptbsp ) );
	}
	bool SetAnchorHighlight( bool bAnchor ) const {
		return SendMessage( TB_SETANCHORHIGHLIGHT, bAnchor ) != 0;
	}
	bool SetBitmapSize( int nWidth, int nHeight ) const {
		return SendMessage( TB_SETBITMAPSIZE, 0, MAKELONG( nWidth, nHeight ) ) != 0;
	}
	bool SetButtonInfo( int idCommand, LPTBBUTTONINFO lptbbi ) const {
		return SendMessage( TB_SETBUTTONINFO, idCommand, reinterpret_cast<LPARAM>( lptbbi ) ) != 0;
	}
	bool SetButtonSize( int nWidth, int nHeight ) const {
		return SendMessage( TB_SETBUTTONSIZE, 0, MAKELONG( nWidth, nHeight ) ) != 0;
	}
	bool SetButtonWidth( int nMin, int nMax ) const {
		return SendMessage( TB_SETBUTTONWIDTH, 0, MAKELONG( nMin, nMax ) ) != 0;
	}
	bool SetCmdID( int nButton, int idCommand ) const {
		return SendMessage( TB_SETCMDID, nButton, idCommand ) != 0;
	}
	void SetColorScheme( LPCOLORSCHEME lpcs ) const {
		SendMessage( TB_SETCOLORSCHEME, 0, reinterpret_cast<LPARAM>( lpcs ) );
	}
	ImageList SetDisabledImageList( HIMAGELIST himlNewDisabled ) const {
		return windows_cast<HIMAGELIST>( SendMessage( TB_SETDISABLEDIMAGELIST, 0, reinterpret_cast<LPARAM>( himlNewDisabled ) ) );
	}
	int SetDrawTextFlags( int nMask, int nDTFlags ) const {
		return SendMessage( TB_SETDRAWTEXTFLAGS, nMask, nDTFlags );
	}
	int SetExtendedStyle( int nStyle ) const {
		return SendMessage( TB_SETEXTENDEDSTYLE, 0, nStyle );
	}
	ImageList SetHotImageList( HIMAGELIST himl ) const {
		return windows_cast<HIMAGELIST>( SendMessage( TB_SETHOTIMAGELIST, 0, reinterpret_cast<LPARAM>( himl ) ) );
	}
	int SetHotItem( int nHot ) const {
		return SendMessage( TB_SETHOTITEM, nHot );
	}
	ImageList SetImageList( HIMAGELIST himl ) const {
		return windows_cast<HIMAGELIST>( SendMessage( TB_SETIMAGELIST, 0, reinterpret_cast<LPARAM>( himl ) ) );
	}
	bool SetIndent( int nIndent ) const {
		return SendMessage( TB_SETINDENT, nIndent ) != 0;
	}
	void SetInsertMark( LPTBINSERTMARK lptbim ) const {
		SendMessage( TB_SETINSERTMARK, 0, reinterpret_cast<LPARAM>( lptbim ) );
	}
	COLORREF SetInsertMarkColor( COLORREF clr ) const {
		return static_cast<COLORREF>( SendMessage( TB_SETINSERTMARKCOLOR, 0, static_cast<LPARAM>( clr ) ) );
	}
	bool SetMaxTextRows( int nMaxRows ) const {
		return SendMessage( TB_SETMAXTEXTROWS, nMaxRows ) != 0;
	}
	WSize SetPadding( const SIZE& pad ) const {
		const int p = SendMessage( TB_SETPADDING, 0, MAKELPARAM( pad.cx, pad.cy ) );
		return WSize( LOWORD( p ), HIWORD( p ) );
	}
	GenericWindow SetParent( HWND hwndParent ) const {
		return windows_cast<HWND>( SendMessage( TB_SETPARENT, reinterpret_cast<WPARAM>( hwndParent ) ) );
	}
	void SetRows( int nRows, bool bLarger = true ) const {
		SendMessage( TB_SETROWS, MAKEWPARAM( nRows, bLarger ) );
	}
	bool SetState( int idCommand, int nState ) const {
		return SendMessage( TB_SETSTATE, idCommand, nState ) != 0;
	}
	void SetStyle( int nStyle ) const {
		SendMessage( TB_SETSTYLE, 0, nStyle );
	}
	void SetToolTips( HWND hwndToolTip ) const {
		SendMessage( TB_SETTOOLTIPS, reinterpret_cast<WPARAM>( hwndToolTip ) );
	}
	bool SetUnicodeFormat( bool bUnicode ) const {
		return SendMessage( TB_SETUNICODEFORMAT, bUnicode ) != 0;
	}
};

/**********************************************************************************************
* Trackbar
**********************************************************************************************/

class Trackbar: public Control {
public:
	Trackbar(): Control( 0 ) { }
	Trackbar( HWND hwnd ): Control( hwnd ) { }
	bool Create( const RECT& rc, HWND hwndParent, int id, DWORD dwStyle = TBS_HORZ | TBS_BOTTOM | TBS_AUTOTICKS, DWORD dwExStyle = 0 ) {
		return Control::Create( TRACKBAR_CLASS, _T(""), rc, hwndParent, id, dwStyle, dwExStyle );
	}
	void ClearSel( bool bRedraw = true ) const {
		SendMessage( TBM_CLEARSEL, bRedraw );
	}
	void ClearTics( bool bRedraw = true ) const {
		SendMessage( TBM_CLEARTICS, bRedraw );
	}
	WRect GetChannelRect() const {
		WRect rc;
		SendMessage( TBM_GETCHANNELRECT, 0, reinterpret_cast<LPARAM>( &rc ) );
		return rc;
	}
	int GetLineSize() const {
		return SendMessage( TBM_GETLINESIZE );
	}
	int GetNumTics() const {
		return SendMessage( TBM_GETNUMTICS );
	}
	int GetPageSize() const {
		return SendMessage( TBM_GETPAGESIZE );
	}
	int GetPos() const {
		return SendMessage( TBM_GETPOS );
	}
	const int* GetPTics() const {
		return windows_cast<int*>( SendMessage( TBM_GETPTICS ) );
	}
	int GetRangeMax() const {
		return SendMessage( TBM_GETRANGEMAX );
	}
	int GetRangeMin() const {
		return SendMessage( TBM_GETRANGEMIN );
	}
	int GetSelStart() const {
		return SendMessage( TBM_GETSELSTART );
	}
	int GetSelEnd() const {
		return SendMessage( TBM_GETSELEND );
	}
	int GetThumbLength() const {
		return SendMessage( TBM_GETTHUMBLENGTH );
	}
	WRect GetThumbRect() const {
		WRect rc;
		SendMessage( TBM_GETTHUMBRECT, 0, reinterpret_cast<LPARAM>( &rc ) );
		return rc;
	}
	int GetTic( int nTic ) const {
		return SendMessage( TBM_GETTIC, nTic );
	}
	int GetTicPos( int nTic ) const {
		return SendMessage( TBM_GETTICPOS, nTic );
	}
	ToolTip GetToolTips() const {
		return windows_cast<HWND>( SendMessage( TBM_GETTOOLTIPS ) );
	}
	bool GetUnicodeFormat() const {
		return SendMessage( TBM_GETUNICODEFORMAT ) != 0;
	}
	GenericWindow SetLeftBuddy( HWND hwndBuddy ) const {
		return windows_cast<HWND>( SendMessage( TBM_SETBUDDY, TRUE, reinterpret_cast<LPARAM>( hwndBuddy ) ) );
	}
	GenericWindow SetRightBuddy( HWND hwndBuddy ) const {
		return windows_cast<HWND>( SendMessage( TBM_SETBUDDY, FALSE, reinterpret_cast<LPARAM>( hwndBuddy ) ) );
	}
	GenericWindow GetLeftBuddy() const {
		return windows_cast<HWND>( SendMessage( TBM_GETBUDDY, TRUE ) );
	}
	GenericWindow GetRightBuddy() const {
		return windows_cast<HWND>( SendMessage( TBM_GETBUDDY, FALSE ) );
	}
	int SetLineSize( int nLineSize ) const {
		return SendMessage( TBM_SETLINESIZE, 0, nLineSize );
	}
	int SetPageSize( int nPageSize ) const {
		return SendMessage( TBM_SETPAGESIZE, 0, nPageSize );
	}
	void SetPos( int nPos, bool bRedraw = true ) const {
		SendMessage( TBM_SETPOS, bRedraw, nPos );
	}
	void SetRange( int nMin, int nMax, bool bRedraw = true ) const {
		SendMessage( TBM_SETRANGEMIN, bRedraw, nMin );
		SendMessage( TBM_SETRANGEMAX, bRedraw, nMax );
	}
	void SetSel( int nStart, int nEnd, bool bRedraw = true ) const {
		SendMessage( TBM_SETSELSTART, bRedraw, nStart );
		SendMessage( TBM_SETSELEND, bRedraw, nEnd );
	}
	void SetThumbLength( int nLen ) const {
		SendMessage( TBM_SETTHUMBLENGTH, nLen );
	}
	bool SetTic( int nPos ) const {
		return SendMessage( TBM_SETTIC, 0, nPos ) != 0;
	}
	void SetTicFreq( int nFreq ) const {
		SendMessage( TBM_SETTICFREQ, nFreq );
	}
	int SetTipSide( int nSide ) const {
		return SendMessage( TBM_SETTIPSIDE, nSide );
	}
	ToolTip SetToolTips( HWND hwndToolTips ) const {
		return windows_cast<HWND>( SendMessage( TBM_SETTOOLTIPS, 0, reinterpret_cast<LPARAM>( hwndToolTips ) ) );
	}
	bool SetUnicodeFormat( bool bUnicode ) const {
		return SendMessage( TBM_SETUNICODEFORMAT, bUnicode ) != 0;
	}
};

/**********************************************************************************************
* TreeItem
**********************************************************************************************/

class TreeItem {
	HWND hWnd;
	HTREEITEM hitem;
public:
	TreeItem(): hWnd( 0 ), hitem( 0 ) { }
	TreeItem( HWND hwndTreeView, HTREEITEM hitem ):
	hWnd( hwndTreeView ), hitem( hitem ) { }
	operator HTREEITEM() const {
		return hitem;
	}
	ImageList CreateDragImage() const {
		return TreeView_CreateDragImage( hWnd, hitem );
	}
	bool Delete() {
		const HTREEITEM old = hitem; hitem = 0;
		return TreeView_DeleteItem( hWnd, old ) != FALSE;
	}
	Edit EditLabel() const {
		return TreeView_EditLabel( hWnd, hitem );
	}
	bool EnsureVisible() const {
		return TreeView_EnsureVisible( hWnd, hitem ) != FALSE;
	}
	bool Expand() const {
		return Expand( TVE_EXPAND );
	}
	bool ExpandPartial() const {
		return Expand( TVE_EXPAND | TVE_EXPANDPARTIAL );
	}
	bool Collapse() const {
		return Expand( TVE_COLLAPSE );
	}
	bool CollapseReset() const {
		return Expand( TVE_COLLAPSE | TVE_COLLAPSERESET );
	}
	bool Toggle() const {
		return Expand( TVE_TOGGLE );
	}
	TreeItem GetChild() const {
		return GetNextItem( TVGN_CHILD );
	}
	TreeItem GetNextSibling() const {
		return GetNextItem( TVGN_NEXT );
	}
	TreeItem GetPrevSibling() const {
		return GetNextItem( TVGN_PREVIOUS );
	}
	TreeItem GetNextVisible() const {
		return GetNextItem( TVGN_NEXTVISIBLE );
	}
	TreeItem GetPrevVisible() const {
		return GetNextItem( TVGN_PREVIOUSVISIBLE );
	}
	TreeItem GetParent() const {
		return GetNextItem( TVGN_PARENT );
	}
	bool GetItem( TVITEM* pItem ) const {
		pItem->hItem = hitem;
		return TreeView_GetItem( hWnd, pItem ) != FALSE;
	}
#ifndef SKIP_STL
	tstring GetItemText() const;
#endif
	bool GetItemImage( int* pImage, int* pSelectedImage ) const {
		TVITEM item;
		item.mask = TVIF_HANDLE | TVIF_IMAGE | TVIF_SELECTEDIMAGE;
		const bool b_ret = GetItem( &item );
		*pImage = item.iImage;
		*pSelectedImage = item.iSelectedImage;
		return b_ret;
	}
	bool GetItemParam( int* pParam ) const {
		TVITEM item;
		item.mask = TVIF_HANDLE | TVIF_PARAM;
		const bool b_ret = GetItem( &item );
		*pParam = static_cast<int>( item.lParam );
		return b_ret;
	}
	bool GetItemState( int* nState, int nStateMask = -1 ) const {
		TVITEM item;
		item.mask = TVIF_HANDLE | TVIF_STATE;
		item.stateMask = nStateMask;
		const bool b_ret = GetItem( &item );
		*nState = item.state;
		return b_ret;
	}
	bool GetItemRect( RECT* prc, bool bOnlyText = false ) const {
		return TreeView_GetItemRect( hWnd, hitem, prc, bOnlyText ) != 0;
	}
	TreeItem InsertItem( HTREEITEM hInsertAfter, const TVITEM& item ) const {
		TVINSERTSTRUCT is;
		is.hParent = hitem;
		is.hInsertAfter = hInsertAfter;
		is.item = item;
		return TreeItem( hWnd, TreeView_InsertItem( hWnd, &is ) );
	}
	TreeItem InsertItemFirst( const TVITEM& item ) const {
		return InsertItem( TVI_FIRST, item );
	}
	TreeItem InsertItemLast( const TVITEM& item ) const {
		return InsertItem( TVI_LAST, item );
	}
	TreeItem InsertItemSort( const TVITEM& item ) const {
		return InsertItem( TVI_SORT, item );
	}
	TreeItem InsertItem( HTREEITEM hInsertAfter, LPCTSTR lpszText, int nImage, int nSelectedImage, int nState, int lParam ) const {
		TVITEM item;
		item.mask = TVIF_TEXT | TVIF_IMAGE | TVIF_SELECTEDIMAGE | TVIF_STATE | TVIF_PARAM;
		item.pszText = const_cast<TCHAR*>( lpszText );
		item.iImage = nImage;
		item.iSelectedImage = nSelectedImage;
		item.state = nState;
		item.stateMask = nState;
		item.lParam = lParam;
		return InsertItem( hInsertAfter, item );
	}
	TreeItem InsertItemFirst( LPCTSTR lpszText, int nImage, int nSelectedImage, int nState, int lParam ) const {
		return InsertItem( TVI_FIRST, lpszText, nImage, nSelectedImage, nState, lParam );
	}
	TreeItem InsertItemLast( LPCTSTR lpszText, int nImage, int nSelectedImage, int nState, int lParam ) const {
		return InsertItem( TVI_LAST, lpszText, nImage, nSelectedImage, nState, lParam );
	}
	TreeItem InsertItemSort( LPCTSTR lpszText, int nImage, int nSelectedImage, int nState, int lParam ) const {
		return InsertItem( TVI_SORT, lpszText, nImage, nSelectedImage, nState, lParam );
	}
	TreeItem InsertItem( HTREEITEM hInsertAfter, LPCTSTR lpszText, int nImage, int nState = 0, int lParam = 0 ) const {
		return InsertItem( hInsertAfter, lpszText, nImage, nImage, nState, lParam );
	}
	TreeItem InsertItemFirst( LPCTSTR lpszText, int nImage, int nState = 0, int lParam = 0 ) const {
		return InsertItem( TVI_FIRST, lpszText, nImage, nImage, nState, lParam );
	}
	TreeItem InsertItemLast( LPCTSTR lpszText, int nImage, int nState = 0, int lParam = 0 ) const {
		return InsertItem( TVI_LAST, lpszText, nImage, nImage, nState, lParam );
	}
	TreeItem InsertItemSort( LPCTSTR lpszText, int nImage, int nState = 0, int lParam = 0 ) const {
		return InsertItem( TVI_SORT, lpszText, nImage, nImage, nState, lParam );
	}
	bool Select( int flag = TVGN_CARET | TVGN_FIRSTVISIBLE ) const {
		return TreeView_Select( hWnd, hitem, flag ) != FALSE;
	}
	bool SelectDropTarget() const {
		return TreeView_SelectDropTarget( hWnd, hitem ) != FALSE;
	}
	bool SelectItem() const {
		return TreeView_SelectItem( hWnd, hitem ) != FALSE;
	}
	bool SelectSetFirstVisible() const {
		return TreeView_SelectSetFirstVisible( hWnd, hitem ) != FALSE;
	}
	bool SetInsertMark( bool bAfter = true ) const {
		return TreeView_SetInsertMark( hWnd, hitem, bAfter ) != FALSE;
	}
	bool SetItem( const TVITEM& item ) const {
		const_cast<TVITEM&>( item ).hItem = hitem;
		return TreeView_SetItem( hWnd, const_cast<TVITEM*>( &item ) ) != FALSE;
	}
	bool SetItemState( int nState, int nStateMask = -1 ) const {
		TVITEM item;
		item.mask = TVIF_HANDLE | TVIF_STATE;
		item.state = nState;
		item.stateMask = nStateMask;
		return SetItem( item );
	}
	bool SetItem( LPCTSTR lpszText, int nImage, int nSelectedImage ) const {
		TVITEM item;
		item.mask = TVIF_HANDLE | TVIF_TEXT | TVIF_IMAGE | TVIF_SELECTEDIMAGE;
		item.pszText = const_cast<TCHAR*>( lpszText );
		item.iImage = nImage;
		item.iSelectedImage = nSelectedImage;
		return SetItem( item );
	}
	bool SetItem( LPCTSTR lpszText, int nImage ) const {
		return SetItem( lpszText, nImage, nImage );
	}
	bool SetItemParam( LPARAM lParam ) const {
		TVITEM item;
		item.mask = TVIF_HANDLE | TVIF_PARAM;
		item.lParam = lParam;
		return SetItem( item );
	}
	bool SortChildren( bool bRecurse = true ) const {
		return TreeView_SortChildren( hWnd, hitem, bRecurse ) != FALSE;
	}
	bool SortChildren( PFNTVCOMPARE lpfnCompare, LPARAM lParam = 0, bool bRecurse = true ) const {
		const TVSORTCB sort = { hitem, lpfnCompare, lParam };
		return TreeView_SortChildrenCB( hWnd, &sort, bRecurse ) != FALSE;
	}

private:
	bool Expand( int nFlag ) const {
		return TreeView_Expand( hWnd, hitem, nFlag ) != FALSE;
	}
	TreeItem GetNextItem( int nFlag ) const {
		return TreeItem( hWnd, TreeView_GetNextItem( hWnd, hitem, nFlag ) );
	}
};

/**********************************************************************************************
* TreeView
**********************************************************************************************/

class TreeView: public Control {
public:
	TreeView(): Control( 0 ) { }
	TreeView( HWND hwnd ): Control( hwnd ) { }
	bool Create( const RECT& rc, HWND hwndParent, int id, DWORD dwStyle = TVS_HASBUTTONS | TVS_HASLINES | TVS_LINESATROOT | TVS_SHOWSELALWAYS, DWORD dwExStyle = 0 ) {
		return Control::Create( WC_TREEVIEW, _T(""), rc, hwndParent, id, dwStyle, dwExStyle );
	}
	bool DeleteAllItems() const {
		return TreeView_DeleteAllItems( hWnd ) != FALSE;
	}
	bool EndEditLabelNow( bool bCancel = true ) const {
		return TreeView_EndEditLabelNow( hWnd, bCancel ) != FALSE;
	}
	COLORREF GetBkColor() const {
		return TreeView_GetBkColor( hWnd );
	}
	int GetCount() const {
		return TreeView_GetCount( hWnd );
	}
	TreeItem GetSelection() const {
		return GetItem( TVGN_CARET );
	}
	TreeItem GetDropHilight() const {
		return GetItem( TVGN_DROPHILITE );
	}
	TreeItem GetFirstVisible() const {
		return GetItem( TVGN_FIRSTVISIBLE );
	}
	TreeItem GetLastVisible() const {
		return GetItem( TVGN_LASTVISIBLE );
	}
	TreeItem GetRoot() const {
		return GetItem( TVGN_ROOT );
	}
	Edit GetEditControl() const {
		return TreeView_GetEditControl( hWnd );
	}
	ImageList GetImageList() const {
		return TreeView_GetImageList( hWnd, TVSIL_NORMAL );
	}
	ImageList GetStateImageList() const {
		return TreeView_GetImageList( hWnd, TVSIL_STATE );
	}
	int GetIndent() const {
		return TreeView_GetIndent( hWnd );
	}
	COLORREF GetInsertMarkColor() const {
		return TreeView_GetInsertMarkColor( hWnd );
	}
	bool GetISearchString( LPTSTR lpsz ) const {
		return TreeView_GetISearchString( hWnd, lpsz ) != FALSE;
	}
	int GetItemHeight() const {
		return TreeView_GetItemHeight( hWnd );
	}
	int GetScrollTime() const {
		return TreeView_GetScrollTime( hWnd );
	}
	COLORREF GetTextColor() const {
		return TreeView_GetTextColor( hWnd );
	}
	ToolTip GetToolTips() const {
		return windows_cast<HWND>( SendMessage( TVM_GETTOOLTIPS ) );
	}
	bool GetUnicodeFormat() const {
		return SendMessage( TVM_GETUNICODEFORMAT ) != 0;
	}
	int GetVisibleCount() const {
		return TreeView_GetVisibleCount( hWnd );
	}
	TreeItem HitTest( const POINT& pt, int* flags = 0 ) const {
		TVHITTESTINFO ht;
		ht.pt = pt;
		const TreeItem ti( hWnd, TreeView_HitTest( hWnd, &ht ) );
		if ( flags != 0 ) *flags = ht.flags;
		return ti;
	}
	COLORREF SetBkColor( COLORREF clr ) const {
		return TreeView_SetBkColor( hWnd, clr );
	}
	ImageList SetImageList( HIMAGELIST himl ) const {
		return TreeView_SetImageList( hWnd, himl, TVSIL_NORMAL );
	}
	ImageList SetStateImageList( HIMAGELIST himl ) const {
		return TreeView_SetImageList( hWnd, himl, TVSIL_STATE );
	}
	bool SetIndent( int indent ) const {
		return TreeView_SetIndent( hWnd, indent ) != FALSE;
	}
	COLORREF SetInsertMarkColor( COLORREF clr ) const {
		return TreeView_SetInsertMarkColor( hWnd, clr );
	}
	int SetItemHeight( int nHeight ) const {
		return TreeView_SetItemHeight( hWnd, nHeight );
	}
	int SetScrollTime( int nTime ) const {
		return TreeView_SetScrollTime( hWnd, nTime );
	}
	COLORREF SetTextColor( COLORREF clr ) const {
		return TreeView_SetTextColor( hWnd, clr );
	}
	ToolTip SetToolTips( HWND hwndToolTips ) const {
		return windows_cast<HWND>( SendMessage( TVM_SETTOOLTIPS, 0, reinterpret_cast<LPARAM>( hwndToolTips ) ) );
	}
	bool SetUnicodeFormat( bool bUnicode ) const {
		return SendMessage( TVM_SETUNICODEFORMAT, bUnicode ) != 0;
	}

private:
	TreeItem GetItem( int flag ) const {
		return TreeItem( *this, TreeView_GetNextItem( hWnd, 0, flag ) );
	}
};

/**********************************************************************************************
* ComboBoxEx
**********************************************************************************************/

class ComboBoxEx: public ComboBox {
public:
	ComboBoxEx() { }
	ComboBoxEx( HWND hwnd ): ComboBox( hwnd ) { }
	bool Create( const RECT& rc, HWND hwndParent, int id, DWORD dwStyle = CBS_DROPDOWN | CBS_AUTOHSCROLL ) {
		return Control::Create( WC_COMBOBOXEX, _T(""), rc, hwndParent, id, dwStyle );
	}
	int DeleteItem( int nIndex ) const {
		return SendMessage( CBEM_DELETEITEM, nIndex );
	}
	ComboBox GetComboControl() const {
		return windows_cast<HWND>( SendMessage( CBEM_GETCOMBOCONTROL ) );
	}
	Edit GetEditControl() const {
		return windows_cast<HWND>( SendMessage( CBEM_GETEDITCONTROL ) );
	}
	int GetExtendedStyle() const {
		return SendMessage( CBEM_GETEXTENDEDSTYLE );
	}
	ImageList GetImageList() const {
		return windows_cast<HIMAGELIST>( SendMessage( CBEM_GETIMAGELIST ) );
	}
	bool GetItem( PCOMBOBOXEXITEM pCBItem ) const {
		return SendMessage( CBEM_GETITEM, 0, reinterpret_cast<LPARAM>( pCBItem ) ) != 0;
	}
#ifndef SKIP_STL
	tstring GetItemText( int nItem ) const;
#endif
	bool GetItemImage( int nItem, int* nImage, int* nSelectedImage ) const {
		COMBOBOXEXITEM item;
		item.mask = CBEIF_IMAGE | CBEIF_SELECTEDIMAGE;
		item.iItem = nItem;
		const bool b_ret = GetItem( &item );
		*nImage = item.iImage;
		*nSelectedImage = item.iSelectedImage;
		return b_ret;
	}
	bool GetItemParam( int nItem, int* pParam ) const {
		COMBOBOXEXITEM item;
		item.mask = CBEIF_LPARAM;
		item.iItem = nItem;
		const bool b_ret = GetItem( &item );
		*pParam = static_cast<int>( item.lParam );
		return b_ret;
	}
	bool GetUnicodeFormat() const {
		return SendMessage( CBEM_GETUNICODEFORMAT ) != 0;
	}
	bool HasEditChanged() const {
		return SendMessage( CBEM_HASEDITCHANGED ) != 0;
	}
	int InsertItem( const COMBOBOXEXITEM* lpcCBItem ) const {
		return SendMessage( CBEM_INSERTITEM, 0, reinterpret_cast<LPARAM>( lpcCBItem ) );
	}
	int InsertItem( int nItem, LPCTSTR lpszText, int nImage, int nSelectedImage, int lParam = 0 ) const {
		COMBOBOXEXITEM item;
		item.mask = CBEIF_TEXT | CBEIF_IMAGE | CBEIF_SELECTEDIMAGE | CBEIF_LPARAM;
		item.iItem = nItem;
		item.pszText = const_cast<TCHAR*>( lpszText );
		item.iImage = nImage;
		item.iSelectedImage = nSelectedImage;
		item.lParam = lParam;
		return InsertItem( &item );
	}
	int InsertItem( int nItem, LPCTSTR lpszText, int nImage ) const {
		return InsertItem( nItem, lpszText, nImage, nImage );
	}
	int InsertItem( LPCTSTR lpszText, int nImage, int nSelectedImage, int lParam = 0 ) const {
		return InsertItem( -1, lpszText, nImage, nSelectedImage, lParam );
	}
	int InsertItem( LPCTSTR lpszText, int nImage ) const {
		return InsertItem( lpszText, nImage, nImage );
	}
	int SetExtendedStyle( int nStyle, int nMask ) const {
		return SendMessage( CBEM_SETEXTENDEDSTYLE, nMask, nStyle );
	}
	ImageList SetImageList( HIMAGELIST himl ) const {
		return reinterpret_cast<HIMAGELIST>( SendMessage( CBEM_SETIMAGELIST ), 0, reinterpret_cast<LPARAM>( himl ) );
	}
	bool SetItem( const COMBOBOXEXITEM* lpcCBItem ) const {
		return SendMessage( CBEM_INSERTITEM, 0, reinterpret_cast<LPARAM>( lpcCBItem ) ) != 0;
	}
	bool SetItem( int nItem, LPCTSTR lpszText, int nImage, int nSelectedImage ) const {
		COMBOBOXEXITEM item;
		item.mask = CBEIF_TEXT | CBEIF_IMAGE | CBEIF_SELECTEDIMAGE;
		item.iItem = nItem;
		item.pszText = const_cast<TCHAR*>( lpszText );
		item.iImage = nImage;
		item.iSelectedImage = nSelectedImage;
		return SetItem( &item );
	}
	bool SetItem( int nItem, LPCTSTR lpszText, int nImage ) const {
		return SetItem( nItem, lpszText, nImage, nImage );
	}
	bool SetItemParam( int nItem, int lParam ) const {
		COMBOBOXEXITEM item;
		item.mask = CBEIF_LPARAM;
		item.iItem = nItem;
		item.lParam = lParam;
		return SetItem( &item );
	}
	bool SetUnicodeFormat( bool bUnicode ) const {
		return SendMessage( CBEM_SETUNICODEFORMAT, bUnicode ) != 0;
	}
};

/**********************************************************************************************
* UpDown
**********************************************************************************************/

class UpDown: public Control {
public:
	UpDown(): Control( 0 ) { }
	UpDown( HWND hwnd ): Control( hwnd ) { }
	bool Create( const RECT& rc, HWND hwndParent, int id, DWORD dwStyle = UDS_ALIGNRIGHT | UDS_ARROWKEYS | UDS_HOTTRACK | UDS_NOTHOUSANDS | UDS_SETBUDDYINT, DWORD dwExStyle = 0 ) {
		return Control::Create( UPDOWN_CLASS, 0, rc, hwndParent, id, dwStyle, dwExStyle );
	}
	int GetAccel( LPUDACCEL paAccels, int num_accels ) const {
		return SendMessage( UDM_GETACCEL, num_accels, reinterpret_cast<LPARAM>( paAccels ) );
	}
	int GetBase() const {
		return SendMessage( UDM_GETBASE );
	}
	GenericWindow GetBuddy() const {
		return windows_cast<HWND>( SendMessage( UDM_GETBUDDY ) );
	}
	int GetPos( bool* b_error = 0 ) const {
		BOOL error = FALSE;
#if (_WIN32_IE >= 0x0500)
		const int retval = SendMessage( UDM_GETPOS32, 0, reinterpret_cast<LPARAM>( &error ) );
#else
		const int retval = SendMessage( UDM_GETPOS, 0, reinterpret_cast<LPARAM>( &error ) );
#endif
		if ( b_error != 0 ) *b_error = error != FALSE;
		return retval;
	}
	void GetRange( int* p_min, int* p_max ) const {
		SendMessage( UDM_GETRANGE32, reinterpret_cast<WPARAM>( p_min ), reinterpret_cast<LPARAM>( p_max ) );
	}
	bool SetAccel( LPUDACCEL pAccels, int num_accels ) const {
		return SendMessage( UDM_SETACCEL, num_accels, reinterpret_cast<LPARAM>( pAccels ) ) != FALSE;
	}
	int SetBase( int base ) const {
		return SendMessage( UDM_SETBASE, base );
	}
	GenericWindow SetBuddy( HWND hwndBuddy ) const {
		return windows_cast<HWND>( SendMessage( UDM_SETBUDDY, reinterpret_cast<WPARAM>( hwndBuddy ) ) );
	}
	int SetPos( int new_pos ) const {
#if _WIN32_IE >= 0x0500
		return SendMessage( UDM_SETPOS32, 0, new_pos );
#else
		return SendMessage( UDM_SETPOS, 0, new_pos );
#endif
	}
	void SetRange( int min, int max ) const {
		SendMessage( UDM_SETRANGE32, min, max );
	}
};

/**********************************************************************************************
* Pager
**********************************************************************************************/

class Pager: public Control {
public:
	Pager(): Control( 0 ) { }
	Pager( HWND hwnd ): Control( hwnd ) { }
	bool Create( HWND hwnd, DWORD dwStyle = PGS_HORZ, DWORD dwExStyle = WS_EX_TOPMOST ) {
		return GenericWindow::Create( 0, WC_PAGESCROLLER, WS_CHILD | dwStyle,
			WRect( WPoint( CW_USEDEFAULT, 0 ), CW_USEDEFAULT, 0 ), hwnd, 0, dwExStyle );
	}
	void ForwardMouse( bool bEnable ) const {
		SendMessage( PGM_FORWARDMOUSE, static_cast<WPARAM>( bEnable ), 0 );
	}
	COLORREF GetBkColor() const {
		return SendMessage( PGM_GETBKCOLOR, 0, 0 );
	}
	int GetBorder() const {
		return SendMessage( PGM_GETBORDER, 0, 0 );
	}
	int GetButtonSize() const {
		return SendMessage( PGM_GETBUTTONSIZE, 0, 0 );
	}
	int GetButtonState( int iButton ) const {
		return SendMessage( PGM_GETBUTTONSTATE, 0, static_cast<LPARAM>( iButton ) );
	}
	int GetPos() const {
		return SendMessage( PGM_GETPOS, 0, 0 );
	}
	void RecalcSize() const {
		SendMessage( PGM_RECALCSIZE, 0, 0 );
	}
	COLORREF SetBkColor( COLORREF color ) const {
		return SendMessage( PGM_SETBKCOLOR, 0, static_cast<LPARAM>( color ) );
	}
	int SetBorder( int iBorder ) {
		return SendMessage( PGM_SETBORDER, 0, static_cast<LPARAM>( iBorder ) );
	}
	int SetButtonSize( int iButtonSize ) {
		return SendMessage( PGM_SETBUTTONSIZE, 0, static_cast<LPARAM>( iButtonSize ) );
	}
	void SetChild( HWND hwnd_child ) const {
		SendMessage( PGM_SETCHILD, 0, (LPARAM)hwnd_child );
	}
	void SetPos( int iPos ) {
		SendMessage( PGM_SETPOS, 0, static_cast<LPARAM>( iPos ) );
	}
};

/**********************************************************************************************
* File Open/Save common dialog box
**********************************************************************************************/

class GetFileNameDialog: public OPENFILENAME {
	enum { FILENAME_LENGTH = 2*MAX_PATH };
	TCHAR filename[ FILENAME_LENGTH ];
public:
	GetFileNameDialog( const OPENFILENAME& ofn ) { *this = ofn; }
	GetFileNameDialog(
				HWND hwndParent = 0,
				const TCHAR* lpstrFilter = _T("All Files (*.*)\0*.*\0"),
				DWORD Flags = OFN_EXPLORER | OFN_HIDEREADONLY,
				const TCHAR* lpstrFileName = _T(""),
				const TCHAR* lpstrDialogTitle = 0,
				const TCHAR* lpstrDefaultExtension = 0 ) {
		OPENFILENAME& ofn = *this;
		ZeroMemory( &ofn, sizeof( ofn ) );
		ofn.lStructSize = sizeof( OPENFILENAME );
		ofn.lpstrFilter = lpstrFilter;
		ofn.lpstrFile = filename;
		ofn.nMaxFile = FILENAME_LENGTH;
		ofn.lpstrTitle = lpstrDialogTitle;
		ofn.Flags = Flags;
		ofn.lpstrDefExt = lpstrDefaultExtension;
		ofn.hInstance = __MODULE_HANDLE;
		ofn.hwndOwner = hwndParent;
		SetFileName( lpstrFileName );
	}

	bool GoOpen() { return GetOpenFileName( this ) != FALSE; }
	bool GoSave() { return GetSaveFileName( this ) != FALSE; }

	void SetFilterIndex( int index ) { nFilterIndex = static_cast<DWORD>( index ); }
	int GetFilterIndex() const { return nFilterIndex; }
	void SetFileName( const TCHAR* lpstrFileName ) {
		#ifdef _MSC_VER
		#pragma warning(push)
		#pragma warning(disable : 4996)
		#endif
		// Ensure that the end of the buffer is null terminated
		_tcsncpy( filename, lpstrFileName, FILENAME_LENGTH );
		filename[FILENAME_LENGTH - 1] = '\0';
		#ifdef _MSC_VER
		#pragma warning(pop)
		#endif
	}
	const TCHAR* GetPathName() const { return filename; }
	const TCHAR* GetFileName() const { return filename + nFileOffset; }
	const TCHAR* GetExtension() const { return filename + nFileExtension; }
	int GetFileNameOffset() const { return nFileOffset; }
	int GetExtensionOffset() const { return nFileExtension; }
};

/**********************************************************************************************
* Browse for folder common dialog box
**********************************************************************************************/

class GetFolderDialog: public BROWSEINFO {
	TCHAR foldername[ MAX_PATH ];
	LPCITEMIDLIST lpiil;
public:
	GetFolderDialog( HWND hwndParent = 0, LPCTSTR lpszTitle = 0, UINT ulFlags = 0, LPCITEMIDLIST pidlRoot = 0 ) {
		ZeroMemory( static_cast<BROWSEINFO*>( this ), sizeof( BROWSEINFO ) );
		hwndOwner = hwndParent;
		this->pidlRoot = pidlRoot;
		this->lpszTitle = lpszTitle;
		this->ulFlags = ulFlags;
	}
	bool Go() {
		lpiil = SHBrowseForFolder( this );
		if ( lpiil == NULL ) return false;
		return SHGetPathFromIDList( lpiil, foldername ) != FALSE;
	}
	const TCHAR* GetFolderName() const { return foldername; }
	LPCITEMIDLIST GetIDList() const { return lpiil; }
};

/**********************************************************************************************
* CriticalSection
**********************************************************************************************/

class CriticalSection: public NonCopyAble {
	mutable CRITICAL_SECTION cs;
public:
	CriticalSection() {
		InitializeCriticalSection( &cs );
	}
	~CriticalSection() {
		DeleteCriticalSection( &cs );
	}
	void Enter() const {
		EnterCriticalSection( &cs );
	}
	void Leave() const {
		LeaveCriticalSection( &cs );
	}
#if (_WIN32_WINNT >= 0x0400)
	bool TryEnter() const { // Warning: not available on WinNT 3.51, Win95, Win98, WinME
		return TryEnterCriticalSection( &cs ) != FALSE;
	}
#endif

	class Lock: public NonCopyAble {
		const CriticalSection& cs;
	public:
		Lock( const CriticalSection& cs ): cs( cs ) {
			cs.Enter();
		}
		~Lock() {
			cs.Leave();
		}
	};
};

/**********************************************************************************************
* Semaphore
**********************************************************************************************/

class Semaphore: public NonCopyAble {
	HANDLE hSem;
public:
	Semaphore( LONG lInitialCount, LONG lMaximumCount = 0x7FFFFFFF, LPCTSTR lpName = 0, LPSECURITY_ATTRIBUTES lpSecurityAttributes = 0 ): hSem( NULL ) {
		hSem = CreateSemaphore( lpSecurityAttributes, lInitialCount, lMaximumCount, lpName );
	}
	~Semaphore() {
		CloseHandle( hSem );
	}
	DWORD Acquire( DWORD dwMilliseconds = INFINITE ) const {
		WaitForSingleObject( hSem, dwMilliseconds );
	}
	bool Release( LONG lReleaseCount = 1 ) const {
		return ReleaseSemaphore( hSem, lReleaseCount, 0 ) != FALSE;
	}
};

/**********************************************************************************************
* Thread
**********************************************************************************************/

class Thread: public NonCopyAble {
	HANDLE hThread;
protected:
	virtual void Run() = 0;
public:
	Thread(): hThread( NULL ) { }
	~Thread() {
		WaitForSingleObject( hThread, INFINITE );
		CloseHandle( hThread );
	}
	operator HANDLE() const { return hThread; }
	bool Create( DWORD dwCreationFlags = 0, DWORD dwStackSize = 0, LPSECURITY_ATTRIBUTES lpSecurityAttributes = 0 ) {
		if ( hThread != NULL ) return false;
		DWORD id;
		hThread = CreateThread( lpSecurityAttributes, dwStackSize, TheProc, this, dwCreationFlags, &id );
		return hThread != NULL;
	}
	int Resume() const {
		return ResumeThread( hThread );
	}
	static void Exit( DWORD dwExitCode = 0 ) {
		ExitThread( dwExitCode );
	}
	int GetPriority() const {
		return GetThreadPriority( hThread );
	}
	static void Sleep( DWORD dwMilliseconds ) {
		::Sleep( dwMilliseconds );
	}
	DWORD SetAffinityMask( DWORD dwThreadAffinityMask ) const {
		return DWORD( SetThreadAffinityMask( hThread, dwThreadAffinityMask ) );
	}
#if (_WIN32_WINNT >= 0x0400)
	DWORD SetIdealProcessor( DWORD dwIdealProcessor ) const { // Warning: not available on WinNT 3.51, Win95, Win98, WinME
		return SetThreadIdealProcessor( hThread, dwIdealProcessor );
	}
#endif
	bool SetPriority( int nPriority ) const {
		return SetThreadPriority( hThread, nPriority ) != FALSE;
	}
	int Suspend() const {
		return SuspendThread( hThread );
	}
	bool Terminate( DWORD dwExitCode = 0 ) const {
		return TerminateThread( hThread, dwExitCode ) != FALSE;
	}
private:
	static DWORD WINAPI TheProc( LPVOID lpParam );
};

/**********************************************************************************************
* IPtr<T> - Interface pointer for COM objects
**********************************************************************************************/

// IMPORTANT:
//   The constructors of IPtr<T> expect the obtained pointer's reference count
//   to be already incremented, e.g. during object/pointer creation.

class IPtrBase {
	IUnknown* ptr;
protected:
	template< typename T > T* XPtr() const { assert( ptr != 0 ); return reinterpret_cast< T* >( ptr ); }
public:
	// Construction
	IPtrBase( IUnknown* ptr = 0 ): ptr( ptr ) { }
	// Assignment
	IPtrBase( const IPtrBase& p ): ptr( 0 ) { *this = p; }
	IPtrBase& operator=( const IPtrBase& p ) { if ( p.ptr ) p.ptr->AddRef(); Release(); ptr = p.ptr; return *this; }
	// Destruction
	~IPtrBase() { Release(); }
	void Release() { if ( ptr != 0 ) ptr->Release(), ptr = 0; }
	// Validation
	bool IsValid() const { return ptr != 0; }
	// Double pointer used during creation of objects and interfaces
	void** GetVPP() { Release(); return GetVPP_Unsafe(); }
	void** GetVPP_Unsafe() { return reinterpret_cast<void**>( &ptr ); }
	// Object creation
	IPtrBase( REFCLSID rclsid, REFIID riid, DWORD dwClsContext = CLSCTX_INPROC_SERVER, LPUNKNOWN pUnkOuter = 0 ): ptr( 0 ) { CreateInstance( rclsid, riid, dwClsContext, pUnkOuter ); }
	bool CreateInstance( REFCLSID rclsid, REFIID riid, DWORD dwClsContext = CLSCTX_INPROC_SERVER, LPUNKNOWN pUnkOuter = 0 ) { return SUCCEEDED( CoCreateInstance( rclsid, pUnkOuter, dwClsContext, riid, GetVPP() ) ); }
	// Obtaining interfaces
	IPtrBase( IUnknown* p_object, REFIID riid ): ptr( 0 ) { CreateInterface( p_object, riid ); }
	bool CreateInterface( IUnknown* p_object, REFIID riid ) { assert( p_object != 0 ); return SUCCEEDED( p_object->QueryInterface( riid, GetVPP() ) ); }
	// Accessing IUnknown
	IUnknown* GetIUnknown() const { assert( ptr != 0 ); return ptr; }
	operator IUnknown*() const { assert( ptr != 0 ); return ptr; }
};

#ifdef MINGW
#ifndef __uuidof
#define __uuidof(x) 0
#endif
#endif

template< typename T >
class IPtr: public IPtrBase {
public:
	// Construction
	IPtr( T* ptr = 0 ): IPtrBase( ptr ) { }
	// Object creation
	IPtr( REFCLSID rclsid, REFIID riid = __uuidof(T), DWORD dwClsContext = CLSCTX_INPROC_SERVER, LPUNKNOWN pUnkOuter = 0 ): IPtrBase( rclsid, riid, dwClsContext, pUnkOuter ) { }
	IPtr( REFCLSID rclsid, DWORD dwClsContext, LPUNKNOWN pUnkOuter = 0 ): IPtrBase( rclsid, __uuidof(T), dwClsContext, pUnkOuter ) { }
	// Obtaining interfaces
	IPtr( IUnknown* p_object, REFIID riid = __uuidof(T) ): IPtrBase( p_object, riid ) { }
	// Actual object access
	T* Get() const { return XPtr<T>(); }
	operator T*() const { return XPtr<T>(); }
	T& operator*() const { return *XPtr<T>(); }
	T* operator->() const { return XPtr<T>(); }
	// Double pointer used during creation of objects and interfaces
	T** GetPP() { return reinterpret_cast<T**>( GetVPP() ); }
	// Obtaining interfaces
	template< typename T2 >
	IPtr& operator=( const IPtr<T2>& p_object ) { CreateInterface( p_object, __uuidof(T2) ); return *this; }
};

/**********************************************************************************************
* COMInitializer
**********************************************************************************************/

class COMInitializer: public NonCopyAble {
public:
	COMInitializer() { CoInitialize( 0 ); }
	~COMInitializer() { CoUninitialize(); }
};

/**********************************************************************************************
* Application window creation traits
**********************************************************************************************/

template< class T >
class ApplicationWindowTraits: public NonCopyAble {
public:
	typedef T WindowType;
	typedef WindowType* WindowTypePointer;

	ApplicationWindowTraits(): p_window( 0 ) { }
	virtual bool CreateMainWindow() {
		p_window = new WindowType;
		if ( ! p_window->IsCreated() ) {
			if ( p_window != 0 ) Window::Delete( p_window );
			return false;
		}
		p_window->Show();
		p_window->Update();
		return true;
	}
	WindowTypePointer GetWindow() const { return p_window; }
private:
	WindowTypePointer p_window;
};

/**********************************************************************************************
* Application initialization and shutdown traits
**********************************************************************************************/

class ApplicationInitShutdownTraits: public NonCopyAble {
public:
	virtual bool Init() { return true; }
	virtual void Quit() { }
};

/**********************************************************************************************
* Accelerator processing traits
**********************************************************************************************/

template< class _Window >
class AcceleratorTraits: public NonCopyAble {
public:
	virtual bool ProcessAccel( MessageQueue& queue, _Window* p_window ) { return false; }
};

template< class _Window, int id >
class AccelFromResource: public NonCopyAble {
	HACCEL acc;
public:
	AccelFromResource(): acc( LoadAccelerators( __MODULE_HANDLE, MAKEINTRESOURCE( id ) ) ) { }
	virtual bool ProcessAccel( MessageQueue& queue, _Window* p_window ) {
		return queue.TranslateAccelerator( *p_window, acc );
	}
};

template< class _Window >
class ModelessDialogAccel: public NonCopyAble {
public:
	virtual bool ProcessAccel( MessageQueue& queue, _Window* p_window ) {
		return queue.IsDialogMessage( *p_window );
	}
};

template< class _Window >
class MDIAccel: public NonCopyAble {
public:
	virtual bool ProcessAccel( MessageQueue& queue, _Window* p_window ) {
		return queue.TranslateMDISysAccel( p_window->GetClientWindow() );
	}
};

template< class _Window, int id >
class MDIAccelFromResource: public NonCopyAble {
	HACCEL acc;
public:
	MDIAccelFromResource(): acc( LoadAccelerators( __MODULE_HANDLE, MAKEINTRESOURCE( id ) ) ) { }
	virtual bool ProcessAccel( MessageQueue& queue, _Window* p_window ) {
		return queue.TranslateMDISysAccel( p_window->GetClientWindow() )
			|| queue.TranslateAccelerator( *p_window, acc );
	}
};

/**********************************************************************************************
* ApplicationBase
**********************************************************************************************/

class ApplicationBase: public NonCopyAble {
public:
	static ApplicationBase* p_application;
	~ApplicationBase() { ApplicationBase::p_application = 0; }
	virtual void Run() = 0;
};

/**********************************************************************************************
* Application - regular main loop
**********************************************************************************************/

template< class _Window,
		  class _Accelerator = AcceleratorTraits< _Window >,
		  class _InitShutdown = ApplicationInitShutdownTraits,
		  class _CreateWindow = ApplicationWindowTraits< _Window > >
class Application: public ApplicationBase, public _InitShutdown, public _Accelerator, public _CreateWindow {
public:
	Application() { ApplicationBase::p_application = this; }
	virtual void Run() {
		if ( this->Init() && this->CreateMainWindow() ) {
			MessageQueue queue;
			while ( queue.GetMessage() )
				if ( ! this->ProcessAccel( queue, this->GetWindow() ) ) {
					queue.TranslateMessage();
					queue.DispatchMessage();
				}
		}
		this->Quit();
	}
};

/**********************************************************************************************
* Library defined WinMain Macro
**********************************************************************************************/

extern int WINAPI Lib_WinMain( HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nShowCmd );

#define USE_LIB_WINMAIN \
int WINAPI WinMain( HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nShowCmd ) { \
  return WinAPI::Lib_WinMain(hInstance, hPrevInstance, lpCmdLine, nShowCmd ); \
}

} // namespace WinAPI

