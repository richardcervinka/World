#pragma once

#include "..\Framework\Math.h"
#include "..\Framework\Color.h"

// forward declarations
class Window;

namespace RenderInterface {
	
	// forward declarations
	class Device;
	class Display;
	class BackBuffer;
	class CommandInterface;
	class CommandList;
	class Buffer;
	class TextureDescriptor;
	class RenderTargetDescriptor;
	class DepthStencilDescriptor;
	class ConstantBufferDescriptor;
	class Shader;
	class Sampler;

	// Render Interface constants
	const int MAX_MULTISAMPLE_QUALITY = -1;
	const int MAX_TEXTURE_LOD = -1;
	const int MAX_RENDER_TARGETS = 8;
	const int MAX_VERTEX_INPUT_SLOTS = 16;
	
	// Informace potrebne k vytvoreni objektu DX11Device
	struct DX11CreateDeviceParams {
		int adapter;				// id adapteru, 0 je vychozi adapter
		int majorFeatureLevels;
		int minorFeatureLevels;
	};
	
	// Vytvori device objekt implementovany v DirectX 11
	Device* DX11CreateDevice( const DX11CreateDeviceParams& params );
	
	/*
	Priznaky se doporucuje definovat nasledujicim zpusobem:
	namespace CustomFlags {
		const Flags FLAG1 = 0x01;
		const Flags FLAG2 = 0x02;
	}
	*/
	typedef uint32_t Flags;

	/*
	Format dat ulozenych v bufferu
	*/
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
	FormatInfo
	pointPitch = blockByteWidth / blockSize
	rowPitch = pointPitch * textureWidth
	*/
	struct FormatInfo {
		int channelCount;		// pocet kanalu na pixel
		int channelByteWidth;	// pocet bytes na kanal
		int blockSize;			// velikost bloku
		int blockByteWidth;		// pocet bytes na blok
	};
	
	const FormatInfo GetFormatInfo( const Format format );
	
	/*
	Fullscreen rezim displeje
	*/
	struct DisplayMode {
		int width;
		int height;
		int refreshRateNumerator;
		int refreshRateDenominator;
	};

	/*
	Usage popisuje jakym zpusobem pristupuje GPU a CPU do bufferu
	*/
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
	Informace o bufferu, parametry konkretnich bufferu (napr texture buffer) musi spravovat klient
	*/
	struct BufferInfo {
		BufferType type;		// typ bufferu
		int byteWidth;			// minimalni velikost bufferu v bytech, realna velikost bufferu muze byt vetsi
		BufferUsage usage;		// zpusob pristupu do bufferu
		BufferAccess access;	// cpu access
	};

	/*
	Rozmery textury. Pokud neni rozmer definovan, ma hodnotu 1.
	*/
	struct TextureDimmensions {
		int width;
		int height;
		int depth;
	};
	
	/*
	Priznaky texture bufferu
	*/
	namespace TextureBufferFlags {
		const Flags RENDER_TARGET = 0x01;
	}

	/*
	Parametry funkce Device::CreateTextureBuffer()
	Generovani mipmap pri vytvareni texture bufferu je zakazane, pocet mip urovni musi byt znamy pred vytvorenim objektu.
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
		Flags flags;
	};

	/*
	Filtrovani textur
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
	Adresovani textur
	*/
	enum class TextureAddressing {
		WRAP,
		MIRROR,
		CLAMP
	};
	
	struct SamplerParams {
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

	enum class DepthStencilUsage {
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

