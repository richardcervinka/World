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
	RenderInterface::Device *renderDevice;
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
	renderDevice = nullptr;
}

bool DevApp::Create( const HINSTANCE hInstance ) {
	if ( !WindowsApplication::Create( hInstance ) ) {
		return false;
	}
	RenderInterface::DX11CreateDeviceParams deviceParams;
	deviceParams.adapter = 0;
	deviceParams.majorFeatureLevels = 11;
	deviceParams.minorFeatureLevels = 0;
	renderDevice = RenderInterface::DX11CreateDevice( deviceParams );
	
	window.CreateAppWindow( hInstance, 1024, 768 );
	window.SetName( String( u"World" ) );
	window.SetBackgroundColor( Color::BLACK );
	window.SetRenderDevice( renderDevice );

	commandInterface = renderDevice->CreateCommandInterface();
	commandInterface->Begin( renderDevice );
	RenderInterface::BackBuffer *backBuffer = window.GetBackBuffer();
	commandInterface->ClearRenderTarget( backBuffer->GetRenderTargetObject(), Color( 1.0f, 0.5f, 0, 1.0f ) );
	commandInterface->End();
	window.PresentBackBuffer( 0 );

	// display mode
	//RenderInterface::Display *display = device->CreateDisplay( 0 );
	//RenderInterface::DisplayMode mode;
	//display->GetBestMode( mode );
	//display->SetMode( mode, window );
	
	// uvolneni objektu
	//window.SetRenderDevice( nullptr );
	//display->Release();
	//device->Release();
	
	return true;
}
/*
void DevApp::Update() {	
}
*/