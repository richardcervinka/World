#pragma once

#include "..\Framework\Color.h"

// forward declarations
class Window;

namespace RenderInterface {
	
	// forward declarations
	class Device;
	class Display;
	class CommandInterface;
	class CommandList;
	class TextureBuffer;
	class BackBuffer;
	class RenderTargetDescriptor;
	class TextureDescriptor;
	class DepthStencilDescriptor;
	class TextureSampler;
	class VertexBuffer;
	class VertexBufferDescriptor;
	class IndexBuffer;
	class IndexBufferDescriptor;
	class Shader;

	// Render Interface global constants
	const int MAX_RENDER_TARGETS = 8;
	const int MAX_MULTISAMPLE_QUALITY = -1;
	const int MAX_TEXTURE_LOD = -1;
	
	// Informace potrebne k vytvoreni objektu DX11Device
	struct DX11CreateDeviceParams {
		int adapter;				// id adapteru, 0 je vychozi adapter
		int majorFeatureLevels;
		int minorFeatureLevels;
	};
	
	// Vytvori device objekt implementovany v DirectX 11
	Device* DX11CreateDevice( const DX11CreateDeviceParams &params );
	
	// Format dat ulozenych v bufferu
	enum class Format {
		UNKNOWN = 0,
		R8G8B8A8_UNORM,
		R8G8B8A8_SNORM,
		R16G16B16A16_FLOAT,
		R16G16_FLOAT,
		R8_UNORM,
		R16_FLOAT,
		R32_FLOAT,
		DEPTH_24_UNORM_STENCIL_8_UINT,
		BC1,
		BC3
	};
	
	/*
	Formaty, ktere maji ruzny pocet bitu na jednotlive kanaly vraceji hodnotu channels = 1
	pr. D24_UNORM_S8_UINT: channels = 1, channelBits = 32
	*/
	struct FormatDesc {
		Format format;
		int channelCount;	// pocet kanalu na pixel
		int channelBits;	// pocet bitu na kanal
		int pointPitch; 	// slouzi k vypoctu row pitch; rowPitch = pointPitch * textureWidth
	};
	
	const FormatDesc GetFormatDesc( const Format format );
	
	// Fullscreen rezim displeje
	struct DisplayMode {
		int width;
		int height;
		int refreshRateNumerator;
		int refreshRateDenominator;
	};

	// Usage popisuje jakym zpusobem pristupuje GPU a CPU do bufferu
	enum class BufferUsage {
		DRAW,		// GPU ma plny pristup do bufferu (CPU nema zadny pristup), typicke pouziti render target buffer
		STATIC,		// GPU readonly, buffer musi byt inicializovan pri svem vytvoreni, obsah nemuze byt nijak zmenen
		DYNAMIC,	// GPU read only, CPU write only; napr. dynamicky vertex buffer
		COPY		// GPU read write, CPU read write
	};

	/*
	CPU pristupova prava do bufferu, konkretizuje hodnotu BufferUsage.
	Hodnoty je mozne prevest na uint a pracovat s nimi jako s priznaky (flags)
	*/
	enum class BufferAccess {
		NONE		= 0x00,
		READ		= 0x01,
		WRITE		= 0x02,
		READ_WRITE	= READ | WRITE
	};

	// Rozmery texture bufferu. Pokud neni rozmer definovan, je jeho hodnota 1.
	struct TextureDimmensions {
		int width;
		int height;
		int depth;
	};
	
	// Typ texture bufferu
	enum class TextureBufferType {
		UNDEFINED,
		TEXTURE_1D,
		TEXTURE_1D_ARRAY,
		TEXTURE_2D,
		TEXTURE_2D_ARRAY,
		TEXTURE_2D_MS,
		TEXTURE_2D_MS_ARRAY,
		TEXTURE_3D
	};
	
