#pragma once

#include <memory>
#include "..\Framework\Math.h"
#include "..\Framework\Color.h"

// forward declarations
class Window;

namespace RenderInterface {
	
	// forward declarations
	class Device;
	class BackBuffer;
	class Display;
	class CommandInterface;
	class CommandList;
	class Buffer;
	class TextureView;
	class RenderTargetView;
	class DepthStencilView;
	class ConstantBufferView;
	class Shader;
	class RenderProgram;
	class Sampler;
	class BlendState;
	class RasterizerState;
	class DepthStencilState;
	class VertexLayout;
	class VertexStream;

	// Deleter for device object unique_ptr
	template< typename T >
	class Deleter {
	public:
		void operator()( T* const ptr ) const {
			ptr->Release();
		}
	};

	// device objects template alliases
	using PDevice				= std::unique_ptr< Device, Deleter< Device > >;
	using PBackBuffer			= std::unique_ptr< BackBuffer, Deleter< BackBuffer > >;
	using PDisplay				= std::unique_ptr< Display, Deleter< Display > >;
	using PCommandInterface		= std::unique_ptr< CommandInterface, Deleter< CommandInterface > >;
	using PCommandList			= std::unique_ptr< CommandList, Deleter< CommandList > >;
	using PBuffer				= std::unique_ptr< Buffer, Deleter< Buffer > >;
	using PTextureView			= std::unique_ptr< TextureView, Deleter< TextureView > >;
	using PRenderTargetView		= std::unique_ptr< RenderTargetView, Deleter< RenderTargetView > >;
	using PDepthStencilView		= std::unique_ptr< DepthStencilView, Deleter< DepthStencilView > >;
	using PConstantBufferView	= std::unique_ptr< ConstantBufferView, Deleter< ConstantBufferView > >;
	using PShader				= std::unique_ptr< Shader, Deleter< Shader > >;
	using PRenderProgram		= std::unique_ptr< RenderProgram, Deleter< RenderProgram > >;
	using PSampler				= std::unique_ptr< Sampler, Deleter< Sampler > >;
	using PBlendState			= std::unique_ptr< BlendState, Deleter< BlendState > >;
	using PRasterizerState		= std::unique_ptr< RasterizerState, Deleter< RasterizerState > >;
	using PDepthStencilState	= std::unique_ptr< DepthStencilState, Deleter< DepthStencilState > >;
	using PVertexLayout			= std::unique_ptr< VertexLayout, Deleter< VertexLayout > >;
	using PVertexStream			= std::unique_ptr< VertexStream, Deleter< VertexStream > >;

	// Maximalni kvalita multisample algoritmu
	const int MAX_MULTISAMPLE_QUALITY = -1;

	// max texture lod
	const int MAX_TEXTURE_LOD = -1;

	// maximalni pocettextur nabindovanych shaderu
	const int MAX_TEXTURES = 32;

	// maximalni pocet aktivnich sampleru
	const int MAX_SAMPLERS = 8;

	// maximalni pocet aktivnich render targets
	const int MAX_RENDER_TARGETS = 8;

	// maximalni pocet slotu, na ktere se pripojuje vertex buffer
	const int MAX_VERTEX_INPUT_SLOTS = 16;

	// Maximalni pocet aktivnich konstant bufferu
	const int MAX_CBUFFER_SLOTS = 8;

	// Maximalni pocet viewportu
	const int MAX_VIEWPORTS = 8;
	
	/*
	Informace potrebne k vytvoreni objektu DX11Device
	*/
	struct DX11CreateDeviceParams {
		int adapter;				// id adapteru, 0 je vychozi adapter
		int majorFeatureLevels;
		int minorFeatureLevels;
	};
	
	// Vytvori Device objekt (DirectX implementace)
	PDevice DX11CreateDevice( const DX11CreateDeviceParams& params );

	enum class Format {
		UNKNOWN = 0,

		// 32 bit component
		R32G32B32A32_FLOAT,
		R32G32B32A32_UINT,
		R32G32B32_FLOAT,
		R32G32B32_UINT,
		R32G32_FLOAT,
		R32G32_UINT,
		R32_FLOAT,
		R32_UINT,

