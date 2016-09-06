#include "Framework/Core.h"
#include "Platform/Window.h"
#include "Platform/File.h"
#include "Engine/Paths.h"
#include "Renderer.h"

// G-Buffers
enum {
	GBUFFER_COLOR,		// (non-multisampled) render pass render target (resolved result)
	GBUFFER_DIFUSE,		// (multisampled) RGBA difuse slozka, pouziva se take jako druhy buffer u postrpocesingu
	GBUFFER_NORMAL,		// (multisampled) normal buffer
	GBUFFER_SPECULAR,	// (multisampled)
	GBUFFER_COVERAGE,	// (non-multisampled) coverage map for msaa
	GBUFFERS_COUNT
};

// depth stencil states
enum {
	DEPTH_STENCIL_STATE_NO_DEPTH_NO_STENCIL,
	DEPTH_STENCIL_STATE_LIGHT_PASS,
	DEPTH_STENCIL_STATES_COUNT
};

/*
// Viewport renderbufferViewport;
// FramebufferViewport renderbufferViewport;
// engine poskytuje pevnou sadu texture sampleru, defaultni je mozne modifikovat nastavenim
enum {
	SAMPLER_DEFAULT,
	SAMPLER_POINT,
	SAMPLER_LINEAR,
	SAMPLERS_COUNT
};
RenderInterface::Sampler *samplers[ SAMPLERS_COUNT ];
*/

// Renderer::RenderBuffer

bool Renderer::RenderBuffer::Create(
	std::shared_ptr< RenderInterface::Device > device,
	const RenderInterface::Format format,
	const int width,
	const int height,
	const int samplesCount,
	const int samplesQuality
) {
	// buffer je nejdriv uvolnen, pri selhani zustane null
	Release();

	RenderInterface::TextureBufferParams params;
	params.type				= ( samplesCount == 1 ? RenderInterface::BufferType::TEXTURE_2D : RenderInterface::BufferType::TEXTURE_2D_MS );
	params.format			= format;
	params.usage			= RenderInterface::BufferUsage::DRAW;
	params.access			= RenderInterface::BufferAccess::NONE;
	params.width			= width;
	params.height			= height;
	params.depth			= 1;
	params.mipLevels		= 1;
	params.arraySize		= 1;
	params.samplesCount		= samplesCount;
	params.samplesQuality	= samplesQuality;
	params.flags			= RenderInterface::TEXTURE_BUFFER_FLAG_RENDER_TARGET;

	auto buffer = device->CreateTextureBuffer( params, nullptr );
	if ( !buffer ) {
		return false;
	}
	auto textureView = device->CreateTextureView( buffer.get(), nullptr );
	if ( !textureView ) {
		return false;
	}
	auto renderTargetView = device->CreateRenderTargetView( buffer.get() );
	if ( !renderTargetView ) {
		return false;
	}
	this->buffer = std::move( buffer );
	this->textureView = std::move( textureView );
	this->renderTargetView = std::move( renderTargetView );
	return true;
}

void Renderer::RenderBuffer::Release() {
	renderTargetView = nullptr;
	textureView = nullptr;
	buffer = nullptr;
}

RenderInterface::TextureView* Renderer::RenderBuffer::GetTextureView() {
	return textureView.get();
}

RenderInterface::RenderTargetView* Renderer::RenderBuffer::GetRenderTargetView() {
	return renderTargetView.get();
}

// Renderer::DepthStencilBuffer

bool Renderer::DepthStencilBuffer::Create(
	std::shared_ptr< RenderInterface::Device > device,
	const int width,
	const int height,
	const int samplesCount,
	const int samplesQuality
) {
	// buffer je nejdriv uvolnen, pri selhani zustane null
	Release();

	// depth stencil buffer
	RenderInterface::TextureBufferParams params;
	params.type				= ( samplesCount == 1 ? RenderInterface::BufferType::TEXTURE_2D : RenderInterface::BufferType::TEXTURE_2D_MS );
	params.format			= RenderInterface::Format::DEPTH_24_UNORM_STENCIL_8_UINT;
	params.usage			= RenderInterface::BufferUsage::DRAW;
	params.access			= RenderInterface::BufferAccess::NONE;
	params.width			= width;
	params.height			= height;
	params.depth			= 1;
	params.mipLevels		= 1;
	params.arraySize		= 1;
	params.samplesCount		= samplesCount;
	params.samplesQuality	= samplesQuality;
	params.flags			= static_cast< RenderInterface::TextureBufferFlags >( 0 );

	auto buffer = device->CreateTextureBuffer( params, nullptr );
	if ( !buffer ) {
		return false;
	}
	auto view = device->CreateDepthStencilView( buffer.get(), false );
	if ( !view ) {
		return false;
	}
	this->buffer = std::move( buffer );
	this->view = std::move( view );
	return true;
}

