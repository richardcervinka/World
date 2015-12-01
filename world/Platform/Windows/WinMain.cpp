#include "WindowsWindow.h"
#include "WindowsApplication.h"
#include "..\..\Renderer\RenderInterface.h"

// Testovaci aplikace
class DevApp: public WindowsApplication {
public:
	DevApp();
	virtual bool Create( const HINSTANCE hInstance ) override;
	//virtual void Update() override;
	
private:
	WindowsAppWindow window;
	RenderInterface::Device *device;
	RenderInterface::CommandInterface *commandInterface;
};

// Windows entry point
int WINAPI WinMain( HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow ) {
	DevApp app;
	app.Create( hInstance );
	app.Run();
	return 0;
}

// DevApp class implementation

DevApp::DevApp() {
	device = nullptr;
}

bool DevApp::Create( const HINSTANCE hInstance ) {
	if ( !WindowsApplication::Create( hInstance ) ) {
		return false;
	}
	RenderInterface::DX11CreateDeviceParams deviceParams;
	deviceParams.adapter = 0;
	deviceParams.majorFeatureLevels = 11;
	deviceParams.minorFeatureLevels = 0;
	device = RenderInterface::DX11CreateDevice( deviceParams );
	
	window.CreateAppWindow( hInstance, 1024, 768 );
	window.SetName( String( u"World" ) );
	window.SetBackgroundColor( Color::BLACK );

	RenderInterface::Display* display = device->CreateDisplay( 0 );
	RenderInterface::DisplayMode displayMode;
	display->GetBestMode( displayMode );
	display->SetMode( displayMode, window );
	//display->SetMode( )

	RenderInterface::CommandInterface* commandInterface = device->CreateCommandInterface();
	RenderInterface::BackBuffer* backBuffer = device->CreateBackBuffer( window );


	//window.SetRenderer();

	// uvolneni objektu
	//device->Release();

	return true;
}