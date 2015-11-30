#include <d3dcompiler.h>
#include "DX11RenderInterface.h"
#include "..\..\Platform\Application.h"
#include "..\..\Platform\Windows\WindowsWindow.h"
#include "..\..\Framework\Math.h"
#include "..\..\Framework\Debug.h"

const DXGI_FORMAT BACK_BUFFER_FORMAT = DXGI_FORMAT_R8G8B8A8_UNORM;

/*
Smart pointer (Pouze pro interni pouziti).
Slouzi ke sprave ukazatelu na COM interface.
*/
template <typename T>
class ComPtr {
public:
	ComPtr() {
		ptr = nullptr;
	}
	
	~ComPtr() {
		Release();
	}
	
	explicit ComPtr( T* const ptr ) {
		this->ptr = ptr;
		if ( ptr != nullptr ) {
			ptr->AddRef();
		}
	}
	
	ComPtr( const ComPtr& comPtr ) {
		ptr = comPtr.GetRef();
	}
	
	ComPtr& operator=( const ComPtr& comPtr ) {
		if ( this == &comPtr ) {
			return *this;
		}
		Release();
		ptr = comPtr.ptr;
		if ( ptr != nullptr ) {
			ptr->AddRef();
		}
		return *this;
	}

	T* operator->() {
		return ptr;
	}
	
	T** operator&() {
		Release();
		return &ptr;
	}

	void Release() {
		if ( ptr != nullptr ) {
			ptr->Release();
			ptr = nullptr;
		}
	}
	
	T* GetRef() {
		if ( ptr != nullptr ) {
			ptr->AddRef();
		}
		return ptr;
	}

	T* Get() {
		return ptr;
	}

private:
	T* ptr;
};

// RenderInterface to DX11 enum wrappers

DXGI_FORMAT GetDXGIFormat(const Format format) {
	switch ( format ) {
	case Format::UNKNOWN:						return DXGI_FORMAT_UNKNOWN;
	case Format::R32G32B32A32_FLOAT:			return DXGI_FORMAT_R32G32B32A32_FLOAT;
	case Format::R32G32B32A32_UINT:				return DXGI_FORMAT_R32G32B32A32_UINT;
	case Format::R32G32B32_FLOAT:				return DXGI_FORMAT_R32G32B32_FLOAT;
	case Format::R32G32B32_UINT:				return DXGI_FORMAT_R32G32B32_UINT;
	case Format::R32G32_FLOAT:					return DXGI_FORMAT_R32G32_FLOAT;
	case Format::R32G32_UINT:					return DXGI_FORMAT_R32G32_UINT;
	case Format::R32_FLOAT:						return DXGI_FORMAT_R32_FLOAT;
	case Format::R32_UINT:						return DXGI_FORMAT_R32_UINT;
	case Format::R16G16B16A16_FLOAT:			return DXGI_FORMAT_R16G16B16A16_FLOAT;
	case Format::R16G16B16A16_UINT:				return DXGI_FORMAT_R16G16B16A16_UINT;
	case Format::R16G16B16A16_UNORM:			return DXGI_FORMAT_R16G16B16A16_UNORM;
	case Format::R16G16B16A16_SINT:				return DXGI_FORMAT_R16G16B16A16_SINT;
	case Format::R16G16B16A16_SNORM:			return DXGI_FORMAT_R16G16B16A16_SNORM;
	case Format::R16G16_FLOAT:					return DXGI_FORMAT_R16G16_FLOAT;
	case Format::R16G16_UINT:					return DXGI_FORMAT_R16G16_UINT;
	case Format::R16G16_UNORM:					return DXGI_FORMAT_R16G16_UNORM;
	case Format::R16G16_SINT:					return DXGI_FORMAT_R16G16_SINT;
	case Format::R16G16_SNORM:					return DXGI_FORMAT_R16G16_SNORM;
	case Format::R16_FLOAT:						return DXGI_FORMAT_R16_FLOAT;
	case Format::R16_UINT:						return DXGI_FORMAT_R16_UINT;
	case Format::R16_UNORM:						return DXGI_FORMAT_R16_UNORM;
	case Format::R16_SINT:						return DXGI_FORMAT_R16_SINT;
	case Format::R16_SNORM:						return DXGI_FORMAT_R16_SNORM;
	case Format::R8G8B8A8_UINT:					return DXGI_FORMAT_R8G8B8A8_UINT;
	case Format::R8G8B8A8_UNORM:				return DXGI_FORMAT_R8G8B8A8_UNORM;
	case Format::R8G8B8A8_SINT:					return DXGI_FORMAT_R8G8B8A8_SINT;
	case Format::R8G8B8A8_SNORM:				return DXGI_FORMAT_R8G8B8A8_SNORM;
	case Format::R8G8_UINT:						return DXGI_FORMAT_R8G8_UINT;
	case Format::R8G8_UNORM:					return DXGI_FORMAT_R8G8_UNORM;
	case Format::R8G8_SINT:						return DXGI_FORMAT_R8G8_SINT;
	case Format::R8G8_SNORM:					return DXGI_FORMAT_R8G8_SNORM;
	case Format::R8_UINT:						return DXGI_FORMAT_R8_UINT;
	case Format::R8_UNORM:						return DXGI_FORMAT_R8_UNORM;
	case Format::R8_SINT:						return DXGI_FORMAT_R8_SINT;
	case Format::R8_SNORM:						return DXGI_FORMAT_R8_SNORM;
	case Format::DEPTH_24_UNORM_STENCIL_8_UINT:	return DXGI_FORMAT_D24_UNORM_S8_UINT;
	case Format::BC1:							return DXGI_FORMAT_BC1_UNORM;
	case Format::BC3:							return DXGI_FORMAT_BC3_UNORM;
	}
	return DXGI_FORMAT_UNKNOWN;
}

