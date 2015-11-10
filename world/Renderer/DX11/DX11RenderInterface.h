#pragma once

#include <d3d11.h>
#include <dxgi1_2.h>
#include <memory>
#include "..\RenderInterface.h"
#include "..\..\Framework\Array.h"

// forward declarations
class DX11Device;
class DX11Display;
class DX11BackBuffer;
class DX11CommandInterface;
class DX11CommandList;
class DX11Buffer;
class DX11TextureBuffer;
class DX11VertexBuffer;
class DX11IndexBuffer;
class DX11ConstantBuffer;
class DX11RenderTargetDescriptor;
class DX11TextureDescriptor;
class DX11VertexBufferDescriptor;
class DX11IndexBufferDescriptor;
class DX11ConstantBufferDescriptor;
class DX11Shader;
class DX11Sampler;

using namespace RenderInterface;

// uvolneni COM objektu
template < typename T >
inline void ReleaseCOM( T** target ) {
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
	bool Create( const DX11CreateDeviceParams& params );

	// pristupove funkce k rozhrani DirectX11
	ID3D11DeviceContext* GetContext();
	ID3D11Device* GetDevice();

	// implementace rozhrani Device
	virtual CommandInterface* CreateCommandInterface() override;
	virtual Display* CreateDisplay( const int outputId ) override;
	virtual BackBuffer* CreateBackBuffer( Window& window ) override;
	virtual Buffer* CreateVertexBuffer( const VertexBufferParams& params, const void* const initialData  ) override;
	virtual Buffer* CreateIndexBuffer( const IndexBufferParams& params, const void* const initialData  ) override;
	virtual Buffer* CreateTextureBuffer( const TextureBufferParams& params, const void* const initialData[] ) override;
	virtual Buffer* CreateConstantBuffer( const ConstantBufferParams& params, const void* const initialData ) override;
	virtual RenderTargetDescriptor* CreateRenderTargetDescriptor( BackBuffer* const backBbuffer ) override;
	virtual RenderTargetDescriptor* CreateRenderTargetDescriptor( Buffer* const textureBuffer ) override;
	virtual DepthStencilDescriptor* CreateDepthStencilDescriptor( Buffer* const buffer, const DepthStencilDescriptorParams& params ) override;
	virtual VertexBufferDescriptor* CreateVertexBufferDescriptor( Buffer* const buffer, const int vertexOffset ) override;
	virtual ConstantBufferDescriptor* CreateConstantBufferDescriptor( Buffer* const buffer, const ConstantBufferDescriptorParams& params ) override;
	virtual Shader* CreateShader( const ShaderParams& params ) override;
	virtual Sampler* CreateSampler( const SamplerParams& params ) override;

	virtual int GetMultisampleQuality( const int samplesCount ) const override;

private:
	IDXGIFactory1* dxgiFactory;
	IDXGIAdapter* dxgiAdapter;
	ID3D11Device* device;
	ID3D11DeviceContext* context;
};

class DX11Display: public Display {
public:
	virtual ~DX11Display();
	
	bool Create( ID3D11Device* const device, IDXGIAdapter* const adapter, const int outputId );

	// implementace rozhrani Display
	virtual bool SetMode( const DisplayMode& mode, Window& window ) override;
	virtual void SetSystemMode() override;
	virtual bool GetMode( const int id, DisplayMode& result ) const override;
	virtual void FindMode( const DisplayMode& request, DisplayMode& result ) const override;
	virtual void GetBestMode( DisplayMode& result ) const override;
	
private:
	void EnumDisplayModes();
	
public:
	IDXGIOutput* dxgiOutput;
	Window* window;
	Array< DisplayMode > modes;
};

class DX11CommandInterface: public CommandInterface {
public:
	DX11CommandInterface();
	~DX11CommandInterface();
	bool Create();

	// implementace rozhrani CommandInterface
	virtual void Begin( Device* const device ) override;
	virtual void Begin( CommandList* const commandList ) override;
	virtual void End() override;
	virtual void SetRenderTargets( RenderTargetDescriptor* const renderTargets[], const int count, DepthStencilDescriptor* const depthStencil ) override;
	virtual void ClearRenderTarget( RenderTargetDescriptor* const renderTarget, const Color& color ) override;
	virtual void ClearDepthStencil( DepthStencilDescriptor* const descriptor, const float depth, const uint8_t stencil ) override;
	virtual void ClearDepth( DepthStencilDescriptor* const descriptor, const float depth ) override;
	virtual void ClearStencil( DepthStencilDescriptor* const descriptor, const uint8_t stencil ) override;
	virtual void ClearState() override;
	virtual void Flush() override;

private:
	ID3D11DeviceContext* context;
};

class DX11BackBuffer: public BackBuffer {
public:
	DX11BackBuffer();
	virtual ~DX11BackBuffer();
	bool Create( ID3D11Device* const device, IDXGIFactory1* const factory, Window& window );

	// implementace rozhrani BackBuffer
	virtual void Present( const int vsync ) override;
	virtual void Resize() override;
	
	// implementation interface
	IDXGISwapChain* GetSwapChain();

private:
	IDXGISwapChain* dxgiSwapChain;
	ID3D11Device* device;
	Window* window;
	int width;
	int height;
};

class DX11Buffer: public Buffer {
public:
	DX11Buffer();
	~DX11Buffer();

	// implementace rozhrani Buffer
	virtual void GetInfo( BufferInfo& result ) const override;
	virtual BufferType GetType() const override;
	virtual int GetByteWidth() const override;
	virtual BufferUsage GetUsage() const override;
	virtual BufferAccess GetAccess() const override;

