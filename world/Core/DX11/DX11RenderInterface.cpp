#include <vector>
#include <d3dcompiler.h>
#include "DX11RenderInterface.h"
#include "Framework/Math.h"
#include "Framework/Debug.h"

template< typename T, typename TPtr >
T* down_cast( TPtr* const ptr ) {
#ifdef _DEBUG
	assert( dynamic_cast< T* >( ptr ) );
#endif
	return static_cast< T* >( ptr );
}

template< typename T, typename TPtr >
T* down_cast( const std::shared_ptr< TPtr >& ptr ) {
#ifdef _DEBUG
	assert( dynamic_cast< T* >( ptr.get() ) );
#endif
	return static_cast< T* >( ptr.get() );
}

// RenderInterface to DX11 enum wrappers

DXGI_FORMAT GetDXGIFormat( const RenderInterface::Format format ) {
	switch ( format ) {
	case RenderInterface::Format::UNKNOWN:							return DXGI_FORMAT_UNKNOWN;
	case RenderInterface::Format::R32G32B32A32_FLOAT:				return DXGI_FORMAT_R32G32B32A32_FLOAT;
	case RenderInterface::Format::R32G32B32A32_UINT:				return DXGI_FORMAT_R32G32B32A32_UINT;
	case RenderInterface::Format::R32G32B32_FLOAT:					return DXGI_FORMAT_R32G32B32_FLOAT;
	case RenderInterface::Format::R32G32B32_UINT:					return DXGI_FORMAT_R32G32B32_UINT;
	case RenderInterface::Format::R32G32_FLOAT:						return DXGI_FORMAT_R32G32_FLOAT;
	case RenderInterface::Format::R32G32_UINT:						return DXGI_FORMAT_R32G32_UINT;
	case RenderInterface::Format::R32_FLOAT:						return DXGI_FORMAT_R32_FLOAT;
	case RenderInterface::Format::R32_UINT:							return DXGI_FORMAT_R32_UINT;
	case RenderInterface::Format::R16G16B16A16_FLOAT:				return DXGI_FORMAT_R16G16B16A16_FLOAT;
	case RenderInterface::Format::R16G16B16A16_UINT:				return DXGI_FORMAT_R16G16B16A16_UINT;
	case RenderInterface::Format::R16G16B16A16_UNORM:				return DXGI_FORMAT_R16G16B16A16_UNORM;
	case RenderInterface::Format::R16G16B16A16_SINT:				return DXGI_FORMAT_R16G16B16A16_SINT;
	case RenderInterface::Format::R16G16B16A16_SNORM:				return DXGI_FORMAT_R16G16B16A16_SNORM;
	case RenderInterface::Format::R16G16_FLOAT:						return DXGI_FORMAT_R16G16_FLOAT;
	case RenderInterface::Format::R16G16_UINT:						return DXGI_FORMAT_R16G16_UINT;
	case RenderInterface::Format::R16G16_UNORM:						return DXGI_FORMAT_R16G16_UNORM;
	case RenderInterface::Format::R16G16_SINT:						return DXGI_FORMAT_R16G16_SINT;
	case RenderInterface::Format::R16G16_SNORM:						return DXGI_FORMAT_R16G16_SNORM;
	case RenderInterface::Format::R16_FLOAT:						return DXGI_FORMAT_R16_FLOAT;
	case RenderInterface::Format::R16_UINT:							return DXGI_FORMAT_R16_UINT;
	case RenderInterface::Format::R16_UNORM:						return DXGI_FORMAT_R16_UNORM;
	case RenderInterface::Format::R16_SINT:							return DXGI_FORMAT_R16_SINT;
	case RenderInterface::Format::R16_SNORM:						return DXGI_FORMAT_R16_SNORM;
	case RenderInterface::Format::R8G8B8A8_UINT:					return DXGI_FORMAT_R8G8B8A8_UINT;
	case RenderInterface::Format::R8G8B8A8_UNORM:					return DXGI_FORMAT_R8G8B8A8_UNORM;
	case RenderInterface::Format::R8G8B8A8_SINT:					return DXGI_FORMAT_R8G8B8A8_SINT;
	case RenderInterface::Format::R8G8B8A8_SNORM:					return DXGI_FORMAT_R8G8B8A8_SNORM;
	case RenderInterface::Format::R8G8_UINT:						return DXGI_FORMAT_R8G8_UINT;
	case RenderInterface::Format::R8G8_UNORM:						return DXGI_FORMAT_R8G8_UNORM;
	case RenderInterface::Format::R8G8_SINT:						return DXGI_FORMAT_R8G8_SINT;
	case RenderInterface::Format::R8G8_SNORM:						return DXGI_FORMAT_R8G8_SNORM;
	case RenderInterface::Format::R8_UINT:							return DXGI_FORMAT_R8_UINT;
	case RenderInterface::Format::R8_UNORM:							return DXGI_FORMAT_R8_UNORM;
	case RenderInterface::Format::R8_SINT:							return DXGI_FORMAT_R8_SINT;
	case RenderInterface::Format::R8_SNORM:							return DXGI_FORMAT_R8_SNORM;
	case RenderInterface::Format::DEPTH_24_UNORM_STENCIL_8_UINT:	return DXGI_FORMAT_D24_UNORM_S8_UINT;
	case RenderInterface::Format::BC1:								return DXGI_FORMAT_BC1_UNORM;
	case RenderInterface::Format::BC3:								return DXGI_FORMAT_BC3_UNORM;
	}
	return DXGI_FORMAT_UNKNOWN;
}

D3D11_COMPARISON_FUNC GetD3D11ComparsionFunc( const RenderInterface::DepthStencilComparsion dsc ) {
	switch ( dsc ) {
	case RenderInterface::DepthStencilComparsion::NEVER:			return D3D11_COMPARISON_NEVER;
	case RenderInterface::DepthStencilComparsion::LESS:				return D3D11_COMPARISON_LESS;
	case RenderInterface::DepthStencilComparsion::EQUAL:			return D3D11_COMPARISON_EQUAL;
	case RenderInterface::DepthStencilComparsion::LESS_EQUAL:		return D3D11_COMPARISON_LESS_EQUAL;
	case RenderInterface::DepthStencilComparsion::GREATER:			return D3D11_COMPARISON_GREATER;
	case RenderInterface::DepthStencilComparsion::NOT_EQUAL:		return D3D11_COMPARISON_NOT_EQUAL;
	case RenderInterface::DepthStencilComparsion::GREATER_EQUAL:	return D3D11_COMPARISON_GREATER_EQUAL;
	case RenderInterface::DepthStencilComparsion::ALWAYS:			return D3D11_COMPARISON_ALWAYS;
	}
	return D3D11_COMPARISON_NEVER;
}

D3D11_STENCIL_OP GetD3D11StencilOp( const RenderInterface::StencilOperation op ) {
	switch ( op ) {
	case RenderInterface::StencilOperation::KEEP:		return D3D11_STENCIL_OP_KEEP;
	case RenderInterface::StencilOperation::ZERO:		return D3D11_STENCIL_OP_ZERO;
	case RenderInterface::StencilOperation::REPLACE:	return D3D11_STENCIL_OP_REPLACE;
	case RenderInterface::StencilOperation::INCR_SAT:	return D3D11_STENCIL_OP_INCR_SAT;
	case RenderInterface::StencilOperation::DECR_SAT:	return D3D11_STENCIL_OP_DECR_SAT;
	case RenderInterface::StencilOperation::INVERT:		return D3D11_STENCIL_OP_INVERT;
	case RenderInterface::StencilOperation::INCR:		return D3D11_STENCIL_OP_INCR;
	case RenderInterface::StencilOperation::DECR:		return D3D11_STENCIL_OP_DECR;
	}
	return D3D11_STENCIL_OP_KEEP;
}

D3D11_TEXTURE_ADDRESS_MODE GetD3D11TextureAddressMode( const RenderInterface::TextureAddressing addressing ) {
	switch ( addressing ) {
	case RenderInterface::TextureAddressing::WRAP:		return D3D11_TEXTURE_ADDRESS_WRAP;
	case RenderInterface::TextureAddressing::MIRROR:	return D3D11_TEXTURE_ADDRESS_MIRROR;
	case RenderInterface::TextureAddressing::CLAMP:		return D3D11_TEXTURE_ADDRESS_CLAMP;
	}
	return D3D11_TEXTURE_ADDRESS_WRAP;
}

D3D11_FILTER GetD3D11Filter( const RenderInterface::TextureFilter filter ) {
	switch ( filter ) {
	case RenderInterface::TextureFilter::POINT:								return D3D11_FILTER_MIN_MAG_MIP_POINT;
	case RenderInterface::TextureFilter::POINT_MIP_LINEAR:					return D3D11_FILTER_MIN_MAG_POINT_MIP_LINEAR;
	case RenderInterface::TextureFilter::MIN_POINT_MAG_LINEAR_MIP_POINT:	return D3D11_FILTER_MIN_POINT_MAG_LINEAR_MIP_POINT;
	case RenderInterface::TextureFilter::MIN_POINT_MAG_MIP_LINEAR:			return D3D11_FILTER_MIN_POINT_MAG_MIP_LINEAR;
	case RenderInterface::TextureFilter::MIN_LINEAR_MAG_MIP_POINT:			return D3D11_FILTER_MIN_LINEAR_MAG_MIP_POINT;
	case RenderInterface::TextureFilter::MIN_LINEAR_MAG_POINT_MIP_LINEAR:	return D3D11_FILTER_MIN_LINEAR_MAG_POINT_MIP_LINEAR;
	case RenderInterface::TextureFilter::LINEAR_MIP_POINT:					return D3D11_FILTER_MIN_MAG_LINEAR_MIP_POINT;
	case RenderInterface::TextureFilter::LINEAR:							return D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	case RenderInterface::TextureFilter::ANISOTROPIC:						return D3D11_FILTER_ANISOTROPIC;
	}
	return D3D11_FILTER_MIN_MAG_MIP_POINT;
}

D3D11_USAGE GetD3D11Usage( const RenderInterface::BufferUsage usage ) {
	switch ( usage ) {
	case RenderInterface::BufferUsage::DRAW:		return D3D11_USAGE_DEFAULT;
	case RenderInterface::BufferUsage::STATIC:		return D3D11_USAGE_IMMUTABLE;
	case RenderInterface::BufferUsage::DYNAMIC:		return D3D11_USAGE_DYNAMIC;
	case RenderInterface::BufferUsage::COPY:		return D3D11_USAGE_STAGING;
	}
	return D3D11_USAGE_STAGING;
}

