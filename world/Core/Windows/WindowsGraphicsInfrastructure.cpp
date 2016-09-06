#include <d3d11.h>
#include <dxgi1_2.h>
#include "Platform/Windows/WindowsWindow.h"
#include "WindowsGraphicsInfrastructure.h"

// WindowsAdapter

WindowsAdapter::WindowsAdapter( const ComPtr< IDXGIAdapter1 >& adapter ) {
	this->adapter = adapter;
}

WindowsAdapter::~WindowsAdapter() {
	adapter = nullptr;
}

int WindowsAdapter::GetOutputsCount() noexcept {
	UINT counter = 0;
	IDXGIOutput* output = nullptr;
	while ( adapter->EnumOutputs( counter, &output ) != DXGI_ERROR_NOT_FOUND ) {
		output->Release();
		counter += 1;
	}
	return static_cast< int >( counter );
}

bool WindowsAdapter::CheckCapabilities( const WindowsAdapterCapabilities& capabilities ) noexcept {

	// adapter description
	DXGI_ADAPTER_DESC adapterDesc;
	adapter->GetDesc( &adapterDesc );

	// porovnat mnozstvi dostupne a pozadovane dedikovane pameti
	if ( static_cast< unsigned int >( adapterDesc.DedicatedVideoMemory ) < capabilities.requiredVideoMemory ) {
		return false;
	}
	// zkusit vytvorit ID3D11Device objekt
	if ( WindowsRenderApi::DIRECTX_11_0 == capabilities.api ) {

		D3D_FEATURE_LEVEL featureLevel = D3D_FEATURE_LEVEL::D3D_FEATURE_LEVEL_11_0;
		D3D_FEATURE_LEVEL createdFeatureLevel;
		
		// create temporary device
		ComPtr< ID3D11Device > device;
		HRESULT hresult = D3D11CreateDevice(
			adapter.Raw(),
			D3D_DRIVER_TYPE_HARDWARE,
			NULL,
			0,
			&featureLevel,
			1,
			D3D11_SDK_VERSION,
			&device,
			&createdFeatureLevel,
			NULL
		);
		return !FAILED( hresult );
	}
	return false;
}

std::shared_ptr< Display > WindowsAdapter::CreateDisplay( const int outputId ) noexcept {
	ComPtr< IDXGIOutput > output;
	HRESULT hresult = adapter->EnumOutputs( static_cast< UINT >( outputId ), &output );
	if ( FAILED( hresult ) ) {
		return nullptr;
	}
	return std::shared_ptr< Display >( new( std::nothrow ) WindowsDisplay( output ) );
}

std::shared_ptr< Directx11RenderInterface::Device > WindowsAdapter::CreateDirectx11Device() noexcept {
	std::shared_ptr< Directx11RenderInterface::Device > device( new( std::nothrow ) Directx11RenderInterface::Device() );
	if ( device == nullptr ) {
		return nullptr;
	}
	if ( !device->Create( adapter, D3D_FEATURE_LEVEL::D3D_FEATURE_LEVEL_11_0 ) ) {
		return nullptr;
	}
	return device;
}

// WindowsDisplay

WindowsDisplay::WindowsDisplay( const ComPtr< IDXGIOutput >& output ) {
	this->output = output;
	EnumModes();
}

WindowsDisplay::~WindowsDisplay() {
	output = nullptr;
}

void WindowsDisplay::EnumModes() noexcept {
	if ( output == nullptr ) {
		return;
	}
	HRESULT hresult = 0;

	// zjistit pocet display modes
	UINT count = 0;
	output->GetDisplayModeList( Directx11RenderInterface::BACK_BUFFER_FORMAT, 0, &count, NULL );

	// enumerate modes
	std::vector< DXGI_MODE_DESC > dxgiModes( static_cast< size_t >( count ) );
	output->GetDisplayModeList( Directx11RenderInterface::BACK_BUFFER_FORMAT, 0, &count, dxgiModes.data() );

	// ulozit rezimy ve formatu DisplayMode
	modes.reserve( count );
	for ( UINT i = 0; i < count; i++ ) {
		DisplayMode mode;
		mode.width = static_cast< int >( dxgiModes[ i ].Width );
		mode.height = static_cast< int >( dxgiModes[ i ].Height );
		mode.refreshRateNumerator = static_cast< int >( dxgiModes[ i ].RefreshRate.Numerator );
		mode.refreshRateDenominator = static_cast< int >( dxgiModes[ i ].RefreshRate.Denominator );
		modes.push_back( mode );
	}
}

bool WindowsDisplay::GetMode( const int id, DisplayMode& result ) const noexcept {
	if ( id < 0 || id >= static_cast< int >( modes.size() ) ) {
		return false;
	}
	result = modes[ id ];
	return true;
}

bool WindowsDisplay::GetBestMode( DisplayMode& result ) const noexcept {
	if ( output == nullptr ) {
		return false;
	}
	DXGI_OUTPUT_DESC desc;
	output->GetDesc( &desc );

	DisplayMode mode;
	mode.width = static_cast< int >( desc.DesktopCoordinates.right - desc.DesktopCoordinates.left );
	mode.height = static_cast< int >( desc.DesktopCoordinates.bottom - desc.DesktopCoordinates.top );
	mode.refreshRateNumerator = 1000;
	mode.refreshRateDenominator = 1;

	return FindMode( mode, result );
}

// WindowsSwapChain

