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
	case Format::R16_UINT:						return DXGI_FORMAT_R16_UINT;
	case Format::R32_FLOAT:						return DXGI_FORMAT_R32_FLOAT;
	case Format::R32_UINT:						return DXGI_FORMAT_R32_UINT;
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
	case TextureAddressing::WRAP:	return D3D11_TEXTURE_ADDRESS_WRAP;
	case TextureAddressing::MIRROR:	return D3D11_TEXTURE_ADDRESS_MIRROR;
	case TextureAddressing::CLAMP:	return D3D11_TEXTURE_ADDRESS_CLAMP;
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

D3D11_USAGE GetDXUsage( const BufferUsage usage ) {
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

BackBuffer* DX11Device::CreateBackBuffer( Window& window ) {
	DX11BackBuffer* backBuffer = new DX11BackBuffer();
	if ( !backBuffer->Create( device, dxgiFactory, window ) ) {
		delete backBuffer;
		return nullptr;
	}
	return backBuffer;
}

Buffer* DX11Device::CreateTextureBuffer( const TextureBufferParams& params, const void* const initialData[] ) {
	DX11TextureBuffer* buffer = new DX11TextureBuffer();
	if ( !buffer->Create( device, params, initialData ) ) {
		delete buffer;
		return nullptr;
	}
	return buffer;
}

// pomocna funkce pro vytvareni vertex, index a constant bufferu
Buffer* CreateGenericBuffer(
	ID3D11Device* const device,
	const BufferType type,
	const int byteWidth,
	const BufferUsage usage,
	const BufferAccess access,
	const void* const initialData
) {
	DX11GenericBuffer* buffer = new DX11GenericBuffer();
	if ( !buffer->Create( device, type, byteWidth, usage, access, initialData ) ) {
		delete buffer;
		return nullptr;
	}
	return buffer;
}

Buffer* DX11Device::CreateVertexBuffer( const int byteWidth, const BufferUsage usage, const BufferAccess access, const void* const initialData  ) {
	return CreateGenericBuffer( device, BufferType::VERTEX_BUFFER, byteWidth, usage, access, initialData );
}

Buffer* DX11Device::CreateIndexBuffer( const int byteWidth, const BufferUsage usage, const BufferAccess access, const void* const initialData  ) {
	return CreateGenericBuffer( device, BufferType::INDEX_BUFFER, byteWidth, usage, access, initialData );
}

Buffer* DX11Device::CreateConstantBuffer( const int byteWidth, const BufferUsage usage, const BufferAccess access, const void* const initialData ) {
	return CreateGenericBuffer( device, BufferType::CONSTANT_BUFFER, byteWidth, usage, access, initialData );
}

RenderTargetDescriptor* DX11Device::CreateRenderTargetDescriptor( BackBuffer* const backBuffer ) {
	DX11RenderTargetDescriptor* descriptor = new DX11RenderTargetDescriptor();
	if ( !descriptor->Create( device, backBuffer ) ) {
		delete descriptor;
		return nullptr;
	}
	return descriptor;
}

RenderTargetDescriptor* DX11Device::CreateRenderTargetDescriptor( Buffer* const textureBuffer ) {
	DX11RenderTargetDescriptor* descriptor = new DX11RenderTargetDescriptor();
	if ( !descriptor->Create( device, textureBuffer ) ) {
		delete descriptor;
		return nullptr;
	}
	return descriptor;
}

TextureDescriptor* DX11Device::CreateTextureDescriptor( Buffer* const textureBuffer ) {
	DX11TextureDescriptor* descriptor = new DX11TextureDescriptor();
	if ( !descriptor->Create( device, textureBuffer ) ) {
		delete descriptor;
		return nullptr;
	}
	return descriptor;
}

DepthStencilDescriptor* DX11Device::CreateDepthStencilDescriptor( Buffer* const textureBuffer, const DepthStencilDescriptorParams& params ) {
	DX11DepthStencilDescriptor* descriptor = new DX11DepthStencilDescriptor();
	if ( !descriptor->Create( device, textureBuffer, params ) ) {
		delete descriptor;
		return nullptr;
	}
	return descriptor;
}

VertexBufferDescriptor*	DX11Device::CreateVertexBufferDescriptor( Buffer* const vertexBuffer, const VertexBufferDescriptorParams& params ) {
	DX11VertexBufferDescriptor* descriptor = new DX11VertexBufferDescriptor();
	if ( !descriptor->Create( vertexBuffer, params ) ) {
		delete descriptor;
		return nullptr;
	}
	return descriptor;
}

IndexBufferDescriptor* DX11Device::CreateIndexBufferDescriptor( Buffer* const indexBuffer, const IndexBufferDescriptorParams& params ) {
	DX11IndexBufferDescriptor* descriptor = new DX11IndexBufferDescriptor();
	if ( !descriptor->Create( indexBuffer, params ) ) {
		delete descriptor;
		return nullptr;
	}
	return descriptor;
}

ConstantBufferDescriptor* DX11Device::CreateConstantBufferDescriptor( Buffer* const constantBuffer, const ConstantBufferDescriptorParams& params ) {
	DX11ConstantBufferDescriptor* descriptor = new DX11ConstantBufferDescriptor();
	if ( !descriptor->Create( constantBuffer, params ) ) {
		delete descriptor;
		return nullptr;
	}
	return descriptor;
}

CommandInterface* DX11Device::CreateCommandInterface() {
	DX11CommandInterface* commandInterface = new DX11CommandInterface();
	if ( !commandInterface->Create() ) {
		delete commandInterface;
		return nullptr;
	}
	return commandInterface;
}

Display* DX11Device::CreateDisplay( const int outputId ) {
	DX11Display* display = new DX11Display();
	if ( !display->Create( device, dxgiAdapter, outputId ) ) {
		delete display;
		return nullptr;
	}
	return display;
}

Shader* DX11Device::CreateShader( const ShaderParams& params ) {
	DX11Shader *shader = new DX11Shader();
	if ( !shader->Compile( device, params ) ) {
		delete shader;
		return nullptr;
	}
	return shader;
}

Sampler* DX11Device::CreateSampler( const SamplerParams& params ) {
	DX11Sampler* sampler = new DX11Sampler();
	if ( !sampler->Create( device, params ) ) {
		delete sampler;
		return nullptr;
	}
	return sampler;
}

int DX11Device::GetMultisampleQuality( const int samplesCount) const {
	int levels = 0;
	device->CheckMultisampleQualityLevels( BACK_BUFFER_FORMAT, samplesCount, reinterpret_cast< unsigned int* >( &levels ) );
	return levels;
}

ID3D11DeviceContext* DX11Device::GetContext() {
	return context;
}

ID3D11Device* DX11Device::GetDevice() {
	return device;
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
	ZeroMemory( &bufferInfo, sizeof( bufferInfo ) );
}

DX11Buffer::~DX11Buffer() {
	ReleaseCOM( &resource );
}

void DX11Buffer::SetBuffer( ID3D11Resource* const resource, const BufferInfo& bufferInfo ) {
	this->resource = resource;
	this->resource->AddRef();
	this->bufferInfo = bufferInfo;
}

void DX11Buffer::GetInfo( BufferInfo& result ) const {
	result = bufferInfo;
}

BufferType DX11Buffer::GetType() const {
	return bufferInfo.type;
}

int DX11Buffer::GetByteWidth() const {
	return bufferInfo.byteWidth;
}

BufferUsage DX11Buffer::GetUsage() const {
	return bufferInfo.usage;
}

BufferAccess DX11Buffer::GetAccess() const {
	return bufferInfo.access;
}

ID3D11Resource* DX11Buffer::GetResource() {
	return resource;
}

// DX11TextureBuffer

DX11TextureBuffer::DX11TextureBuffer() {
	format = Format::UNKNOWN;
	width = 0;
	height = 0;
	depth = 0;
	mipLevels = 0;
	arraySize = 0;
	samplesCount = 0;
	samplesQuality = 0;
}

bool DX11TextureBuffer::Create( ID3D11Device* const device, const TextureBufferParams& params, const void* const initialData[] ) {
	const D3D11_USAGE usage = GetDXUsage( params.usage );
	const UINT CPUAccessFlags = GetCPUAccessFlags( params.access );

	UINT bindFlags = D3D11_BIND_SHADER_RESOURCE;
	// set render target bind flag
	if ( params.flags & TextureBufferFlags::RENDER_TARGET ) {
		bindFlags |= D3D11_BIND_RENDER_TARGET;
	}
	// set depth stencil bind flag
	if ( params.format == Format::DEPTH_24_UNORM_STENCIL_8_UINT ) {
		bindFlags |= D3D11_BIND_DEPTH_STENCIL;
	}
	// subresources temporary
	std::unique_ptr< D3D11_SUBRESOURCE_DATA[] > subresources( nullptr );
	
	// initialize subresources temporary
	if ( initialData != nullptr ) {
		subresources.reset( new D3D11_SUBRESOURCE_DATA[ params.arraySize * params.mipLevels ] );
		const FormatInfo formatInfo = GetFormatInfo( params.format );
		int subresourceIndex = 0;

		// for each texture in texture array
		for ( int arrayIndex = 0; arrayIndex < params.arraySize; arrayIndex++ ) {
			int mipWidth = params.width;
			int mipHeight = params.height;

			// for each mip
			for ( int mipIndex = 0; mipIndex < params.mipLevels; mipIndex++ ) {
				// set subresource
				const int rowPitch = ( formatInfo.blockByteWidth / formatInfo.blockSize ) * mipWidth;
				subresources[ subresourceIndex ].pSysMem = initialData[ subresourceIndex ];
				subresources[ subresourceIndex ].SysMemPitch = rowPitch;
				// pouze pro 3D textury, mipHeight neni nutne delit blockSize, protoze 3D textury nepodporuji blokovou kompresi
				subresources[ subresourceIndex ].SysMemSlicePitch = rowPitch * mipHeight;

				// vypocitat rozmery nasledujici mip urovne
				if ( mipWidth > formatInfo.blockSize ) {
					mipWidth /= 2;
				}
				if ( mipHeight > formatInfo.blockSize ) {
					mipHeight /= 2;
				}
				// dalsi subresource
				subresourceIndex += 1;
			}
		}
	}

	// 1D texture
	if ( params.type == BufferType::TEXTURE_1D ||
		 params.type == BufferType::TEXTURE_1D_ARRAY
	) {
		D3D11_TEXTURE1D_DESC textureDesc;
		textureDesc.Width 				= params.width;
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
		SetTextureBuffer( texture, params );
		return true;
	}

	// 2D texture
	if ( params.type == BufferType::TEXTURE_2D ||
		 params.type == BufferType::TEXTURE_2D_ARRAY  ||
		 params.type == BufferType::TEXTURE_2D_MS ||
		 params.type == BufferType::TEXTURE_2D_MS_ARRAY
	) {
		D3D11_TEXTURE2D_DESC textureDesc;
		textureDesc.Width 				= params.width;
		textureDesc.Height				= params.height;
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
		SetTextureBuffer( texture, params );
		return true;
	}

	// 3D texture
	if ( params.type == BufferType::TEXTURE_3D ) {
		D3D11_TEXTURE3D_DESC textureDesc;
		textureDesc.Width 				= params.width;
		textureDesc.Height				= params.height;
		textureDesc.Depth				= params.depth;
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
		SetTextureBuffer( texture, params );
		return true;
	}

	// nepodporovany typ bufferu
	return false;
}

void DX11TextureBuffer::SetTextureBuffer( ID3D11Resource* const resource, const TextureBufferParams& params ) {
	// Ulozit atributy bufferu (validace rozmeru textury, hodnota nesmi byt < 1)
	format = params.format;
	width = Math::Max( params.width, 1 );
	height = Math::Max( params.height, 1 );
	depth = Math::Max( params.depth, 1 );
	mipLevels = params.mipLevels;
	arraySize = params.arraySize;
	samplesCount = params.samplesCount;
	samplesQuality = params.samplesQuality;

	// Vypocitat velikost bufferu (rozhrani ID3DTextureXD neposkytuje informaci o velikosti alokovane pameti pro textury)
	
	const FormatInfo formatInfo = GetFormatInfo( format );
	int mipWidth = width;
	int mipHeight = height;
	int mipDepth = depth;
	int mipSliceSize = 0;

	for ( int i = 0; i < mipLevels; i++ ) {
		// mipDepth se nedeli velikosti bloku, protoze 3D textury nepodporuji blokovou kompresi
		mipSliceSize += ( mipWidth / formatInfo.blockSize ) * ( mipHeight / formatInfo.blockSize ) * mipDepth * formatInfo.blockByteWidth;

		// update mip dimmensions
		if ( mipWidth > formatInfo.blockSize ) {
			mipWidth /= 2;
		}
		if ( mipHeight > formatInfo.blockSize ) {
			mipHeight /= 2;
		}
		if ( mipDepth > 1 ) {
			mipDepth /= 2;
		}
	}
	// ulozit informace o bufferu
	BufferInfo bufferInfo;
	bufferInfo.type = params.type;
	bufferInfo.byteWidth = mipSliceSize * arraySize;
	bufferInfo.usage = params.usage;
	bufferInfo.access = params.access;
	SetBuffer( resource, bufferInfo );
}

Format DX11TextureBuffer::GetFormat() const {
	return format;
}

int DX11TextureBuffer::GetWidth() const {
	return width;
}

int DX11TextureBuffer::GetHeight() const {
	return height;
}

int DX11TextureBuffer::GetDepth() const {
	return depth;
}

int DX11TextureBuffer::GetMipLevels() const {
	return mipLevels;
}

int DX11TextureBuffer::GetArraySize() const {
	return arraySize;
}

int DX11TextureBuffer::GetSamplesCount() const {
	return samplesCount;
}

int DX11TextureBuffer::GetSamplesQuality() const {
	return samplesQuality;
}

// DX11GenericBuffer

bool DX11GenericBuffer::Create(
	ID3D11Device* const device,
	const BufferType type,
	const int byteWidth,
	const BufferUsage usage,
	const BufferAccess access,
	const void* const initialData
) {
	D3D11_BUFFER_DESC bufferDesc;
	bufferDesc.ByteWidth			= static_cast< UINT >( byteWidth );
	bufferDesc.Usage				= GetDXUsage( usage );
	bufferDesc.BindFlags			= 0;
	bufferDesc.CPUAccessFlags		= GetCPUAccessFlags( access );
	bufferDesc.MiscFlags			= 0;
	bufferDesc.StructureByteStride	= 0;

	// bind flags
	switch ( type ) {
	case BufferType::VERTEX_BUFFER:		bufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;	break;
	case BufferType::INDEX_BUFFER:		bufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;		break;
	case BufferType::CONSTANT_BUFFER:	bufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;	break;
	}
	// initial subresource data 
	D3D11_SUBRESOURCE_DATA* pData = NULL;
	D3D11_SUBRESOURCE_DATA data;
	if ( initialData != nullptr ) {
		data.pSysMem = initialData;
		data.SysMemPitch = 0;
		data.SysMemSlicePitch = 0;
		pData = &data;
	}
	// create buffer
	ID3D11Buffer* buffer = nullptr;
	HRESULT hresult = device->CreateBuffer( &bufferDesc, pData, &buffer );
	if ( FAILED( hresult ) ) {
		return false;
	}
	// store info about buffer
	BufferInfo bufferInfo;
	bufferInfo.type = type;
	bufferInfo.byteWidth = byteWidth;
	bufferInfo.usage = usage;
	bufferInfo.access = access;
	SetBuffer( buffer, bufferInfo );
	return true;
}

// DX11RenderTargetDescriptor

DX11RenderTargetDescriptor::DX11RenderTargetDescriptor() {
	view = nullptr;
}

DX11RenderTargetDescriptor::~DX11RenderTargetDescriptor() {
	ReleaseCOM( &view );
}

bool DX11RenderTargetDescriptor::Create( ID3D11Device* const device, BackBuffer* const backBuffer ) {
	HRESULT hresult = 0;

	// get back buffer texture
	ASSERT_DOWNCAST( backBuffer, DX11BackBuffer );
	IDXGISwapChain* swapChain = static_cast< DX11BackBuffer* >( backBuffer )->GetSwapChain();
	ID3D11Texture2D* texture = nullptr;
	hresult = swapChain->GetBuffer( 0, __uuidof( ID3D11Texture2D ), reinterpret_cast< LPVOID* >( &texture ) );
	if ( FAILED( hresult ) ) {
		return false;
	}
	// create RTV
	ID3D11RenderTargetView* view = nullptr;
	hresult = device->CreateRenderTargetView( texture, NULL, &view );
	texture->Release();
	if ( FAILED( hresult ) ) {
		return false;
	}
	this->view = view;
	return true;
}

bool DX11RenderTargetDescriptor::Create( ID3D11Device* const device, Buffer* const textureBuffer ) {
	ASSERT_DOWNCAST( textureBuffer, DX11TextureBuffer );
	ID3D11Resource* resource = static_cast< DX11TextureBuffer* >( textureBuffer )->GetResource();
	ID3D11RenderTargetView* view = nullptr;
	HRESULT hresult = device->CreateRenderTargetView ( resource, NULL, &view );
	if ( FAILED( hresult ) ) {
		return false;
	}
	this->view = view;
	return true;
}

ID3D11RenderTargetView* DX11RenderTargetDescriptor::GetView() {
	return view;
}

// DX11TextureDescriptor

DX11TextureDescriptor::DX11TextureDescriptor() {
	view = nullptr;
}
DX11TextureDescriptor::~DX11TextureDescriptor() {
	ReleaseCOM( &view );
}

bool DX11TextureDescriptor::Create( ID3D11Device* const device, Buffer* const textureBuffer ) {
	ASSERT_DOWNCAST( textureBuffer, DX11TextureBuffer );
	ID3D11Resource* resource = static_cast< DX11TextureBuffer* >( textureBuffer )->GetResource();
	ID3D11ShaderResourceView* view = nullptr;
	HRESULT hresult = device->CreateShaderResourceView( resource, NULL, &view );
	if ( FAILED( hresult ) ) {
		return false;
	}
	this->view = view;
	return true;
}

ID3D11ShaderResourceView* DX11TextureDescriptor::GetView() {
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

bool DX11DepthStencilDescriptor::Create( ID3D11Device* const device, Buffer* const textureBuffer, const DepthStencilDescriptorParams& params ) {
	const BufferType bufferType = textureBuffer->GetType();

	// konrola typu bufferu
	if ( bufferType != BufferType::TEXTURE_2D &&
		 bufferType != BufferType::TEXTURE_2D_MS
	) {
		return false;
	}

	ASSERT_DOWNCAST( textureBuffer, DX11TextureBuffer );

	// kontrola formatu, musi byt DEPTH_24_UNORM_STENCIL_8_UINT 
	const Format format = static_cast< DX11TextureBuffer* >( textureBuffer )->GetFormat();
	if ( format != Format::DEPTH_24_UNORM_STENCIL_8_UINT ) {
		return false;
	}

	ID3D11Resource* texture = static_cast< DX11TextureBuffer* >( textureBuffer )->GetResource();
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
	// set view dimension
	if ( bufferType == BufferType::TEXTURE_2D ) {
		viewDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
		viewDesc.Texture2D.MipSlice = 0;

	} else if ( bufferType == BufferType::TEXTURE_2D_MS ) {
		viewDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2DMS;
	} 
	// create view
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

// DX11VertexBufferDescriptor

DX11VertexBufferDescriptor::DX11VertexBufferDescriptor() {
	resource = nullptr;
	offset = 0;
	stride = 0;
	verticesCount = 0;
}

DX11VertexBufferDescriptor::~DX11VertexBufferDescriptor() {
	ReleaseCOM( &resource );
}

bool DX11VertexBufferDescriptor::Create( Buffer* const vertexBuffer, const VertexBufferDescriptorParams& params ) {
	if ( vertexBuffer->GetType() != BufferType::VERTEX_BUFFER ) {
		return false;
	}
	ASSERT_DOWNCAST( vertexBuffer, DX11Buffer );
	resource = static_cast< DX11Buffer* >( vertexBuffer )->GetResource();
	resource->AddRef();
	stride = static_cast< UINT >( params.vertexByteWidth );
	offset = static_cast< UINT >( params.verticesOffset ) * stride;
	verticesCount = params.verticesCount;
	return true;
}

ID3D11Resource* DX11VertexBufferDescriptor::GetResource() {
	return resource;
}

UINT DX11VertexBufferDescriptor::GetOffset() const {
	return offset;
}

UINT DX11VertexBufferDescriptor::GetStride() const {
	return stride;
}

// DX11IndexBufferDescriptor

DX11IndexBufferDescriptor::DX11IndexBufferDescriptor() {
	resource = nullptr;
	offset = 0;
	indicesCount = 0;
	format = DXGI_FORMAT_UNKNOWN;
}

DX11IndexBufferDescriptor::~DX11IndexBufferDescriptor() {
	ReleaseCOM( &resource );
}

bool DX11IndexBufferDescriptor::Create( Buffer* const indexBuffer, const IndexBufferDescriptorParams& params ) {
	if ( indexBuffer->GetType() != BufferType::INDEX_BUFFER ) {
		return false;
	}
	if ( params.format == Format::R16_UINT ) {
		offset = params.indicesOffset * 2;
		format = DXGI_FORMAT_R16_UINT;

	} else if ( params.format == Format::R16_UINT ) {
		offset = params.indicesOffset * 4;
		format = DXGI_FORMAT_R32_UINT;

	} else {
		return false;
	}
	ASSERT_DOWNCAST( indexBuffer, DX11Buffer );
	resource = static_cast< DX11Buffer* >( indexBuffer )->GetResource();
	resource->AddRef();
	indicesCount = params.indicesCount;
	return true;
}

ID3D11Resource* DX11IndexBufferDescriptor::GetResource() {
	return resource;
}

UINT DX11IndexBufferDescriptor::GetOffset() const {
	return offset;
}

DXGI_FORMAT DX11IndexBufferDescriptor::GetDXGIFormat() const {
	return format;
}

// DX11ConstantBufferDescriptor

DX11ConstantBufferDescriptor::DX11ConstantBufferDescriptor() {
	constantsCount = 0;
	constantsSize = 0;
	slot = 0;
	buffer = nullptr;
}

DX11ConstantBufferDescriptor::~DX11ConstantBufferDescriptor() {
	ReleaseCOM( &buffer );
}

bool DX11ConstantBufferDescriptor::Create( Buffer* const constantBuffer, const ConstantBufferDescriptorParams &params ) {
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
	ASSERT_DOWNCAST( constantBuffer, DX11Buffer );
	ID3D11Resource* resource = static_cast< DX11Buffer* >( constantBuffer )->GetResource();
	ASSERT_DOWNCAST( resource, ID3D11Buffer );
	this->buffer = static_cast< ID3D11Buffer* >( resource );
	this->buffer->AddRef();
	this->constantsCount = params.constantsCount;
	this->constantsSize = offset;
	this->slot = params.slot;

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
	// create shader object
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

// DX11Sampler

DX11Sampler::DX11Sampler() {
	sampler = nullptr;
}

DX11Sampler::~DX11Sampler() {
	ReleaseCOM( &sampler );
}

bool DX11Sampler::Create( ID3D11Device* const device, const SamplerParams& params ) {
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

ID3D11SamplerState* DX11Sampler::GetSampler() {
	return sampler;
}