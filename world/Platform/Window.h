#pragma once

#include "..\Framework\String.h"
#include "..\Framework\Color.h"
#include "..\Renderer\Renderer.h"

// Trida window neni urcena k vytvareni uzivatelskeho rozhrani okennich aplikaci.
// Zapouzdruje funkcionalitu potrebnou k zobrazeni obsahu BackBufferu v klientske oblasti okna.
//
class Window {
public:
	Window();
	virtual ~Window();
	
	// Neni mozne vytvaret kopie okna
	Window( const Window& ) = delete;
	Window& operator=( const Window& ) = delete;
	
	virtual void Destroy() = 0;
	virtual int GetWidth() const = 0;
	virtual int GetHeight() const = 0;
	virtual int GetClientWidth() const = 0;
	virtual int GetClientHeight() const = 0;
	virtual void SetBackgroundColor( const Color& color ) = 0;
	virtual const Color GetBackgroundColor() const = 0;
	virtual void SetPosition( const int x, const int y ) = 0;
	virtual void Show() = 0;
	virtual void Hide() = 0;
	virtual void SetName( const String& str ) = 0;
	virtual const String GetName() const = 0;
	virtual void Update() = 0;

	void SetRenderer( Renderer* const renderer );
	bool IsRendererTarget() const;

protected:
	// Musi byt volano pri zmene velikosti okna
	void OnResized( const int width, const int height );
	
private:
	Renderer* renderer;
};