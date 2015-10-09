#include "WindowsWindow.h"
#include "WindowsApplication.h"

int WINAPI WinMain( HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow ) {
	WindowsAppWindow window;
	window.CreateAppWindow( hInstance, 1280, 720 );
	window.SetName( String( u"World" ) );

	WindowsApplication app;
	app.Create( hInstance );
	app.Run();
	return 0;
}