	/*
	Parametry funkce Device::CreateTextureBuffer()
	Generovani mipmap pri vytvareni texture bufferu je zakazane, pocet mip urovni musi byt znamy pred vytvorenim objektu.
	*/
	struct TextureBufferParams {
		Format format;
		BufferUsage usage;
		BufferAccess access;
		TextureBufferType type;
		TextureDimmensions dimmensions;
		int arraySize;
		int mipLevels;
		int samplesCount;
		int samplesQuality;
		bool renderTarget;
	};

	// Filtrovani textur
	enum class TextureFilter {
		POINT,
		POINT_MIP_LINEAR,
		MIN_POINT_MAG_LINEAR_MIP_POINT,
		MIN_POINT_MAG_MIP_LINEAR,
		MIN_LINEAR_MAG_MIP_POINT,
		MIN_LINEAR_MAG_POINT_MIP_LINEAR,
		LINEAR_MIP_POINT,
		LINEAR,
		ANISOTROPIC
	};
	
	// Adresovani textur
	enum class TextureAddressing {
		WRAP,
		MIRROR,
		CLAMP
	};
	
	struct TextureSamplerParams {
		TextureFilter filter;
		TextureAddressing uAddressing;
		TextureAddressing vAddressing;
		TextureAddressing wAddressing;
		int maxAnisotropy;
		float minLOD;	// hodnota >= 0
		float maxLOD;	// hodnota v rozsahu <0; MAX_TEXTURE_LOD>
	};

	enum class DepthStencilComparsion {
		NEVER,
		LESS,
		EQUAL,
		LESS_EQUAL,
		GREATER,
		NOT_EQUAL,
		GREATER_EQUAL,
		ALWAYS
	};

	enum class StencilOperation {
		KEEP,
		ZERO,
		REPLACE,
		INCR_SAT,
		DECR_SAT,
		INVERT,
		INCR,
		DECR
	};

	enum DepthStencilUsage {
		DISABLED,	// no depth or stencil tests performed
		STANDARD,	// read write depth stencil usage
		READONLY	// readonly depth stencil usage
	};

	/*
	Parametry funkce Device::CreateDepthStencilDescriptor()
	Stejne nastaveni pro front i back face
	*/
	struct DepthStencilDescriptorParams {
		DepthStencilUsage depthUsage;			// default: STANDARD
		DepthStencilUsage stencilUsage;			// default: STANDARD
		DepthStencilComparsion depthFunc;		// default: LESS
		DepthStencilComparsion stencilFunc;		// default: ALWAYS
		StencilOperation stencilPassOp;			// default: KEEP
		StencilOperation stencilFailOp;			// default: KEEP
		StencilOperation stencilDepthFailOp;	// default: KEEP
	};

	//Parametry funkce Device::CreateVertexBuffer()
	struct VertexBufferParams {
		int vertexSize;
		int capacity;
		BufferUsage usage;
		BufferAccess access;
	};

	enum class IndexBufferFormat {
		UINT_16,
		UINT_32
	};

	//Parametry funkce Device::CreateIndexBuffer()
	struct IndexBufferParams {
		IndexBufferFormat format;
		int capacity;
		BufferUsage usage;
		BufferAccess access;
	};

	//Parametry funkce Device::CreateConstantBuffer()
	struct ConstantBufferParams {
		int size;
		BufferUsage usage;
		BufferAccess access;
	};

	/*
	ConstantBufferConstant

	Popisuje konstantu constant bufferu.
	Pouziva se jako pole, konstanty museji byt ulozeny v systemove pameti ve stejnem poradi jako v tomto poli.
	pr.:

	// HLSL
	cbuffer Constants {
		float3 color;
		float4 position;
	}
	// C++
	struct Constants {
		Float4 color;
		Float3 position;
	}
	ConstantBufferConstant map[] = {
		{ "color", 3 },
		{ "position", 4 }
	};
	*/
	struct ConstantBufferConstant {
		const char* name;
		int size;
	};

	enum class ShaderType {
		UNDEFINED,
		VERTEX_SHADER,
		PIXEL_SHADER,
		GEOMETRY_SHADER
	};

