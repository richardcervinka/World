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
	// unbind current
	if ( backBuffer != nullptr ) {
		backBuffer->Release();
		backBuffer = nullptr;
		this->device = nullptr;
	}
	if ( device == nullptr ) {
		return;
	}
	// create new back buffer
	RenderInterface::BackBuffer *newBackBuffer = device->CreateBackBuffer( *this );
	if ( newBackBuffer == nullptr ) {
		return;
	}
	// save new state
	this->backBuffer = newBackBuffer;
	this->device = device;
}

void Window::PresentBackBuffer( const int vsync ) {
	if ( backBuffer != nullptr ) {
		backBuffer->Present( vsync );
	}
}

RenderInterface::BackBuffer *Window::GetBackBuffer() {
	return backBuffer;
}

void Window::ResizeBackBuffer() {
	if ( backBuffer != nullptr ) {
		backBuffer->Resize();
	}
}

bool Window::IsRenderTarget() const {
	return backBuffer != nullptr;
}