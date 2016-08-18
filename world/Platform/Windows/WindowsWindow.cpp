//#define UNICODE
//#define _UNICODE

#include "WindowsWindow.h"
#include "..\Application.h"

WindowsWindow::WindowsWindow():
	hwnd( NULL ),
	width( 0 ),
	height( 0 ),
	clientWidth( 0 ),
	clientHeight( 0 ),
	backgroundColor( 1.0f, 1.0f, 1.0f, 1.0f ),
	name()
{}

WindowsWindow::~WindowsWindow() {
	/*
	Pokud objekt vlastni platny handle okna, je nutne zrusit spojeni s timto oknem a zavolat DestroyWindow().
	Kdyby nebylo spojeni zruseno, odkazovaly by se zpravy okna na neexistujici objekt WindowsWindow.
	*/
	if ( hwnd ) {
		SetWindowLongPtr( hwnd, 0, 0 );
		DestroyWindow( hwnd );
	}
}

bool WindowsWindow::Create( const CreateWindowsWindowParams& params ) {
	
	const WCHAR* const className = L"WindowsWindow";
	
	// okno je uz vytvoreno
	if ( hwnd != NULL ) {
		return false;
	}
	// registrovat tridu okna
	WNDCLASSEXW wc;
	wc.cbSize			= sizeof( WNDCLASSEXW );
	wc.lpszClassName	= className;
	wc.style			= params.classStyle;
	wc.lpfnWndProc		= WindowsWindow::WndProcStatic;
	wc.cbClsExtra		= 0;
	wc.cbWndExtra		= sizeof( LONG ) * 4;
	wc.hInstance		= params.hInstance;
	wc.hIcon			= params.hIcon;
	wc.hIconSm			= params.hIconSm;
	wc.hCursor			= params.hCursor;
	wc.hbrBackground	= NULL;
	wc.lpszMenuName		= params.menuName;
	
	if ( !RegisterClassExW( &wc ) ) {
		return false;
	}
	
	// vytvorit okno
	hwnd = CreateWindowExW(
		params.windowExStyle,
		className,
		params.windowName,
		params.windowStyle,
		params.x,
		params.y,
		params.width,
		params.height,
		params.hParent,
		params.hMenu,
		params.hInstance,
		LPVOID( this )	// predat ukazatel na objekt WindowsWindow zprave WM_CREATE
	);
	if ( hwnd == 0 ) {
		return false;
	}
	// zobrazit okno (prijde zprava WM_SIZE)
	ShowWindow( hwnd, SW_SHOW );
	UpdateWindow( hwnd );

	return true;
}

LRESULT CALLBACK WindowsWindow::WndProcStatic( HWND hwnd, UINT message, WPARAM wparam, LPARAM lparam ) {
	
	// WM_CREATE, ulozit ukazatel na objekt WindowsWindow predany v lparam
	if ( WM_CREATE == message ) {
		LPCREATESTRUCT cs = reinterpret_cast< LPCREATESTRUCT >( lparam );
		WindowsWindow* window = reinterpret_cast< WindowsWindow* >( cs->lpCreateParams );
		SetWindowLongPtr( hwnd, 0, reinterpret_cast< LONG_PTR >( window ) );
		return window->WndProc( hwnd, message, wparam, lparam );
	}
	
	// predat zpracovani zpravy instanci tridy WindowsWindow
	LONG_PTR ptr = GetWindowLongPtr( hwnd, 0 );
	if ( ptr != NULL ) {
		WindowsWindow* window = reinterpret_cast< WindowsWindow* >( ptr );
		return window->WndProc( hwnd, message, wparam, lparam );
	}
	
	// zprava nepatri zadnemu registrovanemu oknu, predat zpracovani windows
	return DefWindowProc( hwnd, message, wparam, lparam );
}

LRESULT WindowsWindow::WndProc( const HWND hwnd, const UINT message, WPARAM wparam, LPARAM lparam ) {
	switch ( message ) {
	case WM_CREATE:
		OnCreate();
		return 0;

	case WM_PAINT:
		OnPaint();
		return 0;

	case WM_SIZE:
		OnSize(
			static_cast< int >( LOWORD( lparam ) ),
			static_cast< int >( HIWORD( lparam ) )
		);
		return 0;

	case WM_ERASEBKGND:
		return 0;

	case WM_DESTROY:
		OnDestroy();
		return 0;
	}
	return DefWindowProc( hwnd, message, wparam, lparam );
}

void WindowsWindow::OnCreate() {}

void WindowsWindow::OnDestroy() {
	hwnd = 0;
	width = 0;
	height = 0;
	clientWidth = 0;
	clientHeight = 0;
	name = u"";
}

