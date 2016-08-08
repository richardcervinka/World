#pragma once

#include <memory>
#include "..\RenderInterface.h"
#include "..\Graphicsinfrastructure.h"

// forward declarations
struct IDXGIFactory1;
struct IDXGIAdapter1;
struct IDXGIOutput;

enum class WindowsRenderApi {
	DIRECTX_11_0
	//DIRECTX_11_1,
	//DIRECTX_12_0,
	//OPENGL_4_0,
	//OPENGL_4_3,
	//VULKAN_1_0
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
Temporary factory.
Po vytvoreni objektu Adapter je mozne objekt WindowsGraphicsInfrastructure bezpecne uvolnit z pameti.
*/
class WindowsGraphicsInfrastructure {
public:
	WindowsGraphicsInfrastructure();
	~WindowsGraphicsInfrastructure();

	// no copy
	WindowsGraphicsInfrastructure( const WindowsGraphicsInfrastructure& ) = delete;
	WindowsGraphicsInfrastructure& operator=( const WindowsGraphicsInfrastructure& ) = delete;

	std::unique_ptr< WindowsAdapter > CreateAdapter( const int id ) noexcept;
	std::unique_ptr< WindowsAdapter > CreateAdapter( const WindowsAdapterCapabilities& capabilities ) noexcept;

private:
	IDXGIFactory1* factory;
};