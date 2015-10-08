#include "WindowsApplication.h"

int WINAPI WinMain( HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow ) {
	WindowsApplication app;
	app.Create( hInstance );
	app.Run();
	return 0;
}