		// 16 bit component
		R16G16B16A16_FLOAT,
		R16G16B16A16_UINT,
		R16G16B16A16_UNORM,
		R16G16B16A16_SINT,
		R16G16B16A16_SNORM,
		R16G16_FLOAT,
		R16G16_UINT,
		R16G16_UNORM,
		R16G16_SINT,
		R16G16_SNORM,
		R16_FLOAT,
		R16_UINT,
		R16_UNORM,
		R16_SINT,
		R16_SNORM,

		// 8 bit component
		R8G8B8A8_UINT,
		R8G8B8A8_UNORM,
		R8G8B8A8_SINT,
		R8G8B8A8_SNORM,
		R8G8_UINT,
		R8G8_UNORM,
		R8G8_SINT,
		R8G8_SNORM,
		R8_UINT,
		R8_UNORM,
		R8_SINT,
		R8_SNORM,

		// packed
		DEPTH_24_UNORM_STENCIL_8_UINT,

		// block compression
		BC1,
		BC3
	};
	
	/*
	FormatInfo: informace o typu Format
	nektere vypocty:
	pointPitch = blockByteWidth / blockSize
	rowPitch = pointPitch * textureWidth
	*/
	struct FormatInfo {
		int channelsCount;		// pocet kanalu na pixel
		int channelByteWidth;	// pocet bytes na kanal
		int blockSize;			// velikost bloku
		int blockByteWidth;		// pocet bytes na blok
	};
	
	const FormatInfo GetFormatInfo( const Format format );

	/*
	Zpusob pristupu do bufferu
	*/
	enum class BufferUsage {
		DRAW,		// GPU vyhradni pristup do bufferu, typicke pouziti render target buffer
		STATIC,		// GPU readonly, buffer musi byt inicializovan pri vytvoreni
		DYNAMIC,	// GPU read, CPU write (napr. dynamicky vertex buffer)
		COPY		// GPU read write, CPU read write
	};

	/*
	Pristup CPU do bufferu, blize specifikuje BufferUsage.
	Hodnoty je mozne prevest na uint a pracovat s nimi jako s priznaky.
	*/
	enum class BufferAccess {
		NONE		= 0x00,
		READ		= 0x01,
		WRITE		= 0x02,
		READ_WRITE	= READ | WRITE
	};

	/*
	Typ bufferu
	*/
	enum class BufferType {
		UNDEFINED,
		TEXTURE_1D,
		TEXTURE_1D_ARRAY,
		TEXTURE_2D,
		TEXTURE_2D_ARRAY,
		TEXTURE_2D_MS,
		TEXTURE_2D_MS_ARRAY,
		TEXTURE_3D,
		VERTEX_BUFFER,
		INDEX_BUFFER,
		CONSTANT_BUFFER
	};

	/*
	Atributy bufferu
	*/
	struct BufferInfo {
		BufferType type;
		BufferUsage usage;
		BufferAccess access;
		int byteWidth;
	};

	/*
	Rozmery textury, hodnoty >= 1
	*/
	struct TextureDimmensions {
		int width;
		int height;
		int depth;
	};

	enum TextureBufferFlags {
		TEXTURE_BUFFER_FLAG_RENDER_TARGET = ( 1 << 1 )
	};

	/*
	Parametry funkce Device::CreateTextureBuffer()
	*/
	struct TextureBufferParams {
		BufferType type;
		Format format;
		BufferUsage usage;
		BufferAccess access;
		int width;
		int height;
		int depth;
		int mipLevels;
		int arraySize;
		int samplesCount;
		int samplesQuality;
		TextureBufferFlags flags;
	};

	/*
	Filtrovani textury
	*/
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

	/*
	Adresovani textury (jak adresovat hodnoty mimo rozsah <0; 1>
	*/
	enum class TextureAddressing {
		WRAP,
		MIRROR,
		CLAMP
	};
	
	/*
	Parametry funkce Device::CreateSampler()
	*/
	struct SamplerParams {
		TextureFilter filter;
		TextureAddressing uAddressing;
		TextureAddressing vAddressing;
		TextureAddressing wAddressing;
		int maxAnisotropy;
		float minLOD; // >= 0
		float maxLOD; // <0; MAX_TEXTURE_LOD>
	};

	enum class DepthStencilUsage {
		STANDARD,	// read write depth stencil usage
		READONLY	// readonly depth stencil usage
	};

	struct DepthStencilViewParams {
		DepthStencilUsage depthUsage;
		DepthStencilUsage stencilUsage;
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

