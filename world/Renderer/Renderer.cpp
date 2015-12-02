
#include <dxgi.h>
#include "..\framework\Core.h"
#include "..\platform\Window.h"
#include "..\platform\File.h"
#include "RenderInterface.h"
#include "Renderer.h"

Renderer::Renderer() {
	device = nullptr;
	immediateCommandInterface = nullptr;
}

bool Renderer::Initialize( const RendererInitialParams &parameters ) {

	// device
	
	RenderInterface::DX11CreateDeviceParams deviceParams;
	ZeroMemory( &deviceParams, sizeof( deviceParams ) );
	deviceParams.majorFeatureLevels = 11;
	deviceParams.minorFeatureLevels = 0;
	deviceParams.adapter = 0;

	RenderInterface::Device *device = RenderInterface::DX11CreateDevice( deviceParams );
	if ( device == nullptr ) {
		return false;
	}

	// immediate command interface

	RenderInterface::CommandInterface *immediateCommandInterface = device->CreateCommandInterface();
	if ( immediateCommandInterface == nullptr ) {
		return false;
	}

	// G-Buffers

	// ...

	return true;
}

RenderInterface::BackBuffer *Renderer::CreateWindowBackBuffer( Window &window ) {
	RenderInterface::BackBuffer *backBuffer = device->CreateBackBuffer( window );
	return backBuffer;
}

void Renderer::ResizeBuffers( Window &window, const int width, const int height ) {
	window.ResizeBackBuffer();

	// resize g-buffers and create render target descriptors
	// ...
}

// !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
//
// TENTO KOD JE POZUSTATEK Z PROTOTYPU, POTREBA KOMPLETRNE PREPSAT !
//
// !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!

Renderer::~Renderer() {
}
/*
void Renderer::Initialize( const RendererInitialParams &parameters ) {

	// inicializace probiha vzdy s nasledujicim rozlysenim
	const int initialScreenWidth = 1024;
	const int initialScreenHeight = 768;

	// device
	
	RenderInterface::DX11CreateDeviceParams params = { 0 };
	params.majorFeatureLevels = 11;
	params.minorFeatureLevels = 0;
	params.adapter = 0;

	device = RenderInterface::DX11CreateDevice( params );
	if ( device == nullptr ) {
		return;
	}

	// immediate command interface
	immediateCommandInterface = device->CreateCommandInterface();


	// G-Buffers

	RenderInterface::RenderBufferDesc rbd;
	ZeroMemory( &rbd, sizeof( rbd ) );
	rbd.width = initialScreenWidth;
	rbd.height = initialScreenHeight;
	rbd.samplesCount = 1;
	rbd.multisampleQuality = RenderInterface::MAX_MULTISAMPLE_QUALITY;

	// normal.x, normal.y, normal.z, specular_exp
	rbd.format = RenderInterface::Format::R16G16B16A16_FLOAT;
	gbuffers[ 0 ] = device->CreateRenderBuffer( rbd );

	// linear depth
	rbd.format = RenderInterface::Format::R32_FLOAT;
	gbuffers[ 1 ] = device->CreateRenderBuffer( rbd );

	// difuse.r, difuse.g, difuse.b
	rbd.format = RenderInterface::Format::R8G8B8A8_UNORM;
	gbuffers[ 2 ] = device->CreateRenderBuffer( rbd );

	// specular.r, specular.g, specular.b
	rbd.format = RenderInterface::Format::R8G8B8A8_UNORM;
	gbuffers[ 3 ] = device->CreateRenderBuffer( rbd );

	// depth stencil buffer

	RenderInterface::DepthStencilBufferDesc dsd;
	ZeroMemory( &dsd, sizeof( dsd ) );
	dsd.width = initialScreenWidth;
	dsd.height = initialScreenHeight;
	dsd.samplesCount = 1;
	dsd.multisampleQuality = RenderInterface::MAX_MULTISAMPLE_QUALITY;
	depthStencilBuffer = device->CreateDepthStencilBuffer( dsd );
	if ( depthStencilBuffer == nullptr ) {
		return;
	}

	// texture samplers

	RenderInterface::TextureSamplerDesc samplerDesc;
	ZeroMemory( &samplerDesc, sizeof( samplerDesc ) );
	samplerDesc.minLOD = 0;
	samplerDesc.maxLOD = RenderInterface::MAX_TEXTURE_LOD;
	samplerDesc.uAddress = RenderInterface::TextureAddress::WRAP;
	samplerDesc.vAddress = RenderInterface::TextureAddress::WRAP;
	samplerDesc.wAddress = RenderInterface::TextureAddress::WRAP;

	// standardni sampler
	if ( parameters.maxAnisotropy > 0 ) {
		samplerDesc.maxAnisotropy = 0;
		samplerDesc.filter = RenderInterface::TextureFilter::LINEAR;
	} else {
		samplerDesc.maxAnisotropy = parameters.maxAnisotropy;
		samplerDesc.filter = RenderInterface::TextureFilter::ANISOTROPIC;
	}
	samplers[ 0 ] = device->CreateTextureSampler( samplerDesc );

	// point sampler
	samplerDesc.maxAnisotropy = 0;
	samplerDesc.filter = RenderInterface::TextureFilter::POINT;
	samplers[ 1 ] = device->CreateTextureSampler( samplerDesc );

	// linear sampler
	samplerDesc.maxAnisotropy = 0;
	samplerDesc.filter = RenderInterface::TextureFilter::LINEAR;
	samplers[ 2 ] = device->CreateTextureSampler( samplerDesc );


	//**************************************************************************
	CommandInterface *commandInterface = device->CreateCommandInterface();
	static RenderOutput *renderOutput = device->CreateRenderOutput( window, 0 );
	// renderOutput->SetDisplayMode( 800, 600, 60.0f, false );

	// device->ResizeRenderOutputBuffer( RenderOutput &renderOutput, const int width, const int height )
	return;

	//DeviceContext *deviceContext = device->CreateContext();

	//**************************************************************************

	// propojeni s oknem
	//this->window = dynamic_cast< Window* >( &window );

	// inicializace DirectX device
	// * CreateDeviceAndSwapChain( parameters );

	// ziskani vlastnosti displeje a graficke karty
	// * EnumDisplayModes();
	// * CheckMultisampleQualityLevels();

	// pokracovani v inicializaci directx
	// * CreateBackBufferView();
	// * CreateDepthStencilBuffer();
	CreateDepthStencilStates();
	CreateRasterizerStates();
	// * CreateTextureSamplers();
	// * CreateGBuffers( parameters.displayMode.width, parameters.displayMode.height );
	SetViewport();
}
*/
void Renderer::SetViewport() {
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
}

