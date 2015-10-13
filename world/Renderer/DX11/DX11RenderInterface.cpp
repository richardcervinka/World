#include "DX11RenderInterface.h"
#include "..\..\Platform\Application.h"
#include "..\..\Platform\Windows\WindowsWindow.h"
#include "..\..\Framework\Math.h"
#include "..\..\Framework\Debug.h"

const DXGI_FORMAT BACK_BUFFER_FORMAT = DXGI_FORMAT_R8G8B8A8_UNORM;

void AbortDXInvalidCall( const HRESULT hresult ) {
	Application::Abort();
}

DXGI_FORMAT GetDXGIFormat(const Format format) {
	switch ( format ) {
	case Format::R8G8B8A8_UNORM:		return DXGI_FORMAT_R8G8B8A8_UNORM;
	case Format::R16G16B16A16_FLOAT:	return DXGI_FORMAT_R16G16B16A16_FLOAT;
	case Format::R16G16_FLOAT:			return DXGI_FORMAT_R16G16_FLOAT;
	case Format::R8_UNORM:				return DXGI_FORMAT_R8_UNORM;
	case Format::R16_FLOAT:				return DXGI_FORMAT_R16_FLOAT;
	case Format::R32_FLOAT:				return DXGI_FORMAT_R32_FLOAT;
	}
	return DXGI_FORMAT_UNKNOWN;
}

// DX11Device

DX11Device::DX11Device() {
	dxgiFactory = nullptr;
	dxgiAdapter = nullptr;
	device  = nullptr;
	context  = nullptr;
}

DX11Device::~DX11Device() {
	ReleaseCOM( &dxgiAdapter );
	ReleaseCOM( &dxgiFactory );
	ReleaseCOM( &context );
	ReleaseCOM( &device );
}

bool DX11Device::Create( const CreateDX11DeviceParams &params ) {
	HRESULT hresult = 0;

	// vytvorit DXGIFactory1
	IDXGIFactory1 *factory = nullptr;
	hresult = CreateDXGIFactory1( __uuidof( IDXGIFactory1 ), ( void** )( &factory ) );
	if ( FAILED( hresult ) ) {
		return false;
	}
	// pokusit se ziskat pozadovany adapter
	IDXGIAdapter *adapter = nullptr;
	hresult = factory->EnumAdapters( static_cast< UINT >( params.adapter ), &adapter );

	// selhani, zkusit ziskat vychozi adapter
	if ( FAILED( hresult ) ) {
		hresult = factory->EnumAdapters( 0, &adapter );
	}
	// nebyl nalezen zadny adapter
	if ( FAILED( hresult ) ) {
		factory->Release();
		return false;
	}
	// feature levels
	D3D_FEATURE_LEVEL featureLevels[ 1 ] = {
		D3D_FEATURE_LEVEL_11_0
	};
	if ( params.majorFeatureLevels == 10 ) {
		if ( params.minorFeatureLevels == 0 ) {
			featureLevels[ 0 ] = D3D_FEATURE_LEVEL_10_0;
		}
		if ( params.minorFeatureLevels == 1 ) {
			featureLevels[ 0 ] = D3D_FEATURE_LEVEL_10_1;
		}
	}
	if ( params.majorFeatureLevels == 11 ) {
		if ( params.minorFeatureLevels == 0 ) {
			featureLevels[ 0 ] = D3D_FEATURE_LEVEL_11_0;
		}
		if ( params.minorFeatureLevels == 1 ) {
			featureLevels[ 0 ] = D3D_FEATURE_LEVEL_11_1;
		}
	}
	D3D_FEATURE_LEVEL createdFeatureLevel;

#ifdef _DEBUG
	UINT flags = D3D11_CREATE_DEVICE_DEBUG;
#else
	UINT flags = 0
#endif
	hresult = D3D11CreateDevice(
		adapter,
		D3D_DRIVER_TYPE_UNKNOWN,
		NULL,
		flags,
		featureLevels,
		ARRAYSIZE( featureLevels ),
		D3D11_SDK_VERSION,
		&device,
		&createdFeatureLevel,
		&context
	);
	if ( FAILED( hresult ) ) {
		factory->Release();
		adapter->Release();
		return false;
	}
	this->dxgiFactory = factory;
	this->dxgiAdapter = adapter;
	return true;
}