/*
WindowsSwapChain::WindowsSwapChain() {
	swapChain = nullptr;
	window = nullptr;
	width = 0;
	height = 0;
}

bool WindowsSwapChain::Create( WindowsWindow* const window, ID3D11Device* const device, IDXGIFactory1* const factory ) noexcept {
	DXGI_SWAP_CHAIN_DESC desc = { 0 };
	desc.BufferCount						= 2;
	desc.BufferDesc.Width					= window->GetClientWidth();
	desc.BufferDesc.Height					= window->GetClientHeight();
	desc.BufferDesc.Format					= BACK_BUFFER_FORMAT;
	desc.BufferDesc.RefreshRate.Numerator	= 0;
	desc.BufferDesc.RefreshRate.Denominator = 0;
	desc.BufferUsage						= DXGI_USAGE_RENDER_TARGET_OUTPUT;
	desc.SampleDesc.Count					= 1;
	desc.SampleDesc.Quality					= 0;
	desc.OutputWindow						= window->GetHandle();
	desc.Windowed							= TRUE;
	desc.SwapEffect							= DXGI_SWAP_EFFECT_DISCARD;
	desc.BufferDesc.Scaling					= DXGI_MODE_SCALING_STRETCHED;
	desc.Flags								= DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;

	IDXGISwapChain* swapChain = nullptr;
	HRESULT hresult = factory->CreateSwapChain( device, &desc, &swapChain );
	if ( FAILED( hresult ) ) {
		return false;
	}
	
	// vypnuti defaultniho prepinani do fullscreenu
	factory->MakeWindowAssociation( window->GetHandle(), DXGI_MWA_NO_ALT_ENTER  );

	// ulozit vysledek
	this->window = window;
	this->swapChain = swapChain;
	this->width = window->GetClientWidth();
	this->height = window->GetClientHeight();

	return true;
}

void WindowsSwapChain::Present() noexcept {}

void WindowsSwapChain::SetFullscreen( Display* const display ) noexcept {}

int WindowsSwapChain::GetWidth() const noexcept {
	return width;
}

int WindowsSwapChain::GetHeight() const noexcept {
	return height;
}

bool WindowsSwapChain::Valid() const noexcept {
	return width == window->GetClientWidth() && height == window->GetClientHeight();
}

// WindowsGraphicsInfrastructure

WindowsGraphicsInfrastructure::WindowsGraphicsInfrastructure() {
	factory = nullptr;
	CreateDXGIFactory1( __uuidof( IDXGIFactory1 ), reinterpret_cast< void** >( &factory ) );
}

WindowsGraphicsInfrastructure::~WindowsGraphicsInfrastructure() {
	if ( factory != nullptr ) {
		factory->Release();
	}
}

std::unique_ptr< WindowsAdapter > WindowsGraphicsInfrastructure::CreateAdapter( const int id ) noexcept {
	if ( factory == nullptr ) {
		return nullptr;
	}
	IDXGIAdapter1* adapter = nullptr;
	HRESULT hresult = factory->EnumAdapters1( id, &adapter );
	if ( FAILED( hresult ) ) {
		return nullptr;
	}
	return std::make_unique< WindowsAdapter >( adapter ); 
}

std::unique_ptr< WindowsAdapter > WindowsGraphicsInfrastructure::CreateAdapter( const WindowsAdapterCapabilities& capabilities ) noexcept {
	if ( factory == nullptr ) {
		return nullptr;
	}
	int id = 0;
	auto adapter = CreateAdapter( id );
	while ( adapter != nullptr ) {
		// check capabilities
		if ( adapter->CheckCapabilities( capabilities ) ) {
			return adapter;
		}
		// next adapter
		id += 1;
		adapter = CreateAdapter( id );
	}
	return nullptr;
}

std::unique_ptr< WindowsSwapChain > WindowsGraphicsInfrastructure::CreateSwapChain( WindowsWindow& window, const std::shared_ptr< DX11Device >& device ) noexcept {
	return nullptr;
}
*/



std::unique_ptr< WindowsAdapter > CreateAdapter( IDXGIFactory1* const factory, const int id ) noexcept {
	if ( factory == nullptr ) {
		return nullptr;
	}
	IDXGIAdapter1* adapter = nullptr;
	HRESULT hresult = factory->EnumAdapters1( id, &adapter );
	if ( FAILED( hresult ) ) {
		return nullptr;
	}
	return std::make_unique< WindowsAdapter >( adapter ); 
}

std::unique_ptr< WindowsAdapter > CreateWindowsAdapter( const int id ) noexcept {
	IDXGIFactory1* factory = nullptr;
	auto hresult = CreateDXGIFactory1( __uuidof( IDXGIFactory1 ), reinterpret_cast< void** >( &factory ) );
	if ( FAILED( hresult ) ) {
		return nullptr;
	}
	auto adapter = CreateAdapter( factory, id );
	factory->Release();
	return adapter;
}

std::unique_ptr< WindowsAdapter > CreateWindowsAdapter( const WindowsAdapterCapabilities& capabilities ) noexcept {
	IDXGIFactory1* factory = nullptr;
	auto hresult = CreateDXGIFactory1( __uuidof( IDXGIFactory1 ), reinterpret_cast< void** >( &factory ) );
	if ( FAILED( hresult ) ) {
		return nullptr;
	}
	int id = 0;
	auto adapter = CreateAdapter( factory, id );
	while ( adapter != nullptr ) {
		// check capabilities
		if ( adapter->CheckCapabilities( capabilities ) ) {
			factory->Release();
			return adapter;
		}
		// next adapter
		id += 1;
		adapter = CreateAdapter( factory, id );
	}
	factory->Release();
	return nullptr;
}