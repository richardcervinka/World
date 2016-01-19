#pragma once

#include <d3d11.h>
#include <dxgi1_2.h>
#include <memory>
#include <vector>
#include "..\RenderInterface.h"

// forward declarations
class DX11Device;
class DX11BackBuffer;
class DX11Display;
class DX11CommandInterface;
class DX11CommandList;
class DX11Buffer;
class DX11TextureBuffer;
class DX11GenericBuffer;
class DX11RenderTargetView;
class DX11TextureView;
class DX11ConstantBufferView;
class DX11DepthStencilView;
class DX11Shader;
class DX11RenderProgram;
class DX11Sampler;
class DX11BlendState;
class DX11RasterizerState;
class DX11DepthStencilState;
class DX11VertexLayout;
class DX11VertexStream;

using namespace RenderInterface;

// uvolneni COM objektu
template < typename T >
inline void ReleaseCom( T** target ) {
	if ( *target != nullptr ) {
		( *target )->Release();
		*target = nullptr;
	}
}

class DX11Device: public Device {
public:
	DX11Device();
	~DX11Device();
	bool Create( const DX11CreateDeviceParams& params );

	// implementace rozhrani Device

	virtual PBackBuffer CreateBackBuffer( const Window& window ) override;
	virtual PBuffer CreateTextureBuffer( const TextureBufferParams& params, const void* const initialData[] ) override;
	virtual PBuffer CreateVertexBuffer( const int byteWidth, const BufferUsage usage, const BufferAccess access, const void* const initialData  ) override;
	virtual PBuffer CreateIndexBuffer( const int byteWidth, const BufferUsage usage, const BufferAccess access, const void* const initialData  ) override;
	virtual PBuffer CreateConstantBuffer( const int byteWidth, const BufferUsage usage, const BufferAccess access, const void* const initialData ) override;

	virtual PRenderTargetView CreateRenderTargetView( BackBuffer* const backBuffer ) override;
	virtual PRenderTargetView CreateRenderTargetView( Buffer* const textureBuffer ) override;
	virtual PTextureView CreateTextureView( Buffer* const textureBuffer, Sampler* const sampler ) override;
	virtual PDepthStencilView CreateDepthStencilView( Buffer* const textureBuffer, const bool readonly ) override;
	virtual PConstantBufferView CreateConstantBufferView( Buffer* const constantBuffer, const ConstantBufferViewParams& params ) override;
	virtual PVertexStream CreateVertexStream( const VertexStreamParams& params ) override;

	virtual PCommandInterface CreateCommandInterface() override;
	virtual PShader CreateShader( const ShaderParams& params ) override;
	virtual PRenderProgram CreateRenderProgram( Shader* const vs, Shader* const ps, Shader* const gs ) override;
	virtual PSampler CreateSampler( const SamplerParams& params ) override;
	virtual PVertexLayout CreateVertexLayout( const VertexAttribute* const attributes, const int attributesCount, RenderProgram* const program ) override;
	virtual PBlendState CreateBlendState( const BlendStateParams& params ) override;
	virtual PRasterizerState CreateRasterizerState( const RasterizerStateParams& params ) override;
	virtual PDepthStencilState CreateDepthStencilState( const DepthStencilStateParams& params ) override;
	virtual PDisplay CreateDisplay( const int outputId ) override;

	virtual int GetMaxMultisampleQuality( const int samplesCount ) const override;
	virtual int GetOutputsCount() const override;

	// implementation interface
	ID3D11DeviceContext* GetD3D11DeviceContext();

private:
	IDXGIFactory1* dxgiFactory;
	IDXGIAdapter* dxgiAdapter;
	ID3D11Device* device;
	ID3D11DeviceContext* context;
	int outputsCount;
};

class DX11CommandInterface: public CommandInterface {
public:
	DX11CommandInterface();
	~DX11CommandInterface();
	bool Create();

