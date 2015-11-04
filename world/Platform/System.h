#pragma once

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

/*
Hardware (krome adapteru) a operacni system
*/
class System {
public:
	// neni povoleno vytvaret instance
	System() = delete;

	// Inicializace, musi byt provedeno pred volanim ostatnich metod.
	static void Initialize();

	// Pristupove thread-safe funkce

	// vraci true, pokud je operacni system kompatibilni s parametrem version
	static bool OSSupport( const OSVersion version );

	// pocet logickych procesoru
	static int GetProcessorsCount();

	// architektura procesoru
	static CPUArchitecture GetCPUArchitecture();

	// pocet bajtu nainstalovane pameti RAM
	static long long GetPhysicalMemory();

	// dostupnost instrukcni sady SSE
	static bool AvailableSSE();
	static bool AvailableSSE2();
	static bool AvailableSSE3();

	// Resetuje casovac, nutne volat pred prvnim pouzitim funkce GetTime()
	static void ResetTime();

	// Vraci pocet sekund od predchoziho volani
	static double GetTime();

private:
	//static ProcessorArchitecture processorArchitecture;
	static long long performanceFrequency;
	static long long performanceCounter;
};