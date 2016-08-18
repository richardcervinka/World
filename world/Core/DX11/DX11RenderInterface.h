#pragma once

#include <d3d11.h>
#include <dxgi1_2.h>
#include <memory>
#include <vector>
#include "..\RenderInterface.h"
#include "..\..\Platform\Windows\WindowsWindow.h"

const DXGI_FORMAT BACK_BUFFER_FORMAT = DXGI_FORMAT_R8G8B8A8_UNORM;

/*
COM temporary smart pointer (pouze pro interni pouziti).
*/
template <typename T>
class ComPtr {
public:
	ComPtr() {
		ptr = nullptr;
	}
	
	~ComPtr() {
		Release();
	}

	ComPtr( const ComPtr& cp ) {
		ptr = cp.Ref();
	}

	ComPtr& operator=( const nullptr_t ptr ) {
		Release();
	}

	template <typename P>
	ComPtr& operator=( const ComPtr< P >& cp ) {
		T* cpptr = cp.Ref();
		Release();
		ptr = cpptr;
	}

	template <typename P>
	ComPtr& operator=( ComPtr< P >&& cp ) {
		T* cpptr = cp.ptr;
		cp.ptr = nullptr;
		ptr = cpptr;
	}

	bool operator==( void* const ptr ) const noexcept {
		return this->ptr == ptr;
	}

	bool operator!=( void* const ptr ) const noexcept {
		return this->ptr != ptr;
	}

	T* operator->() const noexcept {
		return ptr;
	}
	
	T** operator&() noexcept {
		return &ptr;
	}

	T* Raw() const noexcept {
		return ptr;
	}

private:

	T* Ref() noexcept {
		if ( ptr != nullptr ) {
			ptr->AddRef();
		}
		return ptr;
	}

	void Release() noexcept {
		if ( ptr != nullptr ) {
			ptr->Release();
			ptr = nullptr;
		}
	}

private:
	T* ptr;
};

// forward declarations
class DX11Device;
class DX11CommandInterface;
class DX11CommandList;
class DX11SwapChain;
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

class DX11Device: public Device {
public:
	DX11Device();
	~DX11Device();
	bool Create( IDXGIAdapter1* const adapter, const D3D_FEATURE_LEVEL featureLevel ) noexcept;

	// Device implementation

	virtual PSwapChain CreateSwapChain( Window* const window ) noexcept override;

	virtual PBuffer CreateTextureBuffer( const TextureBufferParams& params, const void* const initialData[] ) noexcept override;
	virtual PBuffer CreateVertexBuffer( const int byteWidth, const BufferUsage usage, const BufferAccess access, const void* const initialData  ) noexcept override;
	virtual PBuffer CreateIndexBuffer( const int byteWidth, const BufferUsage usage, const BufferAccess access, const void* const initialData  ) noexcept override;
	virtual PBuffer CreateConstantBuffer( const int byteWidth, const BufferUsage usage, const BufferAccess access, const void* const initialData ) noexcept override;

	virtual PRenderTargetView CreateRenderTargetView( const PBuffer& textureBuffer ) noexcept override;
	virtual PTextureView CreateTextureView( const PBuffer& textureBuffer, const PSampler& sampler ) noexcept override;
	virtual PDepthStencilView CreateDepthStencilView( const PBuffer& textureBuffer, const bool readonly ) noexcept override;
	virtual PConstantBufferView CreateConstantBufferView( const PBuffer& constantBuffer, const ConstantBufferViewParams& params ) noexcept override;
	virtual PVertexStream CreateVertexStream( const VertexStreamParams& params ) noexcept override;

	virtual PCommandInterface CreateCommandInterface() noexcept override;
	virtual PShader CreateShader( const ShaderParams& params ) noexcept override;
	virtual PRenderProgram CreateRenderProgram( const PShader& vs, const PShader& ps, const PShader& gs ) noexcept override;
	virtual PSampler CreateSampler( const SamplerParams& params ) noexcept override;
	virtual PVertexLayout CreateVertexLayout( const VertexAttribute* const attributes, const int attributesCount, const PRenderProgram& program ) noexcept override;
	virtual PBlendState CreateBlendState( const BlendStateParams& params ) noexcept override;
	virtual PRasterizerState CreateRasterizerState( const RasterizerStateParams& params ) noexcept override;
	virtual PDepthStencilState CreateDepthStencilState( const DepthStencilStateParams& params ) noexcept override;

	virtual int GetMaxMultisampleQuality( const int samplesCount ) const noexcept override;

