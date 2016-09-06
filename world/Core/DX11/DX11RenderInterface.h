#pragma once

#include <d3d11.h>
#include <dxgi1_2.h>
#include "Core/RenderInterface.h"
#include "Core/Windows/ComPtr.h"
#include "Platform/Windows/WindowsWindow.h"

namespace Directx11RenderInterface {

	const DXGI_FORMAT BACK_BUFFER_FORMAT = DXGI_FORMAT_R8G8B8A8_UNORM;

	// forward declarations
	class Device;
	class CommandInterface;
	class CommandList;
	class SwapChain;
	class Buffer;
	class TextureBuffer;
	class RenderTargetView;
	class TextureView;
	class ConstantBufferView;
	class DepthStencilView;
	class Shader;
	class RenderProgram;
	class Sampler;
	class BlendState;
	class RasterizerState;
	class DepthStencilState;
	class VertexLayout;
	class VertexStream;

	class Device: public RenderInterface::Device {
	public:
		Device();
		~Device();
		bool Create( const ComPtr< IDXGIAdapter1 >& adapter, const D3D_FEATURE_LEVEL featureLevel ) noexcept;

		// create swap chain
		RenderInterface::PSwapChain CreateSwapChain( WindowsWindow* const window ) noexcept;

		// Device implementation

		virtual RenderInterface::PBuffer CreateTextureBuffer( const RenderInterface::TextureBufferParams& params ) noexcept override;
		virtual RenderInterface::PBuffer CreateVertexBuffer( const RenderInterface::BufferParams& params ) noexcept override;
		virtual RenderInterface::PBuffer CreateIndexBuffer( const RenderInterface::BufferParams& params ) noexcept override;
		virtual RenderInterface::PBuffer CreateConstantBuffer( const RenderInterface::BufferParams& params ) noexcept override;

		virtual RenderInterface::PRenderTargetView CreateRenderTargetView( const RenderInterface::PBuffer& textureBuffer ) noexcept override;
		virtual RenderInterface::PTextureView CreateTextureView( const RenderInterface::PBuffer& textureBuffer, const RenderInterface::PSampler& sampler ) noexcept override;
		virtual RenderInterface::PDepthStencilView CreateDepthStencilView( const RenderInterface::PBuffer& textureBuffer, const bool readonly ) noexcept override;
		virtual RenderInterface::PConstantBufferView CreateConstantBufferView( const RenderInterface::PBuffer& constantBuffer, const RenderInterface::ConstantBufferViewParams& params ) noexcept override;
		virtual RenderInterface::PVertexStream CreateVertexStream( const RenderInterface::VertexStreamParams& params ) noexcept override;

		virtual RenderInterface::PCommandInterface CreateCommandInterface() noexcept override;
		virtual RenderInterface::PShader CreateShader( const RenderInterface::ShaderParams& params ) noexcept override;
		virtual RenderInterface::PRenderProgram CreateRenderProgram( const RenderInterface::PShader& vs, const RenderInterface::PShader& ps, const RenderInterface::PShader& gs ) noexcept override;
		virtual RenderInterface::PSampler CreateSampler( const RenderInterface::SamplerParams& params ) noexcept override;
		virtual RenderInterface::PVertexLayout CreateVertexLayout( const RenderInterface::VertexAttribute* const attributes, const int attributesCount, const RenderInterface::PRenderProgram& program ) noexcept override;
		virtual RenderInterface::PBlendState CreateBlendState( const RenderInterface::BlendStateParams& params ) noexcept override;
		virtual RenderInterface::PRasterizerState CreateRasterizerState( const RenderInterface::RasterizerStateParams& params ) noexcept override;
		virtual RenderInterface::PDepthStencilState CreateDepthStencilState( const RenderInterface::DepthStencilStateParams& params ) noexcept override;

		virtual int GetMaxMultisampleQuality( const int samplesCount ) const noexcept override;

		// directx accessors
		ID3D11DeviceContext* GetD3D11DeviceContext() noexcept;

	private:
		ComPtr< ID3D11Device > device;
		ComPtr< ID3D11DeviceContext > context;
	};

	class CommandInterface: public RenderInterface::CommandInterface {
	public:
		CommandInterface();
		~CommandInterface();
		bool Create() noexcept;