D3D11_BLEND GetD3D11Blend( const RenderInterface::Blend blend ) {
	switch ( blend ) {
	case RenderInterface::Blend::ZERO:				return D3D11_BLEND_ZERO;
	case RenderInterface::Blend::ONE:				return D3D11_BLEND_ONE;
	case RenderInterface::Blend::SRC_COLOR:			return D3D11_BLEND_SRC_COLOR;
	case RenderInterface::Blend::INV_SRC_COLOR:		return D3D11_BLEND_INV_SRC_COLOR;
	case RenderInterface::Blend::SRC_ALPHA:			return D3D11_BLEND_SRC_ALPHA;
	case RenderInterface::Blend::INV_SRC_ALPHA:		return D3D11_BLEND_INV_SRC_ALPHA;
	case RenderInterface::Blend::DEST_ALPHA:		return D3D11_BLEND_DEST_ALPHA;
	case RenderInterface::Blend::INV_DEST_ALPHA:	return D3D11_BLEND_INV_DEST_ALPHA;
	case RenderInterface::Blend::DEST_COLOR:		return D3D11_BLEND_DEST_COLOR;
	case RenderInterface::Blend::INV_DEST_COLOR:	return D3D11_BLEND_INV_DEST_COLOR;
	case RenderInterface::Blend::SRC_ALPHA_SAT:		return D3D11_BLEND_SRC_ALPHA_SAT;
	case RenderInterface::Blend::BLEND_FACTOR:		return D3D11_BLEND_BLEND_FACTOR;
	case RenderInterface::Blend::INV_BLEND_FACTOR:	return D3D11_BLEND_INV_BLEND_FACTOR;
	case RenderInterface::Blend::SRC1_COLOR:		return D3D11_BLEND_SRC1_COLOR;
	case RenderInterface::Blend::INV_SRC1_COLOR:	return D3D11_BLEND_INV_SRC1_COLOR;
	case RenderInterface::Blend::SRC1_ALPHA:		return D3D11_BLEND_SRC1_ALPHA;
	case RenderInterface::Blend::INV_SRC1_ALPHA:	return D3D11_BLEND_INV_SRC1_ALPHA;
	}
	return D3D11_BLEND_ZERO;
}

D3D11_BLEND_OP GetD3D11BlendOp( const RenderInterface::BlendOp op ) {
	switch ( op ) {
	case RenderInterface::BlendOp::ADD:				return D3D11_BLEND_OP_ADD;
	case RenderInterface::BlendOp::SUBTRACT:		return D3D11_BLEND_OP_SUBTRACT;
	case RenderInterface::BlendOp::REV_SUBTRACT:	return D3D11_BLEND_OP_REV_SUBTRACT;
	case RenderInterface::BlendOp::MIN:				return D3D11_BLEND_OP_MIN;
	case RenderInterface::BlendOp::MAX:				return D3D11_BLEND_OP_MAX;
	}
	return D3D11_BLEND_OP_ADD;
}

D3D11_PRIMITIVE_TOPOLOGY GetD3D11PrimitiveTopology( const RenderInterface::PrimitiveTopology topology ) {
	switch ( topology ) {
	case RenderInterface::PrimitiveTopology::DEFAULT:		return D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
	case RenderInterface::PrimitiveTopology::POINTLIST:		return D3D11_PRIMITIVE_TOPOLOGY_POINTLIST;
	case RenderInterface::PrimitiveTopology::LINELIST:		return D3D11_PRIMITIVE_TOPOLOGY_LINELIST;
	case RenderInterface::PrimitiveTopology::LINESTRIP:		return D3D11_PRIMITIVE_TOPOLOGY_LINESTRIP;
	case RenderInterface::PrimitiveTopology::TRIANGLELIST:	return D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
	case RenderInterface::PrimitiveTopology::TRIANGLESTRIP:	return D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP;
	}
	return D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
}

UINT GetD3D11CpuAccess( const RenderInterface::BufferAccess access ) {
	UINT flags = 0;
	if ( static_cast< unsigned int >( access ) & static_cast< unsigned int >( RenderInterface::BufferAccess::READ ) ) {
		flags |= D3D11_CPU_ACCESS_READ;
	}
	if ( static_cast< unsigned int >( access ) & static_cast< unsigned int >( RenderInterface::BufferAccess::WRITE ) ) {
		flags |= D3D11_CPU_ACCESS_WRITE;
	}
	return flags;
}

// Device

Directx11RenderInterface::Device::Device() {}

Directx11RenderInterface::Device::~Device() {
	context = nullptr;
	device = nullptr;
}

bool Directx11RenderInterface::Device::Create( const ComPtr< IDXGIAdapter1 >& adapter, const D3D_FEATURE_LEVEL featureLevel ) noexcept {
	if ( adapter == nullptr ) {
		return false;
	}
	HRESULT hresult = 0;
	D3D_FEATURE_LEVEL createdFeatureLevel;

#ifdef _DEBUG
	UINT flags = D3D11_CREATE_DEVICE_DEBUG;
#else
	UINT flags = 0
#endif

	// create device
	ComPtr< ID3D11Device > device;
	ComPtr< ID3D11DeviceContext > context;
	hresult = D3D11CreateDevice(
		adapter.Raw(),
		D3D_DRIVER_TYPE_HARDWARE,
		NULL,
		flags,
		&featureLevel,
		1,
		D3D11_SDK_VERSION,
		&device,
		&createdFeatureLevel,
		&context
	);
	if ( FAILED( hresult ) ) {
		return false;
	}
	// ulozit objekty
	this->device = device;
	this->context = context;
	return true;
}

RenderInterface::PSwapChain Directx11RenderInterface::Device::CreateSwapChain( WindowsWindow* const window ) noexcept {
	std::shared_ptr< Directx11RenderInterface::SwapChain > swapChain( new( std::nothrow ) Directx11RenderInterface::SwapChain() );
	if ( swapChain == nullptr ) {
		return nullptr;
	}
	if ( !swapChain->Create( device, window ) ) {
		return nullptr;
	}
	return swapChain;
}

RenderInterface::PBuffer Directx11RenderInterface::Device::CreateTextureBuffer( const RenderInterface::TextureBufferParams& params ) noexcept {
	/*
	Pokud ma parametr samplesQuality hodnotu MAX_MULTISAMPLE_QUALITY,
	prevest na platnou hodnotu max quality.
	*/
	RenderInterface::TextureBufferParams validatedParams = params;
	if ( params.samplesQuality == RenderInterface::MAX_MULTISAMPLE_QUALITY ) {
		validatedParams.samplesQuality = GetMaxMultisampleQuality( params.samplesCount );
	}
	std::shared_ptr< Directx11RenderInterface::TextureBuffer > buffer( new( std::nothrow ) Directx11RenderInterface::TextureBuffer() );
	if ( buffer == nullptr ) {
		return nullptr;
	}
	if ( !buffer->Create( device, validatedParams ) ) {
		return nullptr;
	}
	return buffer;
}

RenderInterface::PBuffer CreateBuffer( const ComPtr< ID3D11Device >& device, const RenderInterface::BufferType type, const RenderInterface::BufferParams& params ) noexcept {
	std::shared_ptr< Directx11RenderInterface::Buffer > buffer( new( std::nothrow ) Directx11RenderInterface::Buffer() );
	if ( buffer == nullptr ) {
		return nullptr;
	}
	if ( !buffer->Create( device, type, params ) ) {
		return nullptr;
	}
	return buffer;
}

RenderInterface::PBuffer Directx11RenderInterface::Device::CreateVertexBuffer( const RenderInterface::BufferParams& params ) noexcept {
	return CreateBuffer( device, RenderInterface::BufferType::VERTEX_BUFFER, params );
}

RenderInterface::PBuffer Directx11RenderInterface::Device::CreateIndexBuffer( const RenderInterface::BufferParams& params ) noexcept {
	return CreateBuffer( device, RenderInterface::BufferType::INDEX_BUFFER, params );
}

RenderInterface::PBuffer Directx11RenderInterface::Device::CreateConstantBuffer( const RenderInterface::BufferParams& params ) noexcept {
	return CreateBuffer( device, RenderInterface::BufferType::CONSTANT_BUFFER, params );
}

RenderInterface::PRenderTargetView Directx11RenderInterface::Device::CreateRenderTargetView( const RenderInterface::PBuffer& textureBuffer ) noexcept {
	std::shared_ptr< RenderTargetView > view( new( std::nothrow ) RenderTargetView() );
	if ( view == nullptr ) {
		return nullptr;
	}
	if ( !view->Create( device, textureBuffer ) ) {
		return nullptr;
	}
	return view;
}

RenderInterface::PTextureView Directx11RenderInterface::Device::CreateTextureView( const RenderInterface::PBuffer& textureBuffer, const RenderInterface::PSampler& sampler ) noexcept {
	std::shared_ptr< TextureView > view( new( std::nothrow ) TextureView() );
	if ( view == nullptr ) {
		return nullptr;
	}
	if ( !view->Create( device, textureBuffer ) ) {
		return nullptr;
	}
	return view;
}

RenderInterface::PDepthStencilView Directx11RenderInterface::Device::CreateDepthStencilView( const RenderInterface::PBuffer& textureBuffer, const bool readonly ) noexcept {
	std::shared_ptr< DepthStencilView > view( new( std::nothrow ) DepthStencilView() );
	if ( view == nullptr ) {
		return nullptr;
	}
	if ( !view->Create( device, textureBuffer, readonly ) ) {
		return nullptr;
	}
	return view;
}

RenderInterface::PConstantBufferView Directx11RenderInterface::Device::CreateConstantBufferView( const RenderInterface::PBuffer& constantBuffer, const RenderInterface::ConstantBufferViewParams& params ) noexcept {
	std::shared_ptr< ConstantBufferView > view( new( std::nothrow ) ConstantBufferView() );
	if ( view == nullptr ) {
		return nullptr;
	}
	if ( !view->Create( constantBuffer, params ) ) {
		return nullptr;
	}
	return view;
}

RenderInterface::PVertexStream Directx11RenderInterface::Device::CreateVertexStream( const RenderInterface::VertexStreamParams& params ) noexcept {
	std::shared_ptr< VertexStream > stream( new( std::nothrow ) VertexStream() );
	if ( stream == nullptr ) {
		return nullptr;
	}
	if ( !stream->Create( params ) ) {
		return nullptr;
	}
	return stream;
}

RenderInterface::PCommandInterface Directx11RenderInterface::Device::CreateCommandInterface() noexcept {
	std::shared_ptr< CommandInterface > commandInterface( new( std::nothrow ) CommandInterface() );
	if ( commandInterface == nullptr ) {
		return nullptr;
	}
	if ( !commandInterface->Create() ) {
		return nullptr;
	}
	return commandInterface;
}

RenderInterface::PShader Directx11RenderInterface::Device::CreateShader( const RenderInterface::ShaderParams& params ) noexcept {
	std::shared_ptr< Shader > shader( new( std::nothrow ) Shader() );
	if ( shader == nullptr ) {
		return nullptr;
	}
	if ( !shader->Compile( device, params ) ) {
		return nullptr;
	}
	return shader;
}

RenderInterface::PRenderProgram Directx11RenderInterface::Device::CreateRenderProgram( const RenderInterface::PShader& vs, const RenderInterface::PShader& ps, const RenderInterface::PShader& gs ) noexcept {
	std::shared_ptr< RenderProgram > program( new( std::nothrow ) RenderProgram() );
	if ( program == nullptr ) {
		return nullptr;
	}
	if ( !program->Create( vs, ps, gs ) ) {
		return nullptr;
	}
	return program;
}

