#pragma once

enum class OSVersion {
	UNKNOWN = 0,
	XP,
	XP_SP1,
	XP_SP2,
	XP_SP3,
	VISTA,
	VISTA_SP1,
	VISTA_SP2,
	WIN7,
	WIN7_SP1,
	WIN8
};

enum class ProcessorArchitecture {
	UNKNOWN = 0,
	X64,
	X86,
	ARM
};

// Singleton, reprezentuje operacni system a hardware.

class System {
public:
	// neni povoleno vytvaret instance
	System() = delete;

	// Inicializace, system musi byt inicializovan pred volanim ostatnich metod.
	static void Initialize();

	// Pristupove thread-safe funkce atributu systemu
	static bool OSSupport( const OSVersion version );
	static int ProcessorsCount();
	static long long PhysicalMemory();
	static bool AvailableSSE();
	static bool AvailableSSE2();
	static bool AvailableSSE3();

	// Resetuje casovac, nutne volat pred prvnim pouzitim funkce GetTime()
	static void ResetTime();

	// Vraci pocet sekund od predchoziho volani
	static double GetTime();

private:
	static ProcessorArchitecture processorArchitecture;
	static int processorsCount;
	static long long physicalMemory;
	static bool bSSE, bSSE2, bSSE3;
	static long long performanceFrequency;
	static long long performanceCounter;
};