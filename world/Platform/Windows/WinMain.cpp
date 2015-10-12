#include "WindowsWindow.h"
#include "WindowsApplication.h"

#include "..\..\Renderer\RenderInterface.h"
#include "..\..\Renderer\DX11\DX11RenderInterface.h"

int WINAPI WinMain( HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow ) {
	WindowsAppWindow window;
	window.CreateAppWindow( hInstance, 800, 600 );
	window.SetName( String( u"World" ) );
	//window.SetBackgroundColor( Color::BLACK );

	CreateDX11DeviceParams deviceParams;
	deviceParams.adapter = 0;
	deviceParams.majorFeatureLevels = 11;
	deviceParams.minorFeatureLevels = 0;
	Device *device = CreateDX11Device( deviceParams );

	Display *display = device->CreateDisplay( 0 );
	DisplayMode mode;
	DisplayMode req;
	req.width = 1920;
	req.height = 1080;
	req.refreshRateNumerator = 60;
	req.refreshRateDenominator = 1;
	display->FindMode( req, mode );
	//display->SetMode( mode, window );
	//display->SetSystemMode();

	window.SetRenderDevice( device );

	WindowsApplication app;
	app.Create( hInstance );
	app.Run();

	// uvolneni objektu
	window.SetRenderDevice( nullptr );
	display->Release();
	device->Release();

	return 0;
}