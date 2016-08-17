#include <memory>
#include "WindowsWindow.h"
#include "WindowsApplication.h"
#include "..\..\Core\RenderInterface.h"
#include "..\..\Core\Renderer.h"
#include "..\..\Core\Windows\WindowsGraphicsInfrastructure.h"

//----------------------------------------------------------------------------------------------------------------------------------
// Testovaci aplikace
class DevApp: public WindowsApplication {
public:
	DevApp();
	virtual bool Create( const HINSTANCE hInstance, LPSTR lpCmdLine ) override;
	//virtual void Update() override;
	
private:
	WindowsWindow window;
	Renderer* renderer;
};
//----------------------------------------------------------------------------------------------------------------------------------

// Windows entry point
int WINAPI WinMain( HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow ) {
	DevApp app;
	app.Create( hInstance, lpCmdLine );
	app.Run();
	return 0;
}

//----------------------------------------------------------------------------------------------------------------------------------
// DevApp class implementation

DevApp::DevApp() {
	renderer = nullptr;
}

bool DevApp::Create( const HINSTANCE hInstance, LPSTR lpCmdLine ) {
	if ( !WindowsApplication::Create( hInstance, lpCmdLine ) ) {
		return false;
	}
	// app window
	window.CreateAppWindow( hInstance, 1024, 768 );
	window.SetName( String( u"World" ) );
	window.SetBackgroundColor( Color::BLACK );
	window.onSizeHandler = nullptr;

	// adapter capabilities
	WindowsAdapterCapabilities adapterCapabilities;
	adapterCapabilities.api = WindowsRenderApi::DIRECTX_11_0;
	adapterCapabilities.requiredVideoMemory = 1024;

	// windows GI
	WindowsGraphicsInfrastructure gi;

	// create adapter
	auto adapter = gi.CreateAdapter( adapterCapabilities );
	if ( adapter == nullptr ) {
		return false;
	}

	// primary adapter's output (display)
	auto display = adapter->CreateDisplay( 0 );
	if ( display == nullptr ) {
		return false;
	}

	// create Device
	auto device = adapter->CreateDX11Device();
	if ( device == nullptr ) {
		return false;
	}

	// swap chain
	auto swapChain = gi.CreateSwapChain( window, device );
	if ( swapChain == nullptr ) {
		return false;
	}

	// create Renderer
	// ...

	/*
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