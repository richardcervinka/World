#pragma once

#include <memory>
#include "..\RenderInterface.h"
#include "..\DX11\DX11RenderInterface.h"
#include "..\Graphicsinfrastructure.h"

// forward declarations
struct IDXGIFactory1;
struct IDXGIAdapter1;
struct IDXGIOutput;
//class DX11Device;
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
	WindowsAdapter( IDXGIAdapter1* const adapter );
	~WindowsAdapter();

	// vraci true, pokud ma adapter vsechny pozadovane vlastnosti
	bool CheckCapabilities( const WindowsAdapterCapabilities& capabilities ) noexcept;

	virtual int GetOutputsCount() noexcept override;
	
	virtual std::shared_ptr< Display > CreateDisplay( const int outputId ) noexcept override;
	
	RenderInterface::PDevice CreateDX11Device() noexcept;

private:
	IDXGIAdapter1* adapter;
};

class WindowsDisplay : public Display {
public:
	WindowsDisplay( IDXGIOutput* const output );
	~WindowsDisplay();

	virtual bool GetMode( const int id, DisplayMode& result ) const noexcept override;
	virtual bool GetBestMode( DisplayMode& result ) const noexcept override;

private:
	void EnumModes() noexcept;

private:
	IDXGIOutput* output;
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