ID3D11DeviceContext *DX11Device::GetContext() {
	return context;
}

ID3D11Device *DX11Device::GetDevice() {
	return device;
}

int DX11Device::GetMultisampleQuality( const int samplesCount) const {
	int levels = 0;
	device->CheckMultisampleQualityLevels( BACK_BUFFER_FORMAT, samplesCount, reinterpret_cast< unsigned int* >( &levels ) );
	return levels;
}

CommandInterface *DX11Device::CreateCommandInterface() {
	DX11CommandInterface *commandInterface = new DX11CommandInterface();
	if ( commandInterface == nullptr ) {
		return nullptr;
	}
	if ( !commandInterface->Create( context ) ) {
		delete commandInterface;
		return nullptr;
	}
	return commandInterface;
}

BackBuffer *DX11Device::CreateBackBuffer( Window &window ) {
	DX11BackBuffer *backBuffer = new DX11BackBuffer();
	if ( backBuffer == nullptr ) {
		return nullptr;
	}
	if ( !backBuffer->Create( device, dxgiFactory, window ) ) {
		delete backBuffer;
		return nullptr;
	}
	return backBuffer;
}

RenderBuffer *DX11Device::CreateRenderBuffer( const RenderBufferDesc &desc ) {
	DX11RenderBuffer *buffer = new DX11RenderBuffer();
	if ( buffer == nullptr ) {
		return nullptr;
	}
	if ( !buffer->Create( this, desc ) ) {
		delete buffer;
		return nullptr;
	}
	return buffer;
}

DepthStencilBuffer *DX11Device::CreateDepthStencilBuffer( const DepthStencilBufferDesc &desc ) {
	DX11DepthStencilBuffer *buffer = new DX11DepthStencilBuffer();
	if ( buffer == nullptr ) {
		return nullptr;
	}
	if ( !buffer->Create( device, desc ) ) {
		delete buffer;
		return nullptr;
	}
	return buffer;
}

Display *DX11Device::CreateDisplay( const int outputId ) {
	DX11Display *display = new DX11Display();
	if ( display == nullptr ) {
		return nullptr;
	}
	if ( !display->Create( device, dxgiAdapter, outputId ) ) {
		delete display;
		return nullptr;
	}
	return display;
}

// *********************************************************************************
TextureSampler *DX11Device::CreateTextureSampler( const TextureSamplerDesc &desc ) {
	return nullptr;
}

// DX11CommandInterface

DX11CommandInterface::DX11CommandInterface() {
	context = nullptr;
}

DX11CommandInterface::~DX11CommandInterface() {
	ReleaseCOM( &context );
}

bool DX11CommandInterface::Create( ID3D11DeviceContext *context ) {
	if ( context == nullptr ) {
		return false;
	}
	return true;
}

void DX11CommandInterface::Begin( Device * const device ) {
	ASSERT_DOWNCAST( device, DX11Device );
	context = static_cast< DX11Device* >( device )->GetContext();
}

void DX11CommandInterface::Begin( CommandList * const commandList ) {
	// UNIMPLEMENTED
}

void DX11CommandInterface::End() {
	context = nullptr;
	// Neimplementovan command list, dodelat!
}

void DX11CommandInterface::SetRenderTargets( RenderTarget * const renderTargets, const int count, DepthStencilBuffer * const depthStencilBuffer ) {
	ASSERT_DOWNCAST( renderTargets, DX11RenderTarget );
	DX11RenderTarget *dxRenderTargets = reinterpret_cast< DX11RenderTarget* >( renderTargets );
	ID3D11RenderTargetView *views[ MAX_RENDER_TARGETS ] = { NULL };

	int bindCount = min( count, MAX_RENDER_TARGETS );
	for ( int i = 0; i < bindCount; i++ ) {
		views[ i ] = dxRenderTargets[ i ].GetRenderTargetView();
	}
	if ( depthStencilBuffer == nullptr ) {
		context->OMSetRenderTargets( MAX_RENDER_TARGETS, views, NULL );
		return;
	}
	ASSERT_DOWNCAST( depthStencilBuffer, DX11DepthStencilBuffer );
	context->OMSetRenderTargets(
		MAX_RENDER_TARGETS,
		views,
		static_cast< DX11DepthStencilBuffer* >( depthStencilBuffer )->GetDepthStencilView()
	);
}