void Renderer::CreateGBuffers( const int width, const int height ) {
	/*
	// normal.x, normal.y, normal.z, specular_exp
	CreateRenderTarget( width, height, DXGI_FORMAT_R16G16B16A16_FLOAT, parameters.antialiasing, gbuffers[ 0 ] );
	// linear depth
	CreateRenderTarget( width, height, DXGI_FORMAT_R32_FLOAT, parameters.antialiasing, gbuffers[ 1 ] );
	// difuse.r, difuse.g, difuse.b
	CreateRenderTarget( width, height, DXGI_FORMAT_R8G8B8A8_UNORM, parameters.antialiasing, gbuffers[ 2 ] );
	// specular.r, specular.g, specular.b
	CreateRenderTarget( width, height, DXGI_FORMAT_R8G8B8A8_UNORM, parameters.antialiasing, gbuffers[ 3 ] );
	*/
}

void Renderer::ResizeGBuffers( const int width, const int height ) {
	/*
	return;
	UnbindRenderTargets();
	UnbindShaderResources();

	// release
	for ( RenderTarget &rt : gbuffers ) {
		//ReleaseD3DInterface( &rt.renderTargetView );
		//ReleaseD3DInterface( &rt.shaderResourceView );
		//ReleaseD3DInterface( &rt.texture );
	}
	CreateGBuffers( width, height );
	*/
}
/*
void Renderer::CreateRenderTarget( const int width, const int height, const DXGI_FORMAT format, const Antialiasing antialiasing, RenderTarget &target ) {
	
	return;
	D3D11_TEXTURE2D_DESC textureDesc;
	textureDesc.Width = static_cast< UINT >( width );
	textureDesc.Height = static_cast< UINT >( height );
	textureDesc.MipLevels = 1;
	textureDesc.ArraySize = 1;
	textureDesc.Format = format;
	textureDesc.SampleDesc.Count = GetSamplesCount( antialiasing );
	textureDesc.SampleDesc.Quality = 0;//GetSampleQuality( antialiasing );
	textureDesc.Usage = D3D11_USAGE_DEFAULT;
	textureDesc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
	textureDesc.CPUAccessFlags = 0;
	textureDesc.MiscFlags = 0;

	D3D11_SHADER_RESOURCE_VIEW_DESC viewDesc;
	ZeroMemory( &viewDesc, sizeof( viewDesc ) );
	viewDesc.Format = format;
	viewDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	viewDesc.Texture2D.MipLevels = 1;
	viewDesc.Texture2D.MostDetailedMip = 0;

	// uvolnit puvodni objekty
	//ReleaseD3DInterface( &target.shaderResourceView );
	//ReleaseD3DInterface( &target.renderTargetView );
	//ReleaseD3DInterface( &target.texture );

	HRESULT hresult = 0;
	hresult = device_->CreateTexture2D( &textureDesc, NULL, &target.texture );
	if ( FAILED( hresult ) ) {
		return;
	}
	hresult = device_->CreateRenderTargetView( target.texture, NULL, &target.renderTargetView );
	if ( FAILED( hresult ) ) {
		target.texture->Release();
		target.texture = nullptr;
		return;
	}
	hresult = device_->CreateShaderResourceView( target.texture, &viewDesc, &target.shaderResourceView );
	if ( FAILED( hresult ) ) {
		target.texture->Release();
		target.texture = nullptr;
		target.renderTargetView->Release();
		target.renderTargetView = nullptr;
		return;
	}
	
}
*/
/*
int Renderer::GetSamplesCount( const Antialiasing antialiasing ) const {
	switch ( antialiasing ) {
	case Antialiasing::MSAA2:
		return deviceFeatures.msaaQuality[ 0 ] == 0 ? 1 : 2;

	case Antialiasing::MSAA4:
		return deviceFeatures.msaaQuality[ 1 ] == 0 ? 1 : 4;

	case Antialiasing::MSAA8:
		return deviceFeatures.msaaQuality[ 2 ] == 0 ? 1 : 8;

	case Antialiasing::MSAA16:
		return deviceFeatures.msaaQuality[ 3 ] == 0 ? 1 : 16;
	}
	return 1;
}
*/

