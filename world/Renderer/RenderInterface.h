#pragma once

#include "..\Framework\Color.h"

// forward declarations
class Window;

namespace RenderInterface {
	
	class Device;
	class Display;
	class CommandInterface;
	class CommandList;
	class RenderTargetDescriptor;
	class TextureDescriptor;
	class TextureBuffer;
	class TextureBuffer1D;
	class TextureBuffer1DArray;
	class TextureBuffer2D;
	class TextureBuffer2DArray;
	class BackBuffer;
	class DepthStencilBuffer;
	class TextureSampler;
	
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
	Device *DX11CreateDevice( const DX11CreateDeviceParams &params );
	
	// Buffer formaty
	enum class Format {
		UNKNOWN = 0,
		R8G8B8A8_UNORM,
		R8G8B8A8_SNORM,
		R16G16B16A16_FLOAT,
		R16G16_FLOAT,
		R8_UNORM,
		R16_FLOAT,
		R32_FLOAT,
		BC1,
		BC3
	};
	
	// Rezim displeje
	struct DisplayMode {
		int width;
		int height;
		int refreshRateNumerator;
		int refreshRateDenominator;
	};
	
	struct DepthStencilBufferDesc {
		int width;
		int height;
		int samplesCount;
		int multisampleQuality;
	};
	
	/*
	Pristupova prava CPU do bufferu
	*/
	enum class CPUAccess {
		NONE = 0,
		READ = 0x0001,
		WRITE = 0x0002,
		READ_WRITE = READ | WRITE
	};

	/*
	Pristupova prava GPU do bufferu
	*/
	enum class GPUAccess {
		READ,
		READ_WRITE
	};
	
	// Rezimy filtrovani textur
	enum class TextureFilter {
		POINT,
		POIN_MIP_LINEAR,
		MIN_POINT_MAG_LINEAR_MIP_POINT,
		MIN_POINT_MAG_MIP_LINEAR,
		MIN_LINEAR_MAG_MIP_POINT,
		MIN_LINEAR_MAG_POINT_MIP_LINEAR,
		LINEAR_MIP_POINT,
		LINEAR,
		ANISOTROPIC
	};
	
	// Rezimy adresovani textur
	enum class TextureAddress {
		WRAP = 0,
		MIRROR,
		CLAMP
	};
	
	/*
	Rozmery textury, pokud neni rozmer definovan, je jeho hodnota 1
	Rozmer nemuze mit nikdy nulovou velikost
	*/
	struct TextureDimmensions {
		int width;
		int height;
		int depth;
	};
	
	// Podporovane typy texture bufferu
	enum class TextureBufferType {
		TEXTURE_1D,
		TEXTURE_1D_ARRAY,
		TEXTURE_2D,
		TEXTURE_2D_ARRAY,
		TEXTURE_3D,
		TEXTURE_CUBE
	};

	struct TextureBufferDesc {
		TextureBufferType type;
		Format format;
		TextureDimmensions dimmensions;
		GPUAccess gpuAccess;
		CPUAccess cpuAccess;
		int arraySize;
		int samplesCount;
		int samplesQuality;
		int mipLevels;
		bool renderTarget;
	};

	struct TextureSubresourceData {
		void *data;
		TextureDimmensions dimmensions;
	};
	
	struct TextureSamplerDesc {
		TextureFilter filter;
		TextureAddress uAddress;
		TextureAddress vAddress;
		TextureAddress wAddress;
		int maxAnisotropy;
		float minLOD;
		float maxLOD;	// hodnota v rozsahu <0; MAX_TEXTURE_LOD>
	};
	
	// Identifikace shader stag, hodnoty lze kombinovat operatorem OR
	typedef unsigned int ShaderStage;
	const ShaderStage VERTEX_SHADER_STAGE = 0x0001;
	const ShaderStage PIXEL_SHADER_STAGE = 0x0002;
	const ShaderStage GEOMETRY_SHADER_STAGE = 0x0004;
	
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
		DeviceObject &operator=( const DeviceObject& ) = delete;
		
		// Pokud je pocet referenci 0, uvolni objekt z pameti a nesmi byt dale pouzivan!
		void Release();
		
		// inkrementuje pocitadlo referenci, objekt neni uvolnen z pameti, dokud je pocet referenci vetsi nez 0
		void AddRef();
		
		// Vraci true, pokud je objekt pripraveny k pouziti, tj. je radne inicializovany a neni v zadnem chybovem stavu
		//bool Valid() const = 0;
	
