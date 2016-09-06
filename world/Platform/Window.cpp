#include "Window.h"
#include "Core/Renderer.h"

Window::Window() {
	onSizeHandler = nullptr;
}

void Window::OnSize( const int width, const int height ) {
	if ( onSizeHandler != nullptr ) {
		onSizeHandler( width, height );
	}
}