#pragma once

#include <d3d11.h>
#include "..\framework\Array.h"
#include "RenderInterface.h"

// forward deklarace
class Window;
class WinWindow;

// !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
//
// TENTO KOD JE POZUSTATEK Z PROTOTYPU, POTREBA KOMPLETRNE PREPSAT !
//
// !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!

// podporovane rezimy vyhlazovani
enum class Antialiasing {
	DISABLED,
	MSAA2,
	MSAA4,
	MSAA8,
	MSAA16
};

// parametr funkce SetDisplayMode(), vybere nejlepsi rezim pro dane zarizeni
const int BEST_DISPLAY_MODE = -1;

// identifikatory depth stencil states
enum class DepthStencil {
	DEFAULT = -1,
	DISABLED = 0,
	LIGHTPASS,
	READONLY
};

// identifikatory blend states
enum class Blending {
	DEFAULT = -1,
	ADD = 0
	//OIT,		// order independent transparency
	//ODT		// order depend transparency
};

// identifikatory rasterizer states
enum class Rasterizer {
	DEFAULT = -1
	//WIREFRAME = 0
};

// parametry funkce Initialize()
struct RendererInitialParameters {
	RenderInterface::DisplayMode displayMode;
	bool fullscreen;
	Antialiasing antialiasing;
	int maxAnisotropy;			// nastaveni kvality filtrovani textur; 0 = linear; > 0 = anisotropy
};

class Renderer {
public:
	Renderer();
	~Renderer();

	// inicializace
	void Initialize( const RendererInitialParameters &parameters );

private:
	struct RenderTarget {
		ID3D11Texture2D *texture = nullptr;
		ID3D11RenderTargetView *renderTargetView = nullptr;
		ID3D11ShaderResourceView *shaderResourceView = nullptr;
	};

	// parametry rendereru (typicke parametry kvality apod. nastavitelne v menu aplikace)
	struct Parameters {
		//int refreshRate;
		//bool vsync;
		Antialiasing antialiasing;
		RenderInterface::TextureFilter textureSampling;
		int anisotropyLevel;
	};

	// vlastnosti graficke karty
	struct DeviceFeatures {
		int msaaQuality[ 4 ]; // [ 2x, 4x, 8x, 16x ]
	};

private:
	// inicializace Direct3D
	//void CreateDeviceAndSwapChain( const RendererInitialParameters &parameters );
	void CreateBackBufferView();
	void CreateGBuffers( const int width, const int height );
	void CreateRenderTarget( const int width, const int height, const DXGI_FORMAT format, const Antialiasing antialiasing, RenderTarget &target );
	void CreateDepthStencilBuffer();
	void CreateDepthStencilStates();
	void CreateBlendStates();
	void CreateRasterizerStates();
	void CreateTextureSamplers();

	void SetViewport();

	// podpora zmeny rozlyseni
	void ResizeBackBuffer( const int width, const int height );
	void ResizeGBuffers( const int width, const int height );

	// shadery
	void UnbindShaderResources();
	void UnbindRenderTargets();

	// ziskani samples count a sample quality pro rezimy vyhlazovani
	//void CheckMultisampleQualityLevels();
	int GetSamplesCount( const Antialiasing antialiasing ) const;
	//int GetSampleQuality( const Antialiasing antialiasing ) const;

	// zjistit podporovane rezimy rozlyseni a obnovovaci frekvence
	void EnumDisplayModes();

private:
	//////////////////////////////////////////////////////
	// RenderInterface
	//////////////////////////////////////////////////////

	RenderInterface::Device *device;
	RenderInterface::DepthStencilBuffer *depthStencilBuffer;
	RenderInterface::CommandInterface *immediateCommandInterface;
	
	// G-Bffery deferred rendereru, color, normal
	enum { GBUFFERS_COUNT = 4 };
	RenderInterface::RenderBuffer *gbuffers[ GBUFFERS_COUNT ];

	// engine poskytuje pevnou sadu texture sampleru, defaultni je mozne modifikovat nastavenim
	enum { SAMPLERS_COUNT = 3 };
	RenderInterface::TextureSampler *samplers[ SAMPLERS_COUNT ]; // [ default, point, linear ]

	//////////////////////////////////////////////////////

	Parameters parameters;
	bool fullscreen;
	DeviceFeatures deviceFeatures;

	// directx states
	Array< ID3D11DepthStencilState* > depthStencilStates;
	DepthStencil depthStencilState;
	Array< ID3D11BlendState* > blendStates;
	Blending blendState;
	Array< ID3D11RasterizerState* > rasterizerStates;
	Rasterizer rasterizerState;

	Array< RenderInterface::DisplayMode > displayModes;
	int displayMode;
};


//*****************************************************
//*****************************************************