void Renderer::DepthStencilBuffer::Release() {
	buffer = nullptr;
	view = nullptr;
}

RenderInterface::DepthStencilView* Renderer::DepthStencilBuffer::GetDepthStencilView() {
	return view.get();
}

// Renderer

Renderer::Renderer() {
	memset( &viewport, 0, sizeof( viewport ) );
}

Renderer::~Renderer() {
}

bool Renderer::Initialize( std::shared_ptr< RenderInterface::Device > device, const RendererAttributes& attributes ) {
	if ( !device ) {
		return false;
	}
	this->device = device;

	// command interface
	this->immediateCommands = device->CreateCommandInterface();
	if ( !this->immediateCommands ) {
		return false;
	}
	// render buffers
	if ( !CreateRenderBuffers( attributes.renderbuffersWidth, attributes.renderbuffersHeight, attributes.antialiasing ) ) {
		return false;
	}
	// shaders
	if ( !LoadShaders() ) {
		return false;
	}
	// vertex layouts
	if ( !CreateVertexLayouts() ) {
		return false;
	}

	this->attributes = attributes;
	return true;
}

bool Renderer::CreateRenderBuffers( const int width, const int height, const Antialiasing antialiasing ) {
	// msaa attributes
	int samplesCount = 1;
	int samplesQuality = 0;

	switch ( antialiasing ) {
	case Antialiasing::MSAA_2:
		samplesCount = 2;
		samplesQuality = RenderInterface::MAX_MULTISAMPLE_QUALITY;
		break;

	case Antialiasing::MSAA_4:
		samplesCount = 4;
		samplesQuality = RenderInterface::MAX_MULTISAMPLE_QUALITY;
		break;

	case Antialiasing::MSAA_8:
		samplesCount = 8;
		samplesQuality = RenderInterface::MAX_MULTISAMPLE_QUALITY;
		break;
	}

	// uvolnit vsechny buffery
	this->renderBuffers.clear();
	this->depthStencilBuffer.Release();

	// vytvorit buffery
	std::vector< RenderBuffer > buffers( GBUFFERS_COUNT );
	if ( !buffers[ GBUFFER_COLOR ].Create( device, RenderInterface::Format::R8G8B8A8_UNORM, width, height, 1, 0 ) ) {
		return false;
	}
	if ( !buffers[ GBUFFER_DIFUSE ].Create( device, RenderInterface::Format::R8G8B8A8_UNORM, width, height, samplesCount, samplesQuality ) ) {
		return false;
	}
	if ( !buffers[ GBUFFER_NORMAL ].Create( device, RenderInterface::Format::R16G16B16A16_FLOAT, width, height, samplesCount, samplesQuality ) ) {
		return false;
	}
	if ( !buffers[ GBUFFER_SPECULAR ].Create( device, RenderInterface::Format::R16G16_FLOAT, width, height, samplesCount, samplesQuality ) ) {
		return false;
	}
	if ( samplesCount > 1 ) {
		if ( !buffers[ GBUFFER_COVERAGE ].Create( device, RenderInterface::Format::R8_UINT, width, height, 1, 0 ) ) {
			return false;
		}
	}
	// depth stencil je take renderbuffer
	DepthStencilBuffer depthStencilBuffer;
	if ( !depthStencilBuffer.Create( device, width, height, samplesCount, samplesQuality ) ) {
		return false;
	}
	// ulozit nove vytvorene buffery
	this->renderBuffers = std::move( buffers );
	this->depthStencilBuffer = std::move( depthStencilBuffer );

	// upravit informace o renderbufferu
	//renderbufferViewport.width = width;
	//renderbufferViewport.height = height;
	attributes.renderbuffersWidth = width;
	attributes.renderbuffersHeight = height;
}

void Renderer::ResizeRenderBuffers( const int width, const int height ) {
	CreateRenderBuffers( width, height, attributes.antialiasing );
}

void Renderer::CreateBackBuffer( const Window& window ) {
	// overit jestli okno neni registrovane
	for ( const TargetWindow& tw : targetWindows ) {
		if ( tw.window == &window ) {
			return;
		}
	}
	auto backBuffer = device->CreateBackBuffer( window );
	if ( !backBuffer ) {
		return;
	}
	auto renderTargetView = device->CreateRenderTargetView( backBuffer.get() );
	if ( !renderTargetView ) {
		return;
	}
	TargetWindow tw;
	tw.window = &window;
	tw.backBuffer = std::move( backBuffer );
	tw.renderTargetView = std::move( renderTargetView );
	targetWindows.push_back( tw );
}