void DX11CommandInterface::ClearRenderTarget( RenderTarget * const renderTarget, const Color &color ) {
	ASSERT_DOWNCAST( renderTarget, DX11RenderTarget );
	context->ClearRenderTargetView(
		reinterpret_cast< DX11RenderTarget* >( renderTarget )->GetRenderTargetView(),
		reinterpret_cast< const float* >( &color )
	);
}

// DX11Display

DX11Display::~DX11Display() {
	dxgiOutput = nullptr;
	window = nullptr;
}

bool DX11Display::Create( ID3D11Device *const device, IDXGIAdapter *const adapter, const int outputId ) {
	HRESULT hresult = 0;

	// ziskat output s pozadovanym id
	IDXGIOutput *output = nullptr;
	hresult = adapter->EnumOutputs( static_cast< UINT >( outputId ), &output );
	if ( FAILED( hresult ) ) {
		return false;
	}
	// Zjistit dostupne rezimy
	this->dxgiOutput = output;
	EnumDisplayModes();

	return true;
}

void DX11Display::EnumDisplayModes() {
	HRESULT hresult = 0;

	// zjisteni poctu rezimu
	UINT count = 0;
	dxgiOutput->GetDisplayModeList( BACK_BUFFER_FORMAT, 0, &count, NULL );

	// zjistit dostupne mody
	DXGI_MODE_DESC *dxgiModes = new DXGI_MODE_DESC[ count ];
	dxgiOutput->GetDisplayModeList( BACK_BUFFER_FORMAT, 0, &count, dxgiModes );

	// ulozit vysledek
	modes.Clear();
	for ( UINT i = 0; i < count; i++ ) {
		DisplayMode dm;
		dm.width = static_cast< int >( dxgiModes[ i ].Width );
		dm.height = static_cast< int >( dxgiModes[ i ].Height );
		dm.refreshRateNumerator = static_cast< int >( dxgiModes[ i ].RefreshRate.Numerator );
		dm.refreshRateDenominator = static_cast< int >( dxgiModes[ i ].RefreshRate.Denominator );
		modes.Push( dm );
	}
	// uvolneni docasnych objektu
	delete [] dxgiModes;
}

void DX11Display::SetSystemMode() {
	// Pri vstupu do fullscreenu je predan ukazatel na window,
	// pokud neni dostupny neni okno ve fullscreen rezimu
	if ( window == nullptr ) {
		return;
	}
	BackBuffer *backBuffer = window->GetBackBuffer();
	if ( backBuffer == nullptr ) {
		return;
	}
	// ziskat ukazatel na swap chain
	ASSERT_DOWNCAST( backBuffer, DX11BackBuffer );
	IDXGISwapChain *swapChain = reinterpret_cast< DX11BackBuffer* >( backBuffer )->GetSwapChain();
	if ( swapChain == nullptr ) {
		return;
	}
	swapChain->SetFullscreenState( FALSE, NULL );
	window = nullptr;
}

bool DX11Display::SetMode( const DisplayMode &mode, Window &window ) {
	BackBuffer *backBuffer = window.GetBackBuffer();
	if ( backBuffer == nullptr ) {
		return false;
	}
	// ziskat swap chain back bufferu
	ASSERT_DOWNCAST( backBuffer, DX11BackBuffer );
	IDXGISwapChain *swapChain = reinterpret_cast< DX11BackBuffer* >( backBuffer )->GetSwapChain();
	if ( swapChain == nullptr ) {
		return false;
	}
	// najit odpovidajici rezim
	DisplayMode validMode;
	FindMode( mode, validMode );

	// prepnout do rezimu cele obrazovky
	DXGI_MODE_DESC desc;
	ZeroMemory( &desc, sizeof( desc ) );
	desc.Width						= mode.width;
	desc.Height						= mode.height;
	desc.RefreshRate.Numerator		= static_cast< UINT >( mode.refreshRateNumerator );
	desc.RefreshRate.Denominator	= static_cast< UINT >( mode.refreshRateDenominator );
	desc.Format						= DXGI_FORMAT_UNKNOWN;
	desc.ScanlineOrdering			= DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
	desc.Scaling					= DXGI_MODE_SCALING_STRETCHED;

	swapChain->ResizeTarget( &desc );
	swapChain->SetFullscreenState( TRUE, dxgiOutput );
	swapChain->ResizeTarget( &desc );

	this->window = &window;
	return true;
}

