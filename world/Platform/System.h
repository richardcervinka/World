#pragma once

#include <cstdint>

enum class OSVersion {
	UNKNOWN,
	XP,
	XP_SP1,
	XP_SP2,
	XP_SP3,
	VISTA,
	VISTA_SP1,
	VISTA_SP2,
	WIN7,
	WIN7_SP1,
	WIN8,
	WIN10
};

enum class CPUArchitecture {
	UNKNOWN,
	X64,
	X86,
	ARM
};

struct AdapterInfo {
	int id;
	int outputsCount;
	uint32_t vendorId;
	uint64_t dedicatedVideoMemory;
	uint64_t dedicatedSystemMemory;
	uint64_t sharedSystemMemory;
	char16_t description[ 128 ];
};

/*
Informace o hardware a operacnim systemu
*/
class System {
public:
	virtual ~System() = default;

	// Musi byt provedeno pred volanim ostatnich metod.
	virtual void EnumerateCapabilities() = 0;

	// vraci true, pokud je operacni system kompatibilni s parametrem version
	virtual bool CompatibleOS( const OSVersion version ) const = 0;

	// pocet logickych procesoru
	virtual int GetProcessorsCount() const = 0;

	// architektura procesoru
	virtual CPUArchitecture GetCPUArchitecture() const = 0;

	// pocet bajtu nainstalovane pameti RAM
	virtual long long GetPhysicalMemory() const = 0;

	// dostupnost SSE
	virtual bool SSE() const = 0;
	virtual bool SSE2() const = 0;
	virtual bool SSE3() const = 0;

	// Resetuje casovac, nutne volat pred prvnim pouzitim funkce GetTime()
	virtual void ResetTime() = 0;

	// Vraci pocet sekund od predchoziho volani funkce
	virtual double GetTime() = 0;

	// pocet adapteru v systemu (hodnota potrebna k vytvoreni objektu RenderInterface::Device)
	virtual int GetAdaptersCount() const = 0;

	// Parametry grafickeho adapteru
	virtual bool GetAdapterInfo( const int adapterId, AdapterInfo& result ) const = 0;
};