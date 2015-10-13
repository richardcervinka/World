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

void Window::SetRenderDevice( RenderInterface::Device * const device ) {
	if ( device == nullptr ) {
		// unbind render device and delete back buffer
		backBuffer->Release();
		this->device = nullptr;
		this->backBuffer = nullptr;
		return;
	}
	RenderInterface::BackBuffer *backBuffer = device->CreateBackBuffer( *this );
	if ( backBuffer == nullptr ) {
		return;
	}
	this->device = device;
	this->backBuffer = backBuffer;
}

void Window::PresentBackBuffer( const int vsync ) {
	if ( backBuffer != nullptr ) {
		backBuffer->Present( vsync );
	}
}

RenderInterface::BackBuffer *Window::GetBackBuffer() {
	return backBuffer;
}