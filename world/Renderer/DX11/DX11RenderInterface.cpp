#include <d3dcompiler.h>
#include "DX11RenderInterface.h"
#include "..\..\Platform\Application.h"
#include "..\..\Platform\Windows\WindowsWindow.h"
#include "..\..\Framework\Math.h"
#include "..\..\Framework\Debug.h"

const DXGI_FORMAT BACK_BUFFER_FORMAT = DXGI_FORMAT_R8G8B8A8_UNORM;

void AbortDXInvalidCall( const HRESULT hresult ) {
	Application::Abort();
}

// RenderInterface enum to DX11 enum wrappers

DXGI_FORMAT GetDXGIFormat(const Format format) {
	switch ( format ) {
	case Format::UNKNOWN:						return DXGI_FORMAT_UNKNOWN;
	case Format::R8G8B8A8_UNORM:				return DXGI_FORMAT_R8G8B8A8_UNORM;
	case Format::R8G8B8A8_SNORM:				return DXGI_FORMAT_R8G8B8A8_SNORM;
	case Format::R16G16B16A16_FLOAT:			return DXGI_FORMAT_R16G16B16A16_FLOAT;
	case Format::R16G16_FLOAT:					return DXGI_FORMAT_R16G16_FLOAT;
	case Format::R8_UNORM:						return DXGI_FORMAT_R8_UNORM;
	case Format::R16_FLOAT:						return DXGI_FORMAT_R16_FLOAT;
	case Format::R32_FLOAT:						return DXGI_FORMAT_R32_FLOAT;
	case Format::DEPTH_24_UNORM_STENCIL_8_UINT:	return DXGI_FORMAT_D24_UNORM_S8_UINT;
	case Format::BC1:							return DXGI_FORMAT_BC1_UNORM;
	case Format::BC3:							return DXGI_FORMAT_BC3_UNORM;
	}
	return DXGI_FORMAT_UNKNOWN;
}

D3D11_COMPARISON_FUNC GetComparsionFunc( const DepthStencilComparsion dsc ) {
	switch ( dsc ) {
	case DepthStencilComparsion::NEVER:			return D3D11_COMPARISON_NEVER;
	case DepthStencilComparsion::LESS:			return D3D11_COMPARISON_LESS;
	case DepthStencilComparsion::EQUAL:			return D3D11_COMPARISON_EQUAL;
	case DepthStencilComparsion::LESS_EQUAL:	return D3D11_COMPARISON_LESS_EQUAL;
	case DepthStencilComparsion::GREATER:		return D3D11_COMPARISON_GREATER;
	case DepthStencilComparsion::NOT_EQUAL:		return D3D11_COMPARISON_NOT_EQUAL;
	case DepthStencilComparsion::GREATER_EQUAL:	return D3D11_COMPARISON_GREATER_EQUAL;
	case DepthStencilComparsion::ALWAYS:		return D3D11_COMPARISON_ALWAYS;
	}
	return D3D11_COMPARISON_NEVER;
}

D3D11_STENCIL_OP GetStencilOp( const StencilOperation op ) {
	switch ( op ) {
	case StencilOperation::KEEP:		return D3D11_STENCIL_OP_KEEP;
	case StencilOperation::ZERO:		return D3D11_STENCIL_OP_ZERO;
	case StencilOperation::REPLACE:		return D3D11_STENCIL_OP_REPLACE;
	case StencilOperation::INCR_SAT:	return D3D11_STENCIL_OP_INCR_SAT;
	case StencilOperation::DECR_SAT:	return D3D11_STENCIL_OP_DECR_SAT;
	case StencilOperation::INVERT:		return D3D11_STENCIL_OP_INVERT;
	case StencilOperation::INCR:		return D3D11_STENCIL_OP_INCR;
	case StencilOperation::DECR:		return D3D11_STENCIL_OP_DECR;
	}
	return D3D11_STENCIL_OP_KEEP;
}

D3D11_TEXTURE_ADDRESS_MODE GetTextureAddressMode( const TextureAddressing addressing ) {
	switch ( addressing ) {
	case TextureAddressing::WRAP:		return D3D11_TEXTURE_ADDRESS_WRAP;
	case TextureAddressing::MIRROR:		return D3D11_TEXTURE_ADDRESS_MIRROR;
	case TextureAddressing::CLAMP:		return D3D11_TEXTURE_ADDRESS_CLAMP;
	}
	return D3D11_TEXTURE_ADDRESS_WRAP;
}

D3D11_FILTER GetTextureFilter( const TextureFilter filter ) {
	switch ( filter ) {
	case TextureFilter::POINT:								return D3D11_FILTER_MIN_MAG_MIP_POINT;
	case TextureFilter::POINT_MIP_LINEAR:					return D3D11_FILTER_MIN_MAG_POINT_MIP_LINEAR;
	case TextureFilter::MIN_POINT_MAG_LINEAR_MIP_POINT:		return D3D11_FILTER_MIN_POINT_MAG_LINEAR_MIP_POINT;
	case TextureFilter::MIN_POINT_MAG_MIP_LINEAR:			return D3D11_FILTER_MIN_POINT_MAG_MIP_LINEAR;
	case TextureFilter::MIN_LINEAR_MAG_MIP_POINT:			return D3D11_FILTER_MIN_LINEAR_MAG_MIP_POINT;
	case TextureFilter::MIN_LINEAR_MAG_POINT_MIP_LINEAR:	return D3D11_FILTER_MIN_LINEAR_MAG_POINT_MIP_LINEAR;
	case TextureFilter::LINEAR_MIP_POINT:					return D3D11_FILTER_MIN_MAG_LINEAR_MIP_POINT;
	case TextureFilter::LINEAR:								return D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	case TextureFilter::ANISOTROPIC:						return D3D11_FILTER_ANISOTROPIC;
	}
	return D3D11_FILTER_MIN_MAG_MIP_POINT;
}

D3D11_USAGE GetUsage( const BufferUsage usage ) {
	switch ( usage ) {
	case BufferUsage::DRAW:		return D3D11_USAGE_DEFAULT;
	case BufferUsage::STATIC:	return D3D11_USAGE_IMMUTABLE;
	case BufferUsage::DYNAMIC:	return D3D11_USAGE_DYNAMIC;
	case BufferUsage::COPY:		return D3D11_USAGE_STAGING;
	}
	return D3D11_USAGE_STAGING;
}

