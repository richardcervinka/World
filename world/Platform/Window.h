#pragma once

//#include "..\Core\RenderInterface.h"
#include "..\Framework\String.h"
#include "..\Framework\Color.h"

/*
Obecne aplikacni okno.
Zapouzdruje funkcionalitu potrebnou k zobrazeni obsahu BackBufferu v klientske oblasti okna.
*/
class Window {
public:
	Window();
	virtual ~Window() = default;
	
	// no copy
	Window( const Window& ) = delete;
	Window& operator=( const Window& ) = delete;

	virtual int GetWidth() const noexcept = 0;
	virtual int GetHeight() const noexcept = 0;
	virtual void Resize( const int width, const int height ) noexcept = 0;
	virtual int GetClientWidth() const noexcept = 0;
	virtual int GetClientHeight() const noexcept = 0;
	virtual void ResizeClient( const int width, const int height ) noexcept = 0;
	virtual void SetPosition( const int x, const int y ) noexcept = 0;

	virtual void SetBackgroundColor( const Color& color ) noexcept = 0;
	virtual const Color GetBackgroundColor() const noexcept = 0;

	virtual void Show() noexcept = 0;
	virtual void Hide() noexcept = 0;
	virtual void SetName( const String& str ) noexcept = 0;
	virtual const String GetName() const noexcept = 0;
	virtual void Update() noexcept = 0;

protected:
	// defaultni event handlery
	virtual void OnSize( const int width, const int height );
	//OnFocus

public:
	// verejne callback event handlery
	using OnSizeHandler = void (*)( const int width, const int height );
	OnSizeHandler onSizeHandler;
};