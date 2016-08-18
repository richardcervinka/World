#pragma once

#include <memory>

struct DisplayMode {
	int width;
	int height;
	int refreshRateNumerator;
	int refreshRateDenominator;
};

class Display {
public:
	Display() = default;
	virtual ~Display() = default;

	// no copy
	Display( const Display& ) = delete;
	Display& operator=( const Display& ) = delete;

	// Ziskani rezimu, pokud rezim s pozadovanym id neexistuje, vrati false
	virtual bool GetMode( const int id, DisplayMode& result ) const noexcept = 0;

	// Najde rezim, ktery nejlepe odpovida pozadovanemu rezimu
	bool FindMode( const DisplayMode& request, DisplayMode& result ) const noexcept;

	// Najde nejlepsi dostupny rezim pro cilove zarizeni
	virtual bool GetBestMode( DisplayMode& result ) const noexcept = 0;

	// Nastavi rezim co nejvice odpovidajici pozadavku (na desktopu prepne do rezimu cele obrazovky)
	//virtual void SetMode( const DisplayMode& mode, Window& window ) noexcept = 0;
};

/*
Physical device (graphics card)
*/
class Adapter {
public:
	Adapter() = default;
	virtual ~Adapter() = default;
	
	// no copy
	Adapter( const Adapter& ) = delete;
	Adapter& operator=( const Adapter& ) = delete;

	// Pocet "displeju" pripojenych na vystupy graficke karty
	virtual int GetOutputsCount() noexcept = 0;

	// Vytvori objekt Display reprezentujici displej pripojeny k vystupu graficke karty
	virtual std::shared_ptr< Display > CreateDisplay( const int outputId ) noexcept = 0;
};