void WindowsWindow::OnPaint() {
	ValidateRect( hwnd, NULL );
	return;

	// renderer neni aktivni, vyplni klientskou oblast okna barvou pozadi
	PAINTSTRUCT ps;
	HDC hdc = BeginPaint( hwnd, &ps );
	EndPaint( hwnd, &ps );
}

void WindowsWindow::OnSize( const int width, const int height ) {
	clientWidth = width;
	clientHeight = height;

	// zjistit velikost celeho okna
	RECT rect;
	GetWindowRect( hwnd, &rect );
	this->width = rect.right - rect.left;
	this->height = rect.bottom - rect.top;

	// default handler
	Window::OnSize( width, height );
}

void WindowsWindow::Show() noexcept {
	if ( hwnd ) {
		ShowWindow( hwnd, SW_SHOW );
		UpdateWindow( hwnd );
	}
}

void WindowsWindow::Hide() noexcept {
	if ( hwnd ) {
		ShowWindow( hwnd, SW_HIDE );
	}
}

int WindowsWindow::GetWidth() const noexcept {
	return width;
}

int WindowsWindow::GetHeight() const noexcept {
	return height;
}

void WindowsWindow::Resize( const int width, const int height ) noexcept {
	if ( hwnd ) {
		SetWindowPos( hwnd, 0, 0, 0, width, height, SWP_NOMOVE | SWP_NOZORDER );
	}
}

int WindowsWindow::GetClientWidth() const noexcept {
	return clientWidth;
}

int WindowsWindow::GetClientHeight() const noexcept {
	return clientHeight;
}

void WindowsWindow::ResizeClient( const int width, const int height ) noexcept {
	Resize(
		width + this->width - this->clientWidth,
		height + this->height - this->clientHeight
	);
}

void WindowsWindow::SetPosition( const int x, const int y ) noexcept {
	if ( hwnd ) {
		SetWindowPos( hwnd, 0, x, y, 0, 0, SWP_NOSIZE | SWP_NOZORDER );
	}
}

void* WindowsWindow::GetHandle() noexcept {
	return reinterpret_cast< void* >( hwnd );
}

void WindowsWindow::SetBackgroundColor( const Color& color ) noexcept {
	ColorUnorm rgb8 = color.ToUnorm();
	HBRUSH brush = CreateSolidBrush( RGB( rgb8.r, rgb8.g, rgb8.b ) );
	HBRUSH prevBrush = ( HBRUSH )SetClassLongPtr( hwnd, GCLP_HBRBACKGROUND, LONG_PTR( brush ) );
	if ( prevBrush != NULL ) {
		DeleteObject( prevBrush );
	}
	backgroundColor = color;
}

const Color WindowsWindow::GetBackgroundColor() const noexcept {
	return backgroundColor;
}

void WindowsWindow::Update() noexcept {
	if ( hwnd ) {
		UpdateWindow( hwnd );
	}
}

void WindowsWindow::SetName( const String& str ) noexcept {
	if ( hwnd ) {
		name = str;
		SetWindowTextW( hwnd, reinterpret_cast< LPCWSTR >( str.Raw() ) );
	}
}

const String WindowsWindow::GetName() const noexcept {
	return name;
}

bool WindowsWindow::CreateAppWindow( const HINSTANCE hInstance, const int clientWidth, const int clientHeight ) {	
	CreateWindowsWindowParams params;
	params.classStyle 		= 0;
	params.hIcon			= LoadIcon( hInstance, IDI_APPLICATION );
	params.hCursor			= LoadCursor( NULL, IDC_ARROW );
	params.menuName			= NULL;
	params.hIconSm			= 0;
	params.windowName		= TEXT( "Application Window" );
	params.windowStyle		= WS_OVERLAPPEDWINDOW;
	params.windowExStyle	= 0;
	params.x				= CW_USEDEFAULT;
	params.y				= CW_USEDEFAULT;
	params.width			= CW_USEDEFAULT;
	params.height			= CW_USEDEFAULT;
	params.hParent			= NULL;
	params.hMenu			= NULL;
	params.hInstance		= hInstance;
	
	// vypocet rozmeru okna z pozadovanych rozmeru klientske oblasti:

	RECT dimmension;
	dimmension.left = 0;
	dimmension.top = 0;
	dimmension.right = clientWidth;
	dimmension.bottom = clientHeight;

	BOOL adjusted = AdjustWindowRectEx( &dimmension, params.windowStyle, params.hMenu != NULL, params.windowExStyle );
	if ( adjusted ) {
		params.width = dimmension.right - dimmension.left;
		params.height = dimmension.bottom - dimmension.top;
	}
	return Create( params );
}