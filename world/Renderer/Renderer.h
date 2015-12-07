#pragma once

#include "RenderInterface.h"
#include <vector>

// forward declarations
class Window;

// Chybova hodnota identifikatoru okna (Renderer::RegisterWindow())
const uint32_t INVALID_WINDOW_HANDLE = static_cast< uint32_t >( -1 );

enum class RendererVersion {
	DX_11_0,
	GL_4_3
};

/*
Parametry funkce Initialize()
*/
struct RendererParams {
	// urcuje, jaka implementace RenderInterface bude pouzita
	RendererVersion version;

	// id adapteru poziteho k vykreslovani; 0 je vychozi adapter. V pripade nezdaru se pouzije vychozi adapter
	int adapter;
};

/*
Definuje zpusob mapovani oblasti render target bufferu (src) do back bufferu (dest)
*/
struct RenderOutputViewport {
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
	bool Initialize( const RendererParams& params );

	// Vytvori pro okno back buffer a rtv, vraci identifikator registrovaneho okna
	uint32_t RegisterWindow( const Window& window );

	// Uvolni back buffer a rtv patrici oknu (do okna neni mozne dale vykreslovat)
	void UnregisterWindow( const Window& window );

	// Zmeni velikost back bufferu patrici oknu
	void ResizeBackBuffer( const Window& window, const int width, const int height );

	// ResizeGBuffers( width, height )

private:
	RenderInterface::Device* device;
	RenderInterface::CommandInterface* immediateCommands;
	
	// registrovana okna
	struct RenderWindow {
		const Window* window;
		RenderInterface::BackBuffer* backBuffer;
		RenderInterface::RenderTargetView* renderTargetView;
	};
	std::vector< RenderWindow > renderWindows;

	// G-Buffery
	enum {
		GBUFFER_DIFUSE,
		GBUFFER_NORMAL,
		GBUFFER_SPECULAR,
		GBUFFERS_COUNT
	};
	struct GBuffer {
		RenderInterface::Buffer* buffer;
		RenderInterface::RenderTargetView* renderTargetView;
		RenderInterface::TextureView* textureView;
	};
	GBuffer gbuffers[ GBUFFERS_COUNT ];

	// engine poskytuje pevnou sadu texture sampleru, defaultni je mozne modifikovat nastavenim
	enum {
		SAMPLER_DEFAULT,
		SAMPLER_POINT,
		SAMPLER_LINEAR,
		SAMPLERS_COUNT
	};
	RenderInterface::Sampler *samplers[ SAMPLERS_COUNT ];
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