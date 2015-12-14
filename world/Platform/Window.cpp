#include "Window.h"

Window::Window() {
	renderer = nullptr;
}

Window::~Window() {
	SetRenderer( nullptr );
}

void Window::SetRenderer( Renderer* const renderer ) {
	if ( this->renderer == renderer ) {
		return;
	}
	if ( this->renderer != nullptr ) {
		renderer->DeleteBackBuffer( *this );
	}
	this->renderer = renderer;
	if ( renderer != nullptr ) {
		renderer->CreateBackBuffer( *this );
	}
}

bool Window::IsRendererTarget() const {
	return renderer != nullptr;
}

RenderInterface::BackBuffer* Window::GetBackBuffer() {
	if ( renderer != nullptr ) {
		return renderer->GetBackBuffer( *this );
	}
	return nullptr;
}

void Window::OnResized( const int width, const int height ) {
	if ( renderer != nullptr ) {
		renderer->ResizeBackBuffer( *this );
	}
}