	/*
	Parametry funkce Device::CreateDepthStencilState()
	*/
	struct DepthStencilStateParams {
		bool enableDepth;						// deffault:
		bool enableStencil;						// deffault:
		DepthStencilComparsion depthFunc;		// default: LESS
		DepthStencilComparsion stencilFunc;		// default: ALWAYS
		StencilOperation stencilPassOp;			// default: KEEP
		StencilOperation stencilFailOp;			// default: KEEP
		StencilOperation stencilDepthFailOp;	// default: KEEP
	};

	/*
	ShaderConstant: popisuje konstantu v constant bufferu
	HLSL
	cbuffer Constants {
		float3 color;
		float4 position;
	}
	C++
	struct Constants {
		Float3 color;
		Float4 position;
	}
	ShaderConstant constants[] = {
		{ "color",    sizeof( Constants::color ),    alignof( Constants::color ) },
		{ "position", sizeof( Constants::position ), alignof( Constants::position ) }
	};
	*/
	struct ShaderConstant {
		const char* name;
		int size;
		int align;
	};

	/*
	parametry funkce Device::CreateConstantBufferView()
	*/
	struct ConstantBufferViewParams {
		const char* name;					// nazev konstant bufferu
		RenderProgram* program;
		ShaderConstant* const constants;	// popis konstant
		int constantsCount;					// pocet konstant
	};

	enum class ShaderType {
		UNDEFINED,
		VERTEX_SHADER,
		PIXEL_SHADER,
		GEOMETRY_SHADER
	};

	enum ShaderCompileFlags {
		SHADER_COMPILE_FLAG_WARNINGS_AS_ERRRORS	= 1 << 0,
		SHADER_COMPILE_FLAG_DEBUG				= 1 << 1
	};

	/*
	Nastaveni kompilatoru shaderu
	*/
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
	
	/*
	Popisuje atribut vertexu ve vertex shaderu
	*/
	struct VertexAttribute {
		const char* attribute;		// nazev atributu (HLSL parametr nepouziva,musi byt ale platny)
		const char* semantic;
		int semanticIndex;
		Format format;
		int offset;					// vzdalenost atributu (v bytech) od zacatku bufferu
		int elementsCount;			// pocet elementu (4 pro matice apod.)
		int slot;					// id vstupniho slotu (bufferu)
		int instanceCount;			// pocet instanci se stejnym atributem (0 pro per vertex attribute; >0 pro per instance attribute)
	};

	struct VertexStreamParams {
		Buffer* vertexBuffers[ MAX_VERTEX_INPUT_SLOTS ];
		Buffer* indexBuffer;
		Format indexBufferFormat;
		VertexLayout* vertexLayout;
	};

	enum class PrimitiveTopology {
		DEFAULT,
		POINTLIST,
		LINELIST,
		LINESTRIP,
		TRIANGLELIST,
		TRIANGLESTRIP
	};

	enum class Blend {
		ZERO,
		ONE,
		SRC_COLOR,
		INV_SRC_COLOR,
		SRC_ALPHA,
		INV_SRC_ALPHA,
		DEST_ALPHA,
		INV_DEST_ALPHA,
		DEST_COLOR,
		INV_DEST_COLOR,
		SRC_ALPHA_SAT,
		BLEND_FACTOR,
		INV_BLEND_FACTOR,
		SRC1_COLOR,
		INV_SRC1_COLOR,
		SRC1_ALPHA,
		INV_SRC1_ALPHA
	};

	enum class BlendOp {
		ADD,
		SUBTRACT,
		REV_SUBTRACT,
		MIN,
		MAX
	};

	struct RenderTargetBlend {
		bool enable;
		Blend srcRGB;
		Blend destRGB;
		BlendOp opRGB;
		Blend srcAlpha;
		Blend destAlpha;
		BlendOp opAlpha;
	};

	/*
	Parametry funkce Device::CreateBlendState()
	Pokud ma atribut uniformBlending hodnotu true, pouzije se renderTargets[ 0 ] pro vsechny render targets.
	*/
	struct BlendStateParams {
		RenderTargetBlend renderTargets[ MAX_RENDER_TARGETS ];
		bool uniformBlending;
		bool alphaToCoverage;
	};

