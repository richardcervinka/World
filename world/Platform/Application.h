#pragma once

#include "..\Framework\String.h"

// Klient by nemel vytvaret vice nez jeden objekt Application
//
class Application {
public:
	Application();
	virtual ~Application();
	
	// neni mozne vytvaret kopie aplikace
	Application( const Application& ) = delete;
	Application &operator=( const Application& ) = delete;
	
	// Vstip do smycky zprav. Navrat je pouze pri radnem ukonceni aplikace
	void Run();
	
	// Metoda Update() je cyklicky volana funkci Run(), predefinovana verze by mela aktualizovat novy snimek
	//virtual void Update();
	
	// Standardni zpusob ukonceni aplikace.
	static void Exit();
	
	// Ukonceni aplikace pri vyskytu kriticke chyby
	static void Abort();
	
	// Ukonceni aplikace pri vyskytu kriticke chyby
	static void Abort( const String &errorMessage );
	
private:
	// Implementace statickych metod, static members volaji primo tyto non-static members
	virtual void ExitApp() = 0;
	virtual void AbortApp() = 0;
	virtual void AbortApp( const String &errorMessage ) = 0;
	
	// Funkce je volana funkci Run() dokud vraci hodnotu true, pokud vrati false, aplikace je standardnim zpusobem ukoncena.
	// Ukolem funkce je zpracovavat systemove zpravy.
	// Ve Windows muze napr. zachytit zpravu WM_KEYDOWN, musi ji vsak poslat take oknu pomoci DispatchMessage().
	virtual bool ProcessPlatformMessages() = 0;
	
private:
	static Application *app;
};