/*
#include <d3d11.h>
#include "Core.h"
#include "VertexBuffer.h"
#include "Material.h"
#include "Texture.h"
#include "Vector.h"
#include "Matrix.h"
#include "Light.h"
#include "String.h"
#include "Shader.h"
#include "Mesh.h"
#include "ScreenPlanes.h"
#include "containers\Array.h"

template < typename T >
inline void Release( T **target ) {
	if ( *target != nullptr ) {
		( *target )->Release();
		*target = nullptr;
	}
}

// forward deklarace
class Camera;

enum class Antialiasing {
	DISABLED,
	MSAA2,
	MSAA4,
	MSAA8
};

// nastaveni renderu pri inicializaci systemu
struct RenderParameters {
	int screenWidth;
	int screenHeight;
	int backBufferWidth;
	int backBufferHeight;
	bool fullscreen;
	Antialiasing antialiasing;
};

// parametry operacniho systemu
struct SystemParameters {
	HWND hwnd;
};

// globalni konstanty
const int MAX_SHADER_SLOTS = 1024;
const int MAX_LIGHTS = 1024;
const int MAX_RENDER_LIST_ITEMS = 16384;
const int DEPTH_BUFFERS_COUNT = 2;
const int MAX_CONSTANT_BUFFERS = 8;
const int GLOBAL_CBUFFER_SIZE = 64 * 4;

enum class PrimitiveTopology {
	POINTLIST		= D3D11_PRIMITIVE_TOPOLOGY_POINTLIST,
	LINELIST		= D3D11_PRIMITIVE_TOPOLOGY_LINELIST,
	LINESTRIP		= D3D11_PRIMITIVE_TOPOLOGY_LINESTRIP,
	TRIANGLELIST	= D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST,
	TRIANGLESTRIP	= D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP
};

// identifikatory vertex shaderu
enum class VSSlot {
	MESH = 0,
	LIGHT_PASS = 1,
	BILLBOARD,
	SCREEN_QUAD,
	LIGHT_VOLUME
};

// identifikatory pixel shaderu
enum class PSSlot {
	DEFAULT = 0,
	BILLBOARD,
	GBUFFERS_CLEAR,
	LIGHTTEST
};

// identifikatory geometry shaderu
enum class GSSlot {
	DEFAULT = -1,
	BILLBOARD = 0
};

// identifikatory input layouts
enum class InputLayout {
	DEFAULT = 0,
	MESH,
	BILLBOARD,
	LIGHT_VOLUME,
	SCREEN_QUAD
};

// identifikatory blend states
enum class BlendState {
	DEFAULT = -1,
	ADD = 0,
	OIT,		// order independent transparency
	ODT			// order depend transparency
};

// identifikatory rasterizer states
enum class RasterizerState {
	DEFAULT = -1,
	WIREFRAME = 0
};

// identifikatory depth stencil states
enum class DepthStencilState {
	DEFAULT = -1,
	LIGHT_PASS = 0,
	DISABLED,
	READONLY
};

// identifikatory contant bufferu
enum ConstantBufferSlot {
	GLOBAL = 0
};

struct RenderListItem {
	VertexBuffer *meshData;
	VertexBuffer *instanceData;
	int instanceOffset;
	int count;
};

//=======================================
//
// RenderSystem
//
//=======================================

class RenderSystem {
public:
	RenderSystem();
	~RenderSystem();
	void Initialize( const SystemParameters &sp, const RenderParameters &rp );
	void Release();
	void SetRasterizerState( const RasterizerState rasterizerState );
	void SetCamera( Camera *camera );
	void LoadMaterial( Material &material );
	void ReleaseMaterial( Material &material );
	APIDevice GetAPIObject();

	// public rendering interface
	void AddLight( const Light &light );
	void RemoveLights();
	void AddMesh( const Mesh &mesh );
	void RemoveMeshes();
	void Draw();

	// global shader constants setters
	void SetAmbientColor( const Color &color );
	void SetDrawDistance( const float drawDistance );

	// test
	void RenderTest();
	void InitTest();
	void TestUpdateInput();

private:
	struct RenderTarget {
		ID3D11Texture2D *texture = nullptr;
		ID3D11RenderTargetView *renderTargetView = nullptr;
		ID3D11ShaderResourceView *shaderResourceView = nullptr;
	};

	struct ShaderSlot {
		Shader *shader = nullptr;
		int references = 0;
		String code;
		unsigned int hash = 0;
	};

	struct ConstantBuffer {
		ID3D11Buffer *buffer;
		unsigned int size;
	};

private:
	void CheckMSAALevels();
	unsigned int GetMSAASampleCount( const Antialiasing mantialiasing ) const;
	unsigned int GetMSAASampleQuality( const Antialiasing antialiasing ) const;
	void UpdateCamera();
	void SetRenderTargets( RenderTarget *renderTargets[], const unsigned int count, bool bindDepthStencil );
	void SetPixelShaderResources( ID3D11ShaderResourceView *views[], const unsigned int count );
	bool SetTextures( Material &material );
	bool SetNormalTexture( Texture &texture );
	
	// initialization
	void InitializeAPI();
	void CreateDeviceAndSwapChain();
	void CreateRenderTarget( const unsigned int width, const unsigned int height, const DXGI_FORMAT format, const Antialiasing antialiasing, RenderTarget &target );
	void CreateDepthStencilBuffer();
	void CreateDepthStencilStates();
	void CreateBlendStates();
	void CreateRasterizerStates();
	void InitializeTextureSamplers();
	void CreateInputLayouts();
	void CreateInputLayout( const InputLayout layoutslot, const VSSlot vsslot, const D3D11_INPUT_ELEMENT_DESC *elements, const int elementsCount );
	void CreateGBuffers();
	void SetViewport();

	// constant buffers
	void CreateConstantBuffer( const int index, unsigned int byteSize );
	void ReleaseConstantBuffers();
	void BindConstantBuffers();
	void UnbindConstantBuffers();
	void UpdateConstantBuffer( const int index, const void * const data );

	// rendering
	void BasePass();
	void LightPass();
	void Render( VertexBuffer &buffer );
	void RenderInstanced( VertexBuffer &buffer, VertexBuffer &instanceBuffer );
	void RenderInstanced( VertexBuffer &vertexBuffer, VertexBuffer &instanceBuffer, const unsigned int instancesOffset, const unsigned int instancesCount );
	void DrawScreenQuad();
	void DrawRenderList( RenderListItem *renderList );
	void ClearGBuffers();
	void ClearBackBuffer();
	void ClearDepthStencil();

	// change states
	void SetInputLayout( const InputLayout layout );
	void SetDepthStencilState( const DepthStencilState state );
	void SetBlendState( const BlendState blendState );
	void SetPrimitiveTopology( const PrimitiveTopology topology );

	// system shaders
	void LoadSystemShaders();
	void LoadSystemShader( const PSSlot slot, const wchar_t * const file );
	void LoadSystemShader( const VSSlot slot, const wchar_t * const file );
	void LoadSystemShader( const GSSlot slot, const wchar_t * const file );
	void SetPixelShader( const PSSlot slot );
	void SetVertexShader( const VSSlot slot );
	void SetGeometryShader( const GSSlot slot );

	// material shads
	int CompileMaterialShader( const char * const code );
	void SetMaterialShader( const int id );
	void SetBasePassMaterialShader();
	void SetLightPassMaterialShader();
	void SetMaterial( Material *material );

private:
	SystemParameters systemParams;
	RenderParameters renderParams;

	// vlastnosti zarizeni, ziskane predevsim funkcemi device
	struct RenderSystemAttributes {
		unsigned int MSAAQualityLevels[ 4 ]; // [ 2x, 4x, 8x, 16x ]
	};
	RenderSystemAttributes deviceParameters;

	// stavy tridy
	unsigned int msaaLevel;
	unsigned int msaaQuality;

	// directx rozhrani
	ID3D11Device *device;
	ID3D11DeviceContext *deviceContext;
	IDXGISwapChain *swapChain;
	Array< ID3D11InputLayout* > inputLayouts;
	InputLayout inputLayout;
	ID3D11Texture2D *depthStencilBuffer;
	ID3D11DepthStencilView *depthStencilView;
	Array< ID3D11DepthStencilState* > depthStencilStates;
	DepthStencilState depthStencilState;
	Array< ID3D11BlendState* > blendStates;
	BlendState blendState;
	Array< ID3D11RasterizerState* > rasterizerStates;
	RasterizerState rasterizerState;
	ID3D11SamplerState *textureSampler;
	RenderTarget backBuffer;
	PrimitiveTopology primitiveTopology;
	ConstantBuffer cbuffers[ MAX_CONSTANT_BUFFERS ];
	RenderTarget gbuffers[ 4 ];

	ScreenQuad *screenQuad;
	Camera *camera;

	//====================================
	// Testovaci promenne. Bude soucasti tridy Map (nebo podobne)
	VertexBuffer *mapMeshesBuffer;
	//====================================
	
	// ------------------- zkontrolovat -------------------

	Byte globalCbuffer[ GLOBAL_CBUFFER_SIZE ];

	// systemove shadery
	Array< ShaderSlot > pixelShaders;
	Array< ShaderSlot > vertexShaders;
	Array< ShaderSlot > geometryShaders;
	Array< ShaderSlot > materialShaders;
	ID3D11PixelShader *pixelShader;
	ID3D11VertexShader *vertexShader;
	ID3D11GeometryShader *geometryShader;

	// material shadery
	Material *material;

	// lights
	Array< PointLight* > pointLights;
	Array< DirectionalLight* > directionalLights;

	// render list
	RenderListItem *renderList;
	int renderListSize;
	VertexBuffer meshesInstanceData;

private:
	//===========================
	// TESTS SCENE DATA
	PointLight light1;
	PointLight light2;
	PointLight light3;
	Mesh *mesh;
	Camera *cameraInstance;
	Billboard *billboard;
	//==========================
};
*/
