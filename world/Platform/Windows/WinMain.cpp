#include "WindowsWindow.h"
#include "WindowsApplication.h"
#include "..\..\Renderer\RenderInterface.h"

int WINAPI WinMain( HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow ) {
	WindowsAppWindow window;
	window.CreateAppWindow( hInstance, 800, 600 );
	window.SetName( String( u"World" ) );
	window.SetBackgroundColor( Color::BLACK );

	RenderInterface::CreateDX11DeviceParams deviceParams;
	deviceParams.adapter = 0;
	deviceParams.majorFeatureLevels = 11;
	deviceParams.minorFeatureLevels = 0;
	RenderInterface::Device *device = RenderInterface::CreateDX11Device( deviceParams );
	window.SetRenderDevice( device );

	RenderInterface::Display *display = device->CreateDisplay( 0 );
	RenderInterface::DisplayMode mode;
	display->GetBestMode( mode );
	//display->SetMode( mode, window );

	WindowsApplication app;
	app.Create( hInstance );
	app.Run();

	// uvolneni objektu
	window.SetRenderDevice( nullptr );
	display->Release();
	device->Release();

	return 0;
}