	enum class CullMode {
		DISABLED,
		FRONT_FACE,
		BACK_FACE
	};

	/*
	Parametry funkce Device::CreateRasterizerState()
	*/
	struct RasterizerStateParams {
		bool wireframe;
		CullMode cullMode;
		bool frontCounterClockwise;
		int depthBias;
		float depthBiasClamp;
		float slopeScaledDepthBias;
		bool depthClipping;
		bool scissorCulling;
		bool multisampling;
		bool antialiasedLines;
	};

	/*
	Zpusob pristupu do namapovaneho bufferu.
	WRITE_DISCARD:
	Predchozi obsah bufferu neni definovan, nemuze se proto aktualizovat jen cast bufferu.
	Graficka karta nemusi vytvaret kopii bufferu. Buffer musi byt vytvoren jako BufferUsage::DYNAMIC.
	*/
	enum class MapPolicy {
		READ_ONLY,
		WRITE_ONLY,
		READ_WRITE,
		WRITE_DISCARD
	};

	/*
	Data namapovaneho bufferu (funkce CommandInterface::Map())
	Data jsou ukladana po radcich (ale pouze 2D a 3D textury mohou mit vice nez jeden radek).
	Velikost radku nemusi odpovidat ocekavane velikosti, protoze 3D api muze pridat na konec radku volny prostor.
	Do tohoto prostoru je zakazano zapisovat. Protoze jsou data ulozena po radcich, je nutne pristupovat do
	bufferu take po radcich.
	*/
	struct MappedBuffer {
		void* data;
		int rowPitch;		// bytes vzdalenost mezi radky
		int depthPitch;		// bytes vzdalenost mezi depth slices (pouze pro 3D textury)
		int subresource;
		int rowByteWidth;	// sirka radku dostupne casti pameti (rowPitch = rowByteWidth + row_padding)
		int rowsCount;		// pocet radku ( >= 1 ) dostupne casti pameti (depth_slice_rows = rowsCount / depthsCount)
		int depthsCount;	// pocet depth slices ( >= 1 ) dostupne casti pameti
	};

	/*
	Mapovani vystupu render pipeline do render target.
	*/
	struct Viewport {
		float x;
		float y;
		float width;
		float height;
	};

	/*
	Fullscreen rezim displeje
	*/
	struct DisplayMode {
		int width;
		int height;
		int refreshRateNumerator;
		int refreshRateDenominator;
	};

	// Vypocet rozmeru mipmapy
	void GetMipDimmensions( const int width, const int height, const int depth, const int mipLevel, TextureDimmensions& result );

	/*
	DeviceObject: Zakladni trida pro vsechny objekty vytvarene tridou Device.
	Uvolnovani a vytvareni device objektu je nutne synchronizovat v jednom vlakne:

	thread 0:  create -> draw commands ---> synchronize ------> release
	thread 1:         -> draw commands --------> synchronize ->
	*/
	class DeviceObject {
	public:
		DeviceObject();
		virtual ~DeviceObject() = 0;
		
		// Neni mozne vytvaret kopie device objektu
		DeviceObject( const DeviceObject& ) = delete;
		DeviceObject& operator=( const DeviceObject& ) = delete;
		
		// Uvolni objekt z pameti, objekt nesmi byt dale pouzivan!
		void Release();
	};
	
	/*
	Device reprezentuje graficky adapter, vytvari veskere device objekty.
	Zadny device objekt neuklada ukazatel na jiny device objekt (ktery muze pouzit pri svem vytvareni)
	*/
	class Device: public DeviceObject {
	public:
		// buffers
		virtual PBackBuffer CreateBackBuffer( const Window& window ) = 0;
		virtual PBuffer CreateTextureBuffer( const TextureBufferParams& params, const void* const initialData[] ) = 0;
		virtual PBuffer CreateVertexBuffer( const int byteWidth, const BufferUsage usage, const BufferAccess access, const void* const initialData  ) = 0;
		virtual PBuffer CreateIndexBuffer( const int byteWidth, const BufferUsage usage, const BufferAccess access, const void* const initialData  ) = 0;
		virtual PBuffer CreateConstantBuffer( const int byteWidth, const BufferUsage usage, const BufferAccess access, const void* const initialData ) = 0;