void Renderer::CreateDepthStencilBuffer() {
	/*
	return;
	HRESULT hresult = 0;

	// ziskani info o back bufferu
	ID3D11Texture2D *backBuffer;
	swapChain->GetBuffer( 0, __uuidof( ID3D11Texture2D ), reinterpret_cast< LPVOID* >( &backBuffer ) );
	if ( FAILED( hresult ) ) {
		return;
	}
	D3D11_TEXTURE2D_DESC backBufferDesc;
	backBuffer->GetDesc( &backBufferDesc );
	backBuffer->Release();

	D3D11_TEXTURE2D_DESC bufferDesc;
	ZeroMemory( &bufferDesc, sizeof( bufferDesc ) );
	bufferDesc.Width = backBufferDesc.Width;
	bufferDesc.Height = backBufferDesc.Height;
	bufferDesc.MipLevels = 1;
	bufferDesc.ArraySize = 1;
	bufferDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	bufferDesc.SampleDesc.Count = 1;
	bufferDesc.SampleDesc.Quality = 0;
	bufferDesc.Usage = D3D11_USAGE_DEFAULT;
	bufferDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;

	hresult = device_->CreateTexture2D( &bufferDesc, NULL, &depthStencilBuffer_ );
	if ( FAILED( hresult ) ) {
		return;
	}
	D3D11_DEPTH_STENCIL_VIEW_DESC viewDesc;
	ZeroMemory( &viewDesc, sizeof( viewDesc ) );
	viewDesc.Format = bufferDesc.Format;
	viewDesc.ViewDimension = bufferDesc.SampleDesc.Count == 1 ? D3D11_DSV_DIMENSION_TEXTURE2D : D3D11_DSV_DIMENSION_TEXTURE2DMS;
	viewDesc.Flags = 0;

	hresult = device_->CreateDepthStencilView( depthStencilBuffer_, &viewDesc, &depthStencilView );
	if ( FAILED( hresult ) ) {
		//ReleaseD3DInterface( &depthStencilBuffer );
		return;
	}
	*/
}

void Renderer::CreateDepthStencilStates() {
	/*
	return;
	ID3D11DepthStencilState *state = nullptr;
	D3D11_DEPTH_STENCIL_DESC desc;
	HRESULT hresult = 0;

	// light pass (depth readonly equal)
	ZeroMemory( &desc, sizeof( desc ) );
	desc.DepthEnable					= TRUE;
	desc.DepthWriteMask					= D3D11_DEPTH_WRITE_MASK_ZERO;
	desc.DepthFunc						= D3D11_COMPARISON_EQUAL;
	desc.StencilEnable					= FALSE;
	desc.StencilReadMask				= D3D11_DEFAULT_STENCIL_READ_MASK;
	desc.StencilWriteMask				= D3D11_DEFAULT_STENCIL_WRITE_MASK;
	desc.FrontFace.StencilFunc			= D3D11_COMPARISON_NEVER;
	desc.BackFace.StencilFunc			= D3D11_COMPARISON_NEVER;
	desc.FrontFace.StencilDepthFailOp	= D3D11_STENCIL_OP_KEEP;
	desc.BackFace.StencilDepthFailOp	= D3D11_STENCIL_OP_KEEP;
	desc.FrontFace.StencilPassOp		= D3D11_STENCIL_OP_KEEP;
	desc.BackFace.StencilPassOp			= D3D11_STENCIL_OP_KEEP;
	desc.FrontFace.StencilFailOp		= D3D11_STENCIL_OP_KEEP;
	desc.BackFace.StencilFailOp			= D3D11_STENCIL_OP_KEEP;
	hresult = device_->CreateDepthStencilState( &desc, &state );
	if ( FAILED( hresult ) ) {
		return;
	}
	depthStencilStates.Insert( state, static_cast< int >( DepthStencil::LIGHTPASS ) );

	// no depth, no stencil
	ZeroMemory( &desc, sizeof( desc ) );
	desc.DepthEnable					= FALSE;
	desc.DepthWriteMask					= D3D11_DEPTH_WRITE_MASK_ALL;
	desc.DepthFunc						= D3D11_COMPARISON_NEVER;
	desc.StencilEnable					= FALSE;
	desc.StencilReadMask				= D3D11_DEFAULT_STENCIL_READ_MASK;
	desc.StencilWriteMask				= D3D11_DEFAULT_STENCIL_WRITE_MASK;
	desc.FrontFace.StencilFunc			= D3D11_COMPARISON_NEVER;
	desc.BackFace.StencilFunc			= D3D11_COMPARISON_NEVER;
	desc.FrontFace.StencilDepthFailOp	= D3D11_STENCIL_OP_KEEP;
	desc.BackFace.StencilDepthFailOp	= D3D11_STENCIL_OP_KEEP;
	desc.FrontFace.StencilPassOp		= D3D11_STENCIL_OP_KEEP;
	desc.BackFace.StencilPassOp			= D3D11_STENCIL_OP_KEEP;
	desc.FrontFace.StencilFailOp		= D3D11_STENCIL_OP_KEEP;
	desc.BackFace.StencilFailOp			= D3D11_STENCIL_OP_KEEP;
	hresult = device_->CreateDepthStencilState( &desc, &state );
	if ( FAILED( hresult ) ) {
		return;
	}
	depthStencilStates.Insert( state, static_cast< int >( DepthStencil::DISABLED ) );

	// readonly depth
	ZeroMemory( &desc, sizeof( desc ) );
	desc.DepthEnable					= TRUE;
	desc.DepthWriteMask					= D3D11_DEPTH_WRITE_MASK_ZERO;
	desc.DepthFunc						= D3D11_COMPARISON_LESS;
	desc.StencilEnable					= FALSE;
	desc.StencilReadMask				= D3D11_DEFAULT_STENCIL_READ_MASK;
	desc.StencilWriteMask				= D3D11_DEFAULT_STENCIL_WRITE_MASK;
	desc.FrontFace.StencilFunc			= D3D11_COMPARISON_NEVER;
	desc.BackFace.StencilFunc			= D3D11_COMPARISON_NEVER;
	desc.FrontFace.StencilDepthFailOp	= D3D11_STENCIL_OP_KEEP;
	desc.BackFace.StencilDepthFailOp	= D3D11_STENCIL_OP_KEEP;
	desc.FrontFace.StencilPassOp		= D3D11_STENCIL_OP_KEEP;
	desc.BackFace.StencilPassOp			= D3D11_STENCIL_OP_KEEP;
	desc.FrontFace.StencilFailOp		= D3D11_STENCIL_OP_KEEP;
	desc.BackFace.StencilFailOp			= D3D11_STENCIL_OP_KEEP;
	hresult = device_->CreateDepthStencilState( &desc, &state );
	if ( FAILED( hresult ) ) {
		return;
	}
	depthStencilStates.Insert( state, static_cast< int >( DepthStencil::READONLY ) );
	*/
}

