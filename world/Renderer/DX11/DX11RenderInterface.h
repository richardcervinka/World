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
	virtual RenderTargetDescriptor *CreateRenderTargetDescriptor( TextureBuffer * const buffer ) override;
	virtual RenderTargetDescriptor *CreateRenderTargetDescriptor( BackBuffer * const buffer ) override;
	virtual DepthStencilDescriptor *CreateDepthStencilDescriptor( TextureBuffer * const buffer, const DepthStencilStateDesc &desc ) override;
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

class DX11CommandInterface: public CommandInterface {
public:
	DX11CommandInterface();
	~DX11CommandInterface();
	bool Create();

	// implementace rozhrani CommandInterface
	virtual void Begin( Device * const device ) override;
	virtual void Begin( CommandList * const commandList ) override;
	virtual void End() override;
	virtual void SetRenderTargets( RenderTargetDescriptor * const renderTargets[], const int count, DepthStencilDescriptor * const depthStencil ) override;
	virtual void ClearRenderTarget( RenderTargetDescriptor * const renderTarget, const Color &color ) override;
	virtual void ClearDepthStencil( DepthStencilDescriptor * const descriptor, const float depth, const Uint8 stencil ) override;
	virtual void ClearDepth( DepthStencilDescriptor * const descriptor, const float depth ) override;
	virtual void ClearStencil( DepthStencilDescriptor * const descriptor, const Uint8 stencil ) override;
	virtual void ClearState() override;
	virtual void Flush() override;

private:
	ID3D11DeviceContext *context;
};

class DX11TextureBuffer: public TextureBuffer {
public:
	DX11TextureBuffer();
	~DX11TextureBuffer();
	bool Create( ID3D11Device * const device, const TextureBufferDesc &desc, const void * const initialData[] );

	// DirectX 11 getters
	ID3D11Resource *GetTextureResource();
	
private:
	ID3D11Resource *texture;
};

class DX11TextureDescriptor: public TextureDescriptor {
public:
	DX11TextureDescriptor();
	virtual ~DX11TextureDescriptor();
	bool Create( ID3D11Device * const device, TextureBuffer * const buffer );
	ID3D11ShaderResourceView *GetView();

	// implementace rozhrani TextureDescriptor
	TextureBuffer *GetBuffer() override;

private:
	ID3D11ShaderResourceView *view;
	DX11TextureBuffer *buffer;
};

class DX11RenderTargetDescriptor: public RenderTargetDescriptor {
public:
	DX11RenderTargetDescriptor();
	virtual ~DX11RenderTargetDescriptor();
	bool Create( ID3D11Device * const device, TextureBuffer * const buffer );
	bool Create( ID3D11Device * const device, BackBuffer * const buffer );

	// implementace rozhrani RenderTargetDescriptor
	virtual TextureBuffer *GetBuffer() override;

	// DirectX 11 getters
	ID3D11RenderTargetView *GetView();

private:
	ID3D11RenderTargetView *view;
	TextureBuffer *buffer;
};

class DX11BackBuffer: public BackBuffer {
public:
	DX11BackBuffer();
	virtual ~DX11BackBuffer();
	bool Create( ID3D11Device * const device, IDXGIFactory1 * const factory, Window &window );

	// implementace rozhrani BackBuffer
	virtual void Present( const int vsync ) override;
	virtual void Resize() override;
	
	// DirectX 11 getters
	IDXGISwapChain *GetSwapChain();

private:
	IDXGISwapChain *dxgiSwapChain;
	ID3D11Device *device;
	Window *window;
	int width;
	int height;
};

class DX11DepthStencilDescriptor: public DepthStencilDescriptor {
public:
	DX11DepthStencilDescriptor();
	~DX11DepthStencilDescriptor();
	bool Create( ID3D11Device * const device, TextureBuffer * const buffer, const DepthStencilStateDesc &desc );

	// DirectX 11 getters
	ID3D11DepthStencilView *GetView();
	ID3D11DepthStencilState *GetState();

private:
	ID3D11DepthStencilView *view;
	ID3D11DepthStencilState *state;
};

class DX11TextureSampler: public TextureSampler {
public:
	DX11TextureSampler();
	~DX11TextureSampler();
	bool Create( ID3D11Device * const device, const TextureSamplerDesc &desc );

	// DirectX 11 getters
	ID3D11SamplerState *GetSamplerState();

private:
	ID3D11SamplerState *sampler;
	TextureSamplerDesc desc;
};