void Renderer::DeleteBackBuffer( const Window& window ) {
	for ( auto iter = targetWindows.begin(); iter != targetWindows.end(); iter++ ) {
		if ( iter->window == &window ) {
			targetWindows.erase( iter );
			return;
		}
	}
}

void Renderer::ResizeBackBuffer( const Window& window ) {
	for ( TargetWindow &tw : targetWindows ) {
		if ( tw.window == &window ) {
			tw.renderTargetView->Release();
			tw.renderTargetView = nullptr;
			tw.backBuffer->Resize();
			tw.renderTargetView = device->CreateRenderTargetView( tw.backBuffer.get() );
			return;
		}
	}
}

void Renderer::SetAntialiasing( const Antialiasing antialiasing ) {
	if ( antialiasing != attributes.antialiasing ) {
		CreateRenderBuffers( attributes.renderbuffersWidth, attributes.renderbuffersHeight, attributes.antialiasing );
		attributes.antialiasing = antialiasing;
	}
}

bool Renderer::CreateDepthStencilStates() {
	depthStencilStates.clear();
	depthStencilStates.resize( DEPTH_STENCIL_STATES_COUNT );

	RenderInterface::DepthStencilStateParams params;

	// no depth, no stencil
	params.depthUsage			= RenderInterface::DepthStencilUsage::DISABLED;
	params.stencilUsage			= RenderInterface::DepthStencilUsage::DISABLED;
	params.depthFunc			= RenderInterface::DepthStencilComparsion::NEVER;
	params.stencilFunc			= RenderInterface::DepthStencilComparsion::NEVER;
	params.stencilPassOp		= RenderInterface::StencilOperation::KEEP;
	params.stencilFailOp		= RenderInterface::StencilOperation::KEEP;
	params.stencilDepthFailOp	= RenderInterface::StencilOperation::KEEP;
	auto noDepthStencil = device->CreateDepthStencilState( params );
	if ( !noDepthStencil ) {
		return false;
	}
	// light pass (depth readonly equal, no stencil)
	params.depthUsage			= RenderInterface::DepthStencilUsage::READONLY;
	params.stencilUsage			= RenderInterface::DepthStencilUsage::DISABLED;
	params.depthFunc			= RenderInterface::DepthStencilComparsion::EQUAL;
	params.stencilFunc			= RenderInterface::DepthStencilComparsion::NEVER;
	params.stencilPassOp		= RenderInterface::StencilOperation::KEEP;
	params.stencilFailOp		= RenderInterface::StencilOperation::KEEP;
	params.stencilDepthFailOp	= RenderInterface::StencilOperation::KEEP;
	auto lightPass = device->CreateDepthStencilState( params );
	if ( !lightPass ) {
		return false;
	}
	// presunout vysledek do objektu
	depthStencilStates[ DEPTH_STENCIL_STATE_NO_DEPTH_NO_STENCIL ] = std::move( noDepthStencil );
	depthStencilStates[ DEPTH_STENCIL_STATE_LIGHT_PASS ] = std::move( lightPass );
	return true;
}

RenderInterface::PShader LoadShaderFromFile( std::shared_ptr< RenderInterface::Device > device, const String& path, const RenderInterface::ShaderType type ) {
	// nacist soubor
	auto source = LoadCharFile( path );
	if ( !source ) {
		return nullptr;
	}
	// parametry kompilace
	RenderInterface::ShaderParams params;
	params.string	= source.get();
	params.defines	= nullptr;
	params.type		= type;
	params.version	= RenderInterface::ShaderVersion::HLSL_50_GLSL_430;
	params.flags	= static_cast< RenderInterface::ShaderCompileFlags >( 0 );

#ifdef _DEBUG
	params.optimization	= RenderInterface::ShaderOptimization::DISABLED;
#else
	params.optimization = RenderInterface::ShaderOptimization::HIGH;
#endif

	return device->CreateShader( params );
}

