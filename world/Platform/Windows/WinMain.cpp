#include <memory>
#include "WindowsWindow.h"
#include "WindowsApplication.h"
#include "Core/Renderer.h"
#include "Core/Windows/WindowsGraphicsInfrastructure.h"

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

// DevApp

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

	// create adapter
	auto adapter = CreateWindowsAdapter( adapterCapabilities );
	if ( adapter == nullptr ) {
		return false;
	}
	
	// primary adapter's output (display)
	auto display = adapter->CreateDisplay( 0 );
	if ( display == nullptr ) {
		return false;
	}

	// create platform Device
	auto device = adapter->CreateDirectx11Device();
	if ( device == nullptr ) {
		return false;
	}

	// swap chain associated with app window
	auto swapChain = device->CreateSwapChain( &window );
	if ( swapChain == nullptr ) {
		return false;
	}

	return true;
}