void Renderer::CreateBlendStates() {
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
}

void Renderer::CreateRasterizerStates() {
	/*
	D3D11_RASTERIZER_DESC desc;
	ID3D11RasterizerState *state = nullptr;
	HRESULT hresult = 0;
	
	// wireframe
	ZeroMemory( &desc, sizeof( desc ) );
	desc.FillMode				= D3D11_FILL_WIREFRAME;
	desc.CullMode				= D3D11_CULL_NONE;
	desc.FrontCounterClockwise	= FALSE;
	desc.DepthBias				= 0;
	desc.SlopeScaledDepthBias	= 0.0f;
	desc.DepthBiasClamp			= 0.0f;
	desc.DepthClipEnable		= TRUE;
	desc.ScissorEnable			= FALSE;
	desc.MultisampleEnable		= FALSE;
	desc.AntialiasedLineEnable	= FALSE;
	hresult = device->CreateRasterizerState( &desc, &state );
	if ( FAILED( hresult ) ) {
		return;
	}
	rasterizerStates.Insert( state, static_cast< int >( Rasterizer::WIREFRAME ) );
	*/
}

void Renderer::CreateTextureSamplers() {
	/*
	HRESULT hresult = 0;
	D3D11_SAMPLER_DESC desc;

	ZeroMemory( &desc, sizeof( desc ) );
	desc.Filter			= D3D11_FILTER_ANISOTROPIC;//D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	desc.AddressU		= D3D11_TEXTURE_ADDRESS_WRAP;
	desc.AddressV		= D3D11_TEXTURE_ADDRESS_WRAP;
	desc.AddressW		= D3D11_TEXTURE_ADDRESS_WRAP;
	desc.MipLODBias		= 0;
	desc.MaxAnisotropy	= 16;
	desc.ComparisonFunc	= D3D11_COMPARISON_NEVER;
	desc.MinLOD			= 0;
	desc.MaxLOD			= D3D11_FLOAT32_MAX;
	hresult = device_->CreateSamplerState( &desc, &textureSampler );
	if ( FAILED( hresult ) ) {
		return;
	}
	// set sampler states
	deviceContext_->PSSetSamplers( 0, 1, &textureSampler );
	*/
}

void Renderer::UnbindShaderResources() {
	// ...
	// deviceContext->PSSetShaderResources( 0, )
}