RenderInterface::PBlendState Directx11RenderInterface::Device::CreateBlendState( const RenderInterface::BlendStateParams& params ) noexcept {
	std::shared_ptr< Directx11RenderInterface::BlendState > state( new( std::nothrow ) Directx11RenderInterface::BlendState() );
	if ( state == nullptr ) {
		return nullptr;
	}
	if ( !state->Create( device, params ) ) {
		return nullptr;
	}
	return state;
}

RenderInterface::PRasterizerState Directx11RenderInterface::Device::CreateRasterizerState( const RenderInterface::RasterizerStateParams& params ) noexcept {
	std::shared_ptr< Directx11RenderInterface::RasterizerState > state( new( std::nothrow ) Directx11RenderInterface::RasterizerState() );
	if ( state == nullptr ) {
		return nullptr;
	}
	if ( !state->Create( device, params ) ) {
		return nullptr;
	}
	return state;
}

RenderInterface::PDepthStencilState Directx11RenderInterface::Device::CreateDepthStencilState( const RenderInterface::DepthStencilStateParams& params ) noexcept {
	std::shared_ptr< Directx11RenderInterface::DepthStencilState > state( new( std::nothrow ) Directx11RenderInterface::DepthStencilState() );
	if ( state == nullptr ) {
		return nullptr;
	}
	if ( !state->Create( device, params ) ) {
		return nullptr;
	}
	return state;
}

RenderInterface::PSampler Directx11RenderInterface::Device::CreateSampler( const RenderInterface::SamplerParams& params ) noexcept {
	std::shared_ptr< Directx11RenderInterface::Sampler > sampler( new( std::nothrow ) Directx11RenderInterface::Sampler() );
	if ( sampler == nullptr ) {
		return nullptr;
	}
	if ( !sampler->Create( device, params ) ) {
		return nullptr;
	}
	return sampler;
}

RenderInterface::PVertexLayout Directx11RenderInterface::Device::CreateVertexLayout( const RenderInterface::VertexAttribute* const attributes, const int attributesCount, const RenderInterface::PRenderProgram& program ) noexcept {
	std::shared_ptr< Directx11RenderInterface::VertexLayout > layout( new( std::nothrow ) Directx11RenderInterface::VertexLayout() );
	if ( layout == nullptr ) {
		return nullptr;
	}
	if ( !layout->Create( device, attributes, attributesCount, program ) ) {
		return nullptr;
	}
	return layout;
}

int Directx11RenderInterface::Device::GetMaxMultisampleQuality( const int samplesCount) const noexcept {
	int levels = 0;
	device->CheckMultisampleQualityLevels( BACK_BUFFER_FORMAT, samplesCount, reinterpret_cast< unsigned int* >( &levels ) );
	return levels;
}

ID3D11DeviceContext* Directx11RenderInterface::Device::GetD3D11DeviceContext() noexcept {
	return context.Raw();
}

// DX11SwapChain

Directx11RenderInterface::SwapChain::SwapChain() {
	window = nullptr;
	width = 0;
	height = 0;
}

Directx11RenderInterface::SwapChain::~SwapChain() {
	swapChain = nullptr;
}

bool Directx11RenderInterface::SwapChain::Create( const ComPtr< ID3D11Device >& device, Window* const window ) noexcept {
	const HWND hwnd = reinterpret_cast< HWND >( window->GetHandle() );
	HRESULT hresult = 0;

	ComPtr< IDXGIFactory1 > factory;
	hresult = CreateDXGIFactory1( __uuidof( IDXGIFactory1 ), reinterpret_cast< void** >( &factory ) );
	if ( FAILED( hresult ) ) {
		return nullptr;
	}
	
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
	desc.OutputWindow						= hwnd;
	desc.Windowed							= TRUE;
	desc.SwapEffect							= DXGI_SWAP_EFFECT_DISCARD;
	desc.BufferDesc.Scaling					= DXGI_MODE_SCALING_STRETCHED;
	desc.Flags								= DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;

	ComPtr< IDXGISwapChain > swapChain;
	hresult = factory->CreateSwapChain( device.Raw(), &desc, &swapChain );
	if ( FAILED( hresult ) ) {
		return false;
	}

	// create render target view
	
	ComPtr< ID3D11Texture2D > backBuffer;
	hresult = swapChain->GetBuffer( 0, __uuidof( ID3D11Texture2D ), ( LPVOID* )&backBuffer );
	if ( FAILED( hresult ) ) {
		return false;
	}
	auto rtv = std::shared_ptr< Directx11RenderInterface::RenderTargetView >( new( std::nothrow ) Directx11RenderInterface::RenderTargetView );
	if ( rtv == nullptr ) {
		return false;
	}
	if ( !rtv->Create( device, backBuffer.Raw() ) ) {
		return false;
	}

	// vypnuti defaultniho prepinani do fullscreenu
	factory->MakeWindowAssociation( hwnd, DXGI_MWA_NO_ALT_ENTER  );

	// ulozit vysledek
	this->window = window;
	this->swapChain = swapChain;
	this->rtv = rtv;
	this->width = window->GetClientWidth();
	this->height = window->GetClientHeight();

	return true;
}

RenderInterface::PRenderTargetView Directx11RenderInterface::SwapChain::AcquireRenderTargetView() noexcept {
	return rtv;
}

void Directx11RenderInterface::SwapChain::Present( const RenderInterface::PRenderTargetView& rtv, const RenderInterface::SwapChainPresentMode presentMode ) noexcept {
	if ( presentMode == RenderInterface::SwapChainPresentMode::IMMEDIATE ) {
		swapChain->Present( 0, 0 );
		return;
	}
	if ( presentMode == RenderInterface::SwapChainPresentMode::VSYNC ) {
		swapChain->Present( 1, 0 );
		return;
	}
}

void Directx11RenderInterface::SwapChain::SetFullscreen( Display* const display ) noexcept {
	// ...
}

int Directx11RenderInterface::SwapChain::GetWidth() const noexcept {
	return width;
}

int Directx11RenderInterface::SwapChain::GetHeight() const noexcept {
	return height;
}

bool Directx11RenderInterface::SwapChain::Valid() const noexcept {
	if ( window == nullptr ) {
		return false;
	}
	return width == window->GetClientWidth() && height == window->GetClientHeight();
}

/*
void DX11Display::SetWindowedMode() noexcept {
	//Pri vstupu do fullscreen rezimu je ulozen ukazatel na objekt Window.
	//Pokud je nyni tento ukazatel nullptr, okno neni ve fullscreen rezimu.
	if ( window == nullptr ) {
		return;
	}
	BackBuffer* const backBuffer = window->GetBackBuffer();
	if ( backBuffer == nullptr ) {
		return;
	}
	// ziskat ukazatel na swap chain
	ASSERT_DOWNCAST( backBuffer, DX11BackBuffer );
	IDXGISwapChain* const swapChain = static_cast< DX11BackBuffer* >( backBuffer )->GetDXGISwapChain();
	if ( swapChain == nullptr ) {
		return;
	}
	swapChain->SetFullscreenState( FALSE, NULL );
	window = nullptr;
}

void DX11Display::SetFullscreenMode( const DisplayMode& mode, Window& window ) noexcept {
	BackBuffer* const backBuffer = window.GetBackBuffer();
	if ( backBuffer == nullptr ) {
		return;
	}
	// ziskat swap chain back bufferu
	ASSERT_DOWNCAST( backBuffer, DX11BackBuffer );
	IDXGISwapChain* const swapChain = static_cast< DX11BackBuffer* >( backBuffer )->GetDXGISwapChain();
	if ( swapChain == nullptr ) {
		return;
	}
	// najit odpovidajici rezim
	DisplayMode validMode;
	FindMode( mode, validMode );

	// DXGI mode
	DXGI_MODE_DESC desc;
	ZeroMemory( &desc, sizeof( desc ) );
	desc.Width						= static_cast< UINT >( mode.width );
	desc.Height						= static_cast< UINT >( mode.height );
	desc.RefreshRate.Numerator		= static_cast< UINT >( mode.refreshRateNumerator );
	desc.RefreshRate.Denominator	= static_cast< UINT >( mode.refreshRateDenominator );
	desc.Format						= DXGI_FORMAT_UNKNOWN;
	desc.ScanlineOrdering			= DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
	desc.Scaling					= DXGI_MODE_SCALING_STRETCHED;

	// prepnout do rezimu cele obrazovky
	swapChain->ResizeTarget( &desc );
	swapChain->SetFullscreenState( TRUE, dxgiOutput );
	swapChain->ResizeTarget( &desc );

	this->window = &window;
}

void DX11BackBuffer::Resize() noexcept {
	const int targetWidth = window->GetClientWidth();
	const int targetHeight = window->GetClientHeight();

	DXGI_SWAP_CHAIN_DESC desc;
	dxgiSwapChain->GetDesc( &desc );
	
	HRESULT hresult = dxgiSwapChain->ResizeBuffers(
		desc.BufferCount,
		static_cast< UINT >( width ),
		static_cast< UINT >( height ),
		DXGI_FORMAT_UNKNOWN,
		desc.Flags
	);
	if ( FAILED( hresult ) ) {
		return;
	}
	width = targetWidth;
	height = targetWidth;
}
*/

// DX11TextureBuffer

Directx11RenderInterface::TextureBuffer::TextureBuffer() {
	format = RenderInterface::Format::UNKNOWN;
	width = 0;
	height = 0;
	depth = 0;
	mipLevels = 0;
	arraySize = 0;
	samplesCount = 0;
	samplesQuality = 0;
}