		// views
		virtual PRenderTargetView CreateRenderTargetView( BackBuffer* const backBuffer ) = 0;
		virtual PRenderTargetView CreateRenderTargetView( Buffer* const textureBuffer ) = 0;
		virtual PTextureView CreateTextureView( Buffer* const textureBuffer, Sampler* const sampler ) = 0;
		virtual PDepthStencilView CreateDepthStencilView( Buffer* const textureBuffer, const DepthStencilViewParams& params ) = 0;
		virtual PConstantBufferView CreateConstantBufferView( Buffer* const constantBuffer, const ConstantBufferViewParams& params ) = 0;
		virtual PVertexStream CreateVertexStream( const VertexStreamParams& params ) = 0;

		// objects
		virtual PCommandInterface CreateCommandInterface() = 0;
		virtual PShader CreateShader( const ShaderParams& params ) = 0;
		virtual PRenderProgram CreateRenderProgram( Shader* const vs, Shader* const ps, Shader* const gs ) = 0;
		virtual PSampler CreateSampler( const SamplerParams& params ) = 0;
		virtual PVertexLayout CreateVertexLayout( const VertexAttribute* const attributes, const int attributesCount, RenderProgram* const program ) = 0;
		virtual PBlendState CreateBlendState( const BlendStateParams& params ) = 0;
		virtual PRasterizerState CreateRasterizerState( const RasterizerStateParams& params ) = 0;
		virtual PDepthStencilState CreateDepthStencilState( const DepthStencilStateParams& params ) = 0;
		virtual PDisplay CreateDisplay( const int outputId ) = 0;

		/*
		Vrati max quality pro pozadovany pocet samplu.
		Pokud neni hodnota samplesCount podporovana, vraci 0.
		*/
		virtual int GetMaxMultisampleQuality( const int samplesCount ) const = 0;

		// vraci pocet vystupu (monitoru) pripojenych ke graficke karte
		virtual int GetOutputsCount() const = 0;
	};
	
	/*
	CommandInterface slouzi ke generovani commandu graficke karty
	*/
	class CommandInterface: public DeviceObject {
	public:
		// zahajeni posilani commanu primo do graficke karty
		virtual void Begin( Device* const device ) = 0;
	
		// zahajeni posilani commandu do CommandListu
		virtual void Begin( CommandList* const commandList ) = 0;
	
		// ukonceni sekvence commandu
		virtual void End() = 0;

		// Odesle obsah command bufferu do GPU
		virtual void Flush() = 0;
		
		// Nastavi multiple render targets a depth stencil buffer (pokud je nullptr, pouzije se vychozi depth stencil state)
		virtual void SetRenderTargets( RenderTargetView* const renderTargets[], const int count, DepthStencilView* const depthStencil ) = 0;
		
		// Vyplni render target barvou
		virtual void ClearRenderTarget( RenderTargetView* const renderTarget, const Color& color ) = 0;

		// nastavi depth i stencil buffer na pozadovanou hodnotu
		virtual void ClearDepthStencil( DepthStencilView* const descriptor, const float depth, const uint8_t stencil ) = 0;

		// nastavi depth buffer na hodnotu depth
		virtual void ClearDepth( DepthStencilView* const descriptor, const float depth ) = 0;

		// nastavi stencil buffer na hodnotu stencil
		virtual void ClearStencil( DepthStencilView* const descriptor, const uint8_t stencil ) = 0;

		// Nastavi objekt Device do vychoziho stavu
		virtual void ClearState() = 0;

		/*
		Vrati ukazatel bufferu, parametr subresource se pouziva jen u textur.
		Blizsi info o parametru result v popisu struktury MappedBuffer.
		Vracene hodnoty parametru result jsou urcene jen pro cteni a nesmi byt upravovany (dokud neni zavolana funkce Unmap())
		*/
		virtual bool Map( Buffer* const buffer, const int subresource, const MapPolicy policy, MappedBuffer& result ) = 0;

		/*
		Uvolni namapovany buffer graficke karte.
		Ukazatel na namapovana data nesmi byt dale pouzivan (ukazatel data parametru mappedBuffer je nastaven na nullptr).
		*/
		virtual void Unmap( Buffer* const buffer, MappedBuffer& mappedBuffer ) = 0;