	// directx accessors
	ID3D11DeviceContext* GetD3D11DeviceContext() noexcept;

private:
	ComPtr< ID3D11Device > device;
	ComPtr< ID3D11DeviceContext > context;
};

class DX11CommandInterface: public CommandInterface {
public:
	DX11CommandInterface();
	~DX11CommandInterface();
	bool Create() noexcept;

	// CommandInterface implementation
	virtual void Begin( const PDevice& device ) noexcept override;
	virtual void Begin( const PCommandList& commandList ) noexcept override;
	virtual void End() noexcept override;
	virtual void Flush() noexcept override;
	virtual void SetRenderTargets( const PRenderTargetView* const renderTargets, const int count, const PDepthStencilView& depthStencilView ) noexcept override;
	virtual void ClearRenderTarget( const PRenderTargetView& renderTargetView, const Color& color ) noexcept override;
	virtual void ClearDepthStencil( const PDepthStencilView& depthStencilView, const float depth, const uint8_t stencil ) noexcept override;
	virtual void ClearDepth( const PDepthStencilView& depthStencilView, const float depth ) noexcept override;
	virtual void ClearStencil( const PDepthStencilView& depthStencilView, const uint8_t stencil ) noexcept override;
	virtual void ClearState() noexcept override;
	virtual bool Map( const PBuffer& buffer, const int subresource, const MapPolicy policy, MappedBuffer& result ) noexcept override;
	virtual void Unmap( const PBuffer& buffer, MappedBuffer& mappedBuffer ) noexcept override;
	virtual bool UpdateSubresource( const PBuffer& buffer, const int subresource, const void* const data ) noexcept override;
	virtual bool UpdateBuffer( const PBuffer& buffer, const void* const data, const int bytes, const int offset, const bool discatd ) noexcept override;
	virtual bool UpdateConstantBuffer( const PConstantBufferView& view, const void* const data ) noexcept override;
	virtual void CopyBuffer( const PBuffer& src, const PBuffer& dest ) noexcept override;
	virtual void SetConstantBuffers( const PConstantBufferView* const views, const int count ) noexcept override;
	virtual void SetVertexStream( const PVertexStream& stream ) noexcept override;
	virtual void SetRenderProgram( const PRenderProgram& program ) noexcept override;
	virtual void SetPrimitiveTopology( const PrimitiveTopology topology ) noexcept override;
	virtual void SetBlendState( const PBlendState& state ) noexcept override;
	virtual void SetDepthStencilState( const PDepthStencilState& state, const uint32_t stencilRef ) noexcept override;
	virtual void SetRasterizerState( const PRasterizerState& state ) noexcept override;
	virtual void SetViewports( const Viewport* const viewports[], const int count ) noexcept override;
	virtual void SetScissorRects( const ScissorRect* rects, const int count ) noexcept override;
	virtual void SetVSTextures( const int startSlot, const int count, const PTextureView* const views ) noexcept override;
	virtual void SetPSTextures( const int startSlot, const int count, const PTextureView* const views ) noexcept override;
	virtual void SetGSTextures( const int startSlot, const int count, const PTextureView* const views ) noexcept override;
	virtual void SetVSSamplers( Sampler* const samplers[ MAX_SAMPLERS ] ) noexcept override;
	virtual void SetPSSamplers( Sampler* const samplers[ MAX_SAMPLERS ] ) noexcept override;
	virtual void SetGSSamplers( Sampler* const samplers[ MAX_SAMPLERS ] ) noexcept override;
	virtual void Draw( const int verticesCount, const int startVertex ) noexcept override;
	virtual void DrawIndexed( const int indicesCount, const int startIndex ) noexcept override;
	virtual void DrawInstanced( const int verticesCount, const int startVertex, const int instancesCount, const int startInstance ) noexcept override;
	virtual void DrawIndexedInstanced( const int indicesCount, const int startIndex, const int instancesCount, const int startInstance ) noexcept override;

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

class DX11SwapChain : public SwapChain {
public:
	DX11SwapChain();
	~DX11SwapChain();
	bool Create( const ComPtr< ID3D11Device >& device, Window* const window ) noexcept;

	// SwapChain implementation
	virtual void Present() noexcept override;
	virtual void SetFullscreen( Display* const display ) noexcept override;
	virtual int GetWidth() const noexcept override;
	virtual int GetHeight() const noexcept override;
	virtual bool Valid() const noexcept override;

private:
	ComPtr< IDXGISwapChain > swapChain;
	PRenderTargetView view;
	Window* window;
	int width;
	int height;
};

class DX11Buffer: public Buffer {
public:
	DX11Buffer();
	~DX11Buffer();