	// CommandInterface implementation
	virtual void Begin( Device* const device ) override;
	virtual void Begin( CommandList* const commandList ) override;
	virtual void End() override;
	virtual void Flush() override;
	virtual void SetRenderTargets( RenderTargetView* const renderTargets[], const int count, DepthStencilView* const depthStencil ) override;
	virtual void ClearRenderTarget( RenderTargetView* const renderTarget, const Color& color ) override;
	virtual void ClearDepthStencil( DepthStencilView* const descriptor, const float depth, const uint8_t stencil ) override;
	virtual void ClearDepth( DepthStencilView* const descriptor, const float depth ) override;
	virtual void ClearStencil( DepthStencilView* const descriptor, const uint8_t stencil ) override;
	virtual void ClearState() override;
	virtual bool Map( Buffer* const buffer, const int subresource, const MapPolicy policy, MappedBuffer& result ) override;
	virtual void Unmap( Buffer* const buffer, MappedBuffer& mappedBuffer ) override;
	virtual bool UpdateBuffer( Buffer* const buffer, const int subresource, const void* const data ) override;
	virtual bool UpdateConstantBuffer( ConstantBufferView* const view, const void* const data ) override;
	virtual void CopyBuffer( Buffer* const src, Buffer* const dest ) override;
	virtual void SetConstantBuffers( ConstantBufferView* const views[], const int count ) override;
	virtual void SetVertexStream( VertexStream* const stream ) override;
	virtual void SetRenderProgram( RenderProgram* const program ) override;
	virtual void SetPrimitiveTopology( const PrimitiveTopology topology ) override;
	virtual void SetBlendState( BlendState* const state ) override;
	virtual void SetDepthStencilState( DepthStencilState* const state, const uint32_t stencilRef ) override;
	virtual void SetRasterizerState( RasterizerState* const state ) override;
	virtual void SetViewports( const Viewport* const viewports[], const int count ) override;
	virtual void SetScissorRects( const ScissorRect* rects, const int count ) override;
	virtual void SetVSTextures( const int startSlot, const int count, TextureView* const views[] ) override;
	virtual void SetPSTextures( const int startSlot, const int count, TextureView* const views[] ) override;
	virtual void SetGSTextures( const int startSlot, const int count, TextureView* const views[] ) override;
	virtual void SetVSSamplers( Sampler* const samplers[ MAX_SAMPLERS ] ) override;
	virtual void SetPSSamplers( Sampler* const samplers[ MAX_SAMPLERS ] ) override;
	virtual void SetGSSamplers( Sampler* const samplers[ MAX_SAMPLERS ] ) override;
	virtual void Draw( const int verticesCount, const int startVertex ) override;
	virtual void DrawIndexed( const int indicesCount, const int startIndex ) override;
	virtual void DrawInstanced( const int verticesCount, const int startVertex, const int instancesCount, const int startInstance ) override;
	virtual void DrawIndexedInstanced( const int indicesCount, const int startIndex, const int instancesCount, const int startInstance ) override;

private:
	ID3D11DeviceContext* context;

	// ulozene state objekty (provadi se test, aby nedochazelo k prenastaveni stejnych objektu)
	ID3D11InputLayout* currentInputLayout;
	ID3D11VertexShader* currentVertexShader;
	ID3D11PixelShader* currentPixelShader;
	ID3D11GeometryShader* currentGeometryShader;
	ID3D11BlendState* currentBlendState;
	ID3D11DepthStencilState* currentDepthStencilState;
	ID3D11RasterizerState* currentRasterizerState;
};

class DX11Display: public Display {
public:
	DX11Display();
	~DX11Display();
	bool Create( IDXGIAdapter* const dxgiAdapter, const int outputId );

	// Display implementation
	virtual void SetFullscreenMode( const DisplayMode& mode, Window& window ) override;
	virtual void SetWindowedMode() override;
	virtual void GetMode( const int id, DisplayMode& result ) const override;
	virtual void FindMode( const DisplayMode& request, DisplayMode& result ) const override;
	virtual void GetBestMode( DisplayMode& result ) const override;
	
private:
	void EnumDisplayModes();
	
public:
	Window* window;
	IDXGIOutput* dxgiOutput;
	std::vector< DisplayMode > modes;
};

class DX11BackBuffer: public BackBuffer {
public:
	DX11BackBuffer();
	~DX11BackBuffer();
	bool Create( ID3D11Device* const device, IDXGIFactory1* const factory, const Window& window );