Identifier Renderer::CreatePixelShader( const char* const source, const RenderInterface::ShaderType type ) {
	RenderInterface::ShaderParams params;
	params.string	= source;
	params.defines	= nullptr;
	params.type		= type;
	params.version	= RenderInterface::ShaderVersion::HLSL_50_GLSL_430;
	params.flags	= static_cast< RenderInterface::ShaderCompileFlags >( 0 );

#ifdef _DEBUG
	params.optimization = RenderInterface::ShaderOptimization::DISABLED;
#else
	params.optimization = RenderInterface::ShaderOptimization::HIGH;
#endif

	// vytvorit shader
	auto shader = device->CreateShader( params );
	if ( !shader ) {
		return SHADER_INVALID;
	}
	// ulozit shader a vratit index jako identifikator
	if ( shaders.capacity() == shaders.size() ) {
		shaders.reserve( shaders.capacity() + 128 );
	}
	shaders.push_back( std::move( shader ) );
	return static_cast< Identifier >( shaders.size() ) - 1;
}

bool Renderer::LoadShaders() {
	struct ShaderInfo {
		int id;
		const char16_t* const file;
		RenderInterface::ShaderType type;
	};

	// shader list
	const ShaderInfo shaderInfos[] = {
		{ SHADER_VS_DEFAULT, Enginefile::default_vs, RenderInterface::ShaderType::VERTEX_SHADER },
		{ SHADER_PS_DEFAULT, Enginefile::default_ps, RenderInterface::ShaderType::PIXEL_SHADER }
	};

	std::vector< RenderInterface::PShader > shaders( SHADER_COUNT );
	
	// load shaders
	for ( const ShaderInfo& si : shaderInfos ) {
		shaders[ si.id ] = LoadShaderFromFile( device, String( si.file ), si.type );
		if ( !shaders[ si.id ] ) {
			return false;
		}
	}

	struct RenderProgramParams {
		int id;
		RenderInterface::Shader* const vs;
		RenderInterface::Shader* const ps;
		RenderInterface::Shader* const gs;
	};

	// render program list
	const RenderProgramParams renderProgramParams[] = {
		{ RENDER_PROGRAM_DEFAULT, shaders[ SHADER_VS_DEFAULT ].get(), shaders[ SHADER_PS_DEFAULT ].get(), nullptr }
	};

	std::vector< RenderInterface::PRenderProgram > renderPrograms( RENDER_PROGRAM_COUNT );

	// create render programs
	for ( const RenderProgramParams& rpp : renderProgramParams ) {
		renderPrograms[ rpp.id ] = device->CreateRenderProgram( rpp.vs, rpp.ps, rpp.gs );
		if ( !renderPrograms[ rpp.id ] ) {
			return false;
		}
	}
	// store new objects
	this->shaders = std::move( shaders );
	this->renderPrograms = std::move( renderPrograms );
	return true;
}

Identifier Renderer::CreateRenderProgram( const Identifier vsid, const Identifier psid, const Identifier gsid ) {
	// get vertex shader
	RenderInterface::Shader* vs = nullptr;
	if ( vsid >= 0  && vsid < shaders.size() ) {
		vs = shaders[ vsid ].get();
	}
	if ( vs == nullptr ) {
		return RENDER_PROGRAM_INVALID;
	}
	// get pixel shader
	RenderInterface::Shader* ps = nullptr;
	if ( psid >= 0 && psid < shaders.size() ) {
		ps = shaders[ psid ].get();
	}
	if ( ps == nullptr ) {
		return RENDER_PROGRAM_INVALID;
	}
	// get optional geometry shader
	RenderInterface::Shader* gs = nullptr;
	if ( gsid != SHADER_GS_DEFAULT ) {
		if ( gsid >= 0 && gsid < shaders.size() ) {
			gs = shaders[ gsid ].get();
		}
		if ( gs == nullptr ) {
			return RENDER_PROGRAM_INVALID;
		}
	}
	// create new render program
	RenderInterface::PRenderProgram renderProgram = device->CreateRenderProgram( vs, ps, gs );
	if ( !renderProgram ) {
		return RENDER_PROGRAM_INVALID;
	}
	renderPrograms.push_back( renderProgram );
	return static_cast< Identifier >( renderPrograms.size() ) - 1;
}

void Renderer::ReleaseCreatedShaders() {
	if ( shaders.size() > SHADER_COUNT ) {
		shaders.resize( SHADER_COUNT );
	}
}

void Renderer::ReleaseCreatedRenderPrograms() {
	if ( renderPrograms.size() > RENDER_PROGRAM_COUNT ) {
		renderPrograms.resize( RENDER_PROGRAM_COUNT );
	}
}

RenderInterface::RenderProgram* Renderer::GetRenderProgram( const Identifier id ) {
	if ( id < 0 || id >= renderPrograms.size() ) {
		return nullptr;
	}
	return renderPrograms[ id ].get();
}

