#include "Window.h"

Window::Window() {
	renderer = nullptr;
}

Window::~Window() {
	SetRenderer( nullptr );
}

void Window::SetRenderer( Renderer* const renderer ) {
	if ( this->renderer != nullptr ) {
		renderer->UnregisterWindow( *this );
	}
	this->renderer = renderer;
	if ( renderer != nullptr ) {
		renderer->RegisterWindow( *this );
	}
}

bool Window::IsRendererTarget() const {
	return renderer != nullptr;
}

void Window::OnResized( const int width, const int height ) {
	if ( renderer != nullptr ) {
		renderer->ResizeBackBuffer( *this, width, height );
	}
}