		// CommandInterface implementation
		virtual void Begin( const RenderInterface::PDevice& device ) noexcept override;
		virtual void Begin( const RenderInterface::PCommandList& commandList ) noexcept override;
		virtual void End() noexcept override;
		virtual void Flush() noexcept override;
		virtual void SetRenderTargets( const RenderInterface::PRenderTargetView* const renderTargets, const int count, const RenderInterface::PDepthStencilView& depthStencilView ) noexcept override;
		virtual void ClearRenderTarget( const RenderInterface::PRenderTargetView& renderTargetView, const Color& color ) noexcept override;
		virtual void ClearDepthStencil( const RenderInterface::PDepthStencilView& depthStencilView, const float depth, const uint8_t stencil ) noexcept override;
		virtual void ClearDepth( const RenderInterface::PDepthStencilView& depthStencilView, const float depth ) noexcept override;
		virtual void ClearStencil( const RenderInterface::PDepthStencilView& depthStencilView, const uint8_t stencil ) noexcept override;
		virtual void ClearState() noexcept override;
		virtual bool Map( const RenderInterface::PBuffer& buffer, const int subresource, const RenderInterface::MapPolicy policy, RenderInterface::MappedBuffer& result ) noexcept override;
		virtual void Unmap( const RenderInterface::PBuffer& buffer, RenderInterface::MappedBuffer& mappedBuffer ) noexcept override;
		virtual bool UpdateSubresource( const RenderInterface::PBuffer& buffer, const int subresource, const void* const data ) noexcept override;
		virtual bool UpdateBuffer( const RenderInterface::PBuffer& buffer, const void* const data, const int bytes, const int offset, const bool discatd ) noexcept override;
		virtual bool UpdateConstantBuffer( const RenderInterface::PConstantBufferView& view, const void* const data ) noexcept override;
		virtual void CopyBuffer( const RenderInterface::PBuffer& src, const RenderInterface::PBuffer& dest ) noexcept override;
		virtual void SetConstantBuffers( const RenderInterface::PConstantBufferView* const views, const int count ) noexcept override;
		virtual void SetVertexStream( const RenderInterface::PVertexStream& stream ) noexcept override;
		virtual void SetRenderProgram( const RenderInterface::PRenderProgram& program ) noexcept override;
		virtual void SetPrimitiveTopology( const RenderInterface::PrimitiveTopology topology ) noexcept override;
		virtual void SetBlendState( const RenderInterface::PBlendState& state ) noexcept override;
		virtual void SetDepthStencilState( const RenderInterface::PDepthStencilState& state, const uint32_t stencilRef ) noexcept override;
		virtual void SetRasterizerState( const RenderInterface::PRasterizerState& state ) noexcept override;
		virtual void SetViewports( const RenderInterface::Viewport* const viewports[], const int count ) noexcept override;
		virtual void SetScissorRects( const RenderInterface::ScissorRect* rects, const int count ) noexcept override;
		virtual void SetVSTextures( const int startSlot, const int count, const RenderInterface::PTextureView* const views ) noexcept override;
		virtual void SetPSTextures( const int startSlot, const int count, const RenderInterface::PTextureView* const views ) noexcept override;
		virtual void SetGSTextures( const int startSlot, const int count, const RenderInterface::PTextureView* const views ) noexcept override;
		virtual void SetVSSamplers( RenderInterface::Sampler* const samplers[ RenderInterface::MAX_SAMPLERS ] ) noexcept override;
		virtual void SetPSSamplers( RenderInterface::Sampler* const samplers[ RenderInterface::MAX_SAMPLERS ] ) noexcept override;
		virtual void SetGSSamplers( RenderInterface::Sampler* const samplers[ RenderInterface::MAX_SAMPLERS ] ) noexcept override;
		virtual void Draw( const int verticesCount, const int startVertex ) noexcept override;
		virtual void DrawIndexed( const int indicesCount, const int startIndex ) noexcept override;
		virtual void DrawInstanced( const int verticesCount, const int startVertex, const int instancesCount, const int startInstance ) noexcept override;
		virtual void DrawIndexedInstanced( const int indicesCount, const int startIndex, const int instancesCount, const int startInstance ) noexcept override;

	private:
		ComPtr< ID3D11DeviceContext > context;

		// ulozene state objekty (provadi se test, aby nedochazelo k prenastaveni stejnych objektu)
		ComPtr< ID3D11InputLayout > currentInputLayout;
		ComPtr< ID3D11VertexShader > currentVertexShader;
		ComPtr< ID3D11PixelShader > currentPixelShader;
		ComPtr< ID3D11GeometryShader > currentGeometryShader;
		ComPtr< ID3D11BlendState > currentBlendState;
		ComPtr< ID3D11DepthStencilState > currentDepthStencilState;
		ComPtr< ID3D11RasterizerState > currentRasterizerState;
	};

	class SwapChain : public RenderInterface::SwapChain {
	public:
		SwapChain();
		~SwapChain();
		bool Create( const ComPtr< ID3D11Device >& device, Window* const window ) noexcept;

		// SwapChain implementation
		virtual void SetFullscreen( Display* const display ) noexcept override;
		virtual int GetWidth() const noexcept override;
		virtual int GetHeight() const noexcept override;
		virtual bool Valid() const noexcept override;
		virtual RenderInterface::PRenderTargetView AcquireRenderTargetView() noexcept override;
		virtual void Present( const RenderInterface::PRenderTargetView& rtv, const RenderInterface::SwapChainPresentMode presentMode ) noexcept override;