	// implementation interface
	ID3D11Resource* GetResource();

protected:
	void SetBuffer( ID3D11Resource* const resource, const BufferInfo& bufferInfo );

private:
	ID3D11Resource* resource;
	BufferInfo bufferInfo;
};

class DX11TextureBuffer: public DX11Buffer {
public:
	DX11TextureBuffer();
	bool Create( ID3D11Device* const device, const TextureBufferParams& params, const void* const initialData[] );

	// implementation interface
	Format GetFormat() const;
	int GetWidth() const;
	int GetHeight() const;
	int GetDepth() const;
	int GetMipLevels() const;
	int GetArraySize() const;
	int GetSamplesCount() const;
	int GetSamplesQuality() const;

private:
	void SetTextureBuffer( ID3D11Resource* const resource, const TextureBufferParams& params );

private:
	Format format;
	int width;
	int height;
	int depth;
	int mipLevels;
	int arraySize;
	int samplesCount;
	int samplesQuality;
};

class DX11VertexBuffer: public DX11Buffer {
public:
	bool Create( ID3D11Device* const device, const VertexBufferParams& params, const void* const initialData );
};

class DX11IndexBuffer: public DX11Buffer {
public:
	bool Create( ID3D11Device* const device, const IndexBufferParams& params, const void* const initialData );
};

class DX11ConstantBuffer: public DX11Buffer {
public:
	bool Create( ID3D11Device* const device, const ConstantBufferParams& params, const void* const initialData );
};

class DX11TextureDescriptor: public TextureDescriptor {
public:
	DX11TextureDescriptor();
	virtual ~DX11TextureDescriptor();
	bool Create( ID3D11Device* const device, Buffer* const textureBuffer );

	// implementace rozhrani TextureDescriptor
	Buffer* GetBuffer() override;

	// implementation interface
	ID3D11ShaderResourceView* GetView();

private:
	ID3D11ShaderResourceView* view;
	DX11TextureBuffer* buffer;
};

class DX11RenderTargetDescriptor: public RenderTargetDescriptor {
public:
	DX11RenderTargetDescriptor();
	virtual ~DX11RenderTargetDescriptor();
	bool Create( ID3D11Device* const device, Buffer* const textureBuffer );
	bool Create( ID3D11Device* const device, BackBuffer* const backBuffer );

	// implementace rozhrani RenderTargetDescriptor
	virtual Buffer* GetBuffer() override;

	// implementation interface
	ID3D11RenderTargetView* GetView();

private:
	ID3D11RenderTargetView* view;
	DX11TextureBuffer* textureBuffer;
};

class DX11DepthStencilDescriptor: public DepthStencilDescriptor {
public:
	DX11DepthStencilDescriptor();
	~DX11DepthStencilDescriptor();
	bool Create( ID3D11Device* const device, Buffer* const textureBuffer, const DepthStencilDescriptorParams& params );

	// implementation interface
	ID3D11DepthStencilView* GetView();
	ID3D11DepthStencilState* GetState();

private:
	ID3D11DepthStencilView* view;
	ID3D11DepthStencilState* state;
};

class DX11VertexBufferDescriptor: public VertexBufferDescriptor {
public:
	DX11VertexBufferDescriptor();
	~DX11VertexBufferDescriptor();
	bool Create( Buffer* const vertexBuffer, const int vertexOffset );

	// implementation interface
	ID3D11Buffer* GetBuffer();
	UINT GetOffset() const;
	UINT GetStride() const;

private:
	ID3D11Buffer* buffer;
	UINT offset;
	UINT stride;
};










class DX11IndexBufferDescriptor: public IndexBufferDescriptor {
public:
	DX11IndexBufferDescriptor();
	~DX11IndexBufferDescriptor();
	void Create( IndexBuffer* const indexBuffer, const int vertexOffset );

	// DirectX 11 getters
	ID3D11Buffer* GetBuffer();
	UINT GetOffset() const;
	DXGI_FORMAT GetDXGIFormat() const;

private:
	ID3D11Buffer* buffer;
	UINT offset;
	DXGI_FORMAT format;
};

class DX11ConstantBufferDescriptor: public ConstantBufferDescriptor {
public:
	DX11ConstantBufferDescriptor();
	~DX11ConstantBufferDescriptor();
	bool Create( ConstantBuffer* const constantBuffer, const ConstantBufferDescriptorParams &params );

	void UpdateConstants( void* const src, void* const dest ) const;

private:
	ID3D11Buffer* buffer;
	int slot;

	// mapovani konstant ze systemove pameti do bufferu
	struct ConstantPlacement {
		int sysMemOffset;
		int bufferOffset;
		int size;
	};
	std::unique_ptr< ConstantPlacement[] > map;
	int constantsCount;
	int constantsSize;
};

class DX11Shader: public Shader {
public:
	DX11Shader();
	~DX11Shader();
	bool Compile( ID3D11Device* const device, const ShaderParams& params );

	// implementace rozhrani Shader
	virtual ShaderType GetType() const override;
	virtual ShaderVersion GetVersion() const override;

	// DirectX 11 getters
	ID3DBlob* GetBlob();
	ID3D11DeviceChild* GetShader();

private:
	ID3DBlob* code;
	ID3D11DeviceChild* shader;
	ShaderType type;
	ShaderVersion version;
};

class DX11Sampler: public Sampler {
public:
	DX11TextureSampler();
	~DX11TextureSampler();
	bool Create( ID3D11Device* const device, const TextureSamplerParams& params );

	// DirectX 11 getters
	ID3D11SamplerState* GetSamplerState();

private:
	ID3D11SamplerState* sampler;
};