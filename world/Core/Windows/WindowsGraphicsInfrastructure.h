#pragma once

#include <memory>
#include <vector>
#include "Core/Windows/ComPtr.h"
#include "Core/RenderInterface.h"
#include "Core/DX11/DX11RenderInterface.h"
#include "Core/Graphicsinfrastructure.h"

// forward declarations
struct IDXGIFactory1;
struct IDXGIAdapter1;
struct IDXGIOutput;
class WindowsWindow;

enum class WindowsRenderApi {
	DIRECTX_11_0
};

struct WindowsAdapterCapabilities {
	WindowsRenderApi api;
	unsigned int requiredVideoMemory;
};

class WindowsAdapter : public Adapter {
public:
	WindowsAdapter( const ComPtr< IDXGIAdapter1 >& adapter );
	~WindowsAdapter();

	// vraci true, pokud ma adapter vsechny pozadovane vlastnosti
	bool CheckCapabilities( const WindowsAdapterCapabilities& capabilities ) noexcept;

	virtual int GetOutputsCount() noexcept override;
	
	virtual std::shared_ptr< Display > CreateDisplay( const int outputId ) noexcept override;
	
	std::shared_ptr< Directx11RenderInterface::Device > CreateDirectx11Device() noexcept;

private:
	ComPtr< IDXGIAdapter1 > adapter;
};

class WindowsDisplay : public Display {
public:
	WindowsDisplay( const ComPtr< IDXGIOutput >& output );
	~WindowsDisplay();

	virtual bool GetMode( const int id, DisplayMode& result ) const noexcept override;
	virtual bool GetBestMode( DisplayMode& result ) const noexcept override;

private:
	void EnumModes() noexcept;

private:
	ComPtr< IDXGIOutput > output;
	std::vector< DisplayMode > modes;
};

/*
class WindowsSwapChain : public SwapChain {
public:
	WindowsSwapChain();
	~WindowsSwapChain();
	bool Create( WindowsWindow* const window, ID3D11Device* const device, IDXGIFactory1* const factory ) noexcept;

	// SwapChain implementation
	virtual void Present() noexcept override;
	virtual void SetFullscreen( Display* const display ) noexcept override;
	virtual int GetWidth() const noexcept override;
	virtual int GetHeight() const noexcept override;
	virtual bool Valid() const noexcept override;

private:
	IDXGISwapChain* swapChain;
	WindowsWindow* window;
	int width;
	int height;
};
*/



std::unique_ptr< WindowsAdapter > CreateWindowsAdapter( const int id ) noexcept;
std::unique_ptr< WindowsAdapter > CreateWindowsAdapter( const WindowsAdapterCapabilities& capabilities ) noexcept;

// CreateWindowsSwapChain( device, window )


/*
Temporary factory
*/

/*
class WindowsGraphicsInfrastructure {
public:
	WindowsGraphicsInfrastructure();
	~WindowsGraphicsInfrastructure();

	// nocopy
	WindowsGraphicsInfrastructure( const WindowsGraphicsInfrastructure& ) = delete;
	WindowsGraphicsInfrastructure& operator=( const WindowsGraphicsInfrastructure& ) = delete;

	// factory interface
	std::unique_ptr< WindowsAdapter > CreateAdapter( const int id ) noexcept;
	std::unique_ptr< WindowsAdapter > CreateAdapter( const WindowsAdapterCapabilities& capabilities ) noexcept;
	//std::unique_ptr< WindowsSwapChain > CreateSwapChain( WindowsWindow& window, const std::shared_ptr< DX11Device >& device ) noexcept;

private:
	IDXGIFactory1* factory;
};
*/