	private:
		ComPtr< IDXGISwapChain > swapChain;
		RenderInterface::PRenderTargetView rtv;
		Window* window;
		int width;
		int height;
	};

	/*
	Generic buffer.
	*/
	class Buffer: public RenderInterface::Buffer {
	public:
		Buffer();
		~Buffer();
		bool Create( const ComPtr< ID3D11Device >& device, const RenderInterface::BufferType type, const RenderInterface::BufferParams& params ) noexcept;

		// Buffer implementation
		virtual void GetInfo( RenderInterface::BufferInfo& result ) const noexcept override;
		virtual RenderInterface::BufferType GetType() const noexcept override;
		virtual int GetSize() const noexcept override;
		virtual RenderInterface::BufferUsage GetUsage() const noexcept override;
		virtual RenderInterface::BufferAccess GetAccess() const noexcept override;
		virtual int GetSubresourcesCount() const noexcept override;

		// directx accessor
		ID3D11Buffer* GetD3D11Buffer() noexcept;

		// map helper
		virtual bool Map( const ComPtr< ID3D11DeviceContext >& context, const int subresource, const D3D11_MAP mapType, RenderInterface::MappedBuffer& result ) noexcept;

	protected:
		ComPtr< ID3D11Buffer > buffer;
		RenderInterface::BufferInfo info;
	};

	class TextureBuffer: public RenderInterface::Buffer {
	public:
		TextureBuffer();
		bool Create( const ComPtr< ID3D11Device >& device, const RenderInterface::TextureBufferParams& params ) noexcept;

		// Buffer implementation
		virtual void GetInfo( RenderInterface::BufferInfo& result ) const noexcept override;
		virtual RenderInterface::BufferType GetType() const noexcept override;
		virtual int GetSize() const noexcept override;
		virtual RenderInterface::BufferUsage GetUsage() const noexcept override;
		virtual RenderInterface::BufferAccess GetAccess() const noexcept override;
		virtual int GetSubresourcesCount() const noexcept override;

		// DX11TextureBuffer interface
		RenderInterface::Format GetFormat() const noexcept;
		int GetWidth() const noexcept;
		int GetHeight() const noexcept;
		int GetDepth() const noexcept;
		int GetMipLevels() const noexcept;
		int GetArraySize() const noexcept;
		int GetSamplesCount() const noexcept;
		int GetSamplesQuality() const noexcept;

		// directx object accessor
		ID3D11Resource* GetD3D11Resource() noexcept;

		// map helper
		virtual bool Map( const ComPtr< ID3D11DeviceContext >& context, const int subresource, const D3D11_MAP mapType, RenderInterface::MappedBuffer& result ) noexcept;

	private:
		void SetBuffer( ComPtr< ID3D11Resource > resource, const RenderInterface::TextureBufferParams& params ) noexcept;

	private:
		ComPtr< ID3D11Resource > buffer;
		RenderInterface::BufferInfo info;

		RenderInterface::Format format;
		int width;
		int height;
		int depth;
		int mipLevels;
		int arraySize;
		int samplesCount;
		int samplesQuality;
	};

	class RenderTargetView: public RenderInterface::RenderTargetView {
	public:
		RenderTargetView();
		~RenderTargetView();
		bool Create( const ComPtr< ID3D11Device >& device, const RenderInterface::PBuffer& textureBuffer ) noexcept;
		bool Create( const ComPtr< ID3D11Device >& device, ID3D11Texture2D* const backBuffer ) noexcept;

		// implementation interface
		ID3D11RenderTargetView* GetD3D11RenderTargetView() noexcept;

	private:
		ComPtr< ID3D11RenderTargetView > view;
	};

	class TextureView: public RenderInterface::TextureView {
	public:
		TextureView();
		~TextureView();
		bool Create( const ComPtr< ID3D11Device >& device, const RenderInterface::PBuffer& textureBuffer ) noexcept;

		// implementation interface
		ID3D11ShaderResourceView* GetD3D11ShaderResourceView() noexcept;

	private:
		ComPtr< ID3D11ShaderResourceView > view;
	};

	class DepthStencilView: public RenderInterface::DepthStencilView {
	public:
		DepthStencilView();
		~DepthStencilView();
		bool Create( const ComPtr< ID3D11Device >& device, const RenderInterface::PBuffer& textureBuffer, const bool readonly ) noexcept;

		// implementation interface
		ID3D11DepthStencilView* GetD3D11DepthStencilView() noexcept;

	private:
		ComPtr< ID3D11DepthStencilView > view;
	};

