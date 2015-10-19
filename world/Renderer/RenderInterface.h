#pragma once

#include "..\Framework\Color.h"

// forward declarations
class Window;

namespace RenderInterface {
	
	class Device;
	class Display;
	class CommandInterface;
	class CommandList;
	class RenderTargetObject;
	class ShaderResourceObject;
	class RenderTarget;
		class RenderBuffer;
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
	
	// Buffer formaty graficke karty
	enum class Format {
		UNKNOWN = 0,
		R8G8B8A8_UNORM,
		R8G8B8A8_SNORM,
		R16G16B16A16_FLOAT,
		R16G16_FLOAT,
		R8_UNORM,
		R16_FLOAT,
		R32_FLOAT
	};
	
	// Rezim displeje
	struct DisplayMode {
		int width;
		int height;
		int refreshRateNumerator;
		int refreshRateDenominator;
	};
	
	struct RenderBufferDesc {
		Format format;
		int width;
		int height;
		int samplesCount;
		int multisampleQuality;
	};
	
	struct DepthStencilBufferDesc {
		int width;
		int height;
		int samplesCount;
		int multisampleQuality;
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
	const ShaderStage VERTEX_SHADER_STAGE	= 0x0001;
	const ShaderStage PIXEL_SHADER_STAGE	= 0x0002;
	const ShaderStage GEOMETRY_SHADER_STAGE	= 0x0004;
	
	/*
	Zakladni trida pro vsechny objekty vytvarene tridou Device.
	Objekt Device je factory vsech device objektu, device objekty se uvolnuji metodou Release()
	*/
	class DeviceObject {
	public:
		DeviceObject();
		virtual ~DeviceObject() = 0;
	
		// Pokud je pocet referenci 0, uvolni objekt z pameti a nesmi byt dale pouzivan!
		void Release();

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
		virtual ~Device() {}
	
		// vytvareni device objektu
		virtual CommandInterface *CreateCommandInterface() = 0;
		virtual BackBuffer *CreateBackBuffer( Window &window ) = 0;
		virtual RenderBuffer *CreateRenderBuffer( const RenderBufferDesc &desc ) = 0;
		virtual DepthStencilBuffer *CreateDepthStencilBuffer( const DepthStencilBufferDesc &desc ) = 0;
		virtual TextureSampler *CreateTextureSampler( const TextureSamplerDesc &desc ) = 0;
		virtual Display *CreateDisplay( const int outputId ) = 0;
	
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
		virtual ~CommandInterface() {}
	
		// zahajeni posilani commanu primo do grafickeho zarizeni
		virtual void Begin( Device * const device ) = 0;
	
		// zahajeni ukladani commandu do CommandListu
		virtual void Begin( CommandList * const commandList ) = 0;
	
		// ukonceni sekvence commandu
		virtual void End() = 0;
	
		// Nastavi cil vykreslovani
		virtual void SetRenderTargets( RenderTargetObject * const renderTargets[], const int count, DepthStencilBuffer * const depthStencilBuffer ) = 0;
	
		virtual void ClearRenderTarget( RenderTargetObject * const renderTarget, const Color &color ) = 0;
	
		//virtual void ClearRenderTarget();
		//virtual void ClearDepthStencil();
		//void ClearState();
		//void SetRenderTarget( RenderBuffer )
		//void SetPipelineState()
	};
	
	/*
	Slouzi k zaznemenani commandu a jejich jednorazovemu prehrani. Po prehrani je obsah command listu vyprazdnen.
	*/
	class CommandList: public DeviceObject {
	public:
		//virtual ~CommandList();
		//virtual void ExecuteCommandBatch( CommandBatch *batch );
	};
	
	/*
	Monitor pripojeny k vystupu graficke karty (Device)
	*/
	class Display: public DeviceObject {
	public:
		virtual ~Display() {}
		
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
	Objekt Device pouziva ShaderResourceObject k bindovani zdroje do pipeline stage (napr. k nabindovani texury do pixel shaderu)
	Tento objekt neni vytvaren objektem Device, proto nededi z tridy DeviceObject.
	Tento mechanismus odstranuje nutnost vicenasobne dedicnosti.
	*/
	class ShaderResourceObject {
	public:
		virtual ~ShaderResourceObject() = default;
	};

	/*
	Objekt Device pouziva RenderTargetObject k bindovani zdroje jako render target.
	Tento objekt neni vytvaren objektem Device, proto nededi z tridy DeviceObject.
	Tento mechanismus odstranuje nutnost vicenasobne dedicnosti.
	*/
	class RenderTargetObject {
	public:
		virtual ~RenderTargetObject() = default;
	};

	/*
	Slouzi jako render target a zaroven umoznuje zobrazeni obsahu do klientske oblasti okna.
	Objekt je asociovan s oknem pri svem vytvoreni pomoci Device.
	*/
	class BackBuffer: public DeviceObject {
	public:
		virtual ~BackBuffer() = default;

		virtual void Present( const int vsync ) = 0;
		virtual void Resize() = 0;
		virtual RenderTargetObject *GetRenderTargetObject() = 0;
		//virtual int GetWidth() const = 0;
		//virtual int GetHeight() const = 0;
	};
	
	/*
	Zakladni trida pro vsechny texture buffery (vcetne depth stencil bufferu a render bufferu)
	*/
	class TextureBuffer: public DeviceObject {
	public:
		virtual ~TextureBuffer() = default;

		virtual int GetDimmension() const = 0;
		virtual const Format GetFormat() const = 0;
		virtual ShaderResourceObject *GetShaderResourceObject() = 0; 
	};

	/*
	RenderBuffer Umoznuje cteni i zapis pomoci GPU, pristup pres CPU neni povolen. Podporuje multisampling.
	*/
	class RenderBuffer: public TextureBuffer {
	public:
		virtual ~RenderBuffer() = default;

		virtual RenderBufferDesc GetDesc() const = 0;
		virtual RenderTargetObject *GetRenderTargetObject() = 0;
		// GetSamplesCount()
		// GetWidth()
		// GetHeight()
		// GetFormat()
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