bool DX11Display::GetMode( const int id, DisplayMode &result ) const {
	if ( id < 0 || id >= modes.Length() ) {
		return false;
	}
	result = modes[ id ];
	return true;
}

void DX11Display::FindMode( const DisplayMode &request, DisplayMode &result ) const {
	if ( modes.Length() == 0 ) {
		ZeroMemory( &result, sizeof( DisplayMode ) );
		return;
	}
	const DisplayMode *best = &modes[ 0 ];
	int bestDifference = Math::Abs( request.width - best->width ) + Math::Abs( request.height - best->height );
	int bestId = 0;

	for ( int i = 1; i < modes.Length(); i++ ) {
		const DisplayMode &mode = modes[ i ];
		int difference = Math::Abs( mode.width - request.width ) + Math::Abs( mode.height - request.height );

		// horsi odchylka rozlyseni, porovnat dalsi rezim
		if ( difference > bestDifference ) {
			continue;
		}
		// stejna odchylka rozlyseni, porovnat refresh rate
		if ( difference == bestDifference ) {
			// porovnat refresh rate
			float refreshRate = static_cast< float >( mode.refreshRateNumerator ) / static_cast< float >( mode.refreshRateDenominator );
			float bestRefreshRate = static_cast< float >( best->refreshRateNumerator ) / static_cast< float >( best->refreshRateDenominator );
			
			// nalezeny mod ma horsi refresh rate, hledat dal
			if ( refreshRate < bestRefreshRate ) {
				continue;
			}
			// nalezeny mod ma lepsi refresh rate, ulozit novy mod
			best = &mode;
			bestDifference = difference;
			bestId = i;
			continue;
		}
		// lepsi odchylka rozlyseni, ulozit mod
		best = &mode;
		bestDifference = difference;
		bestId = i;
	}
	result = *best;
}

void DX11Display::GetBestMode( DisplayMode &result ) const {
	DXGI_OUTPUT_DESC desc;
	dxgiOutput->GetDesc( &desc );
	DisplayMode mode;
	mode.width = static_cast< int >( desc.DesktopCoordinates.right - desc.DesktopCoordinates.left );
	mode.height = static_cast< int >( desc.DesktopCoordinates.bottom - desc.DesktopCoordinates.top );
	mode.refreshRateNumerator = 1000;
	mode.refreshRateDenominator = 1;
	FindMode( mode, result );
}

// DX11BackBuffer

DX11BackBuffer::DX11BackBuffer() {
	device = nullptr;
	dxgiSwapChain = nullptr;
	renderTargetView = nullptr;
	window = nullptr;
}

DX11BackBuffer::~DX11BackBuffer() {
	ReleaseCOM( &device );
	ReleaseCOM( &dxgiSwapChain );
	ReleaseCOM( &renderTargetView );
}

bool DX11BackBuffer::Create( ID3D11Device *const device, IDXGIFactory1 *const factory, Window &window ) {
	HRESULT hresult = 0;

	ASSERT_DOWNCAST( &window, WindowsWindow );
	HWND hwnd = static_cast< WindowsWindow* >( &window )->GetHandle();
	
	// create swap chain

	DXGI_SWAP_CHAIN_DESC desc = { 0 };
	desc.BufferCount						= 2;
	desc.BufferDesc.Width					= window.GetClientWidth();
	desc.BufferDesc.Height					= window.GetClientHeight();
	desc.BufferDesc.Format					= BACK_BUFFER_FORMAT;
	desc.BufferDesc.RefreshRate.Numerator	= 0;
	desc.BufferDesc.RefreshRate.Denominator = 0;
	desc.BufferUsage						= DXGI_USAGE_RENDER_TARGET_OUTPUT;
	desc.SampleDesc.Count					= 1;
	desc.SampleDesc.Quality					= 0;
	desc.OutputWindow						= hwnd;
	desc.Windowed							= TRUE;
	desc.SwapEffect							= DXGI_SWAP_EFFECT_DISCARD;
	desc.BufferDesc.Scaling					= DXGI_MODE_SCALING_STRETCHED;
	desc.Flags								= DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;

	IDXGISwapChain *swapChain = nullptr;
	hresult = factory->CreateSwapChain( device, &desc, &swapChain );
	if ( FAILED( hresult ) ) {
		return false;
	}
	// create render target view
	ID3D11Texture2D *buffer = nullptr;
	hresult = swapChain->GetBuffer( 0, __uuidof( ID3D11Texture2D ), reinterpret_cast< LPVOID* >( &buffer ) );
	if ( FAILED( hresult ) ) {
		swapChain->Release();
		return false;
	}
	D3D11_TEXTURE2D_DESC bufferDesc;
	buffer->GetDesc( &bufferDesc );
	ID3D11RenderTargetView *renderTargetView = nullptr;
	hresult = device->CreateRenderTargetView( buffer, NULL, &renderTargetView );
	buffer->Release();
	
	if ( FAILED( hresult ) ) {
		swapChain->Release();
		return false;
	}
	
	// vypnuti defaultniho prepinani do rezimu cele obrazovky
	factory->MakeWindowAssociation( hwnd, DXGI_MWA_NO_ALT_ENTER  );

	// ulozit vytvorene objekty
	this->device = device;
	device->AddRef();
	this->window = &window;
	this->dxgiSwapChain = swapChain;
	this->renderTargetView = renderTargetView;

	return true;
}

