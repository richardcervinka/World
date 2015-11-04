#include "Window.h"

Window::Window() {
	renderer = nullptr;
	backBuffer = nullptr;
}

Window::~Window() {
	if ( backBuffer != nullptr ) {
		backBuffer->Release();
		backBuffer = nullptr;
	}
}

void Window::SetRenderer( Renderer* const renderer ) {
	// unbind current (NOT IMPLEMENTED)
	if ( this->renderer != nullptr ) {
		return;
	}
	if ( renderer == nullptr ) {
		return;
	}
	// create new back buffer
	RenderInterface::BackBuffer* backBuffer = renderer->CreateWindowBackBuffer( *this );
	if ( backBuffer == nullptr ) {
		return;
	}
	// save new state
	this->backBuffer = backBuffer;
	this->renderer = renderer;
}

void Window::PresentBackBuffer( const int vsync ) {
	if ( backBuffer != nullptr ) {
		backBuffer->Present( vsync );
	}
}

RenderInterface::BackBuffer* Window::GetBackBuffer() {
	return backBuffer;
}

void Window::ResizeBackBuffer() {
	if ( backBuffer != nullptr ) {
		backBuffer->Resize();
	}
}

bool Window::RendererControlled() const {
	return renderer != nullptr;
}

void Window::OnResized( const int width, const int height ) {
	if ( renderer != nullptr ) {
		renderer->ResizeBuffers( *this, width, height );
	}
}