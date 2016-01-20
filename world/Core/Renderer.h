#pragma once

#include <vector>
#include "RenderInterface.h"
#include "RenderDeviceResources.h"

// forward declarations
class Window;

typedef int Identifier;

/*
Definuje zpusob mapovani oblasti renderbufferu (renderbuffer) do back bufferu (dest).
Pri primem vystupu do back bufferu se pouziji pouze polozky dest.
*/
struct Viewport {
	float destX;
	float destY;
	float destWidth;
	float destHeight;
	float renderbufferX;
	float renderbufferY;
	float renderbufferWidth;
	float renderbufferHeight;
};

/*
Podporovane rezimy vyhlazovani hran implementovane tridou Renderer
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
struct RendererAttributes {
	int renderbuffersWidth;
	int renderbuffersHeight;
	Antialiasing antialiasing;
};

/*
Formaty vertexu
*/

struct VertexColorLine {
	Float3 position;	// float4: POSITION0
	Float4 color;		// float4: COLOR0
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

	// Inicializace, volat pred pouzitim objektu. Objekt device muze byt sdilen vice objekty Renderer.
	bool Initialize( std::shared_ptr< RenderInterface::Device > device, const RendererAttributes& attributes );

	// Vytvori back buffer patrici oknu
	void CreateBackBuffer( const Window& window );

	// Uvolni backbuffer a rtv patrici oknu
	void DeleteBackBuffer( const Window& window );

	// Zmeni velikost back bufferu
	void ResizeBackBuffer( const Window& window );

	// Zmeni velikost renderbufferu
	void ResizeRenderBuffers( const int width, const int height );

	// Zmeni rezim vyhlazovani
	void SetAntialiasing( const Antialiasing antialiasing );

	/*
	Vytvori shader objekt a vrati jeho identifikator.
	Pri selhani vraci hodnotu Renderer::SHADER_INVALID.
	*/
	Identifier CreateShader( const char* const source, const RenderInterface::ShaderType type );

	Identifier Renderer::CreatePixelShader( const char* const source, const RenderInterface::ShaderType type );

	/*
	Vytvori novy RenderProgram objekt, vraci jeho identifikator.
	Nekontroluje duplicitu (vytvareni by nemelo byt reseno hrubou silou)
	V pripade selhani vraci hodnotu Renderer::RENDER_PROGRAM_INVALID.
	*/
	Identifier CreateRenderProgram( const Identifier vsid, const Identifier psid, const Identifier gsid );

	/*
	Odstrani vsechny shadery vytvorene funkci CreateShader(). Existujici identifikatory se stanou neplatnymi.
	Tato metoda muze byt pouzita napriklad k uvolneni zdroju pri nacitani jineho herniho prostredi.
	*/
	void ReleaseCreatedShaders();

	/*
	Odstrani vsechny renderprogramy vytvorene funkcemi CreateRenderProgram(). Existujici identifikatory se stanou neplatnymi.
	Tato metoda muze byt pouzita napriklad k uvolneni zdroju pri nacitani jineho herniho prostredi.
	*/
	void ReleaseCreatedRenderPrograms();

	/*
	Definuje, oblast oblast do ktere je mapovan vystup z pipeline,
	a take jakym zpusobem se oblast renderbufferu mapuje do back bufferu
	*/
	void SetViewport( const Viewport& viewport );

	// Vrati ukazatel na pamet bufferu, pri selhani vraci nullptr
	//void* Map( Buffer& buffer, const int subresource, const MapPolicy policy, MappedBuffer& result );

	// Zkopiruje do subresource '0' size pocet bajtu, kopirovani zacina offset bajtu od zacatku bufferu.
	void UpdateBuffer( Buffer& buffer, void* const data, const int offset, const int size );


	//void UpdateSubtexture(  buffer, texturearrindex, texturemipindex, data );
	// CreateVertexStream( Identifier layout, vertexBuffer, indexBuffer )
	//ReleaseRenderPrograms()
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

public:
	enum {
		RENDER_PROGRAM_INVALID = -1,
		RENDER_PROGRAM_DEFAULT = 0,
		// RENDER_PROGRAM_COLOR
		// RENDER_PROGRAM_DIFUSE_COLOR
		// RENDER_PROGRAM_DIFUSE_MAP
		// RENDER_PROGRAM_DIFUSE_MAP_NORMAL_MAP
		RENDER_PROGRAM_COUNT
	};

