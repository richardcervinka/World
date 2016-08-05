#pragma once

#include <memory>
#include "..\RenderInterface.h"

// forward declarations
struct IDXGIFactory1;
struct IDXGIAdapter1;

enum class WindowsRenderApi {
	DIRECTX_11_0,
	DIRECTX_11_1,
	OPENGL_4_0,
	OPENGL_4_3
};

struct WindowsAdapterCapabilities {
	WindowsRenderApi api;
	unsigned int requiredVideoMemory;
};

class WindowsAdapter {
public:
	WindowsAdapter( IDXGIAdapter1* const adapter );
	~WindowsAdapter();

	// vraci true, pokud ma adapter vsechny pozadovane vlastnosti
	bool CheckCapabilities( const WindowsAdapterCapabilities& capabilities ) noexcept;

	int GetOutputsCount() noexcept;

	//PDisplay CreateDisplay( const int outputId ) noexcept;

	RenderInterface::PDevice CreateD3D11Device() noexcept;

private:
	IDXGIAdapter1* adapter;
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
	WindowsGraphicsInfrastructure( const WindowsGraphicsInfrastructure& gi ) = delete;
	WindowsGraphicsInfrastructure& operator=( const WindowsGraphicsInfrastructure& gi ) = delete;

	std::unique_ptr< WindowsAdapter > CreateAdapter( const int id ) noexcept;
	std::unique_ptr< WindowsAdapter > CreateAdapter( const WindowsAdapterCapabilities& capabilities ) noexcept;

private:
	IDXGIFactory1* factory;
};