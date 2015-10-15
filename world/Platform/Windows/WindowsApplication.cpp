#include "WindowsApplication.h"

// identifikatory klavesovych zkratek
const int WIN_HOTKEY_ALT_ENTER = 1;

WindowsApplication::WindowsApplication() {
	handle = 0;
}

WindowsApplication::~WindowsApplication() {}

bool WindowsApplication::Create( const HINSTANCE hInstance ) {
	handle = hInstance;
	
	// register raw input
	RAWINPUTDEVICE rid;
	rid.usUsagePage = 0x01; 
	rid.usUsage = 0x02;
	rid.dwFlags = 0;
	rid.hwndTarget = 0;
	RegisterRawInputDevices( &rid, 1, sizeof( rid ) );
	
	// registrace Alt+Enter pro prepinani fullscreen rezimu
	RegisterHotKey( NULL, WIN_HOTKEY_ALT_ENTER, MOD_ALT | MOD_NOREPEAT, VK_RETURN );
	
	return true;
}

bool WindowsApplication::ProcessPlatformMessages() {
	MSG msg;
	while ( PeekMessage( &msg, NULL, 0, 0, PM_REMOVE ) ) {
		// standardni ukonceni Win32 aplikace
		if ( msg.message == WM_QUIT ) {
			return false;
		}
		if ( !ProcessMessage( msg ) ) {
			// odeslat zpravu oknu
			TranslateMessage( &msg );
			DispatchMessage( &msg );
		}
	}
	// Ve fronte nejsou zadne dalsi zpravy, vratit rizeni aplikaci.
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
	
	case WM_KEYDOWN:
		//OnKeyDown()
		break;
		
	case WM_KEYUP:
		//OnKeyUp()
		break;
	
	case WM_INPUT:
		OnInput( msg.wParam, msg.lParam );
		return true;
	}
	return false;
}

void WindowsApplication::OnInput( WPARAM wParam, LPARAM lParam ) {
	const UINT size = 256;
	BYTE data[ size ];
	UINT dataSize = size;
	UINT loadSize = GetRawInputData( reinterpret_cast< HRAWINPUT >( lParam ), RID_INPUT, data, &dataSize, sizeof( RAWINPUTHEADER ) );
	
	if ( loadSize == static_cast< UINT >( -1 ) ) {
		return;
	}
	RAWINPUT *ri = reinterpret_cast< RAWINPUT* >( data );
	
	// mouse
	if ( ri->header.dwType == RIM_TYPEMOUSE ) {
		if ( ri->data.mouse.usFlags == MOUSE_MOVE_RELATIVE ) {
			// x = ri->data.mouse.lLastX
			// y = ri->data.mouse.lLastY
		}
	}
}

void WindowsApplication::ExitApp() {
	// standardni zpusob ukonceni Win32 aplikace
	PostQuitMessage( 0 );
}

void WindowsApplication::AbortApp() {
	abort();
}

void WindowsApplication::AbortApp( const String &errorMessage ) {
	abort();
}