	// BackBuffer implementation
	virtual void Present( const int vsync ) override;
	virtual void Resize() override;
	virtual int GetWidth() const override;
	virtual int GetHeight() const override;

	// implementation interface
	IDXGISwapChain* GetDXGISwapChain();

private:
	IDXGISwapChain* dxgiSwapChain;
	const Window* window;
	int width;
	int height;
};

class DX11Buffer: public Buffer {
public:
	DX11Buffer();
	~DX11Buffer();

	// Buffer implementation
	virtual void GetInfo( BufferInfo& result ) const override;
	virtual BufferType GetType() const override;
	virtual int GetByteWidth() const override;
	virtual BufferUsage GetUsage() const override;
	virtual BufferAccess GetAccess() const override;
	virtual int GetSubresourcesCount() const override;

	// implementation interface
	ID3D11Resource* GetD3D11Resource();
	virtual bool Map( ID3D11DeviceContext* const context, const int subresource, const D3D11_MAP mapType, MappedBuffer& result ) = 0;

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

	// prekryti funkce GetSubresourcesCount, pouze texture buffery muzou mit vice subresources
	virtual int GetSubresourcesCount() const override;

	// implementation interface
	Format GetFormat() const;
	int GetWidth() const;
	int GetHeight() const;
	int GetDepth() const;
	int GetMipLevels() const;
	int GetArraySize() const;
	int GetSamplesCount() const;
	int GetSamplesQuality() const;

	// DX11Buffer implementation
	virtual bool Map( ID3D11DeviceContext* const context, const int subresource, const D3D11_MAP mapType, MappedBuffer& result ) override;

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

class DX11GenericBuffer: public DX11Buffer {
public:
	bool Create(
		ID3D11Device* const device,
		const BufferType type,
		const int byteWidth,
		const BufferUsage usage,
		const BufferAccess access,
		const void* const initialData
	);
	// DX11Buffer implementation
	virtual bool Map( ID3D11DeviceContext* const context, const int subresource, const D3D11_MAP mapType, MappedBuffer& result ) override;

	// implementation interface
	ID3D11Buffer* GetD3D11Buffer();
};

class DX11RenderTargetView: public RenderTargetView {
public:
	DX11RenderTargetView();
	~DX11RenderTargetView();
	bool Create( ID3D11Device* const device, BackBuffer* const backBuffer );
	bool Create( ID3D11Device* const device, Buffer* const textureBuffer );

	// implementation interface
	ID3D11RenderTargetView* GetD3D11RenderTargetView();

private:
	ID3D11RenderTargetView* view;
};

class DX11TextureView: public TextureView {
public:
	DX11TextureView();
	~DX11TextureView();
	bool Create( ID3D11Device* const device, Buffer* const textureBuffer );

	// implementation interface
	ID3D11ShaderResourceView* GetD3D11ShaderResourceView();

private:
	ID3D11ShaderResourceView* view;
};

class DX11DepthStencilView: public DepthStencilView {
public:
	DX11DepthStencilView();
	~DX11DepthStencilView();
	bool Create( ID3D11Device* const device, Buffer* const textureBuffer, const bool readonly );

	// implementation interface
	ID3D11DepthStencilView* GetD3D11DepthStencilView();

private:
	ID3D11DepthStencilView* view;
};

class DX11ConstantBufferView: public ConstantBufferView {
public:
	DX11ConstantBufferView();
	~DX11ConstantBufferView();
	bool Create( Buffer* const constantBuffer, const ConstantBufferViewParams &params );

	// pouze pro interni uziti, zkopiruje data ze systemove pameti (src) do bufferu (dest)
	void UpdateConstants( const void* const src, void* const dest ) const;

	// implementation interface
	ID3D11Buffer* GetD3D11Buffer();
	int GetVSSlot() const;
	int GetPSSlot() const;
	int GetGSSlot() const;

private:
	ID3D11Buffer* buffer;
	int constantsCount;
	int constantsSize;

	// Indexy cbuffer slotu pro shadery
	int vsSlot;
	int psSlot;
	int gsSlot;