	class ConstantBufferView: public RenderInterface::ConstantBufferView {
	public:
		ConstantBufferView();
		~ConstantBufferView();
		bool Create( const RenderInterface::PBuffer& constantBuffer, const RenderInterface::ConstantBufferViewParams &params ) noexcept;

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

	class Shader: public RenderInterface::Shader {
	public:
		Shader();
		~Shader();
		bool Compile( const ComPtr< ID3D11Device >& device, const RenderInterface::ShaderParams& params ) noexcept;

		// Shader implementation
		virtual RenderInterface::ShaderType GetType() const noexcept override;
		virtual RenderInterface::ShaderVersion GetVersion() const noexcept override;

		// implementation interface
		ID3DBlob* GetBlob() noexcept;
		ID3D11VertexShader* GetD3D11VertexShader() noexcept;
		ID3D11PixelShader* GetD3D11PixelShader() noexcept;
		ID3D11GeometryShader* GetD3D11GeometryShader() noexcept;

	private:
		ComPtr< ID3DBlob > code;
		ComPtr< ID3D11DeviceChild > shader;
		RenderInterface::ShaderType type;
		RenderInterface::ShaderVersion version;
	};

	class RenderProgram: public RenderInterface::RenderProgram {
	public:
		RenderProgram();
		~RenderProgram();
		bool Create( const RenderInterface::PShader& vs, const RenderInterface::PShader& ps, const RenderInterface::PShader& gs ) noexcept;

		// directx accessors
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

	class Sampler: public RenderInterface::Sampler {
	public:
		Sampler();
		~Sampler();
		bool Create( const ComPtr< ID3D11Device >& device, const RenderInterface::SamplerParams& params ) noexcept;

		// implementation interface
		ID3D11SamplerState* GetD3D11SamplerState() noexcept;

	private:
		ComPtr< ID3D11SamplerState > sampler;
	};

	class BlendState: public RenderInterface::BlendState {
	public:
		BlendState();
		~BlendState();
		bool Create( const ComPtr< ID3D11Device >& device, const RenderInterface::BlendStateParams& params ) noexcept;

		// implementation interface
		ID3D11BlendState* GetD3D11BlendState() noexcept;

	private:
		ComPtr< ID3D11BlendState > state;
	};

	class RasterizerState: public RenderInterface::RasterizerState {
	public:
		RasterizerState();
		~RasterizerState();
		bool Create( const ComPtr< ID3D11Device >& device, const RenderInterface::RasterizerStateParams& params ) noexcept;

		// implementation interface
		ID3D11RasterizerState* GetD3D11RasterizerState() noexcept;

	private:
		ComPtr< ID3D11RasterizerState > state;
	};

	class DepthStencilState: public RenderInterface::DepthStencilState {
	public:
		DepthStencilState();
		~DepthStencilState();
		bool Create( const ComPtr< ID3D11Device >& device, const RenderInterface::DepthStencilStateParams& params ) noexcept;

		// implementation interface
		ID3D11DepthStencilState* GetD3D11DepthStencilState() noexcept;

	private:
		ComPtr< ID3D11DepthStencilState > state;
	};

	class VertexLayout: public RenderInterface::VertexLayout {
	public:
		VertexLayout();
		~VertexLayout();

		bool Create(
			const ComPtr< ID3D11Device >& device,
			const RenderInterface::VertexAttribute* const attributes,
			const int attributesCount,
			const RenderInterface::PRenderProgram& program
		) noexcept;

		// implementation interface
		ComPtr< ID3D11InputLayout > GetD3D11InputLayout() noexcept;

	private:
		ComPtr< ID3D11InputLayout > inputLayout;
	};

	class VertexStream: public RenderInterface::VertexStream {
	public:
		VertexStream();
		~VertexStream();
		bool Create( const RenderInterface::VertexStreamParams& params ) noexcept;

		// implementation interface
		ID3D11Buffer** GetVertexBuffers() noexcept;
		ID3D11Buffer* GetIndexBuffer() noexcept;
		ID3D11InputLayout* GetD3D11InputLayout() noexcept;
		DXGI_FORMAT GetIndexFormat() const noexcept;

	private:
		ComPtr< ID3D11Buffer > indexBuffer;
		ComPtr< ID3D11InputLayout > inputLayout;
		DXGI_FORMAT indexBufferFormat;
	
		// ukazatele na ID3D11Buffer objekty
		ComPtr< ID3D11Buffer > vertexBuffers[ RenderInterface::MAX_VERTEX_INPUT_SLOTS ];

		// Do pipeline potrebujeme nabindovat ID3D11Buffer* [].
		ID3D11Buffer* vertexBuffersBindArray[ RenderInterface::MAX_VERTEX_INPUT_SLOTS ];
	};

} // namespace Directx11RenderInterface