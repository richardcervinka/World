#pragma once

#include <d3d11.h>
#include <dxgi1_2.h>
#include "..\RenderInterface.h"
#include "..\..\Framework\Array.h"

// uvolneni COM objektu
template < typename T >
inline void ReleaseCOM( T **target ) {
	if ( *target != nullptr ) {
		( *target )->Release();
		*target = nullptr;
	}
}

// prevede Format na DXGI_FORMAT
DXGI_FORMAT GetDXGIFormat( const Format format );

class DX11Device: public Device {
public:
	DX11Device();
	virtual ~DX11Device();

	// Funkci Create() vola pouze funkce CreateDX11Device()
	bool Create( const CreateDX11DeviceParams &params );

	// pristupove funkce k rozhrani DirectX11
	ID3D11DeviceContext *GetContext();
	ID3D11Device *GetDevice();

	// implementace rozhrani
	virtual CommandInterface *CreateCommandInterface() override;
	virtual BackBuffer *CreateBackBuffer( Window &window ) override;
	virtual RenderBuffer *CreateRenderBuffer( const RenderBufferDesc &desc ) override;
	virtual DepthStencilBuffer *CreateDepthStencilBuffer( const DepthStencilBufferDesc &desc ) override;
	virtual TextureSampler *CreateTextureSampler( const TextureSamplerDesc &desc ) override;
	//virtual void ResizBackBuffer( RenderOutput * const output ) override;
	virtual int GetMultisampleQuality( const int samplesCount ) const override;

private:
	IDXGIFactory1 *dxgiFactory;
	ID3D11Device *device;
	ID3D11DeviceContext *context;
};

class DX11RenderTarget {
public:
	virtual ID3D11RenderTargetView *GetRenderTargetView() = 0;
};

class DX11BackBuffer: public BackBuffer, public DX11RenderTarget {
public:
	DX11BackBuffer();
	virtual ~DX11BackBuffer();

	bool Create( ID3D11Device *const device, IDXGIFactory1 *const factory, Window &window );

	// implementace rozhrani DX11RenderTarget
	virtual ID3D11RenderTargetView *GetRenderTargetView() override;

	// implementace rozhrani BackBuffer
	virtual void Present( const int vsync ) override;

private:
	IDXGISwapChain *swapChain;
	ID3D11RenderTargetView *renderTargetView;
	Window *window;
};




/*
class DX11RenderOutput: public RenderOutput, DX11RenderTarget {
public:
	DX11RenderOutput();
	~DX11RenderOutput();
	bool Create( IDXGIFactory1 *const factory, ID3D11Device *const device, Window &window, const int display );
	void Resize( ID3D11Device *const device );

	// implementace rozhrani
	virtual ID3D11RenderTargetView *GetRenderTargetView() override;
	virtual void Present( bool vsync ) override;
	virtual DisplayMode GetDisplayMode( const int id ) override;
	virtual int GetDisplayModeId( const int width, const int height, const float refreshRate ) override;
	virtual void SetDisplayMode( const int id, const bool fullscreen ) override;
	virtual void SetDisplayMode( const int width, const int height, const float refreshRate, const bool fullscreen ) override;
	
private:
	void EnumDisplayModes( IDXGIOutput *const output );

private:
	IDXGISwapChain *swapChain;
	IDXGIOutput *output;
	ID3D11RenderTargetView *renderTargetView;
	Window *window;
	Array< DisplayMode > displayModes;
};
*/
class DX11RenderBuffer: public RenderBuffer, DX11RenderTarget {
public:
	DX11RenderBuffer();
	~DX11RenderBuffer();
	bool Create( DX11Device *const device, const RenderBufferDesc &desc );
	virtual RenderBufferDesc GetDesc() const override;
	virtual ID3D11RenderTargetView *GetRenderTargetView() override;

private:
	ID3D11Texture2D *texture;
	ID3D11RenderTargetView *renderTargetView;
	ID3D11ShaderResourceView *shaderResourceView;
	RenderBufferDesc desc;
};

class DX11DepthStencilBuffer: public DepthStencilBuffer {
public:
	DX11DepthStencilBuffer();
	~DX11DepthStencilBuffer();
	bool Create( ID3D11Device *device, const DepthStencilBufferDesc &desc );
	virtual DepthStencilBufferDesc GetDesc() const override;
	ID3D11DepthStencilView *GetDepthStencilView();

private:
	ID3D11Texture2D *texture;
	ID3D11DepthStencilView *view;
	DepthStencilBufferDesc desc;
};

class DX11CommandInterface: public CommandInterface {
public:
	DX11CommandInterface();
	~DX11CommandInterface();
	bool Create( ID3D11DeviceContext *context );

	// implementace rozhrani
	virtual void Begin( Device * const device ) override;
	virtual void Begin( CommandList * const commandList ) override;
	virtual void End() override;
	virtual void SetRenderTargets( RenderTarget * const renderTargets, const int count, DepthStencilBuffer * const depthStencilBuffer ) override;
	virtual void ClearRenderTarget( RenderTarget * const renderTarget, const Color &color ) override;

private:
	ID3D11DeviceContext *context;
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