D3D11_COMPARISON_FUNC GetD3D11ComparsionFunc( const DepthStencilComparsion dsc ) {
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

D3D11_STENCIL_OP GetD3D11StencilOp( const StencilOperation op ) {
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

D3D11_TEXTURE_ADDRESS_MODE GetD3D11TextureAddressMode( const TextureAddressing addressing ) {
	switch ( addressing ) {
	case TextureAddressing::WRAP:		return D3D11_TEXTURE_ADDRESS_WRAP;
	case TextureAddressing::MIRROR:		return D3D11_TEXTURE_ADDRESS_MIRROR;
	case TextureAddressing::CLAMP:		return D3D11_TEXTURE_ADDRESS_CLAMP;
	}
	return D3D11_TEXTURE_ADDRESS_WRAP;
}

D3D11_FILTER GetD3D11Filter( const TextureFilter filter ) {
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

D3D11_USAGE GetD3D11Usage( const BufferUsage usage ) {
	switch ( usage ) {
	case BufferUsage::DRAW:			return D3D11_USAGE_DEFAULT;
	case BufferUsage::STATIC:		return D3D11_USAGE_IMMUTABLE;
	case BufferUsage::DYNAMIC:		return D3D11_USAGE_DYNAMIC;
	case BufferUsage::COPY:			return D3D11_USAGE_STAGING;
	}
	return D3D11_USAGE_STAGING;
}

D3D11_BLEND GetD3D11Blend( const Blend blend ) {
	switch ( blend ) {
	case Blend::ZERO:				return D3D11_BLEND_ZERO;
	case Blend::ONE:				return D3D11_BLEND_ONE;
	case Blend::SRC_COLOR:			return D3D11_BLEND_SRC_COLOR;
	case Blend::INV_SRC_COLOR:		return D3D11_BLEND_INV_SRC_COLOR;
	case Blend::SRC_ALPHA:			return D3D11_BLEND_SRC_ALPHA;
	case Blend::INV_SRC_ALPHA:		return D3D11_BLEND_INV_SRC_ALPHA;
	case Blend::DEST_ALPHA:			return D3D11_BLEND_DEST_ALPHA;
	case Blend::INV_DEST_ALPHA:		return D3D11_BLEND_INV_DEST_ALPHA;
	case Blend::DEST_COLOR:			return D3D11_BLEND_DEST_COLOR;
	case Blend::INV_DEST_COLOR:		return D3D11_BLEND_INV_DEST_COLOR;
	case Blend::SRC_ALPHA_SAT:		return D3D11_BLEND_SRC_ALPHA_SAT;
	case Blend::BLEND_FACTOR:		return D3D11_BLEND_BLEND_FACTOR;
	case Blend::INV_BLEND_FACTOR:	return D3D11_BLEND_INV_BLEND_FACTOR;
	case Blend::SRC1_COLOR:			return D3D11_BLEND_SRC1_COLOR;
	case Blend::INV_SRC1_COLOR:		return D3D11_BLEND_INV_SRC1_COLOR;
	case Blend::SRC1_ALPHA:			return D3D11_BLEND_SRC1_ALPHA;
	case Blend::INV_SRC1_ALPHA:		return D3D11_BLEND_INV_SRC1_ALPHA;
	}
	return D3D11_BLEND_ZERO;
}

D3D11_BLEND_OP GetD3D11BlendOp( const BlendOp op ) {
	switch ( op ) {
	case BlendOp::ADD:				return D3D11_BLEND_OP_ADD;
	case BlendOp::SUBTRACT:			return D3D11_BLEND_OP_SUBTRACT;
	case BlendOp::REV_SUBTRACT:		return D3D11_BLEND_OP_REV_SUBTRACT;
	case BlendOp::MIN:				return D3D11_BLEND_OP_MIN;
	case BlendOp::MAX:				return D3D11_BLEND_OP_MAX;
	}
	return D3D11_BLEND_OP_ADD;
}

D3D11_PRIMITIVE_TOPOLOGY GetD3D11PrimitiveTopology( const PrimitiveTopology topology ) {
	switch ( topology ) {
	case PrimitiveTopology::DEFAULT:		return D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
	case PrimitiveTopology::POINTLIST:		return D3D11_PRIMITIVE_TOPOLOGY_POINTLIST;
	case PrimitiveTopology::LINELIST:		return D3D11_PRIMITIVE_TOPOLOGY_LINELIST;
	case PrimitiveTopology::LINESTRIP:		return D3D11_PRIMITIVE_TOPOLOGY_LINESTRIP;
	case PrimitiveTopology::TRIANGLELIST:	return D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
	case PrimitiveTopology::TRIANGLESTRIP:	return D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP;
	}
	return D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
}

UINT GetD3D11CPUAccess( const BufferAccess access ) {
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
	ReleaseCom( &dxgiAdapter );
	ReleaseCom( &dxgiFactory );
	ReleaseCom( &context );
	ReleaseCom( &device );
}

bool DX11Device::Create( const DX11CreateDeviceParams& params ) {
	HRESULT hresult = 0;

	// vytvorit DXGIFactory1
	ComPtr< IDXGIFactory1 > factory;
	hresult = CreateDXGIFactory1( __uuidof( IDXGIFactory1 ), reinterpret_cast< void** >( &factory ) );
	if ( FAILED( hresult ) ) {
		return false;
	}
	// pokusit se ziskat pozadovany adapter
	ComPtr< IDXGIAdapter > adapter;
	hresult = factory->EnumAdapters( static_cast< UINT >( params.adapter ), &adapter );

	// selhani, zkusit ziskat vychozi adapter
	if ( FAILED( hresult ) ) {
		hresult = factory->EnumAdapters( 0, &adapter );
	}
	// nebyl nalezen zadny adapter
	if ( FAILED( hresult ) ) {
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
		adapter.Get(),
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
		return false;
	}
	this->dxgiFactory = factory.GetRef();
	this->dxgiAdapter = adapter.GetRef();
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

TextureDescriptor* DX11Device::CreateTextureDescriptor( Buffer* const textureBuffer, Sampler* const sampler ) {
	DX11TextureDescriptor* descriptor = new DX11TextureDescriptor();
	if ( !descriptor->Create( device, textureBuffer ) ) {
		delete descriptor;
		return nullptr;
	}
	return descriptor;
}

DepthStencilBufferDescriptor* DX11Device::CreateDepthStencilBufferDescriptor( Buffer* const textureBuffer ) {
	DX11DepthStencilBufferDescriptor* descriptor = new DX11DepthStencilBufferDescriptor();
	if ( !descriptor->Create( device, textureBuffer ) ) {
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

VertexDescriptor* DX11Device::CreateVertexDescriptor( const VertexDescriptorParams& params ) {
	DX11VertexDescriptor* descriptor = new DX11VertexDescriptor();
	if ( !descriptor->Create( params ) ) {
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

RenderProgram* DX11Device::CreateRenderProgram( Shader* const vs, Shader* const ps, Shader* const gs ) {
	DX11RenderProgram* program = new DX11RenderProgram();
	if ( !program->Create( vs, ps, gs ) ) {
		delete program;
		return nullptr;
	}
	return program;
}

BlendState* DX11Device::CreateBlendState( const BlendStateParams& params ) {
	DX11BlendState* state = new DX11BlendState();
	if ( !state->Create( device, params ) ) {
		delete state;
		return nullptr;
	}
	return state;
}

RasterizerState* DX11Device::CreateRasterizerState( const RasterizerStateParams& params ) {
	DX11RasterizerState* state = new DX11RasterizerState();
	if ( !state->Create( device, params ) ) {
		delete state;
		return nullptr;
	}
	return state;
}

DepthStencilState* DX11Device::CreateDepthStencilState( const DepthStencilStateParams& params ) {
	DX11DepthStencilState* state = new DX11DepthStencilState();
	if ( !state->Create( device, params ) ) {
		delete state;
		return nullptr;
	}
	return state;
}

Sampler* DX11Device::CreateSampler( const SamplerParams& params ) {
	DX11Sampler* sampler = new DX11Sampler();
	if ( !sampler->Create( device, params ) ) {
		delete sampler;
		return nullptr;
	}
	return sampler;
}

VertexLayout* DX11Device::CreateVertexLayout( const VertexAttribute* const attributes, const int attributesCount, RenderProgram* const program ) {
	DX11VertexLayout* layout = new DX11VertexLayout();
	if ( !layout->Create( device, attributes, attributesCount, program ) ) {
		delete layout;
		return nullptr;
	}
	return layout;
}

int DX11Device::GetMultisampleQuality( const int samplesCount) const {
	int levels = 0;
	device->CheckMultisampleQualityLevels( BACK_BUFFER_FORMAT, samplesCount, reinterpret_cast< unsigned int* >( &levels ) );
	return levels;
}

ID3D11DeviceContext* DX11Device::GetContext() {
	return context;
}

// DX11Display

DX11Display::~DX11Display() {
	dxgiOutput = nullptr;
	window = nullptr;
}

bool DX11Display::Create( ID3D11Device* const device, IDXGIAdapter* const adapter, const int outputId ) {
	// ziskat output s pozadovanym id
	ComPtr< IDXGIOutput > output;
	HRESULT hresult = adapter->EnumOutputs( static_cast< UINT >( outputId ), &output );
	if ( FAILED( hresult ) ) {
		return false;
	}
	this->dxgiOutput = output.GetRef();
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
	/*
	Pri vstupu do fullscreenu je predan ukazatel na window,
	pokud neni dostupny neni okno ve fullscreen rezimu
	*/
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
	ReleaseCom( &dxgiSwapChain );
	ReleaseCom( &device );
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

	ComPtr< IDXGISwapChain > swapChain;
	hresult = factory->CreateSwapChain( device, &desc, &swapChain );
	if ( FAILED( hresult ) ) {
		return false;
	}
	
	// vypnuti defaultniho prepinani do rezimu cele obrazovky
	factory->MakeWindowAssociation( hwnd, DXGI_MWA_NO_ALT_ENTER  );

	// ulozit objekty
	this->window = &window;
	this->device = device;
	device->AddRef();
	this->dxgiSwapChain = swapChain.GetRef();

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
	ReleaseCom( &resource );
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

int DX11Buffer::GetSubresourcesCount() const {
	return 0;
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
	const D3D11_USAGE usage = GetD3D11Usage( params.usage );
	const UINT CPUAccessFlags = GetD3D11CPUAccess( params.access );

	UINT bindFlags = D3D11_BIND_SHADER_RESOURCE;
	// set render target bind flag
	if ( params.flags & TextureBufferFlags::TEXTURE_BUFFER_FLAG_RENDER_TARGET ) {
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
		
		ComPtr< ID3D11Texture1D > texture;
		HRESULT hresult = device->CreateTexture1D( &textureDesc, subresources.get(), &texture );
		if ( FAILED( hresult ) ) {
			return false;
		}
		SetTextureBuffer( texture.GetRef(), params );
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
		
		ComPtr< ID3D11Texture2D > texture;
		HRESULT hresult = device->CreateTexture2D( &textureDesc, subresources.get(), &texture );
		if ( FAILED( hresult ) ) {
			return false;
		}
		SetTextureBuffer( texture.GetRef(), params );
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

		ComPtr< ID3D11Texture3D > texture;
		HRESULT hresult = device->CreateTexture3D( &textureDesc, subresources.get(), &texture );
		if ( FAILED( hresult ) ) {
			return false;
		}
		SetTextureBuffer( texture.GetRef(), params );
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

bool DX11TextureBuffer::Map( ID3D11DeviceContext* const context, const int subresource, const D3D11_MAP mapType, MappedBuffer& result ) {
	D3D11_MAPPED_SUBRESOURCE mappedResource;
	HRESULT hresult = context->Map( GetResource(), static_cast< UINT >( subresource ), mapType, 0, &mappedResource );
	if ( FAILED( hresult ) ) {
		ZeroMemory( &result, sizeof( MappedBuffer ) );
		return false;
	}
	TextureDimmensions dimmensions;
	GetMipDimmensions( width, height, depth, subresource % mipLevels, dimmensions );
	FormatInfo formatInfo = GetFormatInfo( format );

	result.data = mappedResource.pData;
	result.rowPitch = static_cast< int >( mappedResource.RowPitch );
	result.depthPitch = static_cast< int >( mappedResource.DepthPitch );
	result.subresource = subresource;
	result.rowByteWidth = ( dimmensions.width / formatInfo.blockSize ) * formatInfo.blockByteWidth;
	result.rowsCount = ( dimmensions.width / formatInfo.blockSize ) * ( dimmensions.height / formatInfo.blockSize ) * dimmensions.depth;
	result.depthsCount = dimmensions.depth;
	return true;
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

int DX11TextureBuffer::GetSubresourcesCount() const {
	return mipLevels * arraySize;
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
	bufferDesc.Usage				= GetD3D11Usage( usage );
	bufferDesc.BindFlags			= 0;
	bufferDesc.CPUAccessFlags		= GetD3D11CPUAccess( access );
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
	ComPtr< ID3D11Buffer > buffer;
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
	SetBuffer( buffer.GetRef(), bufferInfo );
	return true;
}

bool DX11GenericBuffer::Map( ID3D11DeviceContext* const context, const int subresource, const D3D11_MAP mapType, MappedBuffer& result ) {
	D3D11_MAPPED_SUBRESOURCE mappedResource;
	HRESULT hresult = context->Map( GetResource(), 0, mapType, 0, &mappedResource );
	if ( FAILED( hresult ) ) {
		ZeroMemory( &result, sizeof( MappedBuffer ) );
		return false;
	}
	result.data = mappedResource.pData;
	result.rowPitch = static_cast< int >( mappedResource.RowPitch );
	result.depthPitch = static_cast< int >( mappedResource.DepthPitch );
	result.subresource = 0;
	result.rowByteWidth = GetByteWidth();
	result.rowsCount = 1;
	result.depthsCount = 1;
	return true;
}

ID3D11Buffer* DX11GenericBuffer::GetD3D11Buffer() {
	return static_cast< ID3D11Buffer* >( GetResource() );
}

// DX11RenderTargetDescriptor

DX11RenderTargetDescriptor::DX11RenderTargetDescriptor() {
	view = nullptr;
}

DX11RenderTargetDescriptor::~DX11RenderTargetDescriptor() {
	ReleaseCom( &view );
}

bool DX11RenderTargetDescriptor::Create( ID3D11Device* const device, BackBuffer* const backBuffer ) {
	HRESULT hresult = 0;

	// get back buffer texture
	ASSERT_DOWNCAST( backBuffer, DX11BackBuffer );
	IDXGISwapChain* swapChain = static_cast< DX11BackBuffer* >( backBuffer )->GetSwapChain();
	ComPtr< ID3D11Texture2D > texture;
	hresult = swapChain->GetBuffer( 0, __uuidof( ID3D11Texture2D ), reinterpret_cast< LPVOID* >( &texture ) );
	if ( FAILED( hresult ) ) {
		return false;
	}
	// create RTV
	ComPtr< ID3D11RenderTargetView > view;
	hresult = device->CreateRenderTargetView( texture.Get(), NULL, &view );
	if ( FAILED( hresult ) ) {
		return false;
	}
	this->view = view.GetRef();
	return true;
}

bool DX11RenderTargetDescriptor::Create( ID3D11Device* const device, Buffer* const textureBuffer ) {
	ASSERT_DOWNCAST( textureBuffer, DX11TextureBuffer );
	ID3D11Resource* resource = static_cast< DX11TextureBuffer* >( textureBuffer )->GetResource();
	ComPtr< ID3D11RenderTargetView > view;
	HRESULT hresult = device->CreateRenderTargetView ( resource, NULL, &view );
	if ( FAILED( hresult ) ) {
		return false;
	}
	this->view = view.GetRef();
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
	ReleaseCom( &view );
}

bool DX11TextureDescriptor::Create( ID3D11Device* const device, Buffer* const textureBuffer ) {
	ASSERT_DOWNCAST( textureBuffer, DX11TextureBuffer );
	ID3D11Resource* resource = static_cast< DX11TextureBuffer* >( textureBuffer )->GetResource();
	ComPtr< ID3D11ShaderResourceView > view;
	HRESULT hresult = device->CreateShaderResourceView( resource, NULL, &view );
	if ( FAILED( hresult ) ) {
		return false;
	}
	this->view = view.GetRef();
	return true;
}

ID3D11ShaderResourceView* DX11TextureDescriptor::GetD3D11ShaderResourceView() {
	return view;
}

// DX11DepthStencilBufferDescriptor

DX11DepthStencilBufferDescriptor::DX11DepthStencilBufferDescriptor() {
	view = nullptr;
}

DX11DepthStencilBufferDescriptor::~DX11DepthStencilBufferDescriptor() {
	ReleaseCom( &view );
}

bool DX11DepthStencilBufferDescriptor::Create( ID3D11Device* const device, Buffer* const textureBuffer ) {
	const BufferType bufferType = textureBuffer->GetType();

	// check buffer type
	if ( bufferType != BufferType::TEXTURE_2D &&
		 bufferType != BufferType::TEXTURE_2D_MS
	) {
		return false;
	}
	// format must be DEPTH_24_UNORM_STENCIL_8_UINT
	ASSERT_DOWNCAST( textureBuffer, DX11TextureBuffer );
	const Format format = static_cast< DX11TextureBuffer* >( textureBuffer )->GetFormat();
	if ( format != Format::DEPTH_24_UNORM_STENCIL_8_UINT ) {
		return false;
	}
	ID3D11Resource* texture = static_cast< DX11TextureBuffer* >( textureBuffer )->GetResource();
	HRESULT hresult = 0;

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
	// texture view dimension
	if ( bufferType == BufferType::TEXTURE_2D ) {
		viewDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
		viewDesc.Texture2D.MipSlice = 0;

	// multisampled texture view dimension
	} else if ( bufferType == BufferType::TEXTURE_2D_MS ) {
		viewDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2DMS;
	} 
	// create view
	ComPtr< ID3D11DepthStencilView > view;
	hresult = device->CreateDepthStencilView( texture, &viewDesc, &view );
	if ( FAILED( hresult )  ) {
		return false;
	}
	this->view = view.GetRef();
	return true;
}

ID3D11DepthStencilView* DX11DepthStencilBufferDescriptor::GetD3D11DepthStencilView() {
	return view;
}

// DX11ConstantBufferDescriptor

const int UNUSED_CBUFFER_SLOT = MAX_CBUFFER_SLOTS;

DX11ConstantBufferDescriptor::DX11ConstantBufferDescriptor() {
	buffer = nullptr;
	constantsCount = 0;
	constantsSize = 0;
	vsSlot = 0;
	psSlot = 0;
	gsSlot = 0;
}

DX11ConstantBufferDescriptor::~DX11ConstantBufferDescriptor() {
	ReleaseCom( &buffer );
}

/*
Pozn.: Funkce Create() nekontroluje signaturu konstant!
*/
bool DX11ConstantBufferDescriptor::Create( Buffer* const constantBuffer, const ConstantBufferDescriptorParams &params ) {
	HRESULT hresult = 0;

	ASSERT_DOWNCAST( params.program, DX11RenderProgram );
	DX11RenderProgram* const program = static_cast< DX11RenderProgram* >( params.program );

	const int VS = 0;
	const int PS = 1;
	const int GS = 2;
	const int SHADERS_COUNT = 3;

	ID3DBlob* shaderBytecode = nullptr;
	ID3DBlob* shaders[ SHADERS_COUNT ] = { nullptr };
	shaders[ VS ] = program->GetVertexShaderByteCode();
	shaders[ PS ] = program->GetPixelShaderByteCode();
	shaders[ GS ] = program->GetGeometryShaderByteCode();

	int slots[ SHADERS_COUNT ] = { UNUSED_CBUFFER_SLOT };

	// najit sloty pro vsechny shadery
	for ( int i = 0; i < SHADERS_COUNT; i++ ) {
		ID3DBlob* const shader = shaders[ i ];
		if ( shader == nullptr ) {
			continue;
		}
		// reflection
		ComPtr< ID3D11ShaderReflection > reflector;
		hresult = D3DReflect(
			shader->GetBufferPointer(),
			shader->GetBufferSize(),
			IID_ID3D11ShaderReflection,
			reinterpret_cast< void** >( &reflector )
		);
		if ( FAILED( hresult ) ) {
			return false;
		}
		// reflect constant buffer, pokud neni definovan, vrati nasledujici volani GetDesc() chybu
		ID3D11ShaderReflectionConstantBuffer* cbufferReflector = nullptr;
		cbufferReflector = reflector->GetConstantBufferByName( params.name );

		// konstant buffer description
		D3D11_SHADER_BUFFER_DESC bufferDesc;
		hresult = cbufferReflector->GetDesc( &bufferDesc );

		// failed, shader neobsahuje definici pozadovaneho bufferu
		if ( FAILED( hresult ) ) {
			continue;
		}
		// index slotu
		D3D11_SHADER_INPUT_BIND_DESC inputBindDesc;
		hresult = reflector->GetResourceBindingDescByName( params.name, &inputBindDesc );
		if ( FAILED( hresult ) ) {
			return false;
		}
		slots[ i ] = 1 + static_cast< int >( inputBindDesc.BindPoint );

		// referencni bytecode
		shaderBytecode = shader;
		break;
	}

	// v zadnem shaderu neni buffer s pozadovanym nazvem definovan
	if ( shaderBytecode == nullptr ) {
		return false;
	}
	// reflect shader
	ComPtr< ID3D11ShaderReflection > reflector;
	hresult = D3DReflect(
		shaderBytecode->GetBufferPointer(),
		shaderBytecode->GetBufferSize(),
		IID_ID3D11ShaderReflection,
		reinterpret_cast< void** >( &reflector )
	);
	if ( FAILED( hresult ) ) {
		return false;
	}
	// reflect constant buffer
	ID3D11ShaderReflectionConstantBuffer* cbufferReflector = nullptr;
	cbufferReflector = reflector->GetConstantBufferByName( params.name );

	// mapovani konstant
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
		// velikost konstanty musi byt shodna
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

	// nepodarilo se namapovat vsechny konstanty
	if ( mapped != params.constantsCount ) {
		return false;
	}
	// nesouhlasi zarovnani pameti, ulozit mapu
	if ( !aligned ) {
		this->map = std::move( map );
	}
	// ulozit vysledky
	ASSERT_DOWNCAST( constantBuffer, DX11GenericBuffer );
	this->buffer = static_cast< DX11GenericBuffer* >( constantBuffer )->GetD3D11Buffer();
	this->buffer->AddRef();
	this->constantsCount = params.constantsCount;
	this->constantsSize = offset;
	vsSlot = slots[ VS ];
	psSlot = slots[ PS ];
	gsSlot = slots[ GS ];

	return true;
}

void DX11ConstantBufferDescriptor::UpdateConstants( const void* const src, void* const dest ) const {
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
			static_cast< const Byte* >( src ) + constant.sysMemOffset,
			constant.size
		);
	}
}

ID3D11Buffer* DX11ConstantBufferDescriptor::GetBuffer() {
	return buffer;
}

int DX11ConstantBufferDescriptor::GetVSSlot() const {
	return vsSlot;
}

int DX11ConstantBufferDescriptor::GetPSSlot() const {
	return psSlot;
}

int DX11ConstantBufferDescriptor::GetGSSlot() const {
	return gsSlot;
}

// DXShader

DX11Shader::DX11Shader() {
	code = nullptr;
	shader = nullptr;
	type = ShaderType::UNDEFINED;
	version = ShaderVersion::UNDEFINED;
}

DX11Shader::~DX11Shader() {
	ReleaseCom( &shader );
	ReleaseCom( &code );
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
	// shader target (type and version)
	const char* target = nullptr;
	switch ( params.type ) {
	case ShaderType::VERTEX_SHADER:		target = "vs_5_0"; break;
	case ShaderType::PIXEL_SHADER:		target = "ps_5_0"; break;
	case ShaderType::GEOMETRY_SHADER:	target = "gs_5_0"; break;
	}
	// flags
	UINT flags = 0;
	if ( params.flags & SHADER_COMPILE_FLAG_WARNINGS_AS_ERRRORS ) {
		flags |= D3DCOMPILE_WARNINGS_ARE_ERRORS;
	}
	if ( params.flags & SHADER_COMPILE_FLAG_DEBUG ) {
		flags |= SHADER_COMPILE_FLAG_DEBUG;
	}
	// optimization flags
	switch ( params.optimization ) {
	case ShaderOptimization::DISABLED:	flags |= D3DCOMPILE_SKIP_OPTIMIZATION;   break;
	case ShaderOptimization::LOW:		flags |= D3DCOMPILE_OPTIMIZATION_LEVEL0; break;
	case ShaderOptimization::MEDIUM:	flags |= D3DCOMPILE_OPTIMIZATION_LEVEL3; break;
	case ShaderOptimization::HIGH:		flags |= D3DCOMPILE_OPTIMIZATION_LEVEL2; break;
	}
	// compile
	ComPtr< ID3DBlob > code;
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
		return false;
	}
	// ulozit vysledek
	this->code = code.GetRef();
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

ID3D11VertexShader* DX11Shader::GetD3D11VertexShader() {
	if ( type == ShaderType::VERTEX_SHADER ) {
		return static_cast< ID3D11VertexShader* >( shader );
	}
	return nullptr;
}

ID3D11PixelShader* DX11Shader::GetD3D11PixelShader() {
	if ( type == ShaderType::PIXEL_SHADER ) {
		return static_cast< ID3D11PixelShader* >( shader );
	}
	return nullptr;
}

ID3D11GeometryShader* DX11Shader::GetD3D11GeometryShader() {
	if ( type == ShaderType::GEOMETRY_SHADER ) {
		return static_cast< ID3D11GeometryShader* >( shader );
	}
	return nullptr;
}

// DX11RenderProgram

DX11RenderProgram::DX11RenderProgram() {
	vs = nullptr;
	ps = nullptr;
	gs = nullptr;
	vsByteCode = nullptr;
	psByteCode = nullptr;
	gsByteCode = nullptr;
}

DX11RenderProgram::~DX11RenderProgram() {
	ReleaseCom( &vs );
	ReleaseCom( &ps );
	ReleaseCom( &gs );
	ReleaseCom( &vsByteCode );
	ReleaseCom( &psByteCode );
	ReleaseCom( &gsByteCode );
}

bool DX11RenderProgram::Create( Shader* const vs, Shader* const ps, Shader* const gs ) {
	// vertex shader a pixel shader musi byt vzdy definovan
	if ( vs == nullptr || ps == nullptr ) {
		return false;
	}
	// kontrola typu shaderu
	if ( vs->GetType() != ShaderType::VERTEX_SHADER ) {
		return false;
	}
	if ( ps->GetType() != ShaderType::PIXEL_SHADER ) {
		return false;
	}
	if ( gs != nullptr ) {
		if ( gs->GetType() != ShaderType::GEOMETRY_SHADER ) {
			return false;
		}
	}
	// ulozit vertex shader
	ASSERT_DOWNCAST( vs, DX11Shader );
	this->vs = static_cast< DX11Shader* >( vs )->GetD3D11VertexShader();
	this->vs->AddRef();
	this->vsByteCode = static_cast< DX11Shader* >( vs )->GetBlob();
	this->vsByteCode->AddRef();

	// ulozit pixel shader
	ASSERT_DOWNCAST( ps, DX11Shader );
	this->ps = static_cast< DX11Shader* >( ps )->GetD3D11PixelShader();
	this->ps->AddRef();
	this->psByteCode = static_cast< DX11Shader* >( ps )->GetBlob();
	this->psByteCode->AddRef();

	// ulozit geometry shader
	if ( gs != nullptr ) {
		ASSERT_DOWNCAST( gs, DX11Shader );
		this->gs = static_cast< DX11Shader* >( gs )->GetD3D11GeometryShader();
		this->gs->AddRef();
		this->gsByteCode = static_cast< DX11Shader* >( gs )->GetBlob();
		this->gsByteCode->AddRef();
	}
	return true;
}

ID3D11VertexShader* DX11RenderProgram::GetD3D11VertexShader() {
	return vs;
}

ID3D11PixelShader* DX11RenderProgram::GetD3D11PixelShader() {
	return ps;
}

ID3D11GeometryShader* DX11RenderProgram::GetD3D11GeometryShader() {
	return gs;
}

ID3DBlob* DX11RenderProgram::GetVertexShaderByteCode() {
	return vsByteCode;
}

ID3DBlob* DX11RenderProgram::GetPixelShaderByteCode() {
	return psByteCode;
}

ID3DBlob* DX11RenderProgram::GetGeometryShaderByteCode() {
	return gsByteCode;
}

// DX11Sampler

DX11Sampler::DX11Sampler() {
	sampler = nullptr;
}

DX11Sampler::~DX11Sampler() {
	ReleaseCom( &sampler );
}

bool DX11Sampler::Create( ID3D11Device* const device, const SamplerParams& params ) {
	D3D11_SAMPLER_DESC samplerDesc;
	ZeroMemory( &samplerDesc, sizeof( samplerDesc ) );
	samplerDesc.Filter			= GetD3D11Filter( params.filter );
	samplerDesc.AddressU		= GetD3D11TextureAddressMode( params.uAddressing );
	samplerDesc.AddressV		= GetD3D11TextureAddressMode( params.vAddressing );
	samplerDesc.AddressW		= GetD3D11TextureAddressMode( params.wAddressing );
	samplerDesc.MipLODBias		= 0;
	samplerDesc.MaxAnisotropy	= params.maxAnisotropy;
	samplerDesc.ComparisonFunc	= D3D11_COMPARISON_NEVER;
	samplerDesc.MinLOD			= params.minLOD;
	samplerDesc.MaxLOD			= ( params.maxLOD == MAX_TEXTURE_LOD ? D3D11_FLOAT32_MAX : params.maxLOD );

	ComPtr< ID3D11SamplerState > sampler;
	HRESULT hresult = device->CreateSamplerState( &samplerDesc, &sampler );
	if ( FAILED( hresult ) ) {
		return false;
	}
	this->sampler = sampler.GetRef();
	return true;
}

ID3D11SamplerState* DX11Sampler::GetSampler() {
	return sampler;
}

// DX11BlendState

DX11BlendState::DX11BlendState() {
	state = nullptr;
}

DX11BlendState::~DX11BlendState() {
	ReleaseCom( &state );
}

bool DX11BlendState::Create( ID3D11Device* const device, const BlendStateParams& params ) {
	D3D11_BLEND_DESC desc;
	ZeroMemory( &desc, sizeof( desc ) );
	desc.AlphaToCoverageEnable = FALSE;
	desc.IndependentBlendEnable = ( params.uniformBlending ? FALSE : TRUE );

	for ( int i = 0; i < MAX_RENDER_TARGETS; i++ ) {
		const RenderTargetBlend& rtParam = params.renderTargets[ i ];
		D3D11_RENDER_TARGET_BLEND_DESC& rtDesc = desc.RenderTarget[ i ];
		rtDesc.BlendEnable			 = ( rtParam.enable ? TRUE : FALSE );
		rtDesc.SrcBlend				 = GetD3D11Blend( rtParam.srcRGB );
		rtDesc.DestBlend			 = GetD3D11Blend( rtParam.destRGB );
		rtDesc.BlendOp				 = GetD3D11BlendOp( rtParam.opRGB );
		rtDesc.SrcBlendAlpha		 = GetD3D11Blend( rtParam.srcAlpha );
		rtDesc.DestBlendAlpha		 = GetD3D11Blend( rtParam.destAlpha );
		rtDesc.BlendOpAlpha			 = GetD3D11BlendOp( rtParam.opAlpha );
		rtDesc.RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
	}
	ComPtr< ID3D11BlendState > state;
	HRESULT hresult = device->CreateBlendState( &desc, &state );
	if ( FAILED( hresult ) ) {
		return false;
	}
	this->state = state.GetRef();
	return true;
}

ID3D11BlendState* DX11BlendState::GetD3D11BlendState() {
	return state;
}

// DX11RasterizerState

DX11RasterizerState::DX11RasterizerState() {
	state = nullptr;
}

DX11RasterizerState::~DX11RasterizerState() {
	ReleaseCom( &state );
}

bool DX11RasterizerState::Create( ID3D11Device* const device, const RasterizerStateParams& params ) {
	D3D11_RASTERIZER_DESC desc;

	// culling
	switch ( params.cullMode ) {
	case CullMode::DISABLED:	desc.CullMode = D3D11_CULL_NONE;  break;
	case CullMode::FRONT_FACE:	desc.CullMode = D3D11_CULL_FRONT; break;
	case CullMode::BACK_FACE:	desc.CullMode = D3D11_CULL_BACK;  break;
	}
	desc.FillMode				= ( params.wireframe ? D3D11_FILL_WIREFRAME : D3D11_FILL_SOLID );
	desc.FrontCounterClockwise	= ( params.frontCounterClockwise ? TRUE : FALSE );
	desc.DepthBias				= static_cast< INT >( params.depthBias );
	desc.DepthBiasClamp			= static_cast< FLOAT >( params.depthBiasClamp );
	desc.SlopeScaledDepthBias	= static_cast< FLOAT >( params.slopeScaledDepthBias );
	desc.DepthClipEnable		= ( params.depthClipping ? TRUE : FALSE );
	desc.ScissorEnable			= ( params.scissorCulling ? TRUE : FALSE );
	desc.MultisampleEnable		= ( params.multisampling ? TRUE : FALSE );
	desc.AntialiasedLineEnable	= ( params.antialiasedLines ? TRUE : FALSE );

	ComPtr< ID3D11RasterizerState > state;
	HRESULT hresult = device->CreateRasterizerState( &desc, &state );
	if ( FAILED( hresult ) ) {
		return false;
	}
	this->state = state.GetRef();
	return true;
}

ID3D11RasterizerState* DX11RasterizerState::GetD3D11RasterizerState() {
	return state;
}

// DX11DepthStencilState

DX11DepthStencilState::DX11DepthStencilState() {
	state = nullptr;
}

DX11DepthStencilState::~DX11DepthStencilState() {
	ReleaseCom( &state );
}

bool DX11DepthStencilState::Create( ID3D11Device* const device, const DepthStencilStateParams& params ) {
	D3D11_DEPTH_STENCIL_DESC stateDesc;
	stateDesc.DepthWriteMask				= D3D11_DEPTH_WRITE_MASK_ALL;
	stateDesc.DepthEnable					= ( params.enableDepth ? TRUE : FALSE );
	stateDesc.DepthFunc						= GetD3D11ComparsionFunc( params.depthFunc );
	stateDesc.StencilEnable					= ( params.enableStencil ? TRUE : FALSE );
	stateDesc.StencilReadMask				= D3D11_DEFAULT_STENCIL_READ_MASK;
	stateDesc.StencilWriteMask				= D3D11_DEFAULT_STENCIL_WRITE_MASK;
	stateDesc.FrontFace.StencilFunc			= GetD3D11ComparsionFunc( params.stencilFunc );
	stateDesc.FrontFace.StencilPassOp		= GetD3D11StencilOp( params.stencilPassOp );
	stateDesc.FrontFace.StencilFailOp		= GetD3D11StencilOp( params.stencilFailOp );
	stateDesc.FrontFace.StencilDepthFailOp	= GetD3D11StencilOp( params.stencilDepthFailOp );
	stateDesc.BackFace						= stateDesc.FrontFace;

	ComPtr< ID3D11DepthStencilState > state;
	HRESULT hresult = device->CreateDepthStencilState( &stateDesc, &state );
	if ( FAILED( hresult )  ) {
		return false;
	}
	this->state = state.GetRef();
	return true;
}

ID3D11DepthStencilState* DX11DepthStencilState::GetD3D11DepthStencilState() {
	return state;
}

// DX11VertexLayout

DX11VertexLayout::DX11VertexLayout() {
	inputLayout = nullptr;
}

DX11VertexLayout::~DX11VertexLayout() {
	ReleaseCom( &inputLayout );
}

bool DX11VertexLayout::Create( ID3D11Device* const device, const VertexAttribute* const attributes, const int attributesCount, RenderProgram* const program ) {
	ASSERT_DOWNCAST( program, DX11RenderProgram );
	ID3DBlob* const shaderCode = static_cast< DX11RenderProgram* >( program )->GetVertexShaderByteCode();

	Array< D3D11_INPUT_ELEMENT_DESC > elements;
	elements.Reserve( attributesCount );

	// initialize elements
	for ( int attributeIndex = 0; attributeIndex < attributesCount; attributeIndex++ ) {
		const VertexAttribute& attribute = attributes[ attributeIndex ];
		const FormatInfo formatInfo = GetFormatInfo( attribute.format );

		D3D11_INPUT_ELEMENT_DESC desc;
		desc.SemanticName			= attribute.semantic;
		desc.SemanticIndex			= 0; // default value
		desc.Format					= GetDXGIFormat( attribute.format );
		desc.InputSlot				= static_cast< UINT >( attribute.slot );
		desc.AlignedByteOffset		= 0; // default value
		desc.InputSlotClass			= ( attribute.instanceCount == 0 ? D3D11_INPUT_PER_VERTEX_DATA : D3D11_INPUT_PER_INSTANCE_DATA );
		desc.InstanceDataStepRate	= static_cast< UINT >( attribute.instanceCount );

		// iterate all attribute elements
		for ( int i = 0; i < attribute.elementsCount; i++ ) {
			desc.SemanticIndex		= static_cast< UINT >( attribute.semanticIndex + i );
			desc.AlignedByteOffset	= static_cast< UINT >( attribute.offset + i * formatInfo.blockByteWidth );
			elements.Push( desc );
		}
	}
	// create input layout
	ComPtr< ID3D11InputLayout > inputLayout;
	HRESULT hresult = device->CreateInputLayout(
		elements.Raw(),
		static_cast< UINT >( elements.Length() ),
		shaderCode->GetBufferPointer(),
		static_cast< SIZE_T >( shaderCode->GetBufferSize() ),
		&inputLayout
	);
	if ( FAILED( hresult ) ) {
		return false;
	}
	this->inputLayout = inputLayout.GetRef();
	return true;
}

ID3D11InputLayout* DX11VertexLayout::GetInputLayout() {
	return inputLayout;
}

// DX11VertexDescriptor

DX11VertexDescriptor::DX11VertexDescriptor() {
	ZeroMemory( vertexBuffers, sizeof( vertexBuffers ) );
	indexBuffer = nullptr;
	indexBufferFormat = DXGI_FORMAT_UNKNOWN;
	inputLayout = nullptr;
}

DX11VertexDescriptor::~DX11VertexDescriptor() {
	for ( int i = 0; i < MAX_VERTEX_INPUT_SLOTS; i++ ) {
		ReleaseCom( &vertexBuffers[ i ] );
	}
	ReleaseCom( &indexBuffer );
	ReleaseCom( &inputLayout );
}

bool DX11VertexDescriptor::Create( const VertexDescriptorParams& params ) {
	// save index buffer
	if ( params.indexBuffer != nullptr ) {
		if ( params.indexBuffer->GetType() != BufferType::INDEX_BUFFER ) {
			return false;
		}
		if ( params.indexBufferFormat != Format::R16_UINT &&
			params.indexBufferFormat != Format::R32_UINT
		) {
			return false;
		}
		ASSERT_DOWNCAST( params.indexBuffer, DX11GenericBuffer );
		indexBuffer = static_cast< DX11GenericBuffer* >( params.indexBuffer )->GetD3D11Buffer();
		indexBufferFormat = GetDXGIFormat( params.indexBufferFormat );
	}
	// save vertex buffers
	for ( int i = 0; i < MAX_VERTEX_INPUT_SLOTS; i++ ) {
		Buffer* const buffer = params.vertexBuffers[ i ];
		if ( buffer == nullptr ) {
			continue;
		}
		if ( buffer->GetType() != BufferType::VERTEX_BUFFER ) {
			return false;
		}
		ASSERT_DOWNCAST( buffer, DX11Buffer );
		ID3D11Resource* resource = static_cast< ID3D11Buffer* >( static_cast< DX11Buffer* >( buffer )->GetResource() );
		ASSERT_DOWNCAST( resource, ID3D11Buffer );
		vertexBuffers[ i ] = static_cast< ID3D11Buffer* >( resource );
		vertexBuffers[ i ]->AddRef();
	}
	// save input layout
	ASSERT_DOWNCAST( params.vertexLayout, DX11VertexLayout );
	inputLayout = static_cast< DX11VertexLayout* >( params.vertexLayout )->GetInputLayout();

	return true;
}

ID3D11Buffer** DX11VertexDescriptor::GetVertexBuffers() {
	return vertexBuffers;
}

ID3D11Buffer* DX11VertexDescriptor::GetIndexBuffer() {
	return indexBuffer;
}

ID3D11InputLayout* DX11VertexDescriptor::GetInputLayout() {
	return inputLayout;
}

DXGI_FORMAT DX11VertexDescriptor::GetIndexBufferDXGIFormat() const {
	return indexBufferFormat;
}

// DX11CommandInterface

DX11CommandInterface::DX11CommandInterface() {
	context = nullptr;
}

DX11CommandInterface::~DX11CommandInterface() {
	ReleaseCom( &context );
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

void DX11CommandInterface::Flush() {
	context->Flush();
}

void DX11CommandInterface::SetRenderTargets( RenderTargetDescriptor* const renderTargets[], const int count, DepthStencilBufferDescriptor* const depthStencilBuffer ) {
	if ( count > MAX_RENDER_TARGETS ) {
		return;
	}
	ID3D11RenderTargetView* renderTargetViews[ MAX_RENDER_TARGETS ] = { NULL };
	for ( int i = 0; i < count; i++ ) {
		ASSERT_DOWNCAST( renderTargets[ i ], DX11RenderTargetDescriptor );
		renderTargetViews[ i ] = static_cast< DX11RenderTargetDescriptor* >( renderTargets[ i ] )->GetView();
	}
	ID3D11DepthStencilView* depthStencilView = NULL;
	if ( depthStencilBuffer != nullptr ) {
		ASSERT_DOWNCAST( depthStencilBuffer, DX11DepthStencilBufferDescriptor );
		depthStencilView = static_cast< DX11DepthStencilBufferDescriptor* >( depthStencilBuffer )->GetD3D11DepthStencilView();
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

void DX11CommandInterface::ClearDepthStencil( DepthStencilBufferDescriptor* const descriptor, const float depth, const uint8_t stencil ) {
	ASSERT_DOWNCAST( descriptor, DX11DepthStencilBufferDescriptor );
	context->ClearDepthStencilView(
		static_cast< DX11DepthStencilBufferDescriptor* >( descriptor )->GetD3D11DepthStencilView(),
		D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL,
		static_cast< FLOAT >( depth ),
		static_cast< UINT8 >( stencil )
	);
}

void DX11CommandInterface::ClearDepth( DepthStencilBufferDescriptor* const descriptor, const float depth ) {
	ASSERT_DOWNCAST( descriptor, DX11DepthStencilBufferDescriptor );
	context->ClearDepthStencilView(
		static_cast< DX11DepthStencilBufferDescriptor* >( descriptor )->GetD3D11DepthStencilView(),
		D3D11_CLEAR_DEPTH,
		static_cast< FLOAT >( depth ),
		0
	);
}

void DX11CommandInterface::ClearStencil( DepthStencilBufferDescriptor* const descriptor, const uint8_t stencil ) {
	ASSERT_DOWNCAST( descriptor, DX11DepthStencilBufferDescriptor );
	context->ClearDepthStencilView(
		static_cast< DX11DepthStencilBufferDescriptor* >( descriptor )->GetD3D11DepthStencilView(),
		D3D11_CLEAR_STENCIL,
		0,
		static_cast< UINT8 >( stencil )
	);
}

void DX11CommandInterface::ClearState() {
	context->ClearState();
}

bool DX11CommandInterface::Map( Buffer* const buffer, const int subresource, const MapPolicy policy, MappedBuffer& result ) {
	D3D11_MAP type = D3D11_MAP::D3D11_MAP_READ;
	switch ( policy ) {
	case MapPolicy::READ_ONLY:		type = D3D11_MAP_READ;			break;
	case MapPolicy::WRITE_ONLY:		type = D3D11_MAP_WRITE;			break;
	case MapPolicy::READ_WRITE:		type = D3D11_MAP_READ_WRITE;	break;
	case MapPolicy::WRITE_DISCARD:	type = D3D11_MAP_WRITE_DISCARD; break;
	}
	ASSERT_DOWNCAST( buffer, DX11Buffer );
	return static_cast< DX11Buffer* >( buffer )->Map( context, subresource, type, result );
}

void DX11CommandInterface::Unmap( Buffer* const buffer, MappedBuffer& mappedBuffer ) {
	mappedBuffer.data = nullptr;
	ASSERT_DOWNCAST( buffer, DX11Buffer );
	context->Unmap( static_cast< DX11Buffer* >( buffer )->GetResource(), static_cast< UINT >( mappedBuffer.subresource ) );
}

bool DX11CommandInterface::UpdateBuffer( Buffer* const buffer, const int subresource, const void* const data ) {
	// map
	MapPolicy policy = ( buffer->GetUsage() == BufferUsage::DYNAMIC ? MapPolicy::WRITE_DISCARD : MapPolicy::WRITE_ONLY );
	MappedBuffer mappedBuffer;
	if ( !Map( buffer, subresource, policy, mappedBuffer ) ) {
		return false;
	}
	// memcpy
	const Byte* src = static_cast< const Byte* >( data );
	Byte* dest = static_cast< Byte* >( mappedBuffer.data );
	const int depthSliceRows = mappedBuffer.rowsCount / mappedBuffer.depthsCount;
	for ( int depth = 0; depth < mappedBuffer.depthsCount; depth++ ) {
		for ( int row = 0; row < depthSliceRows; row++ ) {
			memcpy( dest, src, mappedBuffer.rowByteWidth );
			dest += mappedBuffer.rowPitch;
			src += mappedBuffer.rowByteWidth;
		}
		dest = static_cast< Byte* >( mappedBuffer.data ) + ( depth * mappedBuffer.depthPitch );
	}
	// unmap
	Unmap( buffer, mappedBuffer );
	return true;
}

bool DX11CommandInterface::UpdateConstantBuffer( ConstantBufferDescriptor* const descriptor, const void* const data ) {
	ASSERT_DOWNCAST( descriptor, DX11ConstantBufferDescriptor );
	ID3D11Buffer* buffer = static_cast< DX11ConstantBufferDescriptor* >( descriptor )->GetBuffer();
	D3D11_MAPPED_SUBRESOURCE mappedSubresource;
	HRESULT hresult = context->Map( buffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedSubresource );
	if ( FAILED( hresult ) ) {
		return false;
	}
	static_cast< DX11ConstantBufferDescriptor* >( descriptor )->UpdateConstants( data, mappedSubresource.pData );
	context->Unmap( buffer, 0 );
	return true;
}

void DX11CommandInterface::CopyBuffer( Buffer* const src, Buffer* const dest ) {
	ASSERT_DOWNCAST( src, DX11Buffer );
	ASSERT_DOWNCAST( dest, DX11Buffer );
	context->CopyResource(
		static_cast< DX11Buffer* >( dest )->GetResource(),
		static_cast< DX11Buffer* >( src )->GetResource()
	);
}

void DX11CommandInterface::SetConstantBuffers( ConstantBufferDescriptor* const descriptors[], const int count ) {
	// deaktivovat vsechny sloty
	if ( descriptors == nullptr ) {
		ID3D11Buffer* buffers[ MAX_CBUFFER_SLOTS ] = { nullptr };
		context->VSSetConstantBuffers( 0, MAX_CBUFFER_SLOTS, buffers );
		context->PSSetConstantBuffers( 0, MAX_CBUFFER_SLOTS, buffers );
		context->GSSetConstantBuffers( 0, MAX_CBUFFER_SLOTS, buffers );
		return;
	}
	// + 1 pro UNUSED_CBUFFER_SLOT
	ID3D11Buffer* vsBuffers[ MAX_CBUFFER_SLOTS + 1 ] = { nullptr };
	ID3D11Buffer* psBuffers[ MAX_CBUFFER_SLOTS + 1 ] = { nullptr };
	ID3D11Buffer* gsBuffers[ MAX_CBUFFER_SLOTS + 1 ] = { nullptr };

	for ( int i = 0; i < count; i++ ) {
		ASSERT_DOWNCAST( descriptors[ i ], DX11ConstantBufferDescriptor );
		DX11ConstantBufferDescriptor* descriptor = static_cast< DX11ConstantBufferDescriptor* >( descriptors[ i ] );
		vsBuffers[ descriptor->GetVSSlot() ] = descriptor->GetBuffer();
		psBuffers[ descriptor->GetPSSlot() ] = descriptor->GetBuffer();
		gsBuffers[ descriptor->GetGSSlot() ] = descriptor->GetBuffer();
	}
	context->VSSetConstantBuffers( 0, MAX_CBUFFER_SLOTS, vsBuffers );
	context->PSSetConstantBuffers( 0, MAX_CBUFFER_SLOTS, psBuffers );
	context->GSSetConstantBuffers( 0, MAX_CBUFFER_SLOTS, gsBuffers );
}

void DX11CommandInterface::SetVertexInput( VertexDescriptor* const descriptor ) {
	ASSERT_DOWNCAST( descriptor, DX11VertexDescriptor );
	context->IASetInputLayout( static_cast< DX11VertexDescriptor* >( descriptor )->GetInputLayout() );
	context->IASetVertexBuffers(
		0,
		MAX_VERTEX_INPUT_SLOTS,
		static_cast< DX11VertexDescriptor* >( descriptor )->GetVertexBuffers(),
		NULL,
		NULL
	);
	context->IASetIndexBuffer(
		static_cast< DX11VertexDescriptor* >( descriptor )->GetIndexBuffer(),
		static_cast< DX11VertexDescriptor* >( descriptor )->GetIndexBufferDXGIFormat(),
		0
	);
}

void DX11CommandInterface::SetRenderProgram( RenderProgram* const program ) {
	ASSERT_DOWNCAST( program, DX11RenderProgram );
	context->VSSetShader( static_cast< DX11RenderProgram* >( program )->GetD3D11VertexShader(), NULL, 0 );
	context->PSSetShader( static_cast< DX11RenderProgram* >( program )->GetD3D11PixelShader(), NULL, 0 );
	context->GSSetShader( static_cast< DX11RenderProgram* >( program )->GetD3D11GeometryShader(), NULL, 0 );
}

void DX11CommandInterface::Draw( const int verticesCount, const int startVertex ) {
	context->Draw( static_cast< UINT >( verticesCount ), static_cast< UINT >( startVertex ) );
}

void DX11CommandInterface::DrawIndexed( const int indicesCount, const int startIndex ) {
	context->DrawIndexed( static_cast< UINT >( indicesCount ), static_cast< UINT >( startIndex ), 0 );
}

void DX11CommandInterface::DrawInstanced( const int verticesCount, const int startVertex, const int instancesCount, const int startInstance ) {
	context->DrawInstanced(
		static_cast< UINT >( verticesCount ),
		static_cast< UINT >( instancesCount ),
		static_cast< UINT >( startVertex ),
		static_cast< UINT >( startInstance )
	);
}

void DX11CommandInterface::DrawIndexedInstanced( const int indicesCount, const int startIndex, const int instancesCount, const int startInstance ) {
	context->DrawIndexedInstanced(
		static_cast< UINT >( indicesCount ),
		static_cast< UINT >( instancesCount ),
		static_cast< UINT >( startIndex ),
		0,
		static_cast< UINT >( startInstance )
	);
}

void DX11CommandInterface::SetPrimitiveTopology( const PrimitiveTopology topology ) {
	context->IASetPrimitiveTopology( GetD3D11PrimitiveTopology( topology ) );
}

void DX11CommandInterface::SetBlendState( BlendState* const state ) {
	if ( state == nullptr ) {
		context->OMSetBlendState( NULL, NULL, 0 );
		return;
	}
	ASSERT_DOWNCAST( state, DX11BlendState );
	context->OMSetBlendState(
		static_cast< DX11BlendState* >( state )->GetD3D11BlendState(),
		NULL,
		0xffffffff
	);
}

void DX11CommandInterface::SetDepthStencilState( DepthStencilState* const state, const uint32_t stencilRef ) {
	if ( state == nullptr ) {
		context->OMSetDepthStencilState( NULL, 0 );
		return;
	}
	ASSERT_DOWNCAST( state, DX11DepthStencilState );
	context->OMSetDepthStencilState(
		static_cast< DX11DepthStencilState* >( state )->GetD3D11DepthStencilState(),
		static_cast< UINT >( stencilRef )
	);
}

void DX11CommandInterface::SetRasterizerState( RasterizerState* const state ) {
	ASSERT_DOWNCAST( state, DX11RasterizerState );
	context->RSSetState( static_cast< DX11RasterizerState* >( state )->GetD3D11RasterizerState() );
}

void DX11CommandInterface::SetVSTextures( TextureDescriptor* const descriptors[], const int count ) {
	ID3D11ShaderResourceView* views[ MAX_TEXTURES ] = { NULL };
	if ( descriptors != nullptr ) {
		for ( int i = 0; i < count; i++ ) {
			ASSERT_DOWNCAST( descriptors[ i ], DX11TextureDescriptor );
			views[ i ] = static_cast< DX11TextureDescriptor* >( descriptors[ i ] )->GetD3D11ShaderResourceView();
		}
	}
	context->VSSetShaderResources( 0, static_cast< UINT >( MAX_TEXTURES ), views );
}

void DX11CommandInterface::SetPSTextures( TextureDescriptor* const descriptors[], const int count ) {
	ID3D11ShaderResourceView* views[ MAX_TEXTURES ] = { NULL };
	if ( descriptors != nullptr ) {
		for ( int i = 0; i < count; i++ ) {
			ASSERT_DOWNCAST( descriptors[ i ], DX11TextureDescriptor );
			views[ i ] = static_cast< DX11TextureDescriptor* >( descriptors[ i ] )->GetD3D11ShaderResourceView();
		}
	}
	context->PSSetShaderResources( 0, static_cast< UINT >( MAX_TEXTURES ), views );
}

void DX11CommandInterface::SetGSTextures( TextureDescriptor* const descriptors[], const int count ) {
	ID3D11ShaderResourceView* views[ MAX_TEXTURES ] = { NULL };
	if ( descriptors != nullptr ) {
		for ( int i = 0; i < count; i++ ) {
			ASSERT_DOWNCAST( descriptors[ i ], DX11TextureDescriptor );
			views[ i ] = static_cast< DX11TextureDescriptor* >( descriptors[ i ] )->GetD3D11ShaderResourceView();
		}
	}
	context->GSSetShaderResources( 0, static_cast< UINT >( MAX_TEXTURES ), views );
}