	typedef unsigned int ShaderCompileFlags;
	const ShaderCompileFlags SHADER_COMPILE_WARNINGS_AS_ERRRORS	= ( 0x01 );
	const ShaderCompileFlags SHADER_COMPILE_DEBUG				= ( 0x01 << 1 );

	enum class ShaderOptimization {
		DISABLED,	// optimalizace vypnuta (nejrychlejsi kompilace)
		LOW,
		MEDIUM,
		HIGH		// nejlepsi mozna optimalizace (nejpomalejsi kompilace)
	};

	enum class ShaderVersion {
		UNDEFINED,
		HLSL_50_GLSL_430	// HLSL 5.0, GLSL 4.30
	};

	struct ShaderParams {
		char* const string;		// null terminated ASCII string
		char** const defines;	// null terminated array, seznam identifikatoru vlozenych zacatek kodu (#define)
		ShaderType type;
		ShaderVersion version;
		ShaderCompileFlags flags;
		ShaderOptimization optimization;
	};

	// Identifikace shader stag, hodnoty lze kombinovat operatorem OR
	/*
	typedef unsigned int ShaderStage;
	const ShaderStage SHADER_STAGE_VERTEX_SHADER	= 0x0001;
	const ShaderStage SHADER_STAGE_PIXEL_SHADER		= 0x0002;
	const ShaderStage SHADER_STAGE_GEOMETRY_SHADER	= 0x0004;
	*/

	/*
	DEVICE CREATE TARGETS:

	BlendState
	ClassLinkage
	ComputeShader
	Counter
	DomainShader
	GeometryShader
	GeometryShaderWithStreamOutput
	HullShader
	InputLayout
	PixelShader
	Predicate
	Query
	RasterizerState
	UnorderedAccessView
	VertexShader
	*/

	/*
	Zakladni trida pro vsechny objekty vytvarene tridou Device.
	Objekt Device je factory vsech device objektu, device objekty se uvolnuji metodou Release()
	*/
	class DeviceObject {
	public:
		DeviceObject();
		virtual ~DeviceObject() = 0;
		
		// neni mozne vytvaret kopie device objektu, jediny, kdo vytvari device objekty je objekt Device
		DeviceObject( const DeviceObject& ) = delete;
		DeviceObject& operator=( const DeviceObject& ) = delete;
		
		// Pokud je pocet referenci 0, uvolni objekt z pameti a nesmi byt dale pouzivan!
		void Release();
		
		// inkrementuje pocitadlo referenci, objekt neni uvolnen z pameti, dokud je pocet referenci vetsi nez 0
		void AddRef();

	private:
		int references;
	};
	
	/*
	Reprezentuje grafickou kartu, vytvari veskere zdroje.
	*/
	class Device: public DeviceObject {
	public:
		// vytvareni device objektu
		virtual CommandInterface*		CreateCommandInterface() = 0;
		virtual Display*				CreateDisplay( const int outputId ) = 0;
		virtual BackBuffer*				CreateBackBuffer( Window& window ) = 0;
		virtual VertexBuffer*			CreateVertexBuffer( const VertexBufferParams& params, const void* const initialData  ) = 0;
		virtual IndexBuffer*			CreateIndexBuffer( const IndexBufferParams& params, const void* const initialData  ) = 0;
		virtual VertexBufferDescriptor*	CreateVertexBufferDescriptor( VertexBuffer* const buffer, const int vertexOffset ) = 0;
		virtual TextureBuffer*			CreateTextureBuffer( const TextureBufferParams& params, const void* const initialData[] ) = 0;
		virtual RenderTargetDescriptor*	CreateRenderTargetDescriptor( TextureBuffer* const buffer ) = 0;
		virtual RenderTargetDescriptor*	CreateRenderTargetDescriptor( BackBuffer* const buffer ) = 0;
		virtual DepthStencilDescriptor*	CreateDepthStencilDescriptor( TextureBuffer* const buffer, const DepthStencilDescriptorParams& params ) = 0;
		virtual TextureSampler*			CreateTextureSampler( const TextureSamplerParams& params ) = 0;
		virtual Shader*					CreateShader( const ShaderParams& params ) = 0;
		/*
		vrati max quality pro pozadovany pocet msaa level.
		Ne vsechny karty a ne vsechna api museji podporovat tuto vlastnost, pak musi funkce vracet 1.
		Pokud neni msaa level podporovan, funkce vraci 0;
		*/
		virtual int GetMultisampleQuality( const int samplesCount ) const = 0;

