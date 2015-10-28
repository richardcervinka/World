#pragma once

#include <d3d11.h>
#include <dxgi1_2.h>
#include "..\RenderInterface.h"
#include "..\..\Framework\Array.h"

class DX11Device;
class DX11Display;
class DX11CommandInterface;
class DX11CommandList;
class DX11RenderTargetDescriptor;
class DX11TextureDescriptor;
class DX11TextureBuffer;
class DX11TextureBuffer1D;
class DX11TextureBuffer1DArray;
class DX11TextureBuffer2D;
class DX11TextureBuffer2DArray;
class DX11BackBuffer;
class DX11DepthStencilBuffer;
class DX11TextureSampler;

using namespace RenderInterface;

// uvolneni COM objektu
template < typename T >
inline void ReleaseCOM( T **target ) {
	if ( *target != nullptr ) {
		( *target )->Release();
		*target = nullptr;
	}
}

/*
Ukonci aplikaci v dusledku chyby volani DirectX
Funkce nesmi byt volana pri vytvareni RenderDevice objektu
*/
void AbortDXInvalidCall( const HRESULT hresult );

// prevede typ Format na DXGI_FORMAT
DXGI_FORMAT GetDXGIFormat( const Format format );

class DX11Device: public Device {
public:
	DX11Device();
	virtual ~DX11Device();

	// Funkci Create() vola pouze funkce CreateDX11Device()
	bool Create( const DX11CreateDeviceParams &params );

	// pristupove funkce k rozhrani DirectX11
	ID3D11DeviceContext *GetContext();
	ID3D11Device *GetDevice();

	// implementace rozhrani Device
	virtual CommandInterface *CreateCommandInterface() override;
	virtual Display *CreateDisplay( const int outputId ) override;
	virtual BackBuffer *CreateBackBuffer( Window &window ) override;
	virtual TextureBuffer *CreateTextureBuffer( const TextureBufferDesc &desc, const void * const initialData[] ) override;
	//CreateUnorderedAccesTextureBuffer
	
	//******************
	
	virtual DepthStencilBuffer *CreateDepthStencilBuffer( const DepthStencilBufferDesc &desc ) override;
	virtual TextureSampler *CreateTextureSampler( const TextureSamplerDesc &desc ) override;
	virtual int GetMultisampleQuality( const int samplesCount ) const override;

private:
	IDXGIFactory1 *dxgiFactory;
	IDXGIAdapter *dxgiAdapter;
	ID3D11Device *device;
	ID3D11DeviceContext *context;
};

class DX11Display: public Display {
public:
	virtual ~DX11Display();
	
	bool Create( ID3D11Device *const device, IDXGIAdapter *const adapter, const int outputId );

	// implementace rozhrani Display
	virtual bool SetMode( const DisplayMode &mode, Window &window ) override;
	virtual void SetSystemMode() override;
	virtual bool GetMode( const int id, DisplayMode &result ) const override;
	virtual void FindMode( const DisplayMode &request, DisplayMode &result ) const override;
	virtual void GetBestMode( DisplayMode &result ) const override;
	
private:
	void EnumDisplayModes();
	
public:
	IDXGIOutput *dxgiOutput;
	Window *window;
	Array< DisplayMode > modes;
};

class DX11BackBuffer: public BackBuffer {
public:
	DX11BackBuffer();
	virtual ~DX11BackBuffer();
	bool Create( ID3D11Device *const device, IDXGIFactory1 *const factory, Window &window );

	// implementace rozhrani BackBuffer
	virtual void Present( const int vsync ) override;
	virtual void Resize() override;
	
	// DirectX Interface getters
	ID3D11RenderTargetView *GetView();
	IDXGISwapChain *GetSwapChain();

private:
	ID3D11RenderTargetView *renderTargetView;
	IDXGISwapChain *dxgiSwapChain;
	ID3D11Device *device;
	Window *window;
	int width;
	int height;
};

class DX11CommandInterface: public CommandInterface {
public:
	DX11CommandInterface();
	~DX11CommandInterface();
	bool Create();

	// implementace rozhrani
	virtual void Begin( Device * const device ) override;
	virtual void Begin( CommandList * const commandList ) override;
	virtual void End() override;
	//virtual void SetRenderTargets( RenderTargetObject * const renderTargets[], const int count, DepthStencilBuffer * const depthStencilBuffer ) override;
	//virtual void ClearRenderTarget( RenderTargetObject * const renderTarget, const Color &color ) override;

private:
	ID3D11DeviceContext *context;
};

class DX11TextureBuffer: public TextureBuffer {
public:
	DX11TextureBuffer();
	~DX11TextureBuffer();
	bool Create( ID3D11Device * const device, const TextureBufferDesc &desc, const void * const initialData[] );
	
	// implementace rozhrani TextureBuffer
	virtual const Format GetFormat() const override;
	virtual TextureBufferType GetType() const override;
	virtual int GetDimmension() const override;
	virtual TextureDimmensions GetDimmensions() const override;
	virtual int GetArraySize() const override;
	virtual int GetSamplesCount() const override;
	virtual int GetSamplesQuality() const override;

	// DirectX 11 getters
	ID3D11Resource *GetTextureResource();
	
private:
	TextureBufferDesc desc;
	ID3D11Resource *texture;
};

class DX11TextureDescriptor: public TextureDescriptor {
public:
	DX11TextureDescriptor();
	virtual ~DX11TextureDescriptor();
	bool Create( ID3D11Device * const device, DX11TextureBuffer * const buffer );
	ID3D11ShaderResourceView *GetView();

private:
	ID3D11ShaderResourceView *view;
};



//******************




class DX11DepthStencilBuffer: public DepthStencilBuffer {
public:
	DX11DepthStencilBuffer();
	~DX11DepthStencilBuffer();
	bool Create( ID3D11Device *device, const DepthStencilBufferDesc &desc );
	virtual DepthStencilBufferDesc GetDesc() const override;
	ID3D11DepthStencilView *GetDepthStencilView();

	// implementace rozhrani TextureBuffer
	//virtual int GetDimmension() const override { return 0; }
	//virtual const Format GetFormat() const override { return Format::UNKNOWN; }

private:
	ID3D11Texture2D *texture;
	ID3D11DepthStencilView *view;
	DepthStencilBufferDesc desc;
};



class DX11TextureSampler: public TextureSampler {
public:
	DX11TextureSampler();
	~DX11TextureSampler();
	bool Create( DX11Device * const device, const TextureSamplerDesc &desc );
	virtual TextureSamplerDesc GetDesc() const override;
	ID3D11SamplerState *GetSamplerState();

private:
	ID3D11SamplerState *sampler;
	TextureSamplerDesc desc;
};