	/*
	ShaderConstant (konstanta constant bufferu)

	// HLSL
	cbuffer Constants {
		float3 color;
		float4 position;
	}
	// C++
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

	// parametry funkce CreateConstantBufferDescriptor
	struct ConstantBufferDescriptorParams {
		Shader* shader;
		const char* bufferObject;
		ShaderConstant* const constants;
		int constantsCount;
		int slot;
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
	
	// Popisuje vstupni atribut vertex shaderu
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

	/*
	Zpusob pristupu do namapovaneho bufferu
	WRITE_DISCARD:
	Obsah bufferu neni definovan, nemuze se proto aktualizovat jen cast bufferu. Protoze neni obsah bufferu definovan,
	nemusi graficka karta vytvaret kopii nebo cekat na dokonceni operaci s bufferem. Buffer musi byt vytvoren jako BufferUsage::DYNAMIC
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

	// Helper function prototypes

	// Vypocet rozmeru mipmapy
	void GetMipDimmensions( const int width, const int height, const int depth, const int mipLevel, TextureDimmensions& result );

	/*
	Zakladni trida pro vsechny objekty vytvarene tridou Device.
	Objekty DeviceObject nejsou thread safe.
	Uvolnovani a vytvareni device objektu je nutne synchronizovat v jednom vlakne:

	thread 0:  create -> draw commands ---> synchronize ------> release
	thread 1:         -> draw commands --------> synchronize ->
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
		// buffers
		virtual BackBuffer* CreateBackBuffer( Window& window ) = 0;
		virtual Buffer* CreateTextureBuffer( const TextureBufferParams& params, const void* const initialData[] ) = 0;
		virtual Buffer* CreateVertexBuffer( const int byteWidth, const BufferUsage usage, const BufferAccess access, const void* const initialData  ) = 0;
		virtual Buffer* CreateIndexBuffer( const int byteWidth, const BufferUsage usage, const BufferAccess access, const void* const initialData  ) = 0;
		virtual Buffer* CreateConstantBuffer( const int byteWidth, const BufferUsage usage, const BufferAccess access, const void* const initialData ) = 0;
		// CreateTextureCubeBuffer

		// descriptors
		virtual RenderTargetDescriptor* CreateRenderTargetDescriptor( BackBuffer* const backBuffer ) = 0;
		virtual RenderTargetDescriptor* CreateRenderTargetDescriptor( Buffer* const textureBuffer ) = 0;
		virtual TextureDescriptor* CreateTextureDescriptor( Buffer* const textureBuffer ) = 0;
		virtual DepthStencilDescriptor* CreateDepthStencilDescriptor( Buffer* const textureBuffer, const DepthStencilDescriptorParams& params ) = 0;
		virtual ConstantBufferDescriptor* CreateConstantBufferDescriptor( Buffer* const constantBuffer, const ConstantBufferDescriptorParams& params ) = 0;

		// objects
		virtual CommandInterface* CreateCommandInterface() = 0;
		virtual Display* CreateDisplay( const int outputId ) = 0;
		virtual Shader* CreateShader( const ShaderParams& params ) = 0;
		virtual Sampler* CreateSampler( const SamplerParams& params ) = 0;

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
		// zahajeni posilani commanu primo do graficke karty
		virtual void Begin( Device* const device ) = 0;
	
		// zahajeni posilani commandu do CommandListu
		virtual void Begin( CommandList* const commandList ) = 0;
	
		// ukonceni sekvence commandu
		virtual void End() = 0;

		// Odesle obsah command bufferu do GPU
		virtual void Flush() = 0;
		
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
		Pokud je usage DYNAMIC, pouzije se sekvence Map ->memcpy -> Unmap
		*/
		virtual bool UpdateBuffer( Buffer* const buffer, const int subresource, const void* const data ) = 0;

		/*
		Konstant buffer by nemel byt mapovan primo, ale radeji pomoci funkce UpdateConstantBuffer.
		Objekt ConstantBufferDescriptor obsahuje vsechny potrebne informace o umisteni konstant (byte offset).
		*/
		virtual bool UpdateConstantBuffer( ConstantBufferDescriptor* const descriptor, const void* const data ) = 0;

		//Kopirovani obsahu bufferu pomoci GPU. Cilovy buffer musi mit shodnou velikost a kompatibilni format
		virtual void CopyBuffer( Buffer* const src, Buffer* const dest ) = 0;

		virtual void SetPipelineState( PipelineState* const state );

		// Draw( VertexDescriptor )

		// DrawIndexed( VertexDescriptor )

		// DrawInstanced( VertexDescriptor, count )

		// DrawIndexedInstanced( VertexDescriptor, count )
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
	Buffer je blok pameti rezervovany v pameti graficke karty
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
	class RenderTargetDescriptor: public DeviceObject {};

	/*
	TextureDescriptor slouzi k bindovani textury do pipeline stage
	*/
	class TextureDescriptor: public DeviceObject {};
	
	/*
	Umoznuje nabindovat DepthStencilBuffer do pipeline.
	Popisuje konfiguraci depth stencil testu a pristup do texture bufferu.
	Podporovane formaty asociovaneho texture bufferu: TEXTURE_2D a TEXTURE_2D_MS
	*/
	class DepthStencilDescriptor: public DeviceObject {};
	
	/*
	TextureSampler
	*/
	class Sampler: public DeviceObject {};

	/*
	Constant buffer descriptor popisuje format, umisteni a usporadani konstant v constant bufferu.
	Pomoci tohoto objektu jsou data namapovana do bufferu tak, aby odpovidala shaderu.
	*/
	class ConstantBufferDescriptor: public DeviceObject {};

	/*
	Shader program (VS, PS i GS)
	Shader jazyk je podmnozina HLSL. Pri OpenGL implementaci je HLSL kod preveden na GLSL kod.
	*/
	class Shader: public DeviceObject {
	public:
		virtual ShaderType GetType() const = 0;
		virtual ShaderVersion GetVersion() const = 0;
	};

	/*
	VertexLayout:

	HLSL
	struct VS_INPUT {
		float4 position: POSITION;
		float4x4 matrix: MATRIX;
	}

	GLSL
	struct VS_INPUT {
		vec4 position;
		mat4 matrix;
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
	Podobne jako texture descriptor slouzi k nabindovani texture bufferu do pipeline stage,
	slouzi vertex descriptor k nabindovani vertex bufferu a index bufferu.
	K vytvoreni vertex descriptoru je potreba vertex layout objekt.
	Objekt vznikl kvuli podpore Vertex Arrays Object (VAO) v OpenGL.
	*/
	class VertexDescriptor: public DeviceObject {};

	/*
	Atributy:
	- vertex shader
	- pixel shader
	- geometry shader
	- vertex layout
	*/
	class PipelineState: public DeviceObject {};


	//**************************************************

	class BlendState {
	};
	
	struct RasterizeStateDes {
	};
	
} // namespace RenderInterface