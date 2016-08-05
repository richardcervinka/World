#pragma once

#include "..\Framework\String.h"

/*
Klient by nemel vytvaret vice nez jeden objekt Application!
Vsechny staticke metody nejdrive ziskaji ukazatel na globalni objekt Application
a pote volaji verejne nestaticke metody tohoto objektu.
*/
class Application {
public:
	Application() = default;
	virtual ~Application();
	
	// no copy
	Application( const Application& ) = delete;
	Application& operator=( const Application& ) = delete;
	
	/*
	Vstup do message loop.
	Zaroven nastavi globalni ukazatel na objekt Application.
	*/
	void Run();
	
	// Funkce Update() je volana vzdy po navratu funkce ProcessMessages()
	virtual void Update() noexcept;
	
	// Standardni ukonceni bezici aplikace.
	static void Exit();
	
	// Ukonceni bezici aplikace pri vyskytu kriticke chyby
	static void Abort();
	
	/*
	Ukonceni bezici aplikace pri vyskytu kriticke chyby.
	Pred ukoncenim vypise zpravu errorMessage.
	*/
	static void Abort( const String& errorMessage );
	
private:
	/*
	Implementace volani statickych metod pro konkretni platformu.
	Staticke metody nejdrive ziskaji ukazatel na globalni objekt
	aplikace a pote volaji nasledujici metody.
	*/
	virtual void ExitApp() = 0;
	virtual void AbortApp() = 0;
	virtual void AbortApp( const String& errorMessage ) = 0;
	
	/*
	Funkce je volana funkci Run() dokud vraci hodnotu true.
	Hodnota false ukonci standardnim zpusobem aplikaci.
	*/
	virtual bool ProcessMessages() = 0;
};