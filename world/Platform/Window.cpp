#include "Window.h"

Window::Window() {
	device = nullptr;
	backBuffer = nullptr;
}

Window::~Window() {
	if ( backBuffer != nullptr ) {
		backBuffer->Release();
		backBuffer = nullptr;
	}
}

void Window::SetRenderDevice( Device * const device ) {
	if ( device == nullptr ) {
		// unbind render device and delete back buffer
		backBuffer->Release();
		this->device = nullptr;
		this->backBuffer = nullptr;
		return;
	}
	this->device = device;
	this->backBuffer = device->CreateBackBuffer( *this );
}

void Window::PresentBackBuffer( const int vsync ) {
	if ( backBuffer != nullptr ) {
		backBuffer->Present( vsync );
	}
}

BackBuffer *Window::GetBackBuffer() {
	return backBuffer;
}