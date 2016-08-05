#include <memory>
#include "WindowsWindow.h"
#include "WindowsApplication.h"
#include "..\..\Core\RenderInterface.h"
#include "..\..\Core\Renderer.h"
#include "..\..\Core\Windows\WindowsGraphicsInfrastructure.h"

// Testovaci aplikace
class DevApp: public WindowsApplication {
public:
	DevApp();
	virtual bool Create( const HINSTANCE hInstance ) override;
	//virtual void Update() override;
	
private:
	WindowsAppWindow *window;
	Renderer* renderer;
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
	window = nullptr;
	renderer = nullptr;
}

bool DevApp::Create( const HINSTANCE hInstance ) {
	if ( !WindowsApplication::Create( hInstance ) ) {
		return false;
	}
	// window
	window = new WindowsAppWindow();
	window->CreateAppWindow( hInstance, 1024, 768 );
	window->SetName( String( u"World" ) );
	window->SetBackgroundColor( Color::BLACK );

	WindowsGraphicsInfrastructure gi;

	// adapter capabilities
	WindowsAdapterCapabilities adapterCapabilities;
	adapterCapabilities.api = WindowsRenderApi::DIRECTX_11_0;
	adapterCapabilities.requiredVideoMemory = 512;

	// create adapter
	auto adapter = gi.CreateAdapter( adapterCapabilities );
	if ( adapter == nullptr ) {
		return false;
	}

	// create Device
	auto device = adapter->CreateDirectXDevice();
	if ( device == nullptr ) {
		return false;
	}

	// create Renderer


	// Nasledujici nahradit necim jako:
	// InitializeEngine( params... )

	// render interface device
	/*
	RenderInterface::DX11CreateDeviceParams deviceParams;
	deviceParams.adapter = 0;
	deviceParams.majorFeatureLevels = 11;
	deviceParams.minorFeatureLevels = 0;
	RenderInterface::PDevice device = RenderInterface::DX11CreateDevice( deviceParams );

	// renderer
	RendererAttributes rendererAttribs;
	renderer = new Renderer();
	renderer->Initialize( std::move( device ), rendererAttribs );

	// associate renderer with window, this allows drawing into the window
	window->SetRenderer( renderer );
	*/

	/*
	RenderInterface::Display* display = device->CreateDisplay( 0 );
	RenderInterface::DisplayMode displayMode;
	display->GetBestMode( displayMode );
	display->SetMode( displayMode, window );
	//display->SetMode( )
	*/

	return true;
}