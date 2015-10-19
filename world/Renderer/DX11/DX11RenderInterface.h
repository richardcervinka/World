#pragma once

#include <d3d11.h>
#include <dxgi1_2.h>
#include "..\RenderInterface.h"
#include "..\..\Framework\Array.h"

using namespace RenderInterface;

// uvolneni COM objektu
template < typename T >
inline void ReleaseCOM( T **target ) {
	if ( *target != nullptr ) {
		( *target )->Release();
		*target = nullptr;
	}
}

// Ukonci aplikaci v dusledku chyby volani DirectX
// Funkce nesmi byt volana pri vytvareni RenderInterface objektu
void AbortDXInvalidCall( const HRESULT hresult );

// prevede hodnotu typu Format na DXGI_FORMAT
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

	// implementace rozhrani
	virtual CommandInterface *CreateCommandInterface() override;
	virtual BackBuffer *CreateBackBuffer( Window &window ) override;
	virtual RenderBuffer *CreateRenderBuffer( const RenderBufferDesc &desc ) override;
	virtual DepthStencilBuffer *CreateDepthStencilBuffer( const DepthStencilBufferDesc &desc ) override;
	virtual TextureSampler *CreateTextureSampler( const TextureSamplerDesc &desc ) override;
	virtual Display *CreateDisplay( const int outputId ) override;
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

// Messenger pattern only
//
class DX11ShaderResourceObject: public ShaderResourceObject {
public:
	ID3D11ShaderResourceView *view = nullptr;
};

// Messenger pattern only
//
class DX11RenderTargetObject: public RenderTargetObject {
public:
	ID3D11RenderTargetView *view = nullptr;
};

class DX11BackBuffer: public BackBuffer {
public:
	DX11BackBuffer();
	virtual ~DX11BackBuffer();

	bool Create( ID3D11Device *const device, IDXGIFactory1 *const factory, Window &window );
	IDXGISwapChain *GetSwapChain();

	// implementace rozhrani BackBuffer
	virtual RenderTargetObject *GetRenderTargetObject() override;
	virtual void Present( const int vsync ) override;
	virtual void Resize() override;

private:
	ID3D11Device *device;
	IDXGISwapChain *dxgiSwapChain;
	DX11RenderTargetObject renderTargetObject;
	Window *window;
	int width;
	int height;
};

class DX11RenderBuffer: public RenderBuffer {
public:
	DX11RenderBuffer();
	~DX11RenderBuffer();
	bool Create( DX11Device *const device, const RenderBufferDesc &desc );

	// implementace rozhrani RenderBuffer
	virtual RenderBufferDesc GetDesc() const override;
	virtual RenderTargetObject *GetRenderTargetObject() override;

	// implementace rozhrani TextureBuffer
	virtual int GetDimmension() const override;
	virtual const Format GetFormat() const override;
	virtual ShaderResourceObject *GetShaderResourceObject() override;

private:
	ID3D11Texture2D *texture;
	DX11RenderTargetObject renderTargetObject;
	DX11ShaderResourceObject shaderResourceObject;
	//ID3D11ShaderResourceView *shaderResourceView;
	RenderBufferDesc desc;
};

class DX11DepthStencilBuffer: public DepthStencilBuffer {
public:
	DX11DepthStencilBuffer();
	~DX11DepthStencilBuffer();
	bool Create( ID3D11Device *device, const DepthStencilBufferDesc &desc );
	virtual DepthStencilBufferDesc GetDesc() const override;
	ID3D11DepthStencilView *GetDepthStencilView();

	// implementace rozhrani TextureBuffer
	virtual int GetDimmension() const override { return 0; }
	virtual const Format GetFormat() const override { return Format::UNKNOWN; }
	virtual ShaderResourceObject *GetShaderResourceObject() override { return new DX11ShaderResourceObject(); }//---------------------------------------------------------------------------

private:
	ID3D11Texture2D *texture;
	ID3D11DepthStencilView *view;
	DepthStencilBufferDesc desc;
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
	virtual void SetRenderTargets( RenderTargetObject * const renderTargets[], const int count, DepthStencilBuffer * const depthStencilBuffer ) override;
	virtual void ClearRenderTarget( RenderTargetObject * const renderTarget, const Color &color ) override;

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