ID3D11RenderTargetView *DX11BackBuffer::GetRenderTargetView() {
	return renderTargetView;
}

IDXGISwapChain *DX11BackBuffer::GetSwapChain() {
	return dxgiSwapChain;
}

void DX11BackBuffer::Present( const int vsync ) {
	dxgiSwapChain->Present( vsync, 0 );
}

void DX11BackBuffer::Resize() {
	// zjistit pocet referenci na RenderTargetView
	ULONG refs = renderTargetView->AddRef();
	renderTargetView->Release();
	
	// nejsou uvolneny vsechny reference na back buffer
	if ( refs > 2 ) {
		return;
	}
	// release RTV
	ReleaseCOM( &renderTargetView );
	
	// resize
	const int width = window->GetClientWidth();
	const int height = window->GetClientHeight();
	DXGI_SWAP_CHAIN_DESC swapChainDesc;
	dxgiSwapChain->GetDesc( &swapChainDesc );
	dxgiSwapChain->ResizeBuffers( swapChainDesc.BufferCount, width, height, DXGI_FORMAT_UNKNOWN, swapChainDesc.Flags );
	
	// create new RTV
	HRESULT hresult = 0;
	ID3D11Texture2D *buffer = nullptr;
	hresult = dxgiSwapChain->GetBuffer( 0, __uuidof( ID3D11Texture2D ), reinterpret_cast< LPVOID* >( &buffer ) );
	if ( FAILED( hresult ) ) {
		AbortDXInvalidCall( hresult );
	}
	D3D11_TEXTURE2D_DESC bufferDesc;
	buffer->GetDesc( &bufferDesc );
	hresult = device->CreateRenderTargetView( buffer, NULL, &renderTargetView );
	buffer->Release();
	if ( FAILED( hresult ) ) {
		AbortDXInvalidCall( hresult );
	}
}

// DX11RenderBuffer

DX11RenderBuffer::DX11RenderBuffer() {
	texture = nullptr;
	renderTargetView = nullptr;
	shaderResourceView = nullptr;
	ZeroMemory( &desc, sizeof( desc ) );
}

DX11RenderBuffer::~DX11RenderBuffer() {
	ReleaseCOM( &texture );
	ReleaseCOM( &renderTargetView );
	ReleaseCOM( &shaderResourceView );
}