		// Vraci pocet zobrazovacich zarizeni pripojenych na vystup graficke karty
		// virtual int GetOutputCount() const;
	};
	
	/*
	CommandInterface slouzi ke generovani commandu graficke karty
	*/
	class CommandInterface: public DeviceObject {
	public:
		// zahajeni posilani commanu primo do grafickeho zarizeni
		virtual void Begin( Device* const device ) = 0;
	
		// zahajeni ukladani commandu do CommandListu
		virtual void Begin( CommandList* const commandList ) = 0;
	
		// ukonceni sekvence commandu
		virtual void End() = 0;
		
		// Nastavi multiple render targets a depth stencil buffer (pokud je nullptr, pouzije se vychozi depth stencil state)
		virtual void SetRenderTargets( RenderTargetDescriptor* const renderTargets[], const int count, DepthStencilDescriptor* const depthStencil ) = 0;
		
		// Vyplni render target barvou
		virtual void ClearRenderTarget( RenderTargetDescriptor* const renderTarget, const Color& color ) = 0;

		// nastavi depth i stencil buffer na pozadovanou hodnotu
		virtual void ClearDepthStencil( DepthStencilDescriptor* const descriptor, const float depth, const uint8_t stencil ) = 0;

		// nastavi depth buffer na hodnotu depth
		virtual void ClearDepth( DepthStencilDescriptor* const descriptor, const float depth ) = 0;

		// nastavi stencil buffer na hodnotu stencil
		virtual void ClearStencil( DepthStencilDescriptor* const descriptor, const uint8_t stencil ) = 0;

		// Nastavi objekt Device do vychoziho stavu
		virtual void ClearState() = 0;

		// Odesle obsah command bufferu do GPU
		virtual void Flush() = 0;
	};
	
	/*
	Slouzi k zaznemenani commandu a jejich jednorazovemu prehrani. Po prehrani je obsah command listu vyprazdnen.
	*/
	class CommandList: public DeviceObject {
	public:
		//virtual void ExecuteCommandBatch( CommandBatch *batch );
	};
	
	/*
	Monitor pripojeny k vystupu graficke karty (Device)
	*/
	class Display: public DeviceObject {
	public:
		// Nastavi rezim co nejvice odpovidajici pozadavku (na desktopu prepne do rezimu cele obrazovky)
		// U zarizeni, ktera maji jediny mozny rezim obrazovky (mobilni zarizeni...), nedela nic
		virtual bool SetMode( const DisplayMode& mode, Window& window ) = 0;
		
		// Nastavi vychozi rezim pro danou platformu (napr. na windows prepne z celoobrazovkoveho rezimu)
		virtual void SetSystemMode() = 0;
		
		// Ziskani rezimu, pokud rezim s pozadovanym id neexistuje, vrati false
		virtual bool GetMode( const int id, DisplayMode& result ) const = 0;
		
		// Najde rezim, ktery co nejlepe (ovsem ne nutne nejvice) odpovida pozadovanemu rezimu
		virtual void FindMode( const DisplayMode& request, DisplayMode& result ) const = 0;
		
		// Najde nejlepsi dostupny rezim pro cilove zarizeni
		virtual void GetBestMode( DisplayMode& result ) const = 0;
	};
	
	/*
	Umoznuje zobrazeni obsahu back bufferu do klientske oblasti okna. Objekt je asociovan s oknem pri svem vytvoreni.
	*/
	class BackBuffer: public DeviceObject {
	public:
		virtual void Present( const int vsync ) = 0;
		virtual void Resize() = 0;
		//virtual int GetWidth() const = 0;
		//virtual int GetHeight() const = 0;
	};