		/*
		Updatuje cely obsah bufferu, resp. konkretni subresource.
		Pokud ma atribut usage hodnotu DYNAMIC, pouzije se sekvence Map ->memcpy -> Unmap
		*/
		virtual bool UpdateBuffer( Buffer* const buffer, const int subresource, const void* const data ) = 0;

		/*
		Konstant buffer by nemel byt mapovan primo, ale radeji pomoci funkce UpdateConstantBuffer.
		Objekt ConstantBufferDescriptor obsahuje vsechny potrebne informace o umisteni konstant (byte offset).
		*/
		virtual bool UpdateConstantBuffer( ConstantBufferView* const view, const void* const data ) = 0;

		//Kopirovani obsahu bufferu pomoci GPU. Cilovy buffer musi mit shodnou velikost a kompatibilni format
		virtual void CopyBuffer( Buffer* const src, Buffer* const dest ) = 0;

		// Binduje konstant buffery do odpovidajicich slotu. Pokud je parametr descriptors nullptr, jsou vsechny buffery odpojeny.
		virtual void SetConstantBuffers( ConstantBufferView* const views[], const int count ) = 0;

		// Nastavi vertex buffer, index buffer a input layout (podobne jako OpenGL VAO)
		virtual void SetVertexStream( VertexStream* const stream ) = 0;

		// nastavi render program (vertex shader, pixel shader a geometry shader)
		virtual void SetRenderProgram( RenderProgram* const program ) = 0;

		virtual void SetPrimitiveTopology( const PrimitiveTopology topology ) = 0;

		// Pokud je parametr state nullptr, pouzije se vychozi state.
		virtual void SetBlendState( BlendState* const state ) = 0;

		// Pokud je parametr state nullptr, pouzije se vychozi state.
		virtual void SetDepthStencilState( DepthStencilState* const state, const uint32_t stencilRef ) = 0;

		// Pokud je parametr state nullptr, pouzije se vychozi state.
		virtual void SetRasterizerState( RasterizerState* const state ) = 0;

		virtual void SetViewports( const Viewport* const viewports[], const int count ) = 0;

		// Nabinduje textury pro vybrany shader stage. V shaderu je nutne specifikovat slot (bind point).
		virtual void SetVSTextures( const int startSlot, const int count, TextureView* const views[] ) = 0;
		virtual void SetPSTextures( const int startSlot, const int count, TextureView* const views[] ) = 0;
		virtual void SetGSTextures( const int startSlot, const int count, TextureView* const views[] ) = 0;

		// Nabinduje samplery pro vybrany shader stage. Parametr nullptr odpoji vsechny samplery.
		virtual void SetVSSamplers( Sampler* const samplers[ MAX_SAMPLERS ] ) = 0;
		virtual void SetPSSamplers( Sampler* const samplers[ MAX_SAMPLERS ] ) = 0;
		virtual void SetGSSamplers( Sampler* const samplers[ MAX_SAMPLERS ] ) = 0;

		// Draw commands
		virtual void Draw( const int verticesCount, const int startVertex ) = 0;
		virtual void DrawIndexed( const int indicesCount, const int startIndex ) = 0;
		virtual void DrawInstanced( const int verticesCount, const int startVertex, const int instancesCount, const int startInstance ) = 0;
		virtual void DrawIndexedInstanced( const int indicesCount, const int startIndex, const int instancesCount, const int startInstance ) = 0;
	};
	
	/*
	Slouzi k ukladani GPU commandu. Po prehrani commandu je obsah command listu vyprazdnen.
	*/
	class CommandList: public DeviceObject {};
	
	/*
	Displej pripojeny k vystupu graficke karty.
	Pro nastaveni rezimu obrazovky je nutny objekt Window s pripojenym rendererem
	*/
	class Display: public DeviceObject {
	public:
		// Nastavi rezim co nejvice odpovidajici pozadavku (na desktopu prepne do rezimu cele obrazovky)
		virtual void SetFullscreenMode( const DisplayMode& mode, Window& window ) = 0;

		// Nastavi vychozi rezim pro danou platformu (napr. na windows prepne z celoobrazovkoveho rezimu)
		virtual void SetWindowedMode() = 0;
	
		// Ziskani rezimu, pokud rezim s pozadovanym id neexistuje, vrati false
		virtual void GetMode( const int id, DisplayMode& result ) const = 0;

