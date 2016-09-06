#pragma once

#include <Windows.h>
#include "Platform/Application.h"

class WindowsApplication: public Application {
public:
	WindowsApplication();
	virtual ~WindowsApplication();
	virtual bool Create( const HINSTANCE hInstance, LPSTR lpCmdLine );
	
private:
	virtual void ExitApp() override;
	virtual void AbortApp() override;
	virtual void AbortApp( const String& errorMessage ) override;
	virtual bool ProcessMessages() override;
	
	// Zpracovani windows message. Pokud vrati true, zprava nebude odeslana cilovemu oknu.
	bool ProcessMessage( const MSG& msg );
	
	// standardni message handlery
	void OnInput( const WPARAM wParam, const LPARAM lParam );
	
private:
	HINSTANCE instanceHandle;
};