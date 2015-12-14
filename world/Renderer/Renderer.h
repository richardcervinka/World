#pragma once

#include "RenderInterface.h"
#include <vector>

// forward declarations
class Window;

/*
Definuje zpusob mapovani oblasti frame bufferu (src) do back bufferu (dest)
*/
struct FramebufferViewport {
	float srcX;
	float srcY;
	float srcWidth;
	float srcHeight;
	float destX;
	float destY;
	float destWidth;
	float destHeight;
};

/*
Podporovane rezimy vyhlazovani hran, ktere implementuje trida Renderer
*/
enum class Antialiasing {
	DISABLED,
	MSAA_2,
	MSAA_4,
	MSAA_8
};

/*
Parametry funkce Initialize()
*/
struct RendererAttribites {
	int renderbuffersWidth;
	int renderbuffersHeight;
	Antialiasing antialiasing;
};

/*
Rozhrani enginu pro vykreslovani grafiky. Promo vyuziva RenderInterface.
*/
class Renderer {
public:
	Renderer();
	~Renderer();

	// neni povoleno vytvaret kopie
	Renderer( const Renderer& renderer ) = delete;
	Renderer& operator=( const Renderer& renderer ) = delete;

	// inicializace, je nutne volat pred prvnim pouzitim objektu
	bool Initialize( RenderInterface::Device* const device, const RendererAttribites& attributes );

	// Vytvori pro okno back buffer a rtv
	void CreateBackBuffer( const Window& window );

	// Uvolni backbuffer a rtv patrici oknu
	void DeleteBackBuffer( const Window& window );

	// Zmeni velikost back bufferu
	void ResizeBackBuffer( const Window& window );

	// Zmeni velikost renderbufferu
	void ResizeRenderBuffers( const int width, const int height );

	// Zmeni rezim vyhlazovani hran
	void SetAntialiasing( const Antialiasing antialiasing );

	// render passes
	// BeginDrawPass()
	// EndDrawPass()
	// PresentFramebuffer( FramebufferViewport, Window )

	// multithreading
	// BeginDrawThread()
	// EndDrawThread()
	// ExecuteDrawThreads()

	// 2D drawing commands
	// DrawSprites( ... )
	// DrawSpritesDirect()

	// 3D drawing commands

	// Lighting
	// Light( ... )

private:
	void CreateRenderbuffers( const int width, const int height, const Antialiasing antialiasing );
	void CreateGBuffer( const int slot, const RenderInterface::Format format, const int width, const int height, const int samplesCount, const int samplesQuality );
	// SetCurrentBackBuffer( Window )

private:
	RenderInterface::PDevice device;
	RenderInterface::PCommandInterface immediateCommands;
	
	// atributy
	RendererAttribites attributes;

	// registrovana okna
	struct RenderTargetWindow {
		const Window* window;
		RenderInterface::PBackBuffer backBuffer;
		RenderInterface::PRenderTargetView renderTargetView;
	};
	std::vector< RenderTargetWindow > renderTargetWindows;

	struct RenderBuffer {
		RenderInterface::PBuffer buffer;
		RenderInterface::PTextureView textureView;
		RenderInterface::PRenderTargetView renderTargetView;
	};

	// G-Buffers
	enum {
		GBUFFER_FRAME,		// (non-multisampled) render pass render target (resolved result)
		GBUFFER_DIFUSE,		// (multisampled) RGBA difuse slozka, pouziva se take jako druhy buffer v postrpocesingu
		GBUFFER_NORMAL,		// (multisampled) normal buffer
		GBUFFER_SPECULAR,	// (multisampled)
		GBUFFERS_COUNT
	};
	RenderBuffer gbuffers[ GBUFFERS_COUNT ];

	// Pouziva se jen pri msaa
	RenderBuffer coverageMap;

	// depth stencil buffer
	RenderInterface::PBuffer depthStencilBuffer;
	enum {
		DEPTHSTENCILSVIEW_STANDARD,
		DEPTHSTENCILSVIEW_READONLY,
		DEPTHSTENCILSVIEWS_COUNT
	};
	RenderInterface::PDepthStencilView depthStencilViews[ DEPTHSTENCILSVIEWS_COUNT ];

	//Viewport renderbufferViewport;
	//FramebufferViewport renderbufferViewport;

	// engine poskytuje pevnou sadu texture sampleru, defaultni je mozne modifikovat nastavenim
	/*
	enum {
		SAMPLER_DEFAULT,
		SAMPLER_POINT,
		SAMPLER_LINEAR,
		SAMPLERS_COUNT
	};
	RenderInterface::Sampler *samplers[ SAMPLERS_COUNT ];
	*/
};

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
*/