#pragma once

#include <Windows.h>
#include "..\Application.h"

class WindowsApplication: public Application {
public:
	WindowsApplication();
	virtual ~WindowsApplication();
	
	virtual bool Create( const HINSTANCE hInstance );
	
private:
	virtual void ExitApp() override;
	virtual void AbortApp() override;
	virtual void AbortApp( const String &errorMessage ) override;
	virtual bool ProcessPlatformMessages() override;
	
	// zpracuje zpravu windows, pokud vrati true, zprava nebude odeslana cilovemu oknu
	bool ProcessMessage( const MSG &msg );
	
	// predbezne zpracovani zprav (pred tim, nez jsou odeslany oknu)
	void OnInput( const WPARAM wParam, const LPARAM lParam );
	
private:
	HINSTANCE handle;
};