		// Najde rezim, ktery co nejlepe (ovsem ne nutne nejvice) odpovida pozadovanemu rezimu
		virtual void FindMode( const DisplayMode& request, DisplayMode& result ) const = 0;

		// Najde nejlepsi dostupny rezim pro cilove zarizeni
		virtual void GetBestMode( DisplayMode& result ) const = 0;
	};

	/*
	Umoznuje zobrazit obsahu back bufferu do klientske oblasti okna. Objekt je asociovan s oknem pri vytvoreni.
	*/
	class BackBuffer: public DeviceObject {
	public:
		virtual void Present( const int vsync ) = 0;
		virtual void Resize() = 0;
		virtual int GetWidth() const = 0;
		virtual int GetHeight() const = 0;
	};

	/*
	Buffer je blok pameti rezervovany v pameti graficke karty.
	Veskera data jsou do graficke karty posilana pomoci bufferu.
	*/
	class Buffer: public DeviceObject {
	public:
		virtual void GetInfo( BufferInfo& result ) const = 0;
		virtual BufferType GetType() const = 0;
		virtual int GetByteWidth() const = 0;
		virtual BufferUsage GetUsage() const = 0;
		virtual BufferAccess GetAccess() const = 0;
		virtual int GetSubresourcesCount() const = 0;
	};

	/*
	Umoznuje nabindovat texturu jako render target.
	Pouze nasledujici textury muzou byt pouzity jako render target: TEXTURE_1D, TEXTURE_2D, TEXTURE_2D_MS
	*/
	class RenderTargetView: public DeviceObject {};

	/*
	K vytvoreni potrebuje Sampler objekt. To zavazuje klienta, ze bude textura samplovana timto samplerem.
	Hlavne v HLSL implementaci je nutne dbat na dodrzeni tohoto pravidla (neexistuje kontrolni mechanismus).
	Sampler muze byt nullptr, pak se pouzije vychozi point sampler.
	*/
	class TextureView: public DeviceObject {};
	
	/*
	Umoznuje nabindovat DepthStencilBuffer do pipeline.
	Podporovane formaty asociovaneho texture bufferu: TEXTURE_2D a TEXTURE_2D_MS
	*/
	class DepthStencilView: public DeviceObject {};
	
	/*
	Sampler
	*/
	class Sampler: public DeviceObject {};

	/*
	Format, umisteni a usporadani konstant v constant bufferu. Pomoci tohoto objektu jsou data mapovana do bufferu.
	Descriptor je vytvoren pro konkretni render program, objekt je ovsem mozne pouzit pro vice render programu,
	pokud je dodrzen stejny layout constant bufferu.
	*/
	class ConstantBufferView: public DeviceObject {};
	
	/*
	Shader (VS, PS nebo GS)
	Shader jazyk je podmnozina HLSL. OpenGL implementace prevadi HLSL kod na GLSL kod.
	*/
	class Shader: public DeviceObject {
	public:
		virtual ShaderType GetType() const = 0;
		virtual ShaderVersion GetVersion() const = 0;
	};

	/*
	Seskupuje vertex shader, pixel shader a geometry shader do jednoho objektu.
	Neudrzuje ukazatel na objekty Shader, po vytvoreni objektu RenderProgram muzou byt objekty Shader uvolneny.
	*/
	class RenderProgram: public DeviceObject {};

	/*
	Render target blending
	*/
	class BlendState: public DeviceObject {};

	/*
	RasterizerState
	*/
	class RasterizerState: public DeviceObject {};

	/*
	DepthStencilState
	*/
	class DepthStencilState: public DeviceObject {};

	/*
	VertexLayout:

	HLSL
	struct VS_INPUT {
		float4 position: POSITION;
		float4x4 matrix: MATRIX;
	}
	C++
	const VertexLayoutAttribute layout[] = {
		{ "position",	"POSITION",	0, Format::R32G32B32A32_FLOAT, ... },
		{ "matrix",		"MATRIX",	0, Format::R32G32B32A32_FLOAT, ... }
	}
	*/
	class VertexLayout: public DeviceObject {
	public:
		//bool CheckInputSignature( Shader* const vertexShader );
	};

	/*
	Popisuje umisteni a usporadani vstupnich dat vertex shaderu.
	Objekt vznikl kvuli podpore OpenGL VAO.
	*/
	class VertexStream: public DeviceObject {};
	
} // namespace RenderInterface