bool Renderer::CreateVertexLayouts() {
	std::vector< RenderInterface::PVertexLayout > layouts( LAYOUT_COUNT );
	/*
	// VertexColorLine
	const RenderInterface::VertexAttribute colorLineAttribs[] = {
		{ "position",	"POSITION", 0, RenderInterface::Format::R32G32B32_FLOAT,	0,	1, 0, 0 },
		{ "color",		"COLOR",	0, RenderInterface::Format::R32G32B32A32_FLOAT, 12,	1, 0, 0 }
	};
	layouts[ LAYOUT_COLORLINE ] = device->CreateVertexLayout( colorLineAttribs, sizeof( colorLineAttribs ) / sizeof( *colorLineAttribs ), GetRenderProgram( RENDER_PROGRAM_COLOR_LINE ) );
	if ( !layouts[ LAYOUT_COLORLINE ] ) {
		return false;
	}

	this->vertexLayouts = std::move( layouts );
	*/
	return true;
}

void Renderer::SetViewport( const Viewport& viewport ) {
	this->viewport = viewport;
}

void Renderer::UpdateBuffer( Buffer& buffer, void* const data, const int offset, const int size ) {
	RenderInterface::Buffer* const dest = buffer.GetBuffer();
	if ( dest == nullptr ) {
		return;
	}
	RenderInterface::MappedBuffer map;
	if ( !immediateCommands->Map( dest, 0, RenderInterface::MapPolicy::WRITE_DISCARD, map ) ) {
		return;
	}
	memcpy( static_cast< char* >( map.data ) + static_cast< unsigned int >( offset ), data, static_cast< unsigned int >( size ) );
	immediateCommands->Unmap( dest, map );
}

//******************************************************************************


/*
Nahradit funkci SetCurrentBackBuffer
RenderInterface::BackBuffer* Renderer::GetBackBuffer( const Window& window ) {
	for ( RenderTargetWindow& rtw : renderTargetWindows ) {
		if ( rtw.window == &window ) {
			return rtw.backBuffer.get();
		}
	}
	return nullptr;
}
*/



// !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
//
// NASLEDUJE POZUSTATEK PROTOTYPU, POTREBA KOMPLETRNE PREPSAT!
//
// !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!

//void Renderer::SetViewport() {
	/*
	D3D11_VIEWPORT viewport;
	viewport.TopLeftX = 0;
	viewport.TopLeftY = 0;
	viewport.Width = static_cast< FLOAT >( window->GetClientWidth() );
	viewport.Height = static_cast< FLOAT >( window->GetClientHeight() );
	viewport.MinDepth = 0;
	viewport.MaxDepth = 1;
	deviceContext_->RSSetViewports( 1, &viewport );
	*/
//}

//void Renderer::CreateBlendStates() {
	/*
	ID3D11BlendState *state = nullptr;
	D3D11_BLEND_DESC desc;
	HRESULT hresult = 0;

	// add blend state
	ZeroMemory( &desc, sizeof( desc ) );
	desc.AlphaToCoverageEnable						= FALSE;
	desc.IndependentBlendEnable						= FALSE;
	desc.RenderTarget[ 0 ].BlendEnable				= TRUE;
	desc.RenderTarget[ 0 ].SrcBlend					= D3D11_BLEND_ONE;
	desc.RenderTarget[ 0 ].DestBlend				= D3D11_BLEND_ONE;
	desc.RenderTarget[ 0 ].BlendOp					= D3D11_BLEND_OP_ADD;
	desc.RenderTarget[ 0 ].SrcBlendAlpha			= D3D11_BLEND_ONE;
	desc.RenderTarget[ 0 ].DestBlendAlpha			= D3D11_BLEND_ZERO;
	desc.RenderTarget[ 0 ].BlendOpAlpha				= D3D11_BLEND_OP_ADD;
	desc.RenderTarget[ 0 ].RenderTargetWriteMask	= D3D11_COLOR_WRITE_ENABLE_ALL;
	hresult = device_->CreateBlendState( &desc, &state );
	if ( FAILED( hresult ) ) {
		return;
	}
	blendStates.Insert( state, static_cast< int >( Blending::ADD ) );
	*/
	/*
	// order independent transparency
	ZeroMemory( &desc, sizeof( desc ) );
	desc.AlphaToCoverageEnable						= FALSE;
	desc.IndependentBlendEnable						= FALSE;
	desc.RenderTarget[ 0 ].BlendEnable				= TRUE;
	desc.RenderTarget[ 0 ].SrcBlend					= D3D11_BLEND_INV_DEST_ALPHA;
	desc.RenderTarget[ 0 ].DestBlend				= D3D11_BLEND_ONE;
	desc.RenderTarget[ 0 ].BlendOp					= D3D11_BLEND_OP_ADD;
	desc.RenderTarget[ 0 ].SrcBlendAlpha			= D3D11_BLEND_ONE;
	desc.RenderTarget[ 0 ].DestBlendAlpha			= D3D11_BLEND_INV_SRC_ALPHA;
	desc.RenderTarget[ 0 ].BlendOpAlpha				= D3D11_BLEND_OP_ADD;
	desc.RenderTarget[ 0 ].RenderTargetWriteMask	= D3D11_COLOR_WRITE_ENABLE_ALL;
	hresult = device->CreateBlendState( &desc, &state );
	if ( FAILED( hresult ) ) {
		return;
	}
	blendStates.Insert( state, static_cast< int >( BlendState::OIT ) );

	// order depend transparency
	ZeroMemory( &desc, sizeof( desc ) );
	desc.AlphaToCoverageEnable = FALSE;
	desc.IndependentBlendEnable = FALSE;
	desc.RenderTarget[ 0 ].BlendEnable				= TRUE;
	desc.RenderTarget[ 0 ].SrcBlend					= D3D11_BLEND_INV_DEST_ALPHA;
	desc.RenderTarget[ 0 ].DestBlend				= D3D11_BLEND_ONE;
	desc.RenderTarget[ 0 ].BlendOp					= D3D11_BLEND_OP_ADD;
	desc.RenderTarget[ 0 ].SrcBlendAlpha			= D3D11_BLEND_SRC_ALPHA;
	desc.RenderTarget[ 0 ].DestBlendAlpha			= D3D11_BLEND_INV_SRC_ALPHA;
	desc.RenderTarget[ 0 ].BlendOpAlpha				= D3D11_BLEND_OP_ADD;
	desc.RenderTarget[ 0 ].RenderTargetWriteMask	= D3D11_COLOR_WRITE_ENABLE_ALL;
	hresult = device->CreateBlendState( &desc, &state );
	if ( FAILED( hresult ) ) {
		return;
	}
	blendStates.Insert( state, static_cast< int >( BlendState::ODT ) );
	*/
