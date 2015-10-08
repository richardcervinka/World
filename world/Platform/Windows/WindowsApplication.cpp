#include "WindowsApplication.h"

// identifikatory klavesovych zkratek
const int WIN_HOTKEY_ALT_ENTER = 1;

WindowsApplication::WindowsApplication() {
	handle = 0;
}

WindowsApplication::~WindowsApplication() {}

bool WindowsApplication::Create( const HINSTANCE hInstance ) {
	handle = hInstance;
	return true;
}

bool WindowsApplication::ProcessPlatformMessages() {
	MSG msg;
	while ( PeekMessage( &msg, NULL, 0, 0, PM_REMOVE ) ) {
		// standardni ukonceni aplikace
		if ( msg.message == WM_QUIT ) {
			return false;
		}
		if ( !ProcessMessage( msg ) ) {
			// odeslat zpravu oknu
			TranslateMessage( &msg );
			DispatchMessage( &msg );
		}
	}
	return true;
}

bool WindowsApplication::ProcessMessage( const MSG &msg ) {
	switch ( msg.message ) {
	case WM_HOTKEY:
		if ( msg.wParam == WIN_HOTKEY_ALT_ENTER ) {
			//renderer->SwitchFullscreen();
			return true;
		}
		return false;
	
	//case WM_INPUT:
		//OnRawInput( msg.wParam, msg.lParam );
		//return true;
	}
	return false;
}

void WindowsApplication::ExitApp() {
	// standardni zpusob ukonceni Win32 aplikace
	PostQuitMessage( 0 );
}

void WindowsApplication::AbortApp() {
	MessageBoxW( NULL, L"Uspecified", L"Critical Error!!", MB_OK | MB_ICONERROR );
	abort();
}

void WindowsApplication::AbortApp( const String &errorMessage ) {
	MessageBoxW( NULL, reinterpret_cast< LPCWSTR >( errorMessage.Raw() ), L"Critical Error!", MB_OK | MB_ICONERROR );
	abort();
}


/*
ZALOHA PUVODNI IMPLEMENTACE

// identifikatory klavesovych zkratek
const int HOTKEY_ALT_ENTER = 1;

bool WinApplication::Create( HINSTANCE hInstance, const int cmdShow ) {
	// chybne volani, objekt aplikaace uz je vytvoren
	if ( app != nullptr ) {
		return false;
	}
	// ulozit adresu objektu do globalni promenne
	app = this;

	// vytvorit hlavni okno aplikace
	WinWindow *window = new WinWindow();
	this->window = window;
	window->Create( hInstance, cmdShow, 800, 600, WindowStyle::APPLICATION );

	// initialize raw input
	RAWINPUTDEVICE rid;
	rid.usUsagePage = 0x01; 
	rid.usUsage = 0x02;
	rid.dwFlags = 0;
	rid.hwndTarget = 0;
	BOOL result = RegisterRawInputDevices( &rid, 1, sizeof( rid ) );

	// registrace Alt+Enter pro prepinani fullscreen rezimu
	RegisterHotKey( NULL, HOTKEY_ALT_ENTER, MOD_ALT | MOD_NOREPEAT, VK_RETURN );

	return true;
}

void WinApplication::Initialize() {
	System::Initialize();
}

void WinApplication::OnRawInput( WPARAM wParam, LPARAM lParam ) {
	const int RAW_DATA_SIZE = 256;
	BYTE data[ RAW_DATA_SIZE ];
	UINT dataSize = static_cast< UINT >( RAW_DATA_SIZE );
	UINT loadSize = GetRawInputData( reinterpret_cast< HRAWINPUT >( lParam ), RID_INPUT, data, &dataSize, sizeof( RAWINPUTHEADER ) );
	if ( loadSize == static_cast< UINT >( -1 ) ) {
		return;
	}
	RAWINPUT *ri = reinterpret_cast< RAWINPUT* >( data );
	if ( ri->header.dwType == RIM_TYPEMOUSE ) {
		if ( ri->data.mouse.usFlags == MOUSE_MOVE_RELATIVE ) {
			//GetInput()->MouseInput( int( ri->data.mouse.lLastX ), int( ri->data.mouse.lLastY ) );
		}
	}
}
*/