UINT GetCPUAccessFlags( const BufferAccess access ) {
	UINT flags = 0;
	if ( static_cast< unsigned int >( access ) & static_cast< unsigned int >( BufferAccess::READ ) ) {
		flags |= D3D11_CPU_ACCESS_READ;
	}
	if ( static_cast< unsigned int >( access ) & static_cast< unsigned int >( BufferAccess::WRITE ) ) {
		flags |= D3D11_CPU_ACCESS_WRITE;
	}
	return flags;
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

bool DX11Device::Create( const DX11CreateDeviceParams& params ) {
	HRESULT hresult = 0;

	// vytvorit DXGIFactory1
	IDXGIFactory1* factory = nullptr;
	hresult = CreateDXGIFactory1( __uuidof( IDXGIFactory1 ), ( void** )( &factory ) );
	if ( FAILED( hresult ) ) {
		return false;
	}
	// pokusit se ziskat pozadovany adapter
	IDXGIAdapter* adapter = nullptr;
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

ID3D11DeviceContext* DX11Device::GetContext() {
	return context;
}

ID3D11Device* DX11Device::GetDevice() {
	return device;
}

int DX11Device::GetMultisampleQuality( const int samplesCount) const {
	int levels = 0;
	device->CheckMultisampleQualityLevels( BACK_BUFFER_FORMAT, samplesCount, reinterpret_cast< unsigned int* >( &levels ) );
	return levels;
}

CommandInterface* DX11Device::CreateCommandInterface() {
	DX11CommandInterface* commandInterface = new DX11CommandInterface();
	if ( !commandInterface->Create() ) {
		delete commandInterface;
		return nullptr;
	}
	return commandInterface;
}

BackBuffer* DX11Device::CreateBackBuffer( Window& window ) {
	DX11BackBuffer* backBuffer = new DX11BackBuffer();
	if ( !backBuffer->Create( device, dxgiFactory, window ) ) {
		delete backBuffer;
		return nullptr;
	}
	return backBuffer;
}

VertexBuffer* DX11Device::CreateVertexBuffer( const VertexBufferParams& params, const void* const initialData  ) {
	DX11VertexBuffer* buffer = new DX11VertexBuffer();
	if ( !buffer->Create( device, params, initialData ) ) {
		delete buffer;
		return nullptr;
	}
	return buffer;
}

IndexBuffer* DX11Device::CreateIndexBuffer( const IndexBufferParams& params, const void* const initialData  ) {
	DX11IndexBuffer* buffer = new DX11IndexBuffer();
	if ( !buffer->Create( device, params, initialData ) ) {
		delete buffer;
		return nullptr;
	}
	return buffer;
}

VertexBufferDescriptor*	DX11Device::CreateVertexBufferDescriptor( VertexBuffer* const buffer, const int vertexOffset ) {
	DX11VertexBufferDescriptor* descriptor = new DX11VertexBufferDescriptor();
	descriptor->Create( buffer, vertexOffset );
	return descriptor;
}

TextureBuffer* DX11Device::CreateTextureBuffer( const TextureBufferParams& params, const void* const initialData[] ) {
	DX11TextureBuffer* buffer = new DX11TextureBuffer();
	if ( !buffer->Create( device, params, initialData ) ) {
		delete buffer;
		return nullptr;
	}
	return buffer;
}

ConstantBuffer* DX11Device::CreateConstantBuffer( const ConstantBufferParams& params, const void* const initialData ) {
	DX11ConstantBuffer* buffer = new DX11ConstantBuffer();
	if ( !buffer->Create( device, params, initialData ) ) {
		delete buffer;
		return false;
	}
	return buffer;
}

RenderTargetDescriptor* DX11Device::CreateRenderTargetDescriptor( TextureBuffer* const buffer ) {
	DX11RenderTargetDescriptor* descriptor = new DX11RenderTargetDescriptor();
	if ( !descriptor->Create( device, buffer ) ) {
		delete descriptor;
		return nullptr;
	}
	return descriptor;
}

RenderTargetDescriptor* DX11Device::CreateRenderTargetDescriptor( BackBuffer* const buffer ) {
	DX11RenderTargetDescriptor* descriptor = new DX11RenderTargetDescriptor();
	if ( !descriptor->Create( device, buffer ) ) {
		delete descriptor;
		return nullptr;
	}
	return descriptor;
}

DepthStencilDescriptor* DX11Device::CreateDepthStencilDescriptor( TextureBuffer* const buffer, const DepthStencilDescriptorParams& params ) {
	DX11DepthStencilDescriptor* descriptor = new DX11DepthStencilDescriptor();
	if ( !descriptor->Create( device, buffer, params ) ) {
		delete descriptor;
		return nullptr;
	}
	return descriptor;
}

ConstantBufferDescriptor* DX11Device::CreateConstantBufferDescriptor( ConstantBuffer* const buffer, const ConstantBufferDescriptorParams& params ) {
	DX11ConstantBufferDescriptor* descriptor = new DX11ConstantBufferDescriptor();
	if ( !descriptor->Create( buffer, params ) ) {
		delete descriptor;
		return nullptr;
	}
	return descriptor;
}

Display* DX11Device::CreateDisplay( const int outputId ) {
	DX11Display* display = new DX11Display();
	if ( !display->Create( device, dxgiAdapter, outputId ) ) {
		delete display;
		return nullptr;
	}
	return display;
}

TextureSampler* DX11Device::CreateTextureSampler( const TextureSamplerParams& params ) {
	DX11TextureSampler* sampler = new DX11TextureSampler();
	if ( !sampler->Create( device, params ) ) {
		delete sampler;
		return nullptr;
	}
	return sampler;
}

Shader* DX11Device::CreateShader( const ShaderParams& params ) {
	DX11Shader *shader = new DX11Shader();
	if ( !shader->Compile( device, params ) ) {
		delete shader;
		return nullptr;
	}
	return shader;
}

// DX11CommandInterface

DX11CommandInterface::DX11CommandInterface() {
	context = nullptr;
}

DX11CommandInterface::~DX11CommandInterface() {
	ReleaseCOM( &context );
}

bool DX11CommandInterface::Create() {
	return true;
}

void DX11CommandInterface::Begin( Device* const device ) {
	ASSERT_DOWNCAST( device, DX11Device );
	context = static_cast< DX11Device* >( device )->GetContext();
}

void DX11CommandInterface::Begin( CommandList* const commandList ) {
	// UNIMPLEMENTED
}

void DX11CommandInterface::End() {
	context = nullptr;

	// UNIMPLEMENTED command list!
}

void DX11CommandInterface::SetRenderTargets( RenderTargetDescriptor* const renderTargets[], const int count, DepthStencilDescriptor* const depthStencil ) {
	if ( count > MAX_RENDER_TARGETS ) {
		return;
	}
	ID3D11RenderTargetView* renderTargetViews[ MAX_RENDER_TARGETS ] = { NULL };
	for ( int i = 0; i < count; i++ ) {
		ASSERT_DOWNCAST( renderTargets[ i ], DX11RenderTargetDescriptor );
		renderTargetViews[ i ] = static_cast< DX11RenderTargetDescriptor* >( renderTargets[ i ] )->GetView();
	}
	ID3D11DepthStencilView* depthStencilView = NULL;
	if ( depthStencil != nullptr ) {
		ASSERT_DOWNCAST( depthStencil, DX11DepthStencilDescriptor );
		depthStencilView = static_cast< DX11DepthStencilDescriptor* >( depthStencil )->GetView();
	}
	context->OMSetRenderTargets( MAX_RENDER_TARGETS, renderTargetViews, depthStencilView );
}

void DX11CommandInterface::ClearRenderTarget( RenderTargetDescriptor* const renderTarget, const Color& color ) {
	ASSERT_DOWNCAST( renderTarget, DX11RenderTargetDescriptor );
	context->ClearRenderTargetView(
		static_cast< DX11RenderTargetDescriptor* >( renderTarget )->GetView(),
		reinterpret_cast< const FLOAT* >( &color )
	);
}

void DX11CommandInterface::ClearDepthStencil( DepthStencilDescriptor* const descriptor, const float depth, const uint8_t stencil ) {
	ASSERT_DOWNCAST( descriptor, DX11DepthStencilDescriptor );
	context->ClearDepthStencilView(
		static_cast< DX11DepthStencilDescriptor* >( descriptor )->GetView(),
		D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL,
		static_cast< FLOAT >( depth ),
		static_cast< UINT8 >( stencil )
	);
}

void DX11CommandInterface::ClearDepth( DepthStencilDescriptor* const descriptor, const float depth ) {
	ASSERT_DOWNCAST( descriptor, DX11DepthStencilDescriptor );
	context->ClearDepthStencilView(
		static_cast< DX11DepthStencilDescriptor* >( descriptor )->GetView(),
		D3D11_CLEAR_DEPTH,
		static_cast< FLOAT >( depth ),
		0
	);
}

void DX11CommandInterface::ClearStencil( DepthStencilDescriptor* const descriptor, const uint8_t stencil ) {
	ASSERT_DOWNCAST( descriptor, DX11DepthStencilDescriptor );
	context->ClearDepthStencilView(
		static_cast< DX11DepthStencilDescriptor* >( descriptor )->GetView(),
		D3D11_CLEAR_STENCIL,
		0,
		static_cast< UINT8 >( stencil )
	);
}

void DX11CommandInterface::ClearState() {
	context->ClearState();
}

void DX11CommandInterface::Flush() {
	context->Flush();
}

// DX11Display

DX11Display::~DX11Display() {
	dxgiOutput = nullptr;
	window = nullptr;
}

bool DX11Display::Create( ID3D11Device* const device, IDXGIAdapter* const adapter, const int outputId ) {
	HRESULT hresult = 0;

	// ziskat output s pozadovanym id
	IDXGIOutput* output = nullptr;
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
	std::unique_ptr< DXGI_MODE_DESC[] > dxgiModes( new DXGI_MODE_DESC[ count ] );
	dxgiOutput->GetDisplayModeList( BACK_BUFFER_FORMAT, 0, &count, dxgiModes.get() );

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
}

void DX11Display::SetSystemMode() {
	// Pri vstupu do fullscreenu je predan ukazatel na window,
	// pokud neni dostupny neni okno ve fullscreen rezimu
	if ( window == nullptr ) {
		return;
	}
	BackBuffer* backBuffer = window->GetBackBuffer();
	if ( backBuffer == nullptr ) {
		return;
	}
	// ziskat ukazatel na swap chain
	ASSERT_DOWNCAST( backBuffer, DX11BackBuffer );
	IDXGISwapChain* swapChain = static_cast< DX11BackBuffer* >( backBuffer )->GetSwapChain();
	if ( swapChain == nullptr ) {
		return;
	}
	swapChain->SetFullscreenState( FALSE, NULL );
	window = nullptr;
}

bool DX11Display::SetMode( const DisplayMode& mode, Window& window ) {
	BackBuffer* backBuffer = window.GetBackBuffer();
	if ( backBuffer == nullptr ) {
		return false;
	}
	// ziskat swap chain back bufferu
	ASSERT_DOWNCAST( backBuffer, DX11BackBuffer );
	IDXGISwapChain* swapChain = static_cast< DX11BackBuffer* >( backBuffer )->GetSwapChain();
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

void DX11Display::FindMode( const DisplayMode& request, DisplayMode& result ) const {
	if ( modes.Length() == 0 ) {
		ZeroMemory( &result, sizeof( DisplayMode ) );
		return;
	}
	const DisplayMode* best = &modes[ 0 ];
	int bestDifference = Math::Abs( request.width - best->width ) + Math::Abs( request.height - best->height );
	int bestId = 0;

	for ( int i = 1; i < modes.Length(); i++ ) {
		const DisplayMode& mode = modes[ i ];
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

void DX11Display::GetBestMode( DisplayMode& result ) const {
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
	dxgiSwapChain = nullptr;
	device = nullptr;
	window = nullptr;
	width = 0;
	height = 0;
}

DX11BackBuffer::~DX11BackBuffer() {
	ReleaseCOM( &dxgiSwapChain );
	ReleaseCOM( &device );
}

bool DX11BackBuffer::Create( ID3D11Device* const device, IDXGIFactory1* const factory, Window& window ) {
	HRESULT hresult = 0;

	ASSERT_DOWNCAST( &window, WindowsWindow );
	HWND hwnd = static_cast< WindowsWindow* >( &window )->GetHandle();
	
	// swap chain

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

	IDXGISwapChain* swapChain = nullptr;
	hresult = factory->CreateSwapChain( device, &desc, &swapChain );
	if ( FAILED( hresult ) ) {
		return false;
	}
	
	// vypnuti defaultniho prepinani do rezimu cele obrazovky
	factory->MakeWindowAssociation( hwnd, DXGI_MWA_NO_ALT_ENTER  );

	// ulozit objekty
	this->device = device;
	device->AddRef();
	this->window = &window;
	this->dxgiSwapChain = swapChain;

	return true;
}

void DX11BackBuffer::Resize() {
	const int width = window->GetClientWidth();
	const int height = window->GetClientHeight();
	DXGI_SWAP_CHAIN_DESC swapChainDesc;
	dxgiSwapChain->GetDesc( &swapChainDesc );
	dxgiSwapChain->ResizeBuffers( swapChainDesc.BufferCount, width, height, DXGI_FORMAT_UNKNOWN, swapChainDesc.Flags );
}

void DX11BackBuffer::Present( const int vsync ) {
	dxgiSwapChain->Present( vsync, 0 );
}

IDXGISwapChain* DX11BackBuffer::GetSwapChain() {
	return dxgiSwapChain;
}

// DX11Buffer

DX11Buffer::DX11Buffer() {
	resource = nullptr;
	type = BufferType::UNDEFINED;
	format = Format::UNKNOWN;
	width = 0;
	height = 0;
	depth = 0;
	mipLevels = 0;
	arraySize = 0;
	samplesCount = 0;
	samplesQuality = 0;
}

void DX11Buffer::SetBuffer(
	ID3D11Resource* const resource,
	const BufferType type,
	const Format format,
	const int width,
	const int height,
	const int depth,
	const int mipLevels,
	const int arraySize,
	const int samplesCount,
	const int samplesQuality
) {
	this->resource = resource;
	this->type = type;
	this->format = format;
	this->width = width;
	this->height = height;
	this->depth = depth;
	this->mipLevels = mipLevels;
	this->arraySize = arraySize;
	this->samplesCount = samplesCount;
	this->samplesQuality = samplesQuality;
}

BufferType DX11Buffer::GetType() const {
	return type;
}

Format DX11Buffer::GetFormat() const {
	return format;
}

int DX11Buffer::GetWidth() const {
	return width;
}

int DX11Buffer::GetHeight() const {
	return height;
}

int DX11Buffer::GetDepth() const {
	return depth;
}

int DX11Buffer::GetMipLevels() const {
	return mipLevels;
}

int DX11Buffer::GetArraySize() const {
	return arraySize;
}

int DX11Buffer::GetSamplesCount() const {
	return samplesCount;
}

int DX11Buffer::GetSamplesQuality() const {
	return samplesQuality;
}

// DX11TextureBuffer

DX11TextureBuffer::DX11TextureBuffer() {
	texture = nullptr;
}

DX11TextureBuffer::~DX11TextureBuffer() {
	ReleaseCOM( &texture );
}

bool DX11TextureBuffer::Create( ID3D11Device* const device, const TextureBufferParams& params, const void* const initialData[] ) {
	D3D11_USAGE usage = GetUsage( params.usage );
	UINT CPUAccessFlags = GetCPUAccessFlags( params.access );

	UINT bindFlags = D3D11_BIND_SHADER_RESOURCE;
	// render target bind flag
	if ( params.renderTarget ) {
		bindFlags |= D3D11_BIND_RENDER_TARGET;
	}
	// depth stencil bind flag
	if ( params.format == Format::DEPTH_24_UNORM_STENCIL_8_UINT ) {
		bindFlags |= D3D11_BIND_DEPTH_STENCIL;
	}

	std::unique_ptr< D3D11_SUBRESOURCE_DATA[] > subresources( nullptr );
	
	// initialize subresources
	if ( initialData != nullptr ) {
		subresources.reset( new D3D11_SUBRESOURCE_DATA[ params.arraySize * params.mipLevels ] );
		FormatDesc formatDesc = GetFormatDesc( params.format );

		int subresourceIndex = 0;
		for ( int arrayIndex = 0; arrayIndex < params.arraySize; arrayIndex++ ) {
			TextureDimmensions dimmensions = params.dimmensions;

			for ( int mipIndex = 0; mipIndex < params.mipLevels; mipIndex++ ) {
				// set subresource
				subresources[ subresourceIndex ].pSysMem = initialData[ subresourceIndex ];
				subresources[ subresourceIndex ].SysMemPitch = formatDesc.pointPitch * dimmensions.width;
				subresources[ subresourceIndex ].SysMemSlicePitch = formatDesc.pointPitch * dimmensions.width * dimmensions.height;

				// vypocitat rozmery nasledujici mip urovne
				if ( dimmensions.width > 1 ) {
					dimmensions.width /= 2;
				}
				if ( dimmensions.height > 1 ) {
					dimmensions.height /= 2;
				}
				if ( dimmensions.depth > 1 ) {
					dimmensions.depth /= 2;
				}
				// dalsi subresource
				subresourceIndex += 1;
			}
		}
	}

	// 1D textures
	if ( params.type == TextureBufferType::TEXTURE_1D ||
		 params.type == TextureBufferType::TEXTURE_1D_ARRAY
	) {
		D3D11_TEXTURE1D_DESC textureDesc;
		textureDesc.Width 				= params.dimmensions.width;
		textureDesc.MipLevels 			= params.mipLevels;
		textureDesc.ArraySize			= params.arraySize;
		textureDesc.Format				= GetDXGIFormat( params.format );
		textureDesc.Usage				= usage;
		textureDesc.BindFlags			= bindFlags;
		textureDesc.CPUAccessFlags		= CPUAccessFlags;
		textureDesc.MiscFlags			= 0;
		
		ID3D11Texture1D* texture = nullptr;
		HRESULT hresult = device->CreateTexture1D( &textureDesc, subresources.get(), &texture );
		if ( FAILED( hresult ) ) {
			return false;
		}
		this->texture = texture;
		SetParams( params );
		return true;
	}

	// 2D textures
	if ( params.type == TextureBufferType::TEXTURE_2D ||
		 params.type == TextureBufferType::TEXTURE_2D_ARRAY  ||
		 params.type == TextureBufferType::TEXTURE_2D_MS ||
		 params.type == TextureBufferType::TEXTURE_2D_MS_ARRAY
	) {
		D3D11_TEXTURE2D_DESC textureDesc;
		textureDesc.Width 				= params.dimmensions.width;
		textureDesc.Height				= params.dimmensions.height;
		textureDesc.MipLevels 			= params.mipLevels;
		textureDesc.ArraySize			= params.arraySize;
		textureDesc.Format				= GetDXGIFormat( params.format );
		textureDesc.SampleDesc.Count 	= params.samplesCount;
		textureDesc.SampleDesc.Quality 	= params.samplesQuality;
		textureDesc.Usage				= usage;
		textureDesc.BindFlags			= bindFlags;
		textureDesc.CPUAccessFlags		= CPUAccessFlags;
		textureDesc.MiscFlags			= 0;
		
		ID3D11Texture2D* texture = nullptr;
		HRESULT hresult = device->CreateTexture2D( &textureDesc, subresources.get(), &texture );
		if ( FAILED( hresult ) ) {
			return false;
		}
		this->texture = texture;
		SetParams( params );
		return true;
	}

	// 3D texture
	if ( params.type == TextureBufferType::TEXTURE_3D ) {
		D3D11_TEXTURE3D_DESC textureDesc;
		textureDesc.Width 				= params.dimmensions.width;
		textureDesc.Height				= params.dimmensions.height;
		textureDesc.Depth				= params.dimmensions.depth;
		textureDesc.MipLevels 			= params.mipLevels;
		textureDesc.Format				= GetDXGIFormat( params.format );
		textureDesc.Usage				= usage;
		textureDesc.BindFlags			= bindFlags;
		textureDesc.CPUAccessFlags		= CPUAccessFlags;
		textureDesc.MiscFlags			= 0;

		ID3D11Texture3D* texture = nullptr;
		HRESULT hresult = device->CreateTexture3D( &textureDesc, subresources.get(), &texture );
		if ( FAILED( hresult ) ) {
			return false;
		}
		this->texture = texture;
		SetParams( params );
		return true;
	}

	return false;
}

ID3D11Resource* DX11TextureBuffer::GetTextureResource() {
	return texture;
}














// DX11TextureDescriptor

DX11TextureDescriptor::DX11TextureDescriptor() {
	view = nullptr;
	buffer = nullptr;
}
DX11TextureDescriptor::~DX11TextureDescriptor() {
	ReleaseCOM( &view );
}

bool DX11TextureDescriptor::Create( ID3D11Device* const device, TextureBuffer* const buffer ) {
	ASSERT_DOWNCAST( buffer, DX11TextureBuffer );
	ID3D11Resource* resource = static_cast< DX11TextureBuffer* >( buffer )->GetTextureResource();
	ID3D11ShaderResourceView* view = nullptr;
	HRESULT hresult = device->CreateShaderResourceView( resource, NULL, &view );
	if ( FAILED( hresult ) ) {
		return false;
	}
	this->view = view;
	this->buffer = static_cast< DX11TextureBuffer* >( buffer );
	return true;
}

ID3D11ShaderResourceView* DX11TextureDescriptor::GetView() {
	return view;
}

TextureBuffer* DX11TextureDescriptor::GetBuffer() {
	return buffer;
}

// DX11RenderTargetDescriptor

DX11RenderTargetDescriptor::DX11RenderTargetDescriptor() {
	view = nullptr;
	buffer = nullptr;
}

DX11RenderTargetDescriptor::~DX11RenderTargetDescriptor() {
	ReleaseCOM( &view );
}

bool DX11RenderTargetDescriptor::Create( ID3D11Device* const device, TextureBuffer* const buffer ) {
	ASSERT_DOWNCAST( buffer, DX11TextureBuffer );
	ID3D11Resource* resource = static_cast< DX11TextureBuffer* >( buffer )->GetTextureResource();
	ID3D11RenderTargetView* view = nullptr;
	HRESULT hresult = device->CreateRenderTargetView ( resource, NULL, &view );
	if ( FAILED( hresult ) ) {
		return false;
	}
	this->view = view;
	this->buffer = buffer;
	return true;
}

bool DX11RenderTargetDescriptor::Create( ID3D11Device* const device, BackBuffer* const buffer ) {
	ASSERT_DOWNCAST( buffer, DX11BackBuffer );
	IDXGISwapChain* swapChain = static_cast< DX11BackBuffer* >( buffer )->GetSwapChain();
	ID3D11Texture2D* texture = nullptr;
	HRESULT hresult = 0;
	hresult = swapChain->GetBuffer( 0, __uuidof( ID3D11Texture2D ), reinterpret_cast< LPVOID* >( &texture ) );
	if ( FAILED( hresult ) ) {
		return false;
	}
	ID3D11RenderTargetView* renderTargetView = nullptr;
	hresult = device->CreateRenderTargetView( texture, NULL, &renderTargetView );
	texture->Release();
	if ( FAILED( hresult ) ) {
		return false;
	}
	this->view = view;
	this->buffer = nullptr;
	return true;
}

TextureBuffer* DX11RenderTargetDescriptor::GetBuffer() {
	return buffer;
}

ID3D11RenderTargetView* DX11RenderTargetDescriptor::GetView() {
	return view;
}

// DX11DepthStencilDescriptor

DX11DepthStencilDescriptor::DX11DepthStencilDescriptor() {
	view = nullptr;
	state = nullptr;
}

DX11DepthStencilDescriptor::~DX11DepthStencilDescriptor() {
	ReleaseCOM( &state );
	ReleaseCOM( &view );
}

bool DX11DepthStencilDescriptor::Create( ID3D11Device* const device, TextureBuffer* const buffer, const DepthStencilDescriptorParams& params ) {
	if ( buffer->GetFormat() != Format::DEPTH_24_UNORM_STENCIL_8_UINT ) {
		return false;
	}
	ASSERT_DOWNCAST( buffer, DX11TextureBuffer );
	ID3D11Resource* texture = static_cast< DX11TextureBuffer* >( buffer )->GetTextureResource();
	HRESULT hresult = 0;

	// view

	D3D11_DEPTH_STENCIL_VIEW_DESC viewDesc;
	ZeroMemory( &viewDesc, sizeof( viewDesc ) );
	viewDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;

	// Pouze standard depth stencil usage podporuje zapis do bufferu
	if ( params.depthUsage != DepthStencilUsage::STANDARD ) {
		viewDesc.Flags |= D3D11_DSV_READ_ONLY_DEPTH;
	}
	if ( params.stencilUsage != DepthStencilUsage::STANDARD ) {
		viewDesc.Flags |= D3D11_DSV_READ_ONLY_STENCIL;
	}
	// view dimension
	TextureBufferType type = buffer->GetType();
	if ( type == TextureBufferType::TEXTURE_2D ) {
		viewDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
		viewDesc.Texture2D.MipSlice = 0;

	} else if ( type == TextureBufferType::TEXTURE_2D_MS ) {
		viewDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2DMS;

	} else {
		// not supported texture dimmension
		return false;
	}

	ID3D11DepthStencilView* view = nullptr;
	hresult = device->CreateDepthStencilView( texture, &viewDesc, &view );
	if ( FAILED( hresult )  ) {
		return false;
	}

	// state

	D3D11_DEPTH_STENCIL_DESC stateDesc;
	stateDesc.DepthEnable = static_cast< BOOL >( params.depthUsage != DepthStencilUsage::DISABLED );
	stateDesc.DepthWriteMask = (
		params.depthUsage == DepthStencilUsage::STANDARD ?
		D3D11_DEPTH_WRITE_MASK_ALL :
		D3D11_DEPTH_WRITE_MASK_ZERO
	);
	stateDesc.DepthFunc						= GetComparsionFunc( params.depthFunc );
	stateDesc.StencilEnable					= static_cast< BOOL >( params.stencilUsage != DepthStencilUsage::DISABLED );
	stateDesc.StencilReadMask				= D3D11_DEFAULT_STENCIL_READ_MASK;
	stateDesc.StencilWriteMask				= D3D11_DEFAULT_STENCIL_WRITE_MASK;
	stateDesc.FrontFace.StencilFunc			= GetComparsionFunc( params.stencilFunc );
	stateDesc.FrontFace.StencilPassOp		= GetStencilOp( params.stencilPassOp );
	stateDesc.FrontFace.StencilFailOp		= GetStencilOp( params.stencilFailOp );
	stateDesc.FrontFace.StencilDepthFailOp	= GetStencilOp( params.stencilDepthFailOp );
	stateDesc.BackFace						= stateDesc.FrontFace;

	ID3D11DepthStencilState* state = nullptr;
	hresult = device->CreateDepthStencilState( &stateDesc, &state );
	if ( FAILED( hresult )  ) {
		view->Release();
		return false;
	}

	// ulozit objekty
	this->view = view;
	this->state = state;
	return true;
}

ID3D11DepthStencilView* DX11DepthStencilDescriptor::GetView() {
	return view;
}

ID3D11DepthStencilState* DX11DepthStencilDescriptor::GetState() {
	return state;
}

// DX11TextureSampler

DX11TextureSampler::DX11TextureSampler() {
	sampler = nullptr;
}

DX11TextureSampler::~DX11TextureSampler() {
	ReleaseCOM( &sampler );
}

bool DX11TextureSampler::Create( ID3D11Device* const device, const TextureSamplerParams& params ) {
	D3D11_SAMPLER_DESC samplerDesc;
	ZeroMemory( &samplerDesc, sizeof( samplerDesc ) );
	samplerDesc.Filter			= GetTextureFilter( params.filter );
	samplerDesc.AddressU		= GetTextureAddressMode( params.uAddressing );
	samplerDesc.AddressV		= GetTextureAddressMode( params.vAddressing );
	samplerDesc.AddressW		= GetTextureAddressMode( params.wAddressing );
	samplerDesc.MipLODBias		= 0;
	samplerDesc.MaxAnisotropy	= params.maxAnisotropy;
	samplerDesc.ComparisonFunc	= D3D11_COMPARISON_NEVER;
	samplerDesc.MinLOD			= params.minLOD;
	samplerDesc.MaxLOD			= ( params.maxLOD == MAX_TEXTURE_LOD ? D3D11_FLOAT32_MAX : params.maxLOD );

	ID3D11SamplerState* sampler = nullptr;
	HRESULT hresult = device->CreateSamplerState( &samplerDesc, &sampler );
	if ( FAILED( hresult ) ) {
		return false;
	}

	this->sampler = sampler;
	return true;
}

ID3D11SamplerState* DX11TextureSampler::GetSamplerState() {
	return sampler;
}

// DX11VertexBuffer

DX11VertexBuffer::DX11VertexBuffer() {
	buffer = nullptr;
	ZeroMemory( &params, sizeof( params ) );
}

DX11VertexBuffer::~DX11VertexBuffer() {
	ReleaseCOM( &buffer );
}

bool DX11VertexBuffer::Create( ID3D11Device* const device, const VertexBufferParams& params, const void* const initialData ) {
	D3D11_BUFFER_DESC bufferDesc;
	bufferDesc.ByteWidth			= static_cast< UINT >( params.vertexSize ) * static_cast< UINT >( params.capacity );
	bufferDesc.Usage				= GetUsage( params.usage );
	bufferDesc.BindFlags			= D3D11_BIND_VERTEX_BUFFER;
	bufferDesc.CPUAccessFlags		= GetCPUAccessFlags( params.access );
	bufferDesc.MiscFlags			= 0;
	bufferDesc.StructureByteStride	= 0;

	D3D11_SUBRESOURCE_DATA* pData = NULL;
	D3D11_SUBRESOURCE_DATA data;
	if ( initialData != nullptr ) {
		data.pSysMem = initialData;
		data.SysMemPitch = 0;
		data.SysMemSlicePitch = 0;
		pData = &data;
	}

	ID3D11Buffer* buffer = nullptr;
	HRESULT hresult = device->CreateBuffer( &bufferDesc, pData, &buffer );
	if ( FAILED( hresult ) ) {
		return false;
	}
	this->buffer = buffer;
	this->params = params;
	return true;
}

int DX11VertexBuffer::GetCapacity() const {
	return params.capacity;
}

int DX11VertexBuffer::GetVertexSize() const {
	return params.vertexSize;
}

int DX11VertexBuffer::GetByteSize() const {
	return params.vertexSize * params.capacity;
}

ID3D11Buffer* DX11VertexBuffer::GetBuffer() {
	return buffer;
}

// DX11IndexBuffer

DX11IndexBuffer::DX11IndexBuffer() {
	buffer = nullptr;
	ZeroMemory( &params, sizeof( params ) );
}

DX11IndexBuffer::~DX11IndexBuffer() {
	ReleaseCOM( &buffer );
}

bool DX11IndexBuffer::Create( ID3D11Device* const device, const IndexBufferParams& params, const void* const initialData ) {
	D3D11_BUFFER_DESC bufferDesc;
	bufferDesc.ByteWidth			= ( params.format == IndexBufferFormat::UINT_16 ? 2 : 4 ) * static_cast< UINT >( params.capacity );
	bufferDesc.Usage				= GetUsage( params.usage );
	bufferDesc.BindFlags			= D3D11_BIND_INDEX_BUFFER;
	bufferDesc.CPUAccessFlags		= GetCPUAccessFlags( params.access );
	bufferDesc.MiscFlags			= 0;
	bufferDesc.StructureByteStride	= 0;

	D3D11_SUBRESOURCE_DATA* pData = NULL;
	D3D11_SUBRESOURCE_DATA data;
	if ( initialData != nullptr ) {
		data.pSysMem = initialData;
		data.SysMemPitch = 0;
		data.SysMemSlicePitch = 0;
		pData = &data;
	}

	ID3D11Buffer* buffer = nullptr;
	HRESULT hresult = device->CreateBuffer( &bufferDesc, pData, &buffer );
	if ( FAILED( hresult ) ) {
		return false;
	}
	this->buffer = buffer;
	this->params = params;
	return true;
}

int DX11IndexBuffer::GetCapacity() const {
	return params.capacity;
}

int DX11IndexBuffer::GetByteSize() const {
	return ( params.format == IndexBufferFormat::UINT_16 ? 2 : 4 ) * params.capacity;
}

IndexBufferFormat DX11IndexBuffer::GetFormat() const {
	return params.format;
}

ID3D11Buffer* DX11IndexBuffer::GetBuffer() {
	return buffer;
}

// DX11VertexBufferDescriptor

DX11VertexBufferDescriptor::DX11VertexBufferDescriptor() {
	buffer = nullptr;
	offset = 0;
	stride = 0;
}

DX11VertexBufferDescriptor::~DX11VertexBufferDescriptor() {
	ReleaseCOM( &buffer );
}

void DX11VertexBufferDescriptor::Create( VertexBuffer* const vertexBuffer, const int vertexOffset ) {
	ASSERT_DOWNCAST( vertexBuffer, DX11VertexBuffer );
	buffer = static_cast< DX11VertexBuffer* >( vertexBuffer )->GetBuffer();
	buffer->AddRef();
	stride = static_cast< UINT >( vertexBuffer->GetVertexSize() );
	offset = static_cast< UINT >( vertexOffset ) * stride;
}

ID3D11Buffer* DX11VertexBufferDescriptor::GetBuffer() {
	return buffer;
}

UINT DX11VertexBufferDescriptor::GetOffset() const {
	return offset;
}

UINT DX11VertexBufferDescriptor::GetStride() const {
	return stride;
}

// DX11IndexBufferDescriptor

DX11IndexBufferDescriptor::DX11IndexBufferDescriptor() {
	buffer = nullptr;
	offset = 0;
	format = DXGI_FORMAT_UNKNOWN;
}

DX11IndexBufferDescriptor::~DX11IndexBufferDescriptor() {
	ReleaseCOM( &buffer );
}

void DX11IndexBufferDescriptor::Create( IndexBuffer* const indexBuffer, const int vertexOffset ) {
	ASSERT_DOWNCAST( indexBuffer, DX11IndexBuffer );
	buffer = static_cast< DX11IndexBuffer* >( indexBuffer )->GetBuffer();
	buffer->AddRef();

	IndexBufferFormat indexFormat = indexBuffer->GetFormat();
	if ( indexFormat == IndexBufferFormat::UINT_16 ) {
		offset = vertexOffset * 2;
		format = DXGI_FORMAT_R16_UINT;
	}
	if ( indexFormat == IndexBufferFormat::UINT_32 ) {
		offset = vertexOffset * 4;
		format = DXGI_FORMAT_R32_UINT;
	}
}

ID3D11Buffer* DX11IndexBufferDescriptor::GetBuffer() {
	return buffer;
}

UINT DX11IndexBufferDescriptor::GetOffset() const {
	return offset;
}

DXGI_FORMAT DX11IndexBufferDescriptor::GetDXGIFormat() const {
	return format;
}

// DX11ConstantBuffer

DX11ConstantBuffer::DX11ConstantBuffer() {
	buffer = nullptr;
}

DX11ConstantBuffer::~DX11ConstantBuffer() {
	ReleaseCOM( &buffer );
}

bool DX11ConstantBuffer::Create( ID3D11Device* const device, const ConstantBufferParams& params, const void* const initialData ) {
	// byte width musi byt nasobek 16
	UINT byteWidth = static_cast< UINT >( params.size );
	if ( byteWidth % 16 != 0 ) {
		byteWidth += 16 - byteWidth % 16;
	}

	D3D11_BUFFER_DESC bufferDesc;
	bufferDesc.ByteWidth			= byteWidth;
	bufferDesc.Usage				= GetUsage( params.usage );
	bufferDesc.BindFlags			= D3D11_BIND_CONSTANT_BUFFER;
	bufferDesc.CPUAccessFlags		= GetCPUAccessFlags( params.access );
	bufferDesc.MiscFlags			= 0;
	bufferDesc.StructureByteStride	= 0;

	D3D11_SUBRESOURCE_DATA* pData = NULL;
	D3D11_SUBRESOURCE_DATA data;
	if ( initialData != nullptr ) {
		data.pSysMem = initialData;
		data.SysMemPitch = 0;
		data.SysMemSlicePitch = 0;
		pData = &data;
	}

	ID3D11Buffer* buffer = nullptr;
	HRESULT hresult = device->CreateBuffer( &bufferDesc, pData, &buffer );
	if ( FAILED( hresult ) ) {
		return false;
	}

	this->buffer = buffer;
	return true;
}

ID3D11Buffer* DX11ConstantBuffer::GetBuffer() {
	return buffer;
}

// DX11ConstantBufferDescriptor

DX11ConstantBufferDescriptor::DX11ConstantBufferDescriptor() {
	buffer = nullptr;
	slot = 0;
	constantsCount = 0;
	constantsSize = 0;
}

DX11ConstantBufferDescriptor::~DX11ConstantBufferDescriptor() {
	ReleaseCOM( &buffer );
}

bool DX11ConstantBufferDescriptor::Create( ConstantBuffer* const constantBuffer, const ConstantBufferDescriptorParams &params ) {
	HRESULT hresult = 0;

	// get shader code
	ASSERT_DOWNCAST( params.shader, DX11Shader )
	ID3DBlob* blob = static_cast< DX11Shader* >( params.shader )->GetBlob();
	if ( blob == nullptr ) {
		return false;
	}
	// shader code reflection
	ID3D11ShaderReflection* reflector = nullptr; 
	hresult = D3DReflect(
		blob->GetBufferPointer(),
		blob->GetBufferSize(),
		IID_ID3D11ShaderReflection,
		reinterpret_cast<void** >( &reflector )
	);
	if ( FAILED( hresult ) ) {
		return false;
	}
	// reflect constant buffer
	ID3D11ShaderReflectionConstantBuffer* cbufferReflector = nullptr;
	cbufferReflector = reflector->GetConstantBufferByName( params.bufferObject );

	std::unique_ptr< ConstantPlacement[] > map( new ConstantPlacement[ params.constantsCount ] );
	int mapped = 0;
	int offset = 0;
	bool aligned = true;

	for ( int i = 0; i < params.constantsCount; i++ ) {
		const ShaderConstant& constant = params.constants[ i ];

		// reflect variable
		ID3D11ShaderReflectionVariable* variableReflector = nullptr;
		variableReflector = cbufferReflector->GetVariableByName( constant.name );

		// variable desc
		D3D11_SHADER_VARIABLE_DESC variableDesc;
		hresult = variableReflector->GetDesc( &variableDesc );

		// nezdarilo se volani reflect variable
		if ( FAILED( hresult ) ) {
			break;
		}
		// velikost konstanty musi souhlasit
		if ( variableDesc.Size != static_cast< UINT >( constant.size ) ) {
			break;
		}
		// pokud nesouhlasi offset, nesouhlasi take zarovnani systemove pameti
		if ( offset != static_cast< int >( variableDesc.StartOffset ) ) {
			aligned = false;
		}
		// namapovat konstantu
		map[ i ].size = constant.size;
		map[ i ].sysMemOffset = offset;
		map[ i ].bufferOffset = static_cast< int >( variableDesc.StartOffset );

		// update offset (size + align pad)
		offset += constant.size;
		if ( constant.size % constant.align > 0 ) {
			offset += constant.align - constant.size % constant.align;
		}
		mapped += 1;
	}
	// release reflectors
	reflector->Release();

	// nepodarilo se namapovat vsechny konstanty
	if ( mapped != params.constantsCount ) {
		return false;
	}
	// ulozit vysledky
	ASSERT_DOWNCAST( constantBuffer, DX11ConstantBuffer );
	this->buffer = static_cast< DX11ConstantBuffer* >( constantBuffer )->GetBuffer();
	this->buffer->AddRef();
	this->constantsCount = params.constantsCount;
	this->constantsSize = offset;

	// nesouhlasi zarovnani pameti, ulozit mapu
	if ( !aligned ) {
		this->map = std::move( map );
	}
	return true;
}

void DX11ConstantBufferDescriptor::UpdateConstants( void* const src, void* const dest ) const {
	// zarovnani systemove pameti odpovida zarovnani constant bufferu
	if ( !map ) {
		memcpy( dest, src, constantsSize );
		return;
	}
	// zarovnani pameti nesouhlasi, provest mapovani
	for ( int i = 0; i < constantsCount; i++ ) {
		ConstantPlacement& constant = map[ i ];
		memcpy(
			static_cast< Byte* >( dest ) + constant.bufferOffset,
			static_cast< Byte* >( src ) + constant.sysMemOffset,
			constant.size
		);
	}
}

// DXShader

DX11Shader::DX11Shader() {
	code = nullptr;
	shader = nullptr;
	type = ShaderType::UNDEFINED;
	version = ShaderVersion::UNDEFINED;
}

DX11Shader::~DX11Shader() {
	ReleaseCOM( &shader );
	ReleaseCOM( &code );
}

bool DX11Shader::Compile( ID3D11Device* const device, const ShaderParams& params ) {
	if ( params.version != ShaderVersion::HLSL_50_GLSL_430 ) {
		return false;
	}
	// zjistit velikost pole defines
	int definesCount = 0;
	if ( params.defines != nullptr ) {
		const char* define = params.defines[ 0 ];
		while ( define != nullptr ) {
			definesCount += 1;
			define++;
		}
	}
	// vytvorit pole D3D_SHADER_MACRO
	std::unique_ptr< D3D_SHADER_MACRO[] > macros( new D3D_SHADER_MACRO[ definesCount ] );
	for ( int i = 0; i < definesCount; i++ ) {
		macros[ i ].Name = params.defines[ i ];
		macros[ i ].Definition = "0";
	}
	// shader target
	const char* target = nullptr;
	switch ( params.type ) {
	case ShaderType::VERTEX_SHADER:		target = "vs_5_0"; break;
	case ShaderType::PIXEL_SHADER:		target = "ps_5_0"; break;
	case ShaderType::GEOMETRY_SHADER:	target = "gs_5_0"; break;
	}
	// flags
	UINT flags = 0;
	if ( params.flags & SHADER_COMPILE_WARNINGS_AS_ERRRORS ) {
		flags |= D3DCOMPILE_WARNINGS_ARE_ERRORS;
	}
	if ( params.flags & SHADER_COMPILE_DEBUG ) {
		flags |= D3DCOMPILE_DEBUG;
	}
	// optimization flags
	switch ( params.optimization ) {
	case ShaderOptimization::DISABLED:	flags |= D3DCOMPILE_SKIP_OPTIMIZATION;   break;
	case ShaderOptimization::LOW:		flags |= D3DCOMPILE_OPTIMIZATION_LEVEL0; break;
	case ShaderOptimization::MEDIUM:	flags |= D3DCOMPILE_OPTIMIZATION_LEVEL3; break;
	case ShaderOptimization::HIGH:		flags |= D3DCOMPILE_OPTIMIZATION_LEVEL2; break;
	}
	// compile
	ID3DBlob* code = nullptr;
	HRESULT hresult = D3DCompile(
		params.string,
		strlen( params.string ),
		NULL,
		macros.get(),
		NULL,
		"main",
		target,
		flags,
		0,
		&code,
		NULL
	);
	if ( FAILED( hresult ) ) {
		return false;
	}
	// shader object
	ID3D11DeviceChild* shader = nullptr;
	if ( params.type == ShaderType::VERTEX_SHADER ) {
		ID3D11VertexShader* vs = nullptr;
		device->CreateVertexShader( code->GetBufferPointer(), code->GetBufferSize(), NULL, &vs );
		shader = vs;

	} else if ( params.type == ShaderType::PIXEL_SHADER ) {
		ID3D11PixelShader* ps = nullptr;
		device->CreatePixelShader( code->GetBufferPointer(), code->GetBufferSize(), NULL, &ps );
		shader = ps;

	} else if ( params.type == ShaderType::GEOMETRY_SHADER ) {
		ID3D11GeometryShader* gs = nullptr;
		device->CreateGeometryShader( code->GetBufferPointer(), code->GetBufferSize(), NULL, &gs );
		shader = gs;
	}
	if ( shader == nullptr ) {
		ReleaseCOM( &code );
		return false;
	}
	// ulozit vysledek
	this->code = code;
	this->shader = shader;
	this->type = params.type;
	this->version = params.version;

	return true;
}

ShaderType DX11Shader::GetType() const {
	return type;
}

ShaderVersion DX11Shader::GetVersion() const {
	return version;
}

ID3DBlob* DX11Shader::GetBlob() {
	return code;
}

ID3D11DeviceChild* DX11Shader::GetShader() {
	return shader;
}