	// Buffer implementation
	virtual void GetInfo( BufferInfo& result ) const noexcept override;
	virtual BufferType GetType() const noexcept override;
	virtual int GetByteWidth() const noexcept override;
	virtual BufferUsage GetUsage() const noexcept override;
	virtual BufferAccess GetAccess() const noexcept override;
	virtual int GetSubresourcesCount() const noexcept override;

	// DX11Buffer interface
	ID3D11Resource* GetD3D11Resource() noexcept;
	virtual bool Map( ID3D11DeviceContext* const context, const int subresource, const D3D11_MAP mapType, MappedBuffer& result ) noexcept = 0;

protected:
	//void SetBuffer( ID3D11Resource* const resource, const BufferInfo& bufferInfo ) noexcept;

protected:
	ComPtr< ID3D11Resource > resource;
	BufferInfo bufferInfo;
};

class DX11TextureBuffer: public DX11Buffer {
public:
	DX11TextureBuffer();
	bool Create( const ComPtr< ID3D11Device >& device, const TextureBufferParams& params, const void* const initialData[] ) noexcept;

	// prekryti funkce GetSubresourcesCount, pouze texture buffery muzou mit vice subresources
	virtual int GetSubresourcesCount() const noexcept override;

	// implementation interface
	Format GetFormat() const noexcept;
	int GetWidth() const noexcept;
	int GetHeight() const noexcept;
	int GetDepth() const noexcept;
	int GetMipLevels() const noexcept;
	int GetArraySize() const noexcept;
	int GetSamplesCount() const noexcept;
	int GetSamplesQuality() const noexcept;

	// DX11Buffer implementation
	virtual bool Map( ID3D11DeviceContext* const context, const int subresource, const D3D11_MAP mapType, MappedBuffer& result ) noexcept override;

private:
	void SetTextureBuffer( ID3D11Resource* const resource, const TextureBufferParams& params ) noexcept;

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
		const ComPtr< ID3D11Device >& device,
		const BufferType type,
		const int byteWidth,
		const BufferUsage usage,
		const BufferAccess access,
		const void* const initialData
	) noexcept;

	// DX11Buffer implementation
	virtual bool Map( ID3D11DeviceContext* const context, const int subresource, const D3D11_MAP mapType, MappedBuffer& result ) noexcept override;

	// implementation interface
	ID3D11Buffer* GetD3D11Buffer() noexcept;
};

class DX11RenderTargetView: public RenderTargetView {
public:
	DX11RenderTargetView();
	~DX11RenderTargetView();
	bool Create( const ComPtr< ID3D11Device >& device, const PBuffer& textureBuffer ) noexcept;
	void Create( ComPtr< ID3D11RenderTargetView >& view ) noexcept;

	// implementation interface
	ID3D11RenderTargetView* GetD3D11RenderTargetView() noexcept;

private:
	ComPtr< ID3D11RenderTargetView > view;
};

class DX11TextureView: public TextureView {
public:
	DX11TextureView();
	~DX11TextureView();
	bool Create( const ComPtr< ID3D11Device >& device, const PBuffer& textureBuffer ) noexcept;

	// implementation interface
	ID3D11ShaderResourceView* GetD3D11ShaderResourceView() noexcept;

private:
	ComPtr< ID3D11ShaderResourceView > view;
};

class DX11DepthStencilView: public DepthStencilView {
public:
	DX11DepthStencilView();
	~DX11DepthStencilView();
	bool Create( const ComPtr< ID3D11Device >& device, const PBuffer& textureBuffer, const bool readonly ) noexcept;

	// implementation interface
	ID3D11DepthStencilView* GetD3D11DepthStencilView() noexcept;

private:
	ComPtr< ID3D11DepthStencilView > view;
};

class DX11ConstantBufferView: public ConstantBufferView {
public:
	DX11ConstantBufferView();
	~DX11ConstantBufferView();
	bool Create( const PBuffer& constantBuffer, const ConstantBufferViewParams &params ) noexcept;

	// pouze pro interni uziti, zkopiruje data ze systemove pameti (src) do bufferu (dest)
	void UpdateConstants( const void* const src, void* const dest ) const noexcept;

	// implementation interface
	ID3D11Buffer* GetD3D11Buffer() noexcept;
	int GetVSSlot() const noexcept;
	int GetPSSlot() const noexcept;
	int GetGSSlot() const noexcept;

private:
	ComPtr< ID3D11Buffer > buffer;
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
	bool Compile( const ComPtr< ID3D11Device >& device, const ShaderParams& params ) noexcept;

