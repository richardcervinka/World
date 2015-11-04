#include <Windows.h>
#include <VersionHelpers.h>
#include "System.h"

// staticke promenne singletonu System
long long System::performanceFrequency = 0;
long long System::performanceCounter = 0;
long long System::physicalMemory = 0;

int GetCPUCount() {
	SYSTEM_INFO si;
	GetSystemInfo( &si );
	return static_cast< int >( si.dwNumberOfProcessors );
}

ProcessorArchitecture GetCPUArchitecture() {
	SYSTEM_INFO si;
	GetSystemInfo( &si );
	switch ( si.wProcessorArchitecture ) {
	case PROCESSOR_ARCHITECTURE_AMD64: return ProcessorArchitecture::X64;
	case PROCESSOR_ARCHITECTURE_INTEL: return ProcessorArchitecture::X86;
	case PROCESSOR_ARCHITECTURE_ARM:   return ProcessorArchitecture::ARM;
	}
	return ProcessorArchitecture::UNKNOWN;
}

void System::Initialize() {
	// Podpora instrukcnich sad - inicializace statickych promennych
	AvailableSSE();
	AvailableSSE2();
	AvailableSSE3();

	ProcessorsCount();
	GetProcessorArchitecture();

	// Velikost dostupne systemove pameti
	ULONGLONG memSize = 0;
	GetPhysicallyInstalledSystemMemory( &memSize );
	physicalMemory = memSize;

	// frekvence casovace
	LARGE_INTEGER frequency;
	QueryPerformanceFrequency( &frequency );
	performanceFrequency = static_cast< long long >( frequency.QuadPart );
	ResetTime();
}

bool System::OSSupport( const OSVersion version ) {
	switch ( version ) {
	case OSVersion::XP:			return IsWindowsXPOrGreater();
	case OSVersion::XP_SP1:		return IsWindowsXPSP1OrGreater();
	case OSVersion::XP_SP2:		return IsWindowsXPSP2OrGreater();
	case OSVersion::XP_SP3:		return IsWindowsXPSP3OrGreater();
	case OSVersion::VISTA:		return IsWindowsVistaOrGreater();
	case OSVersion::VISTA_SP1:	return IsWindowsVistaSP1OrGreater();
	case OSVersion::VISTA_SP2:	return IsWindowsVistaSP2OrGreater();
	case OSVersion::WIN7:		return IsWindows7OrGreater();
	case OSVersion::WIN7_SP1:	return IsWindows7SP1OrGreater();
	case OSVersion::WIN8:		return IsWindows8OrGreater();
	}
	return false;
}

int System::ProcessorsCount() {
	static const int count = GetCPUCount();
	return count;
}

static ProcessorArchitecture System::GetProcessorArchitecture() {
	static const ProcessorArchitecture architecture = GetCPUArchitecture();
	return architecture;
}

long long System::PhysicalMemory() {
	return physicalMemory;
}

bool System::AvailableSSE() {
	static const bool bSSE = IsProcessorFeaturePresent( PF_XMMI_INSTRUCTIONS_AVAILABLE ) != 0;
	return bSSE;
}

bool System::AvailableSSE2() {
	static const bool bSSE2 = IsProcessorFeaturePresent( PF_XMMI64_INSTRUCTIONS_AVAILABLE ) != 0;
	return bSSE2;
}

bool System::AvailableSSE3() {
	static const bool bSSE3 = IsProcessorFeaturePresent( PF_SSE3_INSTRUCTIONS_AVAILABLE ) != 0;
	return bSSE3;
}

void System::ResetTime() {
	LARGE_INTEGER pc;
	QueryPerformanceCounter( &pc );
	performanceCounter = pc.QuadPart;
}

double System::GetTime() {
	LARGE_INTEGER pc;
	QueryPerformanceCounter( &pc );
	LONGLONG countDelta = pc.QuadPart - performanceFrequency;
	performanceCounter = pc.QuadPart;
	return double( double( countDelta ) / double( performanceFrequency ) );
}