	private:
		int references;
	};
	
	/*
	Reprezentuje grafickou kartu, vytvari veskere zdroje.
	*/
	class Device: public DeviceObject {
	public:
		// vytvareni device objektu
		virtual CommandInterface *CreateCommandInterface() = 0;
		virtual Display *CreateDisplay( const int outputId ) = 0;
		virtual BackBuffer *CreateBackBuffer( Window &window ) = 0;
		virtual TextureBuffer *CreateTextureBuffer( const TextureBufferDesc &desc ) = 0;
		
		//**************************
		virtual DepthStencilBuffer *CreateDepthStencilBuffer( const DepthStencilBufferDesc &desc ) = 0;
		virtual TextureSampler *CreateTextureSampler( const TextureSamplerDesc &desc ) = 0;
		
		// vrati max quality pro pozadovany pocet msaa level.
		// Ne vsechny karty a ne vsechna api museji podporovat tuto vlastnost, pak musi funkce vracet 1.
		// Pokud neni msaa level podporovan, funkce vraci 0;
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
		virtual void Begin( Device * const device ) = 0;
	
		// zahajeni ukladani commandu do CommandListu
		virtual void Begin( CommandList * const commandList ) = 0;
	
		// ukonceni sekvence commandu
		virtual void End() = 0;
		
		// Nastavi multiple render targets plus nepovinne depth stencil buffer (muze byt nullptr)
		virtual void SetRenderTargets( RenderTargetDescriptor * const renderTargets[], const int count, DepthStencilBuffer * const depthStencilBuffer ) = 0;
		
		//virtual void SetBackBuffer( BackBuffer * const backBuffer, DepthStencilBuffer * const depthStencilBuffer ) = 0;
		
		// Vyplni render target barvou
		virtual void ClearRenderTarget( RenderTargetDescriptor * const renderTarget, const Color &color ) = 0;
		
		//virtual void ClearDepthStencilBuffer( DepthStencilBuffer * const target );
		
		// Nastavi objekt Device do vychoziho stavu
		//void ClearState();
		
		//void SetPipelineState()
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
		virtual bool SetMode( const DisplayMode &mode, Window &window ) = 0;
		
		// Nastavi vychozi rezim pro danou platformu (napr. na windows prepne z celoobrazovkoveho rezimu)
		virtual void SetSystemMode() = 0;
		
		// Ziskani rezimu, pokud rezim s pozadovanym id neexistuje, vrati false
		virtual bool GetMode( const int id, DisplayMode &result ) const = 0;
		
		// Najde rezim, ktery co nejlepe (ovsem ne nutne nejvice) odpovida pozadovanemu rezimu
		virtual void FindMode( const DisplayMode &request, DisplayMode &result ) const = 0;
		
		// Najde nejlepsi dostupny rezim pro cilove zarizeni
		virtual void GetBestMode( DisplayMode &result ) const = 0;
	};

	/*
	TextureDescriptor slouzi k bindovani textury do pipeline stage
	*/
	class TextureDescriptor: public DeviceObject {};
	
	/*
	RenderTargetDescriptor k bindovani textury jako render target.
	*/
	class RenderTargetDescriptor: public DeviceObject {};
	
	/*
	Texture buffer je obecne blok pameti pro ukladani dat textur.
	*/
	class TextureBuffer: public DeviceObject {
	public:
		virtual const Format GetFormat() const = 0;
		
		virtual TextureBufferType GetType() const = 0;
		
		// dimenze textury (napr. vraci hodnotu 2 pro 2D textury) 
		virtual int GetDimmension() const = 0;
		
		// rozmery textury
		virtual TextureDimmensions GetDimmensions() const = 0;
		
		// velikost pole, hodnota >= 1
		virtual int GetArraySize() const = 0;
		
		// pocet samplu, hodnota >= 1
		virtual int GetSamplesCount() const = 0;
		
		// kvalita multisampling algoritmu, hodnota >= 1
		virtual int GetSamplesQuality() const = 0;
		
		// AccessFlags
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
	DepthStencilBuffer
	*/
	class DepthStencilBuffer: public TextureBuffer {
	public:
		virtual ~DepthStencilBuffer() {}
		virtual DepthStencilBufferDesc GetDesc() const = 0;
	};
	
	//**************************************************

	/*
	TextureSampler
	*/
	class TextureSampler: public DeviceObject {
	public:
		virtual ~TextureSampler() {};
		virtual TextureSamplerDesc GetDesc() const = 0;
	};
	
	struct BlendStateDescription {
	};
	
	struct RasterizeStateDescription {
	};
	
	struct PipelineStateDescription {
		BlendStateDescription blendStateDesc;
		RasterizeStateDescription rasterizeStateDesc;
	};
	
	class PipelineState: public DeviceObject {
	public:
	
	private:
	};
	
	/*
	Zakladni trida pro vsechny buffer objekty, jako jsou Textury apod.
	*/
	class Buffer: public DeviceObject {
	public:
		// int GetWidth() const;
		// int GetHeight() const;
		// int GetByteSize() const;
	
	private:
	};
	
} // namespace RenderInterface