bool DX11RenderBuffer::Create( DX11Device *const device, const RenderBufferDesc &desc ) {
	D3D11_TEXTURE2D_DESC textureDesc;
	ZeroMemory( &textureDesc, sizeof( textureDesc ) );
	textureDesc.Width				= static_cast< UINT >( desc.width );
	textureDesc.Height				= static_cast< UINT >( desc.height );
	textureDesc.MipLevels			= 1;
	textureDesc.ArraySize			= 1;
	textureDesc.Format				= GetDXGIFormat( desc.format );
	textureDesc.Usage				= D3D11_USAGE_DEFAULT;
	textureDesc.BindFlags			= D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
	textureDesc.CPUAccessFlags		= 0;
	textureDesc.MiscFlags			= 0;
	textureDesc.SampleDesc.Count	= desc.samplesCount;

	if ( desc.multisampleQuality == MAX_MULTISAMPLE_QUALITY ) {
		textureDesc.SampleDesc.Quality = device->GetMultisampleQuality( desc.samplesCount );
	} else {
		textureDesc.SampleDesc.Quality = desc.multisampleQuality;
	}

	ID3D11Device *dxDevice = device->GetDevice();
	HRESULT hresult = 0;

	ID3D11Texture2D *texture = nullptr;
	hresult = dxDevice->CreateTexture2D( &textureDesc, NULL, &texture );
	if ( FAILED( hresult ) ) {
		return false;
	}
	ID3D11RenderTargetView *rtv = nullptr;
	hresult = dxDevice->CreateRenderTargetView( texture, NULL, &rtv );
	if ( FAILED( hresult ) ) {
		texture->Release();
		return false;
	}
	D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc;
	ZeroMemory( &srvDesc, sizeof( srvDesc ) );
	srvDesc.Format = GetDXGIFormat( desc.format );
	srvDesc.ViewDimension = desc.samplesCount > 1 ? D3D11_SRV_DIMENSION_TEXTURE2DMS : D3D11_SRV_DIMENSION_TEXTURE2D;
	srvDesc.Texture2D.MipLevels = 1;
	srvDesc.Texture2D.MostDetailedMip = 0;

	ID3D11ShaderResourceView *srv = nullptr;
	hresult = dxDevice->CreateShaderResourceView( texture, &srvDesc, &srv );
	if ( FAILED( hresult ) ) {
		texture->Release();
		rtv->Release();
		return false;
	}
	// ulozit vytvorene objekty
	this->texture = texture;
	this->renderTargetView = rtv;
	this->shaderResourceView = srv;
	this->desc = desc;

	return true;
}

RenderBufferDesc DX11RenderBuffer::GetDesc() const {
	return desc;
}

ID3D11RenderTargetView *DX11RenderBuffer::GetRenderTargetView() {
	return renderTargetView;
}

// DX11DepthStencilBuffer

DX11DepthStencilBuffer::DX11DepthStencilBuffer() {
	texture = nullptr;
	view = nullptr;
	ZeroMemory( &desc, sizeof( desc ) );
}

DX11DepthStencilBuffer::~DX11DepthStencilBuffer() {
	ReleaseCOM( &view );
	ReleaseCOM( &texture );
}

bool DX11DepthStencilBuffer::Create( ID3D11Device *device, const DepthStencilBufferDesc &desc ) {
	D3D11_TEXTURE2D_DESC bufferDesc;
	ZeroMemory( &bufferDesc, sizeof( bufferDesc ) );
	bufferDesc.Width				= static_cast< UINT >( desc.width );
	bufferDesc.Height				= static_cast< UINT >( desc.height );
	bufferDesc.MipLevels			= 1;
	bufferDesc.ArraySize			= 1;
	bufferDesc.Format				= DXGI_FORMAT_D24_UNORM_S8_UINT;
	bufferDesc.SampleDesc.Count		= static_cast< UINT >( desc.samplesCount );
	bufferDesc.SampleDesc.Quality	= static_cast< UINT >( desc.multisampleQuality );
	bufferDesc.Usage				= D3D11_USAGE_DEFAULT;
	bufferDesc.BindFlags			= D3D11_BIND_DEPTH_STENCIL;

	ID3D11Texture2D *texture = nullptr;
	HRESULT hresult = device->CreateTexture2D( &bufferDesc, NULL, &texture );
	if ( FAILED( hresult ) ) {
		return false;
	}

	D3D11_DEPTH_STENCIL_VIEW_DESC viewDesc;
	ZeroMemory( &viewDesc, sizeof( viewDesc ) );
	viewDesc.Format = bufferDesc.Format;
	viewDesc.ViewDimension = desc.samplesCount > 1 ? D3D11_DSV_DIMENSION_TEXTURE2DMS : D3D11_DSV_DIMENSION_TEXTURE2D;
	viewDesc.Flags = 0;

	ID3D11DepthStencilView *view = nullptr;
	hresult = device->CreateDepthStencilView( texture, &viewDesc, &view );
	if ( FAILED( hresult ) ) {
		texture->Release();
		return false;
	}
	// ulozit objekty
	this->texture = texture;
	this->view = view;
	this->desc = desc;

	return true;
}