bool Directx11RenderInterface::TextureBuffer::Create( const ComPtr< ID3D11Device >& device, const RenderInterface::TextureBufferParams& params ) noexcept {
	const D3D11_USAGE usage = GetD3D11Usage( params.usage );
	const UINT CPUAccessFlags = GetD3D11CpuAccess( params.access );

	UINT bindFlags = D3D11_BIND_SHADER_RESOURCE;

	// set render target bind flag
	if ( params.flags & RenderInterface::TextureBufferFlags::TEXTURE_BUFFER_FLAG_RENDER_TARGET ) {
		bindFlags |= D3D11_BIND_RENDER_TARGET;
	}
	// set depth stencil bind flag
	if ( params.format == RenderInterface::Format::DEPTH_24_UNORM_STENCIL_8_UINT ) {
		bindFlags |= D3D11_BIND_DEPTH_STENCIL;
	}
	// D3D11 subresource initial data temporary
	std::unique_ptr< D3D11_SUBRESOURCE_DATA[] > subresources( nullptr );
	
	// initialize subresources temporary
	if ( params.data != nullptr ) {
		const int subresourcesCount = params.arraySize * params.mipLevels;
		subresources.reset( new( std::nothrow ) D3D11_SUBRESOURCE_DATA[ subresourcesCount ] );

		const auto formatInfo = GetFormatInfo( params.format );
		int subresourceIndex = 0;

		// for each texture in texture array
		for ( int arrayIndex = 0; arrayIndex < params.arraySize; arrayIndex++ ) {
			int mipWidth = params.width;
			int mipHeight = params.height;

			// for each mip
			for ( int mipIndex = 0; mipIndex < params.mipLevels; mipIndex++ ) {
				// set subresource
				const int rowPitch = ( formatInfo.blockByteWidth / formatInfo.blockSize ) * mipWidth;
				subresources[ subresourceIndex ].pSysMem = params.data[ subresourceIndex ];
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
	if ( params.type == RenderInterface::BufferType::TEXTURE_1D ||
		 params.type == RenderInterface::BufferType::TEXTURE_1D_ARRAY
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
		SetBuffer( texture, params );
		return true;
	}

	// 2D texture
	if ( params.type == RenderInterface::BufferType::TEXTURE_2D ||
		 params.type == RenderInterface::BufferType::TEXTURE_2D_ARRAY  ||
		 params.type == RenderInterface::BufferType::TEXTURE_2D_MS ||
		 params.type == RenderInterface::BufferType::TEXTURE_2D_MS_ARRAY
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
		SetBuffer( texture, params );
		return true;
	}

	// 3D texture
	if ( params.type == RenderInterface::BufferType::TEXTURE_3D ) {
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
		SetBuffer( texture, params );
		return true;
	}

	// nepodporovany typ bufferu
	return false;
}

void Directx11RenderInterface::TextureBuffer::SetBuffer( ComPtr< ID3D11Resource > resource, const RenderInterface::TextureBufferParams& params ) noexcept {
	// ulozit atributy bufferu (validace rozmeru textury, hodnota nesmi byt < 1)
	format = params.format;
	width = Math::Max( params.width, 1 );
	height = Math::Max( params.height, 1 );
	depth = Math::Max( params.depth, 1 );
	mipLevels = params.mipLevels;
	arraySize = params.arraySize;
	samplesCount = params.samplesCount;
	samplesQuality = params.samplesQuality;

	// vypocitat velikost bufferu (rozhrani ID3DTextureXD neposkytuje informaci o velikosti alokovane pameti pro textury)
	
	const auto formatInfo = GetFormatInfo( format );
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
	// ulozit buffer
	info.type = params.type;
	info.size = mipSliceSize * arraySize;
	info.usage = params.usage;
	info.access = params.access;
	buffer = resource;
}

bool Directx11RenderInterface::TextureBuffer::Map( const ComPtr< ID3D11DeviceContext >& context, const int subresource, const D3D11_MAP mapType, RenderInterface::MappedBuffer& result ) noexcept {
	D3D11_MAPPED_SUBRESOURCE mappedSubresource;
	HRESULT hresult = context->Map( buffer.Raw(), static_cast< UINT >( subresource ), mapType, 0, &mappedSubresource );
	if ( FAILED( hresult ) ) {
		ZeroMemory( &result, sizeof( RenderInterface::MappedBuffer ) );
		return false;
	}
	RenderInterface::TextureDimmensions dimmensions;
	GetMipDimmensions( width, height, depth, subresource % mipLevels, dimmensions );
	auto formatInfo = GetFormatInfo( format );

	result.data = mappedSubresource.pData;
	result.rowPitch = static_cast< int >( mappedSubresource.RowPitch );
	result.depthPitch = static_cast< int >( mappedSubresource.DepthPitch );
	result.subresource = subresource;
	result.rowByteWidth = ( dimmensions.width / formatInfo.blockSize ) * formatInfo.blockByteWidth;
	result.rowsCount = ( dimmensions.width / formatInfo.blockSize ) * ( dimmensions.height / formatInfo.blockSize ) * dimmensions.depth;
	result.depthsCount = dimmensions.depth;
	return true;
}

RenderInterface::Format Directx11RenderInterface::TextureBuffer::GetFormat() const noexcept {
	return format;
}

int Directx11RenderInterface::TextureBuffer::GetWidth() const noexcept {
	return width;
}

int Directx11RenderInterface::TextureBuffer::GetHeight() const noexcept {
	return height;
}

int Directx11RenderInterface::TextureBuffer::GetDepth() const noexcept {
	return depth;
}

int Directx11RenderInterface::TextureBuffer::GetMipLevels() const noexcept {
	return mipLevels;
}

int Directx11RenderInterface::TextureBuffer::GetArraySize() const noexcept {
	return arraySize;
}

int Directx11RenderInterface::TextureBuffer::GetSamplesCount() const noexcept {
	return samplesCount;
}

int Directx11RenderInterface::TextureBuffer::GetSamplesQuality() const noexcept {
	return samplesQuality;
}

RenderInterface::BufferType Directx11RenderInterface::TextureBuffer::GetType() const noexcept {
	return info.type;
}

int Directx11RenderInterface::TextureBuffer::GetSize() const noexcept {
	return info.size;
}

RenderInterface::BufferUsage Directx11RenderInterface::TextureBuffer::GetUsage() const noexcept {
	return info.usage;
}

RenderInterface::BufferAccess Directx11RenderInterface::TextureBuffer::GetAccess() const noexcept {
	return info.access;
}

int Directx11RenderInterface::TextureBuffer::GetSubresourcesCount() const noexcept {
	return mipLevels * arraySize;
}

ID3D11Resource* Directx11RenderInterface::TextureBuffer::GetD3D11Resource() noexcept {
	return buffer.Raw();
}

// DX11Buffer

Directx11RenderInterface::Buffer::Buffer() {}

Directx11RenderInterface::Buffer::~Buffer() {
	buffer = nullptr;
}

bool Directx11RenderInterface::Buffer::Create( const ComPtr< ID3D11Device >& device, const RenderInterface::BufferType type, const RenderInterface::BufferParams& params ) noexcept {
	D3D11_BUFFER_DESC bufferDesc;
	bufferDesc.ByteWidth			= static_cast< UINT >( params.size );
	bufferDesc.Usage				= GetD3D11Usage( params.usage );
	bufferDesc.BindFlags			= 0;
	bufferDesc.CPUAccessFlags		= GetD3D11CpuAccess( params.access );
	bufferDesc.MiscFlags			= 0;
	bufferDesc.StructureByteStride	= 0;

	// bind flags
	switch ( type ) {
	case RenderInterface::BufferType::VERTEX_BUFFER:	bufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;	break;
	case RenderInterface::BufferType::INDEX_BUFFER:		bufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;		break;
	case RenderInterface::BufferType::CONSTANT_BUFFER:	bufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;	break;
	}
	// initial subresource data 
	D3D11_SUBRESOURCE_DATA* pData = NULL;
	D3D11_SUBRESOURCE_DATA data;
	if ( params.data != nullptr ) {
		data.pSysMem = params.data;
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
	// store buffer
	info.type = type;
	info.size = params.size;
	info.usage = params.usage;
	info.access = params.access;
	this->buffer = buffer;
	return true;
}

bool Directx11RenderInterface::Buffer::Map( const ComPtr< ID3D11DeviceContext >& context, const int subresource, const D3D11_MAP mapType, RenderInterface::MappedBuffer& result ) noexcept {
	D3D11_MAPPED_SUBRESOURCE mappedSubresource;
	HRESULT hresult = context->Map( buffer.Raw(), 0, mapType, 0, &mappedSubresource );
	if ( FAILED( hresult ) ) {
		ZeroMemory( &result, sizeof( RenderInterface::MappedBuffer ) );
		return false;
	}
	result.data = mappedSubresource.pData;
	result.rowPitch = static_cast< int >( mappedSubresource.RowPitch );
	result.depthPitch = static_cast< int >( mappedSubresource.DepthPitch );
	result.subresource = 0;
	result.rowByteWidth = GetSize();
	result.rowsCount = 1;
	result.depthsCount = 1;
	return true;
}

ID3D11Buffer* Directx11RenderInterface::Buffer::GetD3D11Buffer() noexcept {
	return buffer.Raw();
}

RenderInterface::BufferType Directx11RenderInterface::Buffer::GetType() const noexcept {
	return info.type;
}

int Directx11RenderInterface::Buffer::GetSize() const noexcept {
	return info.size;
}

RenderInterface::BufferUsage Directx11RenderInterface::Buffer::GetUsage() const noexcept {
	return info.usage;
}

RenderInterface::BufferAccess Directx11RenderInterface::Buffer::GetAccess() const noexcept {
	return info.access;
}

int Directx11RenderInterface::Buffer::GetSubresourcesCount() const noexcept {
	return 0;
}

// DX11RenderTargetView

Directx11RenderInterface::RenderTargetView::RenderTargetView() {}

Directx11RenderInterface::RenderTargetView::~RenderTargetView() {
	view = nullptr;
}

bool Directx11RenderInterface::RenderTargetView::Create( const ComPtr< ID3D11Device >& device, const RenderInterface::PBuffer& textureBuffer ) noexcept {
	if ( device == nullptr || textureBuffer == nullptr ) {
		return false;
	}
	auto buffer = down_cast< Directx11RenderInterface::TextureBuffer>( textureBuffer );

	ComPtr< ID3D11RenderTargetView > view;
	HRESULT hresult = device->CreateRenderTargetView ( buffer->GetD3D11Resource(), NULL, &view );
	if ( FAILED( hresult ) ) {
		return false;
	}
	this->view = view;
	return true;
}

bool Directx11RenderInterface::RenderTargetView::Create( const ComPtr< ID3D11Device >& device, ID3D11Texture2D* const backBuffer ) noexcept {
	if ( device == nullptr || backBuffer == nullptr ) {
		return false;
	}
	ComPtr< ID3D11RenderTargetView > d3d11View;
	HRESULT hresult = device->CreateRenderTargetView( backBuffer, NULL, &d3d11View );
	if ( FAILED( hresult ) ) {
		return false;
	}
	this->view = view;
	return true;
}

ID3D11RenderTargetView* Directx11RenderInterface::RenderTargetView::GetD3D11RenderTargetView() noexcept {
	return view.Raw();
}

// DX11TextureView

Directx11RenderInterface::TextureView::TextureView() {}

Directx11RenderInterface::TextureView::~TextureView() {
	view = nullptr;
}

bool Directx11RenderInterface::TextureView::Create( const ComPtr< ID3D11Device >& device, const RenderInterface::PBuffer& textureBuffer ) noexcept {
	if ( device == nullptr || textureBuffer == nullptr ) {
		return false;
	}
	auto* const resource = down_cast< Directx11RenderInterface::TextureBuffer >( textureBuffer )->GetD3D11Resource();
	ComPtr< ID3D11ShaderResourceView > view;
	HRESULT hresult = device->CreateShaderResourceView( resource, NULL, &view );
	if ( FAILED( hresult ) ) {
		return false;
	}
	this->view = view;
	return true;
}

ID3D11ShaderResourceView* Directx11RenderInterface::TextureView::GetD3D11ShaderResourceView() noexcept {
	return view.Raw();
}

// DX11DepthStencilView

Directx11RenderInterface::DepthStencilView::DepthStencilView() {}

Directx11RenderInterface::DepthStencilView::~DepthStencilView() {
	view = nullptr;
}

bool Directx11RenderInterface::DepthStencilView::Create( const ComPtr< ID3D11Device >& device, const RenderInterface::PBuffer& textureBuffer, const bool readonly ) noexcept {
	if ( device == nullptr || textureBuffer == nullptr ) {
		return false;
	}
	Directx11RenderInterface::Buffer* buffer = down_cast< Directx11RenderInterface::Buffer >( textureBuffer.get() );
	const auto bufferType = buffer->GetType();

	// check buffer type
	if ( bufferType != RenderInterface::BufferType::TEXTURE_2D &&
		 bufferType != RenderInterface::BufferType::TEXTURE_2D_MS
	) {
		return false;
	}
	// format must be DEPTH_24_UNORM_STENCIL_8_UINT
	const auto format = down_cast< Directx11RenderInterface::TextureBuffer >( textureBuffer )->GetFormat();
	if ( format != RenderInterface::Format::DEPTH_24_UNORM_STENCIL_8_UINT ) {
		return false;
	}
	ID3D11Resource* const texture = static_cast< Directx11RenderInterface::TextureBuffer* >( textureBuffer.get() )->GetD3D11Resource();

	D3D11_DEPTH_STENCIL_VIEW_DESC viewDesc;
	ZeroMemory( &viewDesc, sizeof( viewDesc ) );
	viewDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;

	// readonly flags
	if ( readonly ) {
		viewDesc.Flags |= D3D11_DSV_READ_ONLY_DEPTH;
		viewDesc.Flags |= D3D11_DSV_READ_ONLY_STENCIL;
	}
	// texture 2D view dimension
	if ( bufferType == RenderInterface::BufferType::TEXTURE_2D ) {
		viewDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
		viewDesc.Texture2D.MipSlice = 0;

	// multisampled texture 2D view dimension
	} else if ( bufferType == RenderInterface::BufferType::TEXTURE_2D_MS ) {
		viewDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2DMS;
	} 
	// create view
	ComPtr< ID3D11DepthStencilView > view;
	HRESULT hresult = device->CreateDepthStencilView( texture, &viewDesc, &view );
	if ( FAILED( hresult )  ) {
		return false;
	}
	this->view = view;
	return true;
}

ID3D11DepthStencilView* Directx11RenderInterface::DepthStencilView::GetD3D11DepthStencilView() noexcept {
	return view.Raw();
}

// DX11ConstantBufferView

const int UNUSED_CBUFFER_SLOT = RenderInterface::MAX_CBUFFER_SLOTS;

Directx11RenderInterface::ConstantBufferView::ConstantBufferView() {
	constantsCount = 0;
	constantsSize = 0;
	vsSlot = 0;
	psSlot = 0;
	gsSlot = 0;
}

Directx11RenderInterface::ConstantBufferView::~ConstantBufferView() {
	buffer = nullptr;
}

/*
Nekontroluje signaturu konstant!
*/
bool Directx11RenderInterface::ConstantBufferView::Create( const RenderInterface::PBuffer& constantBuffer, const RenderInterface::ConstantBufferViewParams &params ) noexcept {
	if ( constantBuffer == nullptr ) {
		return false;
	}
	HRESULT hresult = 0;
	auto* const program = down_cast< Directx11RenderInterface::RenderProgram >( params.program );

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
		const RenderInterface::ShaderConstant& constant = params.constants[ i ];

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
	this->buffer = down_cast< Directx11RenderInterface::Buffer >( constantBuffer )->GetD3D11Buffer();
	this->buffer->AddRef();
	this->constantsCount = params.constantsCount;
	this->constantsSize = offset;
	vsSlot = slots[ VS ];
	psSlot = slots[ PS ];
	gsSlot = slots[ GS ];

	return true;
}

void Directx11RenderInterface::ConstantBufferView::UpdateConstants( const void* const src, void* const dest ) const noexcept {
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

ID3D11Buffer* Directx11RenderInterface::ConstantBufferView::GetD3D11Buffer() noexcept {
	return buffer.Raw();
}

int Directx11RenderInterface::ConstantBufferView::GetVSSlot() const noexcept {
	return vsSlot;
}

int Directx11RenderInterface::ConstantBufferView::GetPSSlot() const noexcept {
	return psSlot;
}

int Directx11RenderInterface::ConstantBufferView::GetGSSlot() const noexcept {
	return gsSlot;
}

// DXShader

Directx11RenderInterface::Shader::Shader() {
	type = RenderInterface::ShaderType::UNDEFINED;
	version = RenderInterface::ShaderVersion::UNDEFINED;
}

Directx11RenderInterface::Shader::~Shader() {
	shader = nullptr;
	code = nullptr;
}

bool Directx11RenderInterface::Shader::Compile( const ComPtr< ID3D11Device >& device, const RenderInterface::ShaderParams& params ) noexcept {
	if ( device == nullptr ) {
		return false;
	}
	if ( params.version != RenderInterface::ShaderVersion::HLSL_50_GLSL_430 ) {
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
	case RenderInterface::ShaderType::VERTEX_SHADER:	target = "vs_5_0"; break;
	case RenderInterface::ShaderType::PIXEL_SHADER:		target = "ps_5_0"; break;
	case RenderInterface::ShaderType::GEOMETRY_SHADER:	target = "gs_5_0"; break;
	}
	// flags
	UINT flags = 0;
	if ( params.flags & RenderInterface::SHADER_COMPILE_FLAG_WARNINGS_AS_ERRRORS ) {
		flags |= D3DCOMPILE_WARNINGS_ARE_ERRORS;
	}
	if ( params.flags & RenderInterface::SHADER_COMPILE_FLAG_DEBUG ) {
		flags |= RenderInterface::SHADER_COMPILE_FLAG_DEBUG;
	}
	// optimization flags
	switch ( params.optimization ) {
	case RenderInterface::ShaderOptimization::DISABLED:	flags |= D3DCOMPILE_SKIP_OPTIMIZATION;   break;
	case RenderInterface::ShaderOptimization::LOW:		flags |= D3DCOMPILE_OPTIMIZATION_LEVEL0; break;
	case RenderInterface::ShaderOptimization::HIGH:		flags |= D3DCOMPILE_OPTIMIZATION_LEVEL3; break;
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

	ComPtr< ID3D11DeviceChild > shader;
	
	// VS
	if ( params.type == RenderInterface::ShaderType::VERTEX_SHADER ) {
		ComPtr< ID3D11VertexShader > vs;
		device->CreateVertexShader( code->GetBufferPointer(), code->GetBufferSize(), NULL, &vs );
		shader = std::move( vs );

	// PS
	} else if ( params.type == RenderInterface::ShaderType::PIXEL_SHADER ) {
		ComPtr< ID3D11PixelShader > ps;
		device->CreatePixelShader( code->GetBufferPointer(), code->GetBufferSize(), NULL, &ps );
		shader = std::move( ps );

	// GS
	} else if ( params.type == RenderInterface::ShaderType::GEOMETRY_SHADER ) {
		ComPtr< ID3D11GeometryShader > gs;
		device->CreateGeometryShader( code->GetBufferPointer(), code->GetBufferSize(), NULL, &gs );
		shader = std::move( gs );
	}
	if ( shader == nullptr ) {
		return false;
	}
	// ulozit vysledek
	this->code = code;
	this->shader = shader;
	this->type = params.type;
	this->version = params.version;

	return true;
}

RenderInterface::ShaderType Directx11RenderInterface::Shader::GetType() const noexcept {
	return type;
}

RenderInterface::ShaderVersion Directx11RenderInterface::Shader::GetVersion() const noexcept {
	return version;
}

ID3DBlob* Directx11RenderInterface::Shader::GetBlob() noexcept {
	return code.Raw();
}

ID3D11VertexShader* Directx11RenderInterface::Shader::GetD3D11VertexShader() noexcept {
	if ( type != RenderInterface::ShaderType::VERTEX_SHADER ) {
		return nullptr;
	}
	return static_cast< ID3D11VertexShader* >( shader.Raw() );
}

ID3D11PixelShader* Directx11RenderInterface::Shader::GetD3D11PixelShader() noexcept {
	if ( type != RenderInterface::ShaderType::PIXEL_SHADER ) {
		return nullptr;
	}
	return static_cast< ID3D11PixelShader* >( shader.Raw() );
}

ID3D11GeometryShader* Directx11RenderInterface::Shader::GetD3D11GeometryShader() noexcept {
	if ( type != RenderInterface::ShaderType::GEOMETRY_SHADER ) {
		return nullptr;
	}
	return static_cast< ID3D11GeometryShader* >( shader.Raw() );
}

// DX11RenderProgram

Directx11RenderInterface::RenderProgram::RenderProgram() {}

Directx11RenderInterface::RenderProgram::~RenderProgram() {
	vs = nullptr;
	ps = nullptr;
	gs = nullptr;
	vsByteCode = nullptr;
	psByteCode = nullptr;
	gsByteCode = nullptr;
}

bool Directx11RenderInterface::RenderProgram::Create( const RenderInterface::PShader& vs, const RenderInterface::PShader& ps, const RenderInterface::PShader& gs ) noexcept {
	// vertex shader a pixel shader musi byt vzdy definovan
	if ( vs == nullptr || ps == nullptr ) {
		return false;
	}
	// kontrola typu shaderu
	if ( vs->GetType() != RenderInterface::ShaderType::VERTEX_SHADER ) {
		return false;
	}
	if ( ps->GetType() != RenderInterface::ShaderType::PIXEL_SHADER ) {
		return false;
	}
	if ( gs != nullptr ) {
		if ( gs->GetType() != RenderInterface::ShaderType::GEOMETRY_SHADER ) {
			return false;
		}
	}
	// ulozit vertex shader
	this->vs = down_cast< Directx11RenderInterface::Shader >( vs )->GetD3D11VertexShader();
	this->vsByteCode = down_cast< Directx11RenderInterface::Shader >( vs )->GetBlob();

	// ulozit pixel shader
	this->ps = down_cast< Directx11RenderInterface::Shader >( ps )->GetD3D11PixelShader();
	this->psByteCode = down_cast< Directx11RenderInterface::Shader >( ps )->GetBlob();

	// ulozit geometry shader
	if ( gs != nullptr ) {
		this->gs = down_cast< Directx11RenderInterface::Shader >( gs )->GetD3D11GeometryShader();
		this->gsByteCode = down_cast< Directx11RenderInterface::Shader >( gs )->GetBlob();
	}
	return true;
}

ID3D11VertexShader* Directx11RenderInterface::RenderProgram::GetD3D11VertexShader() noexcept {
	return vs.Raw();
}

ID3D11PixelShader* Directx11RenderInterface::RenderProgram::GetD3D11PixelShader() noexcept {
	return ps.Raw();
}

ID3D11GeometryShader* Directx11RenderInterface::RenderProgram::GetD3D11GeometryShader() noexcept {
	return gs.Raw();
}

ID3DBlob* Directx11RenderInterface::RenderProgram::GetVertexShaderByteCode() noexcept {
	return vsByteCode.Raw();
}

ID3DBlob* Directx11RenderInterface::RenderProgram::GetPixelShaderByteCode() noexcept {
	return psByteCode.Raw();
}

ID3DBlob* Directx11RenderInterface::RenderProgram::GetGeometryShaderByteCode() noexcept {
	return gsByteCode.Raw();
}

// DX11Sampler

Directx11RenderInterface::Sampler::Sampler() {}

Directx11RenderInterface::Sampler::~Sampler() {
	sampler = nullptr;
}

bool Directx11RenderInterface::Sampler::Create( const ComPtr< ID3D11Device >& device, const RenderInterface::SamplerParams& params ) noexcept {
	if ( device == nullptr ) {
		return false;
	}
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
	samplerDesc.MaxLOD			= ( params.maxLOD == RenderInterface::MAX_TEXTURE_LOD ? D3D11_FLOAT32_MAX : params.maxLOD );

	ComPtr< ID3D11SamplerState > sampler;
	HRESULT hresult = device->CreateSamplerState( &samplerDesc, &sampler );
	if ( FAILED( hresult ) ) {
		return false;
	}
	this->sampler = sampler;
	return true;
}

ID3D11SamplerState* Directx11RenderInterface::Sampler::GetD3D11SamplerState() noexcept {
	return sampler.Raw();
}

// DX11BlendState

Directx11RenderInterface::BlendState::BlendState() {}

Directx11RenderInterface::BlendState::~BlendState() {
	state = nullptr;
}

bool Directx11RenderInterface::BlendState::Create( const ComPtr< ID3D11Device >& device, const RenderInterface::BlendStateParams& params ) noexcept {
	if ( device == nullptr ) {
		return false;
	}
	D3D11_BLEND_DESC desc;
	ZeroMemory( &desc, sizeof( desc ) );
	desc.AlphaToCoverageEnable = ( params.alphaToCoverage ? TRUE : FALSE );
	desc.IndependentBlendEnable = ( params.uniformBlending ? FALSE : TRUE );

	for ( int i = 0; i < RenderInterface::MAX_RENDER_TARGETS; i++ ) {
		const RenderInterface::RenderTargetBlend& rtParam = params.renderTargets[ i ];
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
	this->state = state;
	return true;
}

ID3D11BlendState* Directx11RenderInterface::BlendState::GetD3D11BlendState() noexcept {
	return state.Raw();
}

// DX11RasterizerState

Directx11RenderInterface::RasterizerState::RasterizerState() {}

Directx11RenderInterface::RasterizerState::~RasterizerState() {
	state = nullptr;
}

bool Directx11RenderInterface::RasterizerState::Create( const ComPtr< ID3D11Device >& device, const RenderInterface::RasterizerStateParams& params ) noexcept {
	if ( device == nullptr ) {
		return false;
	}
	D3D11_RASTERIZER_DESC desc;
	// culling
	switch ( params.cullMode ) {
	case RenderInterface::CullMode::DISABLED:	desc.CullMode = D3D11_CULL_NONE;  break;
	case RenderInterface::CullMode::FRONT_FACE:	desc.CullMode = D3D11_CULL_FRONT; break;
	case RenderInterface::CullMode::BACK_FACE:	desc.CullMode = D3D11_CULL_BACK;  break;
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
	this->state = state;
	return true;
}

ID3D11RasterizerState* Directx11RenderInterface::RasterizerState::GetD3D11RasterizerState() noexcept {
	return state.Raw();
}

// DX11DepthStencilState

Directx11RenderInterface::DepthStencilState::DepthStencilState() {}

Directx11RenderInterface::DepthStencilState::~DepthStencilState() {
	state = nullptr;
}

bool Directx11RenderInterface::DepthStencilState::Create( const ComPtr< ID3D11Device >& device, const RenderInterface::DepthStencilStateParams& params ) noexcept {
	if ( device == nullptr ) {
		return false;
	}
	D3D11_DEPTH_STENCIL_DESC stateDesc;
	stateDesc.DepthWriteMask				= D3D11_DEPTH_WRITE_MASK_ALL;
	stateDesc.DepthEnable					= TRUE;
	stateDesc.DepthFunc						= GetD3D11ComparsionFunc( params.depthFunc );
	stateDesc.StencilEnable					= FALSE;
	stateDesc.StencilReadMask				= 0;
	stateDesc.StencilWriteMask				= 0;
	stateDesc.FrontFace.StencilFunc			= GetD3D11ComparsionFunc( params.stencilFunc );
	stateDesc.FrontFace.StencilPassOp		= GetD3D11StencilOp( params.stencilPassOp );
	stateDesc.FrontFace.StencilFailOp		= GetD3D11StencilOp( params.stencilFailOp );
	stateDesc.FrontFace.StencilDepthFailOp	= GetD3D11StencilOp( params.stencilDepthFailOp );
	stateDesc.BackFace						= stateDesc.FrontFace;

	// depth usage
	if ( params.depthUsage == RenderInterface::DepthStencilUsage::STANDARD ) {
		stateDesc.DepthEnable = TRUE;
		stateDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;

	} else if ( params.depthUsage == RenderInterface::DepthStencilUsage::DISABLED ) {
		stateDesc.DepthEnable = FALSE;
		stateDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ZERO;

	} else if ( params.depthUsage == RenderInterface::DepthStencilUsage::READONLY ) {
		stateDesc.DepthEnable = TRUE;
		stateDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ZERO;
	}
	// stencil usage
	if ( params.stencilUsage == RenderInterface::DepthStencilUsage::STANDARD ) {
		stateDesc.StencilEnable = TRUE;
		stateDesc.StencilReadMask = D3D11_DEFAULT_STENCIL_READ_MASK;
		stateDesc.StencilWriteMask = D3D11_DEFAULT_STENCIL_WRITE_MASK;

	} else if ( params.stencilUsage == RenderInterface::DepthStencilUsage::DISABLED ) {
		stateDesc.StencilEnable = FALSE;
		stateDesc.StencilReadMask = 0;
		stateDesc.StencilWriteMask = 0;

	} else if ( params.stencilUsage == RenderInterface::DepthStencilUsage::READONLY ) {
		stateDesc.StencilEnable = TRUE;
		stateDesc.StencilReadMask = D3D11_DEFAULT_STENCIL_READ_MASK;
		stateDesc.StencilWriteMask = 0;
	}
	// create state
	ComPtr< ID3D11DepthStencilState > state;
	HRESULT hresult = device->CreateDepthStencilState( &stateDesc, &state );
	if ( FAILED( hresult )  ) {
		return false;
	}
	this->state = state;
	return true;
}

ID3D11DepthStencilState* Directx11RenderInterface::DepthStencilState::GetD3D11DepthStencilState() noexcept {
	return state.Raw();
}

// DX11VertexLayout

Directx11RenderInterface::VertexLayout::VertexLayout() {}

Directx11RenderInterface::VertexLayout::~VertexLayout() {
	inputLayout = nullptr;
}

bool Directx11RenderInterface::VertexLayout::Create(
	const ComPtr< ID3D11Device >& device,
	const RenderInterface::VertexAttribute* const attributes,
	const int attributesCount,
	const RenderInterface::PRenderProgram& program
) noexcept {

	if ( device == nullptr || attributes == nullptr || program == nullptr ) {
		return false;
	}
	ID3DBlob* const shaderCode = down_cast< Directx11RenderInterface::RenderProgram >( program )->GetVertexShaderByteCode();
	std::vector< D3D11_INPUT_ELEMENT_DESC > elements;
	elements.reserve( attributesCount );

	// initialize elements
	for ( int attributeIndex = 0; attributeIndex < attributesCount; attributeIndex++ ) {
		const RenderInterface::VertexAttribute& attribute = attributes[ attributeIndex ];
		const auto formatInfo = GetFormatInfo( attribute.format );

		D3D11_INPUT_ELEMENT_DESC desc;
		desc.SemanticName			= attribute.semantic;
		desc.SemanticIndex			= 0; // default value, overwrite later
		desc.Format					= GetDXGIFormat( attribute.format );
		desc.InputSlot				= static_cast< UINT >( attribute.slot );
		desc.AlignedByteOffset		= 0; // default value, overwrite later
		desc.InputSlotClass			= ( attribute.instanceCount == 0 ? D3D11_INPUT_PER_VERTEX_DATA : D3D11_INPUT_PER_INSTANCE_DATA );
		desc.InstanceDataStepRate	= static_cast< UINT >( attribute.instanceCount );

		/*
		Iterate all attribute elements (eg. four-component floats in matrix)
		Calculate SemanticIndex and AlignedByteOffset
		*/
		for ( int i = 0; i < attribute.elementsCount; i++ ) {
			desc.SemanticIndex		= static_cast< UINT >( attribute.semanticIndex + i );
			desc.AlignedByteOffset	= static_cast< UINT >( attribute.offset + i * formatInfo.blockByteWidth );
			elements.push_back( desc );
		}
	}
	// create input layout
	ComPtr< ID3D11InputLayout > inputLayout;
	HRESULT hresult = device->CreateInputLayout(
		elements.data(),
		static_cast< UINT >( elements.size() ),
		shaderCode->GetBufferPointer(),
		static_cast< SIZE_T >( shaderCode->GetBufferSize() ),
		&inputLayout
	);
	if ( FAILED( hresult ) ) {
		return false;
	}
	this->inputLayout = inputLayout;
	return true;
}

ComPtr< ID3D11InputLayout > Directx11RenderInterface::VertexLayout::GetD3D11InputLayout() noexcept {
	return inputLayout;
}

// DX11VertexStream

Directx11RenderInterface::VertexStream::VertexStream() {
	ZeroMemory( vertexBuffersBindArray, sizeof( vertexBuffersBindArray ) );
	indexBuffer = nullptr;
	indexBufferFormat = DXGI_FORMAT_UNKNOWN;
	inputLayout = nullptr;
}

Directx11RenderInterface::VertexStream::~VertexStream() {
	/*
	for ( int i = 0; i < MAX_VERTEX_INPUT_SLOTS; i++ ) {
		if ( vertexBuffers[ i ] != nullptr ) {
			vertexBuffers[ i ]->Release();
		}
	}
	indexBuffer = nullptr;
	inputLayout = nullptr;
	*/
}

bool Directx11RenderInterface::VertexStream::Create( const RenderInterface::VertexStreamParams& params ) noexcept {
	// save index buffer
	if ( params.indexBuffer != nullptr ) {
		if ( params.indexBuffer->GetType() != RenderInterface::BufferType::INDEX_BUFFER ) {
			return false;
		}
		if ( params.indexBufferFormat != RenderInterface::Format::R16_UINT &&
			 params.indexBufferFormat != RenderInterface::Format::R32_UINT
		) {
			return false;
		}
		indexBuffer = down_cast< Directx11RenderInterface::Buffer >( params.indexBuffer )->GetD3D11Buffer();
		indexBufferFormat = GetDXGIFormat( params.indexBufferFormat );
	}
	// save vertex buffers
	for ( int i = 0; i < RenderInterface::MAX_VERTEX_INPUT_SLOTS; i++ ) {
		auto& vertexBuffer = params.vertexBuffers[ i ];
		if ( vertexBuffer == nullptr ) {
			continue;
		}
		if ( vertexBuffer->GetType() != RenderInterface::BufferType::VERTEX_BUFFER ) {
			continue;
		}
		auto buffer = down_cast< Directx11RenderInterface::Buffer >( vertexBuffer )->GetD3D11Buffer();
		vertexBuffers[ i ] = buffer; // AddRef()
		vertexBuffersBindArray[ i ] = buffer;
	}
	// save input layout
	inputLayout = down_cast< Directx11RenderInterface::VertexLayout >( params.vertexLayout )->GetD3D11InputLayout();

	return true;
}

ID3D11Buffer** Directx11RenderInterface::VertexStream::GetVertexBuffers() noexcept {
	return vertexBuffersBindArray;
}

ID3D11Buffer* Directx11RenderInterface::VertexStream::GetIndexBuffer() noexcept {
	return indexBuffer.Raw();
}

ID3D11InputLayout* Directx11RenderInterface::VertexStream::GetD3D11InputLayout() noexcept {
	return inputLayout.Raw();
}

DXGI_FORMAT Directx11RenderInterface::VertexStream::GetIndexFormat() const noexcept {
	return indexBufferFormat;
}

// DX11CommandInterface

Directx11RenderInterface::CommandInterface::CommandInterface() {
	context = nullptr;
	currentInputLayout = nullptr;
	currentVertexShader = nullptr;
	currentPixelShader = nullptr;
	currentGeometryShader = nullptr;
	currentBlendState = nullptr;
	currentDepthStencilState = nullptr;
	currentRasterizerState = nullptr;
}

Directx11RenderInterface::CommandInterface::~CommandInterface() {
	context = nullptr;
}

bool Directx11RenderInterface::CommandInterface::Create() noexcept {
	return true;
}

void Directx11RenderInterface::CommandInterface::Begin( const RenderInterface::PDevice& device ) noexcept {
	context = down_cast< Directx11RenderInterface::Device >( device )->GetD3D11DeviceContext();
	context->ClearState();
	currentInputLayout = nullptr;
	currentVertexShader = nullptr;
	currentPixelShader = nullptr;
	currentGeometryShader = nullptr;
	currentBlendState = nullptr;
	currentDepthStencilState = nullptr;
	currentRasterizerState = nullptr;
}

void Directx11RenderInterface::CommandInterface::Begin( const RenderInterface::PCommandList& commandList ) noexcept {
	// UNIMPLEMENTED
}

void Directx11RenderInterface::CommandInterface::End() noexcept {
	context = nullptr;

	// UNIMPLEMENTED command list!
}

void Directx11RenderInterface::CommandInterface::Flush() noexcept {
	context->Flush();
}

void Directx11RenderInterface::CommandInterface::SetRenderTargets( const RenderInterface::PRenderTargetView* const renderTargets, const int count, const RenderInterface::PDepthStencilView& depthStencilView ) noexcept {
	if ( count > RenderInterface::MAX_RENDER_TARGETS ) {
		return;
	}
	ID3D11RenderTargetView* renderTargetViews[ RenderInterface::MAX_RENDER_TARGETS ] = { NULL };
	for ( int i = 0; i < count; i++ ) {
		renderTargetViews[ i ] = down_cast< Directx11RenderInterface::RenderTargetView >( renderTargets[ i ] )->GetD3D11RenderTargetView();
	}
	ID3D11DepthStencilView* d3d11DepthStencilView = NULL;
	if ( depthStencilView != nullptr ) {
		d3d11DepthStencilView = down_cast< Directx11RenderInterface::DepthStencilView >( depthStencilView )->GetD3D11DepthStencilView();
	}
	context->OMSetRenderTargets( RenderInterface::MAX_RENDER_TARGETS, renderTargetViews, d3d11DepthStencilView );
}

void Directx11RenderInterface::CommandInterface::ClearRenderTarget( const RenderInterface::PRenderTargetView& renderTargetView, const Color& color ) noexcept {
	context->ClearRenderTargetView(
		down_cast< Directx11RenderInterface::RenderTargetView >( renderTargetView )->GetD3D11RenderTargetView(),
		reinterpret_cast< const FLOAT* >( &color )
	);
}

void Directx11RenderInterface::CommandInterface::ClearDepthStencil( const RenderInterface::PDepthStencilView& depthStencilView, const float depth, const uint8_t stencil ) noexcept {
	context->ClearDepthStencilView(
		down_cast< Directx11RenderInterface::DepthStencilView >( depthStencilView )->GetD3D11DepthStencilView(),
		D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL,
		static_cast< FLOAT >( depth ),
		static_cast< UINT8 >( stencil )
	);
}

void Directx11RenderInterface::CommandInterface::ClearDepth( const RenderInterface::PDepthStencilView& depthStencilView, const float depth ) noexcept {
	context->ClearDepthStencilView(
		down_cast< Directx11RenderInterface::DepthStencilView >( depthStencilView )->GetD3D11DepthStencilView(),
		D3D11_CLEAR_DEPTH,
		static_cast< FLOAT >( depth ),
		0
	);
}

void Directx11RenderInterface::CommandInterface::ClearStencil( const RenderInterface::PDepthStencilView& depthStencilView, const uint8_t stencil ) noexcept {
	context->ClearDepthStencilView(
		down_cast< Directx11RenderInterface::DepthStencilView >( depthStencilView )->GetD3D11DepthStencilView(),
		D3D11_CLEAR_STENCIL,
		0,
		static_cast< UINT8 >( stencil )
	);
}

void Directx11RenderInterface::CommandInterface::ClearState() noexcept {
	context->ClearState();
}

bool Directx11RenderInterface::CommandInterface::Map( const RenderInterface::PBuffer& buffer, const int subresource, const RenderInterface::MapPolicy policy, RenderInterface::MappedBuffer& result ) noexcept {
	D3D11_MAP type = D3D11_MAP::D3D11_MAP_READ;
	switch ( policy ) {
	case RenderInterface::MapPolicy::READ_ONLY:		type = D3D11_MAP_READ;			break;
	case RenderInterface::MapPolicy::WRITE_ONLY:	type = D3D11_MAP_WRITE;			break;
	case RenderInterface::MapPolicy::READ_WRITE:	type = D3D11_MAP_READ_WRITE;	break;
	case RenderInterface::MapPolicy::WRITE_DISCARD:	type = D3D11_MAP_WRITE_DISCARD; break;
	}
	return down_cast< Directx11RenderInterface::Buffer >( buffer )->Map( context, subresource, type, result );
}

void Directx11RenderInterface::CommandInterface::Unmap( const RenderInterface::PBuffer& buffer, RenderInterface::MappedBuffer& mappedBuffer ) noexcept {
	mappedBuffer.data = nullptr;
	context->Unmap(
		down_cast< Directx11RenderInterface::Buffer >( buffer )->GetD3D11Buffer(),
		static_cast< UINT >( mappedBuffer.subresource )
	);
}

bool Directx11RenderInterface::CommandInterface::UpdateSubresource( const RenderInterface::PBuffer& buffer, const int subresource, const void* const data ) noexcept {
	// map
	RenderInterface::MapPolicy policy =
		buffer->GetUsage() == RenderInterface::BufferUsage::DYNAMIC ?
		RenderInterface::MapPolicy::WRITE_DISCARD :
		RenderInterface::MapPolicy::WRITE_ONLY;

	RenderInterface::MappedBuffer mappedBuffer;
	if ( !Map( buffer, subresource, policy, mappedBuffer ) ) {
		return false;
	}

	// update

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

bool Directx11RenderInterface::CommandInterface::UpdateBuffer( const RenderInterface::PBuffer& buffer, const void* const data, const int bytes, const int offset, const bool discatd ) noexcept {
	RenderInterface::MapPolicy policy = discatd ?
		RenderInterface::MapPolicy::WRITE_DISCARD :
		RenderInterface::MapPolicy::WRITE_ONLY;
	RenderInterface::MappedBuffer mappedBuffer;
	if ( !Map( buffer, 0, policy, mappedBuffer ) ) {
		return false;
	}
	if ( mappedBuffer.rowsCount > 1 || ( offset + bytes > mappedBuffer.rowByteWidth ) ) {
		Unmap( buffer, mappedBuffer );
		return false;
	}
	memcpy( static_cast< Byte* >( mappedBuffer.data ) + offset, buffer.get(), bytes );
	Unmap( buffer, mappedBuffer );
	return true;
}

bool Directx11RenderInterface::CommandInterface::UpdateConstantBuffer( const RenderInterface::PConstantBufferView& view, const void* const data ) noexcept {
	ID3D11Buffer* const buffer = down_cast< Directx11RenderInterface::ConstantBufferView >( view )->GetD3D11Buffer();
	D3D11_MAPPED_SUBRESOURCE mappedSubresource;
	HRESULT hresult = context->Map( buffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedSubresource );
	if ( FAILED( hresult ) ) {
		return false;
	}
	down_cast< Directx11RenderInterface::ConstantBufferView >( view )->UpdateConstants( data, mappedSubresource.pData );
	context->Unmap( buffer, 0 );
	return true;
}

void Directx11RenderInterface::CommandInterface::CopyBuffer( const RenderInterface::PBuffer& src, const RenderInterface::PBuffer& dest ) noexcept {
	context->CopyResource(
		down_cast< Directx11RenderInterface::Buffer >( dest )->GetD3D11Buffer(),
		down_cast< Directx11RenderInterface::Buffer >( src )->GetD3D11Buffer()
	);
}

void Directx11RenderInterface::CommandInterface::SetConstantBuffers( const RenderInterface::PConstantBufferView* const views, const int count ) noexcept {
	// deaktivovat vsechny sloty
	if ( views == nullptr ) {
		ID3D11Buffer* buffers[ RenderInterface::MAX_CBUFFER_SLOTS ] = { nullptr };
		context->VSSetConstantBuffers( 0, RenderInterface::MAX_CBUFFER_SLOTS, buffers );
		context->PSSetConstantBuffers( 0, RenderInterface::MAX_CBUFFER_SLOTS, buffers );
		context->GSSetConstantBuffers( 0, RenderInterface::MAX_CBUFFER_SLOTS, buffers );
		return;
	}
	// + 1 pro UNUSED_CBUFFER_SLOT
	ID3D11Buffer* vsBuffers[ RenderInterface::MAX_CBUFFER_SLOTS + 1 ] = { nullptr };
	ID3D11Buffer* psBuffers[ RenderInterface::MAX_CBUFFER_SLOTS + 1 ] = { nullptr };
	ID3D11Buffer* gsBuffers[ RenderInterface::MAX_CBUFFER_SLOTS + 1 ] = { nullptr };

	for ( int i = 0; i < count; i++ ) {
		auto view = down_cast< Directx11RenderInterface::ConstantBufferView >( views[ i ].get() );
		vsBuffers[ view->GetVSSlot() ] = view->GetD3D11Buffer();
		psBuffers[ view->GetPSSlot() ] = view->GetD3D11Buffer();
		gsBuffers[ view->GetGSSlot() ] = view->GetD3D11Buffer();
	}
	context->VSSetConstantBuffers( 0, RenderInterface::MAX_CBUFFER_SLOTS, vsBuffers );
	context->PSSetConstantBuffers( 0, RenderInterface::MAX_CBUFFER_SLOTS, psBuffers );
	context->GSSetConstantBuffers( 0, RenderInterface::MAX_CBUFFER_SLOTS, gsBuffers );
}

void Directx11RenderInterface::CommandInterface::SetVertexStream( const RenderInterface::PVertexStream& stream ) noexcept {
	ID3D11InputLayout* const inputLayout = down_cast< Directx11RenderInterface::VertexStream >( stream )->GetD3D11InputLayout();
	if ( currentInputLayout != inputLayout ) {
		context->IASetInputLayout( inputLayout );
		currentInputLayout = inputLayout;
	}
	context->IASetVertexBuffers(
		0,
		RenderInterface::MAX_VERTEX_INPUT_SLOTS,
		down_cast< Directx11RenderInterface::VertexStream >( stream )->GetVertexBuffers(),
		NULL,
		NULL
	);
	context->IASetIndexBuffer(
		down_cast< Directx11RenderInterface::VertexStream >( stream )->GetIndexBuffer(),
		down_cast< Directx11RenderInterface::VertexStream >( stream )->GetIndexFormat(),
		0
	);
}

void Directx11RenderInterface::CommandInterface::SetRenderProgram( const RenderInterface::PRenderProgram& program ) noexcept {
	ID3D11VertexShader* const vertexShader = down_cast< Directx11RenderInterface::RenderProgram >( program )->GetD3D11VertexShader();
	if ( currentVertexShader != vertexShader ) {
		context->VSSetShader( vertexShader, NULL, 0 );
		currentVertexShader = vertexShader;
	}
	ID3D11PixelShader* const pixelShader = down_cast< Directx11RenderInterface::RenderProgram >( program )->GetD3D11PixelShader();
	if ( currentPixelShader != pixelShader ) {
		context->PSSetShader( pixelShader, NULL, 0 );
		currentPixelShader = pixelShader;
	}
	ID3D11GeometryShader* const geometryShader = down_cast< Directx11RenderInterface::RenderProgram >( program )->GetD3D11GeometryShader();
	if ( currentGeometryShader != geometryShader ) {
		context->GSSetShader( geometryShader, NULL, 0 );
		currentGeometryShader = geometryShader;
	}
}

void Directx11RenderInterface::CommandInterface::Draw( const int verticesCount, const int startVertex ) noexcept {
	context->Draw( static_cast< UINT >( verticesCount ), static_cast< UINT >( startVertex ) );
}

void Directx11RenderInterface::CommandInterface::DrawIndexed( const int indicesCount, const int startIndex ) noexcept {
	context->DrawIndexed( static_cast< UINT >( indicesCount ), static_cast< UINT >( startIndex ), 0 );
}

void Directx11RenderInterface::CommandInterface::DrawInstanced( const int verticesCount, const int startVertex, const int instancesCount, const int startInstance ) noexcept {
	context->DrawInstanced(
		static_cast< UINT >( verticesCount ),
		static_cast< UINT >( instancesCount ),
		static_cast< UINT >( startVertex ),
		static_cast< UINT >( startInstance )
	);
}

void Directx11RenderInterface::CommandInterface::DrawIndexedInstanced( const int indicesCount, const int startIndex, const int instancesCount, const int startInstance ) noexcept {
	context->DrawIndexedInstanced(
		static_cast< UINT >( indicesCount ),
		static_cast< UINT >( instancesCount ),
		static_cast< UINT >( startIndex ),
		0,
		static_cast< UINT >( startInstance )
	);
}

void Directx11RenderInterface::CommandInterface::SetPrimitiveTopology( const RenderInterface::PrimitiveTopology topology ) noexcept {
	context->IASetPrimitiveTopology( GetD3D11PrimitiveTopology( topology ) );
}

void Directx11RenderInterface::CommandInterface::SetBlendState( const RenderInterface::PBlendState& state ) noexcept {
	if ( state == nullptr ) {
		if ( currentBlendState != nullptr ) {
			context->OMSetBlendState( NULL, NULL, 0 );
			currentBlendState = nullptr;
		}
		return;
	}
	ID3D11BlendState* const blendState = down_cast< Directx11RenderInterface::BlendState >( state )->GetD3D11BlendState();
	if ( currentBlendState != blendState ) {
		context->OMSetBlendState( blendState, NULL, 0xffffffff );
		currentBlendState = blendState;
	}
}

void Directx11RenderInterface::CommandInterface::SetDepthStencilState( const RenderInterface::PDepthStencilState& state, const uint32_t stencilRef ) noexcept {
	if ( state == nullptr ) {
		if ( currentDepthStencilState != nullptr ) {
			context->OMSetDepthStencilState( NULL, 0 );
			currentDepthStencilState = nullptr;
		}
		return;
	}
	ID3D11DepthStencilState* const depthStencilState = down_cast< Directx11RenderInterface::DepthStencilState >( state )->GetD3D11DepthStencilState();
	if ( currentDepthStencilState != depthStencilState ) {
		context->OMSetDepthStencilState( depthStencilState, static_cast< UINT >( stencilRef ) );
		currentDepthStencilState = depthStencilState;
	}
}

void Directx11RenderInterface::CommandInterface::SetRasterizerState( const RenderInterface::PRasterizerState& state ) noexcept {
	if ( state == nullptr ) {
		if ( currentRasterizerState != nullptr ) {
			context->RSSetState( NULL );
			currentRasterizerState = nullptr;
		}
		return;
	}
	ID3D11RasterizerState* const rasterizerState = down_cast< Directx11RenderInterface::RasterizerState >( state )->GetD3D11RasterizerState();
	if ( currentRasterizerState != rasterizerState ) {
		context->RSSetState( rasterizerState );
		currentRasterizerState = rasterizerState;
	}
}

void Directx11RenderInterface::CommandInterface::SetVSTextures( const int startSlot, const int count, const RenderInterface::PTextureView* const views ) noexcept {
	if ( count + startSlot > RenderInterface::MAX_TEXTURES ) {
		return;
	}
	ID3D11ShaderResourceView* srvs[ RenderInterface::MAX_TEXTURES ];
	for ( int i = 0; i < count; i++ ) {
		srvs[ i ] = down_cast< Directx11RenderInterface::TextureView >( views[ i ] )->GetD3D11ShaderResourceView();
	}
	context->VSSetShaderResources( static_cast< UINT >( startSlot ), static_cast< UINT >( count ), srvs );
}

void Directx11RenderInterface::CommandInterface::SetPSTextures( const int startSlot, const int count, const RenderInterface::PTextureView* const views ) noexcept {
	if ( count + startSlot > RenderInterface::MAX_TEXTURES ) {
		return;
	}
	ID3D11ShaderResourceView* srvs[ RenderInterface::MAX_TEXTURES ];
	for ( int i = 0; i < count; i++ ) {
		srvs[ i ] = down_cast< Directx11RenderInterface::TextureView >( views[ i ] )->GetD3D11ShaderResourceView();
	}
	context->PSSetShaderResources( static_cast< UINT >( startSlot ), static_cast< UINT >( count ), srvs );
}

void Directx11RenderInterface::CommandInterface::SetGSTextures( const int startSlot, const int count, const RenderInterface::PTextureView* const views ) noexcept {
	if ( count + startSlot > RenderInterface::MAX_TEXTURES ) {
		return;
	}
	ID3D11ShaderResourceView* srvs[ RenderInterface::MAX_TEXTURES ];
	for ( int i = 0; i < count; i++ ) {
		srvs[ i ] = down_cast< Directx11RenderInterface::TextureView >( views[ i ] )->GetD3D11ShaderResourceView();
	}
	context->GSSetShaderResources( static_cast< UINT >( startSlot ), static_cast< UINT >( count ), srvs );
}

void FillSamplerStateArray( RenderInterface::Sampler* const samplers[ RenderInterface::MAX_SAMPLERS ], ID3D11SamplerState* result[ RenderInterface::MAX_SAMPLERS ] ) noexcept {
	if ( samplers == nullptr ) {
		ZeroMemory( result, RenderInterface::MAX_SAMPLERS * sizeof( void* ) );
		return;
	}
	for ( int i = 0; i < RenderInterface::MAX_SAMPLERS; i++ ) {
		if ( samplers[ i ] == nullptr ) {
			result[ i ] = NULL;
			continue;
		}
		result[ i ] = down_cast< Directx11RenderInterface::Sampler >( samplers[ i ] )->GetD3D11SamplerState();
	}
}

void Directx11RenderInterface::CommandInterface::SetVSSamplers( RenderInterface::Sampler* const samplers[ RenderInterface::MAX_SAMPLERS ] ) noexcept {
	ID3D11SamplerState* samplerStates[ RenderInterface::MAX_SAMPLERS ];
	FillSamplerStateArray( samplers, samplerStates );
	context->VSSetSamplers( 0, static_cast< UINT >( RenderInterface::MAX_SAMPLERS ), samplerStates );
}

void Directx11RenderInterface::CommandInterface::SetPSSamplers( RenderInterface::Sampler* const samplers[ RenderInterface::MAX_SAMPLERS ] ) noexcept {
	ID3D11SamplerState* samplerStates[ RenderInterface::MAX_SAMPLERS ];
	FillSamplerStateArray( samplers, samplerStates );
	context->PSSetSamplers( 0, static_cast< UINT >( RenderInterface::MAX_SAMPLERS ), samplerStates );
}

void Directx11RenderInterface::CommandInterface::SetGSSamplers( RenderInterface::Sampler* const samplers[ RenderInterface::MAX_SAMPLERS ] ) noexcept {
	ID3D11SamplerState* samplerStates[ RenderInterface::MAX_SAMPLERS ];
	FillSamplerStateArray( samplers, samplerStates );
	context->GSSetSamplers( 0, static_cast< UINT >( RenderInterface::MAX_SAMPLERS ), samplerStates );
}

void Directx11RenderInterface::CommandInterface::SetViewports( const RenderInterface::Viewport* const viewports[], const int count ) noexcept {
	if ( count > RenderInterface::MAX_VIEWPORTS ) {
		return;
	}
	D3D11_VIEWPORT d3d11Viewports[ RenderInterface::MAX_VIEWPORTS ];
	for ( int i = 0; i < count; i++ ) {
		d3d11Viewports[ i ].TopLeftX	= viewports[ i ]->x;
		d3d11Viewports[ i ].TopLeftY	= viewports[ i ]->y;
		d3d11Viewports[ i ].Width		= viewports[ i ]->width;
		d3d11Viewports[ i ].Height		= viewports[ i ]->height;
		d3d11Viewports[ i ].MinDepth	= 0;
		d3d11Viewports[ i ].MaxDepth	= 1;
	}
	context->RSSetViewports( static_cast< UINT >( count ), d3d11Viewports );
}

void Directx11RenderInterface::CommandInterface::SetScissorRects( const RenderInterface::ScissorRect* rects, const int count ) noexcept {
	D3D11_RECT scissorRects[ RenderInterface::MAX_VIEWPORTS ];
	const int rectsCount = Math::Min( count, RenderInterface::MAX_VIEWPORTS );
	for ( int i = 0; i < rectsCount; i++ ) {
		scissorRects[ i ].left = static_cast< LONG >( rects[ i ].x );
		scissorRects[ i ].top = static_cast< LONG >( rects[ i ].y );
		scissorRects[ i ].right = scissorRects[ i ].left + static_cast< LONG >( rects[ i ].width );
		scissorRects[ i ].bottom = scissorRects[ i ].top + static_cast< LONG >( rects[ i ].height );
	}
	context->RSSetScissorRects( static_cast< UINT >( rectsCount ), scissorRects );
}
