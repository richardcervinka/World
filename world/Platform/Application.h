#pragma once

#include "..\Framework\String.h"

//class Window;

// Objekt aplikace se stara o inicializaci, vytvoreni hlavniho okna a spusteni smycky zprav.
// odvozene tridy pro cilovou platformu musi implementovat vlastni zpusob vytvoreni objektu (napr. metoda Create() )
//
class Application {
public:
	Application();
	virtual ~Application();
	
	// neni mozne vytvaret kopie aplikace
	Application( const Application& ) = delete;
	Application &operator=( const Application& ) = delete;
	
	// Spusti aplikaci, vstoupi do smycky zprav.
	// Navrat z funkce je pouze pri radnem ukonceni aplikace
	void Run();
	
	// Standardni zpusob ukonceni aplikace.
	// Ukonci vsechna vlakna bezici aplikace, uvolni zdroje a ukonci aplikaci
	static void Exit();
	
	// Ukonceni bezici aplikace pri vyskytu kriticke chyby
	static void Abort();
	
	// Ukonceni bezici aplikace pri vyskytu kriticke chyby
	static void Abort( const String &errorMessage );
	
private:
	virtual void ExitApp() = 0;
	virtual void AbortApp() = 0;
	virtual void AbortApp( const String &errorMessage ) = 0;
	
	// Funkce Run() vola tuto funkci dokud vraci true, pokud vrati false, aplikace bude ukoncena
	virtual bool ProcessPlatformMessages() = 0;
	
private:
	static Application *app;
};