DepthStencilBufferDesc DX11DepthStencilBuffer::GetDesc() const {
	return desc;
}

ID3D11DepthStencilView *DX11DepthStencilBuffer::GetDepthStencilView() {
	return view;
}

// DX11TextureSampler

DX11TextureSampler::DX11TextureSampler() {
	sampler = nullptr;
	ZeroMemory( &desc, sizeof( desc ) );
}

DX11TextureSampler::~DX11TextureSampler() {
	ReleaseCOM( &sampler );
}

bool DX11TextureSampler::Create( DX11Device * const device, const TextureSamplerDesc &desc ) {

	D3D11_TEXTURE_ADDRESS_MODE textureAddressModes[ 3 ];
	textureAddressModes[ static_cast< int >( TextureAddress::WRAP ) ]	= D3D11_TEXTURE_ADDRESS_WRAP;
	textureAddressModes[ static_cast< int >( TextureAddress::MIRROR ) ] = D3D11_TEXTURE_ADDRESS_MIRROR;
	textureAddressModes[ static_cast< int >( TextureAddress::CLAMP ) ]	= D3D11_TEXTURE_ADDRESS_CLAMP;

	D3D11_FILTER filter = D3D11_FILTER_MIN_MAG_MIP_POINT;
	switch ( desc.filter ) {
	case TextureFilter::LINEAR:								filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;				break;
	case TextureFilter::POINT:								filter = D3D11_FILTER_MIN_MAG_MIP_POINT;				break;
	case TextureFilter::ANISOTROPIC:						filter = D3D11_FILTER_ANISOTROPIC;						break;
	case TextureFilter::POIN_MIP_LINEAR:					filter = D3D11_FILTER_MIN_MAG_POINT_MIP_LINEAR;			break;
	case TextureFilter::MIN_POINT_MAG_LINEAR_MIP_POINT:		filter = D3D11_FILTER_MIN_POINT_MAG_LINEAR_MIP_POINT;	break;
	case TextureFilter::MIN_POINT_MAG_MIP_LINEAR:			filter = D3D11_FILTER_MIN_POINT_MAG_MIP_LINEAR;			break;
	case TextureFilter::MIN_LINEAR_MAG_MIP_POINT:			filter = D3D11_FILTER_MIN_LINEAR_MAG_MIP_POINT;			break;
	case TextureFilter::MIN_LINEAR_MAG_POINT_MIP_LINEAR:	filter = D3D11_FILTER_MIN_LINEAR_MAG_POINT_MIP_LINEAR;	break;
	case TextureFilter::LINEAR_MIP_POINT:					filter = D3D11_FILTER_MIN_MAG_LINEAR_MIP_POINT;			break;
	}

	D3D11_SAMPLER_DESC samplerDesc;
	ZeroMemory( &samplerDesc, sizeof( samplerDesc ) );
	samplerDesc.Filter			= filter;
	samplerDesc.AddressU		= textureAddressModes[ static_cast< int >( desc.uAddress ) ];
	samplerDesc.AddressV		= textureAddressModes[ static_cast< int >( desc.vAddress ) ];
	samplerDesc.AddressW		= textureAddressModes[ static_cast< int >( desc.wAddress ) ];
	samplerDesc.MipLODBias		= 0;
	samplerDesc.MaxAnisotropy	= desc.maxAnisotropy;
	samplerDesc.ComparisonFunc	= D3D11_COMPARISON_NEVER;
	samplerDesc.MinLOD			= desc.minLOD;
	samplerDesc.MaxLOD			= ( desc.maxLOD == MAX_TEXTURE_LOD ? D3D11_FLOAT32_MAX : desc.maxLOD );

	ID3D11Device *dxDevice = device->GetDevice();
	HRESULT hresult = dxDevice->CreateSamplerState( &samplerDesc, &sampler );
	if ( FAILED( hresult ) ) {
		return false;
	}

	this->desc = desc;
	return true;
}

ID3D11SamplerState *DX11TextureSampler::GetSamplerState() {
	return sampler;
}

TextureSamplerDesc DX11TextureSampler::GetDesc() const {
	return desc;
}