	// Shader implementation
	virtual ShaderType GetType() const noexcept override;
	virtual ShaderVersion GetVersion() const noexcept override;

	// implementation interface
	ID3DBlob* GetBlob() noexcept;
	ID3D11VertexShader* GetD3D11VertexShader() noexcept;
	ID3D11PixelShader* GetD3D11PixelShader() noexcept;
	ID3D11GeometryShader* GetD3D11GeometryShader() noexcept;

private:
	ComPtr< ID3DBlob > code;
	ComPtr< ID3D11DeviceChild > shader;
	ShaderType type;
	ShaderVersion version;
};

class DX11RenderProgram: public RenderProgram {
public:
	DX11RenderProgram();
	~DX11RenderProgram();
	bool Create( const PShader& vs, const PShader& ps, const PShader& gs ) noexcept;

	// implementation interface
	ID3D11VertexShader* GetD3D11VertexShader() noexcept;
	ID3D11PixelShader* GetD3D11PixelShader() noexcept;
	ID3D11GeometryShader* GetD3D11GeometryShader() noexcept;
	ID3DBlob* GetVertexShaderByteCode() noexcept;
	ID3DBlob* GetPixelShaderByteCode() noexcept;
	ID3DBlob* GetGeometryShaderByteCode() noexcept;

private:
	ComPtr< ID3D11VertexShader > vs;
	ComPtr< ID3D11PixelShader > ps;
	ComPtr< ID3D11GeometryShader > gs;
	ComPtr< ID3DBlob > vsByteCode;
	ComPtr< ID3DBlob > psByteCode;
	ComPtr< ID3DBlob > gsByteCode;
};

class DX11Sampler: public Sampler {
public:
	DX11Sampler();
	~DX11Sampler();
	bool Create( const ComPtr< ID3D11Device >& device, const SamplerParams& params ) noexcept;

	// implementation interface
	ID3D11SamplerState* GetD3D11SamplerState() noexcept;

private:
	ComPtr< ID3D11SamplerState > sampler;
};

class DX11BlendState: public BlendState {
public:
	DX11BlendState();
	~DX11BlendState();
	bool Create( const ComPtr< ID3D11Device >& device, const BlendStateParams& params ) noexcept;

	// implementation interface
	ID3D11BlendState* GetD3D11BlendState() noexcept;

private:
	ComPtr< ID3D11BlendState > state;
};

class DX11RasterizerState: public RasterizerState {
public:
	DX11RasterizerState();
	~DX11RasterizerState();
	bool Create( const ComPtr< ID3D11Device >& device, const RasterizerStateParams& params ) noexcept;

	// implementation interface
	ID3D11RasterizerState* GetD3D11RasterizerState() noexcept;

private:
	ComPtr< ID3D11RasterizerState > state;
};

class DX11DepthStencilState: public DepthStencilState {
public:
	DX11DepthStencilState();
	~DX11DepthStencilState();
	bool Create( const ComPtr< ID3D11Device >& device, const DepthStencilStateParams& params ) noexcept;

	// implementation interface
	ID3D11DepthStencilState* GetD3D11DepthStencilState() noexcept;

private:
	ComPtr< ID3D11DepthStencilState > state;
};

class DX11VertexLayout: public VertexLayout {
public:
	DX11VertexLayout();
	~DX11VertexLayout();
	bool Create( const ComPtr< ID3D11Device >& device, const VertexAttribute* const attributes, const int attributesCount, const PRenderProgram& program ) noexcept;

	// implementation interface
	ID3D11InputLayout* GetD3D11InputLayout() noexcept;

private:
	ComPtr< ID3D11InputLayout > inputLayout;
};

class DX11VertexStream: public VertexStream {
public:
	DX11VertexStream();
	~DX11VertexStream();
	bool Create( const VertexStreamParams& params ) noexcept;

	// implementation interface
	ID3D11Buffer** GetVertexBuffers() noexcept;
	ID3D11Buffer* GetIndexBuffer() noexcept;
	ID3D11InputLayout* GetD3D11InputLayout() noexcept;
	DXGI_FORMAT GetIndexDxgiFormat() const noexcept;

private:
	ID3D11Buffer* vertexBuffers[ MAX_VERTEX_INPUT_SLOTS ];
	ID3D11Buffer* indexBuffer;
	DXGI_FORMAT indexBufferFormat;
	ID3D11InputLayout* inputLayout;
};