	// mapovani konstant ze systemove pameti do bufferu
	struct ConstantPlacement {
		int sysMemOffset;
		int bufferOffset;
		int size;
	};
	std::unique_ptr< ConstantPlacement[] > map;
};

class DX11Shader: public Shader {
public:
	DX11Shader();
	~DX11Shader();
	bool Compile( ID3D11Device* const device, const ShaderParams& params );

	// Shader implementation
	virtual ShaderType GetType() const override;
	virtual ShaderVersion GetVersion() const override;

	// implementation interface
	ID3DBlob* GetBlob();
	ID3D11VertexShader* GetD3D11VertexShader();
	ID3D11PixelShader* GetD3D11PixelShader();
	ID3D11GeometryShader* GetD3D11GeometryShader();

private:
	ID3DBlob* code;
	ID3D11DeviceChild* shader;
	ShaderType type;
	ShaderVersion version;
};

class DX11RenderProgram: public RenderProgram {
public:
	DX11RenderProgram();
	~DX11RenderProgram();
	bool Create( Shader* const vs, Shader* const ps, Shader* const gs );

	// implementation interface
	ID3D11VertexShader* GetD3D11VertexShader();
	ID3D11PixelShader* GetD3D11PixelShader();
	ID3D11GeometryShader* GetD3D11GeometryShader();
	ID3DBlob* GetVertexShaderByteCode();
	ID3DBlob* GetPixelShaderByteCode();
	ID3DBlob* GetGeometryShaderByteCode();

private:
	ID3D11VertexShader* vs;
	ID3D11PixelShader* ps;
	ID3D11GeometryShader* gs;
	ID3DBlob* vsByteCode;
	ID3DBlob* psByteCode;
	ID3DBlob* gsByteCode;
};

class DX11Sampler: public Sampler {
public:
	DX11Sampler();
	~DX11Sampler();
	bool Create( ID3D11Device* const device, const SamplerParams& params );

	// implementation interface
	ID3D11SamplerState* GetD3D11SamplerState();

private:
	ID3D11SamplerState* sampler;
};

class DX11BlendState: public BlendState {
public:
	DX11BlendState();
	~DX11BlendState();
	bool Create( ID3D11Device* const device, const BlendStateParams& params );

	// implementation interface
	ID3D11BlendState* GetD3D11BlendState();

private:
	ID3D11BlendState* state;
};

class DX11RasterizerState: public RasterizerState {
public:
	DX11RasterizerState();
	~DX11RasterizerState();
	bool Create( ID3D11Device* const device, const RasterizerStateParams& params );

	// implementation interface
	ID3D11RasterizerState* GetD3D11RasterizerState();

private:
	ID3D11RasterizerState* state;
};

class DX11DepthStencilState: public DepthStencilState {
public:
	DX11DepthStencilState();
	~DX11DepthStencilState();
	bool Create( ID3D11Device* const device, const DepthStencilStateParams& params );

	// implementation interface
	ID3D11DepthStencilState* GetD3D11DepthStencilState();

private:
	ID3D11DepthStencilState* state;
};

class DX11VertexLayout: public VertexLayout {
public:
	DX11VertexLayout();
	~DX11VertexLayout();
	bool Create( ID3D11Device* const device, const VertexAttribute* const attributes, const int attributesCount, RenderProgram* const program );

	// implementation interface
	ID3D11InputLayout* GetD3D11InputLayout();

private:
	ID3D11InputLayout* inputLayout;
};

class DX11VertexStream: public VertexStream {
public:
	DX11VertexStream();
	~DX11VertexStream();
	bool Create( const VertexStreamParams& params );

	// implementation interface
	ID3D11Buffer** GetVertexBuffers();
	ID3D11Buffer* GetIndexBuffer();
	ID3D11InputLayout* GetD3D11InputLayout();
	DXGI_FORMAT GetIndexDxgiFormat() const;

private:
	ID3D11Buffer* vertexBuffers[ MAX_VERTEX_INPUT_SLOTS ];
	ID3D11Buffer* indexBuffer;
	DXGI_FORMAT indexBufferFormat;
	ID3D11InputLayout* inputLayout;
};