	enum {
		SHADER_INVALID = -1,
		SHADER_GS_DEFAULT = -2,	// specialni identifikator pro vychozi (deaktivovany) geometry shader
		SHADER_VS_DEFAULT = 0,
		SHADER_PS_DEFAULT,
		SHADER_COUNT
	};

	enum {
		LAYOUT_COLORLINE,
		LAYOUT_COUNT
	};

private:
	/*
	RenderBuffer zapouzdruje buffer, texture view a RTV
	*/
	class RenderBuffer {
	public:
		bool Create(
			std::shared_ptr< RenderInterface::Device > device,
			const RenderInterface::Format format,
			const int width,
			const int height,
			const int samplesCount,
			const int samplesQuality
		);
		void Release();
		RenderInterface::TextureView* GetTextureView();
		RenderInterface::RenderTargetView* GetRenderTargetView();

	private:
		RenderInterface::PBuffer buffer;
		RenderInterface::PTextureView textureView;
		RenderInterface::PRenderTargetView renderTargetView;
	};

	/*
	Depth stencil buffer
	*/
	class DepthStencilBuffer {
	public:
		bool Create(
			std::shared_ptr< RenderInterface::Device > device,
			const int width,
			const int height,
			const int samplesCount,
			const int samplesQuality
		);
		void Release();
		RenderInterface::DepthStencilView* GetDepthStencilView();

	private:
		RenderInterface::PBuffer buffer;
		RenderInterface::PDepthStencilView view;
	};

	/*
	Back buffery registrovanych oken
	*/
	struct TargetWindow {
		const Window* window;
		RenderInterface::PBackBuffer backBuffer;
		RenderInterface::PRenderTargetView renderTargetView;
	};

private:
	// Vytvori render buffery. Existujici buffery jsou uvolneny. Pri selhani jsou vsechny buffery nullptr.
	bool CreateRenderBuffers( const int width, const int height, const Antialiasing antialiasing );

	// Vytvori vychozi sadu depth stencil states. Pri selhani jsou vsechny states nastaveny na nullptr.
	bool CreateDepthStencilStates();
	
	/*
	Nacte shadery pouzivane jadrem enginu a vytvori vychozi renderprogramy.
	Shadery jsou ulozeny v souborech definovanych v Paths.h.
	Shadery museji byt nacteny pred vytvarenim vertex layoutu...
	Vsechny dosud nactene shadery a renderprogramy jsou uvolneny.
	Funkce vraci true jen kdyz se podari nacist vsechny shadery (jsou potreba k vytvoreni dalsich objektu).
	Pri selhani nemeni stav objektu Renderer.
	*/
	bool LoadShaders();

	/*
	Vytvori vsechny zakladni vertex layouty.
	K vytvoreni vertex layoutu je potreba shader (RenderProgram objekt),
	proto je nutne pred volanim funkce nejprve nacist vsechny shadery.
	*/
	bool CreateVertexLayouts();

	RenderInterface::RenderProgram* GetRenderProgram( const Identifier id );

	// SetCurrentBackBuffer( Window )

private:
	std::shared_ptr< RenderInterface::Device > device;
	RenderInterface::PCommandInterface immediateCommands;
	
	// atributy
	RendererAttributes attributes;

	// back buffery registrovanych oken
	std::vector< TargetWindow > targetWindows;

	// render buffery
	std::vector< RenderBuffer > renderBuffers;

	// depth stencil objekty
	DepthStencilBuffer depthStencilBuffer;
	std::vector< RenderInterface::PDepthStencilState > depthStencilStates;

	// shadery
	std::vector< RenderInterface::PShader > shaders;

	// render programy
	std::vector< RenderInterface::PRenderProgram > renderPrograms;

	// Vertex layouts
	std::vector< RenderInterface::PVertexLayout > vertexLayouts;

	// Viewport framebufferu a back bufferu.
	//RenderInterface::Viewport viewport;

	// Viewport mapujici oblast renderbufferu do back bufferu.
	Viewport viewport;
};