void Renderer::UnbindRenderTargets() {
	//deviceContext_->OMSetRenderTargets( 0, NULL, NULL );
}
/*
void Renderer::Resize() {
	return;
	if ( !window ) {
		return;
	}
	int width = window->GetClientWidth();
	int height = window->GetClientHeight();
	ResizeBackBuffer( width, height );
}
*/
/*

// mapuje globalni konstanty definovane v shaderu
struct GlobalShaderConstants {
	Float4x4 viewProjection;
	Float4 ambientColor;
	Float4 cameraPosition;
	Float4 cameraLook;
	Float4 cameraUp;
	Float4 cameraRight;
	Float4 projectionPlane; // { halfWidth, halfHeight, distance }
	float drawDistance;
};

RenderSystem::RenderSystem() {
	device = nullptr;
	deviceContext = nullptr;
	swapChain = nullptr;
	materialShaders.Realloc( 128 );
	materialShaders.SetDynamic();
	depthStencilState = DepthStencilState::DEFAULT;
	inputLayout = InputLayout::DEFAULT;
	blendState = BlendState::DEFAULT;
	rasterizerState = RasterizerState::DEFAULT;

	inputLayouts.Realloc( 16 );
	inputLayouts.SetDynamic();
	depthStencilStates.Realloc( 16 );
	depthStencilStates.SetDynamic();
	blendStates.Realloc( 16 );
	blendStates.SetDynamic();
	rasterizerStates.Realloc( 16 );
	rasterizerStates.SetDynamic();

	vertexShaders.Realloc( 32 );
	vertexShaders.SetDynamic();
	pixelShaders.Realloc( 32 );
	pixelShaders.SetDynamic();

	pixelShader = nullptr;
	vertexShader = nullptr;
	geometryShader = nullptr;

	msaaLevel = 4;
	msaaQuality = 16;

	renderList = new RenderListItem[ MAX_RENDER_LIST_ITEMS ];
	ZeroMemory( renderList, sizeof( RenderListItem ) );
	renderListSize = 0;

	pointLights.Realloc( MAX_LIGHTS );
	directionalLights.Realloc( MAX_LIGHTS );
}

RenderSystem::~RenderSystem() {
	Release();
}

void RenderSystem::Initialize( const SystemParameters &sp, const RenderParameters &rp ) {
	systemParams = sp;
	renderParams = rp;
	InitializeAPI();
}

void RenderSystem::Release() {
	for ( ID3D11RasterizerState *state : rasterizerStates ) {
		::Release( &state );
	}
	for ( ID3D11BlendState *state : blendStates ) {
		::Release( &state );
	}
	for ( ID3D11DepthStencilState *state : depthStencilStates ) {
		::Release( &state );
	}
	//::Release( &backBufferView );
	::Release( &textureSampler );
	::Release( &swapChain );
	::Release( &deviceContext );
	::Release( &device );
}

void RenderSystem::InitializeAPI() {
	CreateDeviceAndSwapChain();
	CheckMSAALevels();

	// create back buffer render target view
	ID3D11Texture2D *backBuffer;
	swapChain->GetBuffer( 0, __uuidof( ID3D11Texture2D ), reinterpret_cast< LPVOID* >( &backBuffer ) );
	D3D11_TEXTURE2D_DESC backBufferDesc;
	backBuffer->GetDesc( &backBufferDesc );
	device->CreateRenderTargetView( backBuffer, NULL, &this->backBuffer.renderTargetView);
	backBuffer->Release();

	CreateGBuffers();
	CreateDepthStencilBuffer();
	CreateDepthStencilStates();
	CreateBlendStates();
	CreateRasterizerStates();

	CreateConstantBuffer( ConstantBufferSlot::GLOBAL, GLOBAL_CBUFFER_SIZE );
	BindConstantBuffers();
	SetViewport();
	LoadSystemShaders();
	CreateInputLayouts();
	InitializeTextureSamplers();
}

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

void RenderSystem::CreateInputLayout( const InputLayout layoutslot, const VSSlot vsslot, const D3D11_INPUT_ELEMENT_DESC *elements, const int elementsCount ) {
	if ( elements == nullptr ) {
		return;
	}
	ShaderSlot &shaderSlot = vertexShaders[ static_cast< int >( vsslot ) ];
	if ( shaderSlot.shader == nullptr ) {
		return;
	}
	VertexShader *vertexShader = ( VertexShader* )( shaderSlot.shader );
	ID3D11InputLayout *layout = nullptr;
	HRESULT hresult = device->CreateInputLayout( 
		elements,
		elementsCount,
		vertexShader->GetData(),
		vertexShader->GetDataSize(),
		&layout
	);
	if ( FAILED( hresult ) ) {
		return;
	}
	inputLayouts.Insert( layout, static_cast< int >( layoutslot ) );
}

void RenderSystem::BindConstantBuffers() {
	ID3D11Buffer *buffers[ MAX_CONSTANT_BUFFERS ];
	for ( int i = 0; i < MAX_CONSTANT_BUFFERS; i++ ) {
		buffers[ i ] = cbuffers[ i ].buffer;
	}
	deviceContext->VSSetConstantBuffers( 0, MAX_CONSTANT_BUFFERS, buffers );
	deviceContext->PSSetConstantBuffers( 0, MAX_CONSTANT_BUFFERS, buffers );
	deviceContext->GSSetConstantBuffers( 0, MAX_CONSTANT_BUFFERS, buffers );
}

void RenderSystem::SetRenderTargets( RenderTarget *renderTargets[], const unsigned int count, bool bindDepthStencil ) {
	const unsigned int MAX_RENDER_TARGETS = 8;
	if ( count > MAX_RENDER_TARGETS ) {
		return;
	}
	ID3D11RenderTargetView *views[ MAX_RENDER_TARGETS ] = { NULL };
	if ( renderTargets == nullptr ) {
		deviceContext->OMSetRenderTargets( MAX_RENDER_TARGETS, views, bindDepthStencil ? depthStencilView : NULL );
		return;
	}
	for ( unsigned int i = 0; i < count; i++ ) {
		views[ i ] = renderTargets[ i ]->renderTargetView;
	}
	deviceContext->OMSetRenderTargets( MAX_RENDER_TARGETS, views, bindDepthStencil ? depthStencilView : NULL );
}

void RenderSystem::SetPixelShaderResources( ID3D11ShaderResourceView *views[], const unsigned int count ) {
	const unsigned int MAX_SLOTS = 8;
	if ( count > MAX_SLOTS ) {
		return;
	}
	ID3D11ShaderResourceView *rtv[ MAX_SLOTS ] = { NULL };
	if ( views == nullptr ) {
		deviceContext->PSSetShaderResources( 0, MAX_SLOTS, rtv );
		return;
	}
	for ( unsigned int i = 0; i < count; i++ ) {
		rtv[ i ] = views[ i ];
	}
	deviceContext->PSSetShaderResources( 0, MAX_SLOTS, rtv );
}

void RenderSystem::UnbindConstantBuffers() {
	ID3D11Buffer *buffers[ MAX_CONSTANT_BUFFERS ];
	for ( int i = 0; i < MAX_CONSTANT_BUFFERS; i++ ) {
		buffers[ i ] = NULL;
	}
	deviceContext->VSSetConstantBuffers( 0, MAX_CONSTANT_BUFFERS, buffers );
	deviceContext->PSSetConstantBuffers( 0, MAX_CONSTANT_BUFFERS, buffers );
	deviceContext->GSSetConstantBuffers( 0, MAX_CONSTANT_BUFFERS, buffers );
}

void RenderSystem::CreateConstantBuffer( const int index, unsigned int byteSize ) {
	if ( index < 0 || index >= MAX_CONSTANT_BUFFERS ) {
		return;
	}
	if ( cbuffers[ index ].buffer != nullptr ) {
		return;
	}
	D3D11_BUFFER_DESC desc;
	ZeroMemory( &desc, sizeof( desc ) );
	desc.Usage = D3D11_USAGE_DYNAMIC;
	desc.ByteWidth = byteSize;
	desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	ID3D11Buffer *buffer = NULL;
	HRESULT hresult = device->CreateBuffer( &desc, NULL, &buffer );
	if ( FAILED( hresult ) ) {
		return;
	}
	cbuffers[ index ].size = byteSize;
	cbuffers[ index ].buffer = buffer;
}

void RenderSystem::ReleaseConstantBuffers() {
	for ( ConstantBuffer &cbuffer : cbuffers ) {
		::Release( &cbuffer.buffer );
		cbuffer.size = 0;
	}
}

void RenderSystem::UpdateConstantBuffer( const int index, const void * const data ) {
	if ( data == nullptr ) {
		return;
	}
	ID3D11Buffer *buffer = cbuffers[ index ].buffer;
	if ( buffer == nullptr ) {
		return;
	}
	D3D11_MAPPED_SUBRESOURCE subresource;
	HRESULT hresult = deviceContext->Map( buffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &subresource );
	if ( FAILED( hresult ) ) {
		return;
	}
	::memcpy( subresource.pData, data, cbuffers[ index ].size );
	deviceContext->Unmap( buffer, 0 );
}

APIDevice RenderSystem::GetAPIObject() {
	APIDevice object;
	object.device = device;
	object.deviceContext = deviceContext;
	return object;
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

void RenderSystem::RenderInstanced( VertexBuffer &buffer, VertexBuffer &instanceBuffer ) {
	RenderInstanced(
		buffer,
		instanceBuffer,
		instanceBuffer.GetVerticesOffset(),
		instanceBuffer.GetVerticesCount()
	);
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

void RenderSystem::ClearGBuffers() {
	SetPixelShader( PSSlot::GBUFFERS_CLEAR );
	DrawScreenQuad();
}

void RenderSystem::ClearBackBuffer() {
	const Color color = Color::BLACK;
	deviceContext->ClearRenderTargetView( backBuffer.renderTargetView, ( FLOAT* )&color );
}

void RenderSystem::ClearDepthStencil() {
	deviceContext->ClearDepthStencilView( depthStencilView, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0 );
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

void RenderSystem::AddLight( const Light &light ) {
	switch ( light.GetType() ) {
	case LightType::POINT:
		pointLights.Push( ( PointLight* )( &light ) );
		return;

	case LightType::DIRECTIONAL:
		directionalLights.Push( ( DirectionalLight* )( &light ) );
		return;
	}
}

void RenderSystem::RemoveLights() {
	pointLights.Clear();
	directionalLights.Clear();
}

void RenderSystem::AddMesh( const Mesh &mesh ) {
	if ( renderListSize >= MAX_RENDER_LIST_ITEMS ) {
		return;
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

void RenderSystem::SetRasterizerState( const RasterizerState state ) {
	if ( state == rasterizerState ) {
		return;
	}
	if ( state == RasterizerState::DEFAULT || rasterizerStates[ static_cast< int >( state ) ] == nullptr ) {
		deviceContext->RSSetState( NULL );
		rasterizerState = RasterizerState::DEFAULT;
		return;
	}
	deviceContext->RSSetState( rasterizerStates[ static_cast< int >( state ) ] );
	rasterizerState = state;
}

void RenderSystem::SetInputLayout( const InputLayout layout ) {
	if ( layout == inputLayout ) {
		return;
	}
	if ( inputLayouts[ static_cast< int >( layout ) ] == nullptr ) {
		return;
	}
	deviceContext->IASetInputLayout( inputLayouts[ static_cast< int >( layout ) ] );
	inputLayout = layout;
}

bool RenderSystem::SetTextures( Material &material ) {
	int count = material.GetTexturesCount();
	if ( count <= 0 ) {
		return false;
	}
	ID3D11ShaderResourceView *views[ Material::MAX_TEXTURES ];
	for ( int i = 0; i < count; i++ ) {
		Texture *texture = material.GetTexture( i );
		APITexture object = texture->GetAPIObject();
		views[ i ] = object.view;
	}
	deviceContext->PSSetShaderResources( 3, count, views );
	return true;
}

bool RenderSystem::SetNormalTexture( Texture &texture ) {
	APITexture object = texture.GetAPIObject();
	deviceContext->PSSetShaderResources( 4, 1, &object.view );
	return true;
}

void RenderSystem::SetDepthStencilState( const DepthStencilState state ) {
	if ( state == depthStencilState ) {
		return;
	}
	if ( state == DepthStencilState::DEFAULT || depthStencilStates[ static_cast< int >( state ) ] == nullptr ) {
		deviceContext->OMSetDepthStencilState( NULL, 0 );
		depthStencilState = state;
		return;
	}
	deviceContext->OMSetDepthStencilState( depthStencilStates[ static_cast< int >( state ) ], 0 );
	depthStencilState = state;
}

void RenderSystem::SetCamera( Camera *camera ) {
	this->camera = camera;
	UpdateCamera();
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

void RenderSystem::LoadSystemShaders() {
	LoadSystemShader( VSSlot::MESH,				L"..\\World\\shaders\\MeshVS.hlsl" );
	LoadSystemShader( VSSlot::LIGHT_PASS,		L"..\\World\\shaders\\LightPassVS.hlsl" );
	LoadSystemShader( VSSlot::BILLBOARD,		L"..\\World\\shaders\\BillboardVS.hlsl" );
	LoadSystemShader( VSSlot::SCREEN_QUAD,		L"..\\World\\shaders\\ScreenQuadVS.hlsl" );
	LoadSystemShader( PSSlot::DEFAULT,			L"..\\World\\shaders\\DefaultPS.hlsl" );
	LoadSystemShader( PSSlot::LIGHTTEST,		L"..\\World\\shaders\\LightTestPS.hlsl" );
	LoadSystemShader( PSSlot::GBUFFERS_CLEAR,	L"..\\World\\shaders\\GBuffersClear.hlsl" );
	LoadSystemShader( GSSlot::BILLBOARD,		L"..\\World\\shaders\\BillboardGS.hlsl" );
}

void RenderSystem::LoadSystemShader( const PSSlot slot, const wchar_t * const file ) {
	PixelShader *shader = new PixelShader();
	bool done = shader->CompileFile( file, nullptr );
	if ( !done ) {
		delete shader;
		return;
	}
	ShaderSlot shaderSlot;
	shaderSlot.shader = shader;
	shaderSlot.hash = 0;
	shaderSlot.references = 0;
	pixelShaders.Insert( shaderSlot, static_cast< int >( slot ) );
}

void RenderSystem::LoadSystemShader( const VSSlot slot, const wchar_t * const file ) {
	VertexShader *shader = new VertexShader();
	bool done = shader->CompileFile( file, nullptr );
	if ( !done ) {
		delete shader;
		return;
	}
	ShaderSlot shaderSlot;
	shaderSlot.shader = shader;
	shaderSlot.hash = 0;
	shaderSlot.references = 0;
	vertexShaders.Insert( shaderSlot, static_cast< int >( slot ) );
}

void RenderSystem::LoadSystemShader( const GSSlot slot, const wchar_t * const file ) {
	GeometryShader *shader = new GeometryShader();
	bool done = shader->CompileFile( file, nullptr );
	if ( !done ) {
		delete shader;
		return;
	}
	ShaderSlot shaderSlot;
	shaderSlot.shader = shader;
	shaderSlot.hash = 0;
	shaderSlot.references = 0;
	geometryShaders.Insert( shaderSlot, static_cast< int >( slot ) );
}

int RenderSystem::CompileMaterialShader( const char * const code ) {
	unsigned int hash = String::Hash( code );

	// pokud existuje shader se stejnym hashem, nekompilovat
	for ( int i = 0; i < materialShaders.Size(); i++ ) {
		ShaderSlot &slot = materialShaders[ i ];
		if ( slot.hash == hash ) {
			if ( slot.code == code ) {
				slot.references += 1;
				return i;
			}
		}
	}
	// zkompilovat shader
	PixelShader *shader = new PixelShader;
	if ( !shader->Compile( code, nullptr ) ) {
		delete shader;
		return -1;
	}
	ShaderSlot slot;
	slot.code = code;
	slot.hash = hash;
	slot.references = 1;
	slot.shader = shader;

	materialShaders.Push( slot );
	return materialShaders.Size() - 1;
}

void RenderSystem::LoadMaterial( Material &material ) {
	int basePassSlot = CompileMaterialShader( material.GetBasePassShaderCode() );
	material.SetBasePassShaderSlot( basePassSlot );
	int lightPassSlot = CompileMaterialShader( material.GetLightPassShaderCode() );
	material.SetLightPassShaderSlot( lightPassSlot );
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

void RenderSystem::SetBasePassMaterialShader() {
	if ( material == nullptr ) {
		return;
	}
	SetMaterialShader( material->GetBasePassShaderSlot() );
}

void RenderSystem::SetLightPassMaterialShader() {
	if ( material == nullptr ) {
		return;
	}
	SetMaterialShader( material->GetLightPassShaderSlot() );
}

void RenderSystem::SetMaterial( Material *material ) {
	this->material = material;
}

void RenderSystem::SetPixelShader( const PSSlot slot ) {
	PixelShader *shader = ( PixelShader* )( pixelShaders[ static_cast< int >( slot ) ].shader );
	if ( shader == nullptr ) {
		return;
	}
	ID3D11PixelShader *apiShader = reinterpret_cast< ID3D11PixelShader* >( shader->GetAPIObject() );
	if ( apiShader == this->pixelShader ) {
		return;
	}
	deviceContext->PSSetShader( apiShader, NULL, 0 );
	this->pixelShader = apiShader;
}

void RenderSystem::SetVertexShader( const VSSlot slot ) {
	VertexShader *shader = ( VertexShader* )( vertexShaders[ static_cast< int >( slot ) ].shader );
	if ( shader == nullptr ) {
		return;
	}
	ID3D11VertexShader *apiShader = reinterpret_cast< ID3D11VertexShader* >( shader->GetAPIObject() );
	if ( apiShader == this->vertexShader ) {
		return;
	}
	deviceContext->VSSetShader( apiShader, NULL, 0 );
	this->vertexShader = apiShader;
}

void RenderSystem::SetGeometryShader( const GSSlot slot ) {
	if ( slot == GSSlot::DEFAULT ) {
		deviceContext->GSSetShader( NULL, NULL, 0 );
		this->geometryShader = nullptr;
		return;
	}
	GeometryShader *shader = ( GeometryShader* )( geometryShaders[ static_cast< int >( slot ) ].shader );
	if ( shader == nullptr ) {
		return;
	}
	ID3D11GeometryShader *apiShader = reinterpret_cast< ID3D11GeometryShader* >( shader->GetAPIObject() );
	if ( apiShader == this->geometryShader ) {
		return;
	}
	deviceContext->GSSetShader( apiShader, NULL, 0 );
	this->geometryShader = apiShader;
}

void RenderSystem::SetAmbientColor( const Color &color ) {
	color.Store( reinterpret_cast< GlobalShaderConstants* >( globalCbuffer )->ambientColor );
}

void RenderSystem::SetDrawDistance( const float drawDistance ) {
	reinterpret_cast< GlobalShaderConstants* >( globalCbuffer )->drawDistance = drawDistance;
}

void RenderSystem::SetPrimitiveTopology( const PrimitiveTopology topology ) {
	if ( primitiveTopology != topology ) {
		deviceContext->IASetPrimitiveTopology( static_cast< D3D11_PRIMITIVE_TOPOLOGY >( topology ) );
		primitiveTopology = topology;
	}
}

//==========================================================================================
// TESTS TESTS TESTS TESTS TESTS TESTS TESTS TESTS TESTS TESTS TESTS TESTS TESTS TESTS TESTS
// TESTS TESTS TESTS TESTS TESTS TESTS TESTS TESTS TESTS TESTS TESTS TESTS TESTS TESTS TESTS
//==========================================================================================

#include "File.h"
#include "OBJLoader.h"
#include "ModelLoader.h"
#include "Math.h"
#include "Thread.h"

void RenderSystem::InitTest() {

	ModelLoader modelLoader;
	modelLoader.Load( "cube_teaport.fbx" );
	//modelLoader.CalculateNormals();
	Vertex *vertices = new Vertex[ modelLoader.GetVerticesCount() ];
	Index *indices = new Index[ modelLoader.GetIndicesCount() ];
	modelLoader.StoreVertices( vertices );
	modelLoader.StoreIndices( indices );

	mapMeshesBuffer = new VertexBuffer();
	VertexAllocation vparams;
	vparams.verticesUsage = Usage::IMMUTABLE;
	vparams.verticesCount = modelLoader.GetVerticesCount();
	vparams.vertices = vertices;
	vparams.vertexSize = sizeof( Vertex );
	vparams.indicesUsage = Usage::IMMUTABLE;
	vparams.indicesCount = modelLoader.GetIndicesCount();
	vparams.indices = indices;
	vparams.holdData = false;
	mapMeshesBuffer->Allocate( vparams );

	// instance buffer
	MeshInstanceData mid[ 2 ];
	Matrix mm;
	mm.Identity();
	mm.Translate( 0, -0.5f, 0 );
	mm.StoreColumnMajor( mid[ 0 ].transformations );
	mm.Identity();
	mm.Translate( 2, 2, 2 );
	mm.RotateRollPitchYaw( 2.8f, 0, 0 );
	mm.StoreColumnMajor( mid[ 1 ].transformations );

	vparams.verticesUsage = Usage::DYNAMIC;
	vparams.verticesCount = 2;
	vparams.vertices = mid;
	vparams.vertexSize = sizeof( MeshInstanceData );
	vparams.indicesUsage = Usage::IMMUTABLE;
	vparams.indicesCount = 0;
	vparams.indices = nullptr;
	vparams.holdData = false;
	meshesInstanceData.Allocate( vparams );

	// render list
	//AddMesh(  )
	//AddMesh(  )
	renderList[ 0 ].meshData = mapMeshesBuffer;
	renderList[ 0 ].instanceData = &meshesInstanceData;
	renderList[ 0 ].instanceOffset = 0;
	renderList[ 0 ].count = 2;
	renderList[ 1 ].meshData = nullptr;
	

	cameraInstance = new Camera( float( renderParams.screenWidth ) / float( renderParams.screenHeight ), math::PIDIV2, 50.0f );
	cameraInstance->SetPosition( 0, 0.3f, -2.0f );
	SetCamera( cameraInstance );

	SetAmbientColor( Color( 0.09f, 0.1f, 0.13f, 1.0f ) );
	//UpdateConstantBuffer( ConstantBufferSlot::GLOBAL, globalShaderConstants );

	// textures

	static Image image;
	ImageImport imageImport;
	imageImport.format = Format::BC1;
	imageImport.generateMips = true;
	imageImport.mipsAlgorithm = MipsAlgorithm::DEFAULT;
	image.Import( "texture_checker.tga", imageImport );
	static Texture texture;
	texture.Create( image );
	
	static Image imageSpecular;
	imageSpecular.Import( "specular.png", imageImport );
	static Texture textureSpecular;
	textureSpecular.Create( imageSpecular );
	SetNormalTexture( textureSpecular );

	// lights

	light1.SetPosition( Vector( 0, 4.0f, -4.0f ) );
	light1.SetColor( Color( 0.8f, 0.7f, 0.6f, 1.0f ) );
	light1.SetIntensity( 1.0f );
	light1.SetRadius( 20.0f );
	light1.Enable();

	light2.SetColor( Color( 1.0f, 0.0f, 0.0f, 1.0f ) );
	light2.SetIntensity( 3.0f );
	light2.SetRadius( 4.5f );
	light2.Enable();

	light3.SetColor( Color( 0.0f, 1.0f, 0.0f, 1.0f ) );
	light3.SetIntensity( 3.0f );
	light3.SetRadius( 4.5f );
	light3.Enable();

	AddLight( light1 );
	AddLight( light2 );
	AddLight( light3 );

	mesh = new Mesh();
	mesh->SetVertexBuffer( mapMeshesBuffer );
	mesh->SetColor( Color( 0xffd40b ) );

	Material *material = new Material();
	material->CreateTestMaterial();

	MaterialTextureSampler *sampler1 = new MaterialTextureSampler();
	sampler1->SetTexture( &texture );
	MaterialTextureSampler *sampler2 = new MaterialTextureSampler();
	sampler2->SetTexture( &textureSpecular );

	material->ConnectDifuse( sampler1, ShaderVectorComponent::RGBA );
	material->ConnectSpecular( sampler2, ShaderVectorComponent::RGBA );
	material->CreateShaderCode();

	LoadMaterial( *material );
	SetMaterial( material );

	screenQuad = new ScreenQuad();
	billboard = new Billboard();
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

	//---------------------------------------------------   DEFERRED TEST

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

void RenderSystem::LightPass() {
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
************************************************** */