	/*
	Texture buffer je obecne blok pameti pro ukladani dat textur.
	*/
	class TextureBuffer: public DeviceObject {
	public:
		TextureBuffer();

		// TextureBufferParams member getters
		const Format GetFormat() const;
		TextureBufferType GetType() const;
		int GetDimmension() const;
		TextureDimmensions GetDimmensions() const;
		int GetArraySize() const;
		int GetSamplesCount() const;
		int GetSamplesQuality() const;
		bool RenderTargetUsable() const;

	protected:
		// Odvozena trida nema pristup k privatni promenne desc, rozhrani proto poskytuje setter
		void SetParams( const TextureBufferParams& params );

	private:
		TextureBufferParams params;
	};

	/*
	TextureDescriptor slouzi k bindovani textury do pipeline stage
	*/
	class TextureDescriptor: public DeviceObject {
	public:
		virtual TextureBuffer* GetBuffer() = 0;
	};
	
	/*
	Umoznuje nabindovat texturu jako render target.
	Pouze nasledujici textury muzou byt pouzity jako render target: TEXTURE_1D, TEXTURE_2D, TEXTURE_2D_MS
	*/
	class RenderTargetDescriptor: public DeviceObject {
	public:
		virtual TextureBuffer* GetBuffer() = 0;
	};

	/*
	Umoznuje nabindovat DepthStencilBuffer do pipeline.
	Popisuje konfiguraci depth stencil testu a pristup do texture bufferu.
	Podporovane formaty asociovaneho texture bufferu: TEXTURE_2D a TEXTURE_2D_MS
	*/
	class DepthStencilDescriptor: public DeviceObject {
	};
	
	/*
	TextureSampler
	*/
	class TextureSampler: public DeviceObject {
	public:
	};

	/*
	Vertex buffer
	*/
	class VertexBuffer: public DeviceObject {
	public:
		// Maximalni pocet vertexu ulozenych v bufferu
		virtual int GetCapacity() const = 0;

		// Velikost vertexu v bajtech
		virtual int GetVertexSize() const = 0;

		// Velikost bufferu v bajtech
		virtual int GetByteSize() const = 0;
	};

	/*
	Vertex buffer descriptor umoznuje nabindovat VertexBuffer do pipeline.
	*/
	class VertexBufferDescriptor: public DeviceObject {};

	/*
	Index buffer, indexy jsou ve formatu Uint32
	*/
	class IndexBuffer: public DeviceObject {
	public:
		// Maximalni pocet indexu ulozenych v bufferu
		virtual int GetCapacity() const = 0;

		// Velikost bufferu v bajtech
		virtual int GetByteSize() const = 0;

		// Format indexu
		virtual IndexBufferFormat GetFormat() const = 0;
	};

	/*
	Index buffer descriptor umoznuje nabindovat IndexBuffer do pipeline.
	*/
	class IndexBufferDescriptor: public DeviceObject {};

	/*
	Constant buffer reprezentuje pouze datove uloziste, neobsahuje zadne informace o rozlozeni konstant apod.
	*/
	class ConstantBuffer: public DeviceObject {
	public:
	};

	/*
	Constant buffer descriptor popisuje format, umisteni a usporadani konstant v constant bufferu.
	Pomoci tohoto objektu jsou data namapovana do bufferu tak, aby odpovidala shaderu.
	*/
	class ConstantBufferDescriptor: public DeviceObject {};

	/*
	Trida pro vsechny typy shaderu (VS, PS i GS)
	*/
	class Shader: public DeviceObject {
	public:
		virtual ShaderType GetType() const = 0;
		virtual ShaderVersion GetVersion() const = 0;
	};

	//**************************************************


	struct BlendStateDesc {
	};
	
	struct RasterizeStateDes {
	};
	
	struct PipelineStateDescription {
		BlendStateDesc blendStateDesc;
		//RasterizeStateDesc rasterizeStateDesc;
	};
	
	class PipelineState: public DeviceObject {
	public:
	
	private:
	};
	
} // namespace RenderInterface