//}

/*
void RenderSystem::CreateInputLayouts() {
	D3D11_INPUT_ELEMENT_DESC mesh[] = {
		{ "POSITION",			0, DXGI_FORMAT_R32G32B32_FLOAT,		0, 0,	D3D11_INPUT_PER_VERTEX_DATA,	0 },
		{ "NORMAL",				0, DXGI_FORMAT_R32G32B32_FLOAT,		0, 12,	D3D11_INPUT_PER_VERTEX_DATA,	0 },
		{ "TANGENT",			0, DXGI_FORMAT_R32G32B32_FLOAT,		0, 24,	D3D11_INPUT_PER_VERTEX_DATA,	0 },
		{ "BINORMAL",			0, DXGI_FORMAT_R32G32B32_FLOAT,		0, 36,	D3D11_INPUT_PER_VERTEX_DATA,	0 },
		{ "TEXCOORD",			0, DXGI_FORMAT_R32G32_FLOAT,		0, 48,	D3D11_INPUT_PER_VERTEX_DATA,	0 },
		{ "TRANSFORMATIONS",	0, DXGI_FORMAT_R32G32B32A32_FLOAT,	1, 0,	D3D11_INPUT_PER_INSTANCE_DATA,	1 },
		{ "TRANSFORMATIONS",	1, DXGI_FORMAT_R32G32B32A32_FLOAT,	1, 16,	D3D11_INPUT_PER_INSTANCE_DATA,	1 },
		{ "TRANSFORMATIONS",	2, DXGI_FORMAT_R32G32B32A32_FLOAT,	1, 32,	D3D11_INPUT_PER_INSTANCE_DATA,	1 },
		{ "TRANSFORMATIONS",	3, DXGI_FORMAT_R32G32B32A32_FLOAT,	1, 48,	D3D11_INPUT_PER_INSTANCE_DATA,	1 }
	};
	CreateInputLayout( InputLayout::MESH, VSSlot::MESH, mesh, ARRAYSIZE( mesh ) );

	D3D11_INPUT_ELEMENT_DESC screenQuad[] = {
		{ "POSITION",			0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA,	0 },
		{ "TEXCOORD",			0, DXGI_FORMAT_R32G32_FLOAT,	0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA,	0 }
	};
	CreateInputLayout( InputLayout::SCREEN_QUAD, VSSlot::SCREEN_QUAD, screenQuad, ARRAYSIZE( screenQuad ) );

	D3D11_INPUT_ELEMENT_DESC billboard[] = {
		{ "POSITION",			0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA,	0 },
		{ "TEXCOORD",			0, DXGI_FORMAT_R32G32_FLOAT,	0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA,	0 },	// dimmension
		{ "PSIZE",				0, DXGI_FORMAT_R32_FLOAT,		0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA,	0 },	// rotation
		{ "COLOR",				0, DXGI_FORMAT_R32G32B32_FLOAT, 1, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_INSTANCE_DATA, 1 }
	};
	CreateInputLayout( InputLayout::BILLBOARD, VSSlot::BILLBOARD, billboard, ARRAYSIZE( billboard ) );
}

void RenderSystem::Render( VertexBuffer &buffer ) {
	APIVertexBuffer object = buffer.GetAPIObject();

	// set vertex buffer
	UINT stride = buffer.GetVertexSize();
	UINT offset = 0;
	deviceContext->IASetVertexBuffers( 0, 1, &object.vertexBuffer, &stride, &offset );

	if ( buffer.Indexed() ) {
		deviceContext->IASetIndexBuffer( object.indexBuffer, DXGI_FORMAT_R16_UINT, 0 );
		deviceContext->DrawIndexed( buffer.GetIndicesCount(), buffer.GetIndicesOffset(), buffer.GetVerticesOffset() );
	} else {
		deviceContext->Draw( buffer.GetVerticesCount(), buffer.GetVerticesOffset() );
	}
}

void RenderSystem::RenderInstanced( VertexBuffer &vertexBuffer, VertexBuffer &instanceBuffer, const unsigned int instancesOffset, const unsigned int instancesCount ) {
	APIVertexBuffer perVertexData = vertexBuffer.GetAPIObject();
	APIVertexBuffer perInstanceData = instanceBuffer.GetAPIObject();
	
	// set vertex buffers
	ID3D11Buffer *buffers[ 2 ] = {
		perVertexData.vertexBuffer,
		perInstanceData.vertexBuffer
	};
	UINT strides[ 2 ] = { vertexBuffer.GetVertexSize(), instanceBuffer.GetVertexSize() };
	UINT offsets[ 2 ] = { 0, 0 };
	deviceContext->IASetVertexBuffers( 0, 2, buffers, strides, offsets );

	if ( vertexBuffer.Indexed() ) {
		deviceContext->IASetIndexBuffer( perVertexData.indexBuffer, DXGI_FORMAT_R16_UINT, 0 );
		deviceContext->DrawIndexedInstanced(
			vertexBuffer.GetIndicesCount(),
			instancesCount,
			vertexBuffer.GetIndicesOffset(), 
			vertexBuffer.GetVerticesOffset(), 
			instancesOffset
		);
	} else {
		deviceContext->DrawInstanced( 
			vertexBuffer.GetVerticesCount(),
			instancesCount,
			vertexBuffer.GetVerticesOffset(),
			instancesOffset
		);
	}
}

void RenderSystem::DrawScreenQuad() {
	SetVertexShader( VSSlot::SCREEN_QUAD );
	SetDepthStencilState( DepthStencilState::DISABLED );
	SetInputLayout( InputLayout::SCREEN_QUAD );
	Render( *screenQuad->GetVertexBuffer() );
}

void RenderSystem::DrawRenderList( RenderListItem *renderList ) {
	if ( renderList == nullptr ) {
		return;
	}
	RenderListItem *item = renderList;
	while ( item->meshData != nullptr ) {
		RenderInstanced( *item->meshData, *item->instanceData, item->instanceOffset, item->count );
		item++; // next
	}
}

void RenderSystem::SetBlendState( const BlendState state ) {
	if ( state == blendState ) {
		return;
	}
	if ( state == BlendState::DEFAULT || blendStates[ static_cast< int >( state ) ] == nullptr ) {
		deviceContext->OMSetBlendState( NULL, 0, 0xffffffff );
		blendState = state;
		return;
	}
	deviceContext->OMSetBlendState( blendStates[ static_cast< int >( state ) ], 0, 0xffffffff );
	blendState = state;
}

void RenderSystem::UpdateCamera() {
	GlobalShaderConstants *buffer = reinterpret_cast< GlobalShaderConstants* >( globalCbuffer );
	buffer->viewProjection = camera->GetViewProjectionMatrix();
	buffer->cameraPosition = camera->GetPosition();
	buffer->cameraUp = camera->GetUp();
	buffer->cameraLook = camera->GetDirection();
	buffer->cameraRight = camera->GetRight();
	buffer->projectionPlane = camera->GetProjectionPlane( renderParams.screenWidth, renderParams.screenHeight );
}

void RenderSystem::SetMaterialShader( const int id ) {
	if ( id < 0 || id >=  materialShaders.Size() ) {
		return;
	}
	ShaderSlot &slot = materialShaders[ id ];
	PixelShader *pixelShader = dynamic_cast< PixelShader* >( slot.shader );
	if ( pixelShader == nullptr ) {
		return;
	}
	ID3D11PixelShader *apiShader = reinterpret_cast< ID3D11PixelShader* >( pixelShader->GetAPIObject() );
	if ( apiShader == this->pixelShader ) {
		return;
	}
	deviceContext->PSSetShader( apiShader, NULL, 0 );
	this->pixelShader = apiShader;
}

void RenderSystem::SetAmbientColor( const Color &color ) {
	color.Store( reinterpret_cast< GlobalShaderConstants* >( globalCbuffer )->ambientColor );
}

void RenderSystem::SetDrawDistance( const float drawDistance ) {
	reinterpret_cast< GlobalShaderConstants* >( globalCbuffer )->drawDistance = drawDistance;
}

void RenderSystem::RenderTest() {

	// testovaci objekt
	//mesh->RotateRollPitchYaw( 0, GetSystem()->GetFrameTimeSec() * 0.5f, 0 );
	//object.Translate( 0, 0, GetSystem()->GetFrameTimeSec() * 0.1f );

	// scene update
	TestUpdateInput();
	UpdateCamera();
	UpdateConstantBuffer( ConstantBufferSlot::GLOBAL, globalCbuffer );

	static float lightRad = 0.0f;
	lightRad += GetSystem()->GetFrameTimeSec() * 1.5f;
	light2.SetPosition( Vector( cos( lightRad ) * 3.0f, 2.0f, sin( lightRad ) * 3.0f ) );
	light3.SetPosition( Vector( cos( lightRad + math::PI ) * 3.0f, 2.0f, sin( lightRad + math::PI ) * 3.0f ) );

	SetTextures( *material );

}

void RenderSystem::Draw() {
	1) Base pass (rasterize models)
		- blend mode default
		- depth test default
		- shader material basePass

	2) Deferred lighting (rasterize lights volume)
		- blend mode add
		- depth test ?
		- shader deferred shader

	3) Light pass (rasterize models)
		- blend mode default
		- depth test equal
		- shader material lightPass

	BasePass();
	//DeferredLightingPass();

	//LightPass();

	RenderTarget *renderTargets[] = {
		&backBuffer
	};
	SetRenderTargets( renderTargets, ARRAYSIZE( renderTargets ), true );
	ID3D11ShaderResourceView *views[] = { gbuffers[ 1 ].shaderResourceView };
	SetPixelShaderResources( views, 1 );
	SetPixelShader( PSSlot::LIGHTTEST );
	DrawScreenQuad();
	SetPixelShaderResources( nullptr, 0 );

	swapChain->Present( 0, 0 );
}

void RenderSystem::BasePass() {
	RenderTarget *renderTargets[] = {
		&backBuffer,
		&gbuffers[ 1 ] // linear depth
	};
	SetRenderTargets( renderTargets, ARRAYSIZE( renderTargets ), true );

	ClearGBuffers();

	// pipeline states
	SetBlendState( BlendState::DEFAULT );
	SetDepthStencilState( DepthStencilState::DEFAULT );

	// testovaci render
	RenderTest();
	SetMaterial( material );
	SetInputLayout( InputLayout::MESH );
	SetVertexShader( VSSlot::MESH );
	SetPixelShader( PSSlot::DEFAULT );
	SetPrimitiveTopology( PrimitiveTopology::TRIANGLELIST );
	ClearBackBuffer();
	ClearDepthStencil();
	DrawRenderList( renderList );
}

void RenderSystem::TestUpdateInput() {

	Input *input = GetInput();
	cameraInstance->Rotate( input->GetMouseDeltaY(), input->GetMouseDeltaX(), 0 );
	if ( input->KeyIsDown( Key::KEY_W ) ) {
		cameraInstance->MoveForward( GetSystem()->GetFrameTimeSec() * 1.5f );
	}
	if ( input->KeyIsDown( Key::KEY_S ) ) {
		cameraInstance->MoveForward( -GetSystem()->GetFrameTimeSec() * 1.5f );
	}
	if ( input->KeyIsDown( Key::KEY_D ) ) {
		cameraInstance->MoveRight( GetSystem()->GetFrameTimeSec() * 1.5f );
	}
	if ( input->KeyIsDown( Key::KEY_A ) ) {
		cameraInstance->MoveRight( -GetSystem()->GetFrameTimeSec() * 1.5f );
	}
	input->Update();
}
*/