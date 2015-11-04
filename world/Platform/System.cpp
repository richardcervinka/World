#include <Windows.h>
#include <VersionHelpers.h>
#include "System.h"

// staticke promenne singletonu System
long long System::performanceFrequency = 0;
long long System::performanceCounter = 0;

int DetectCPUCount() {
	SYSTEM_INFO si;
	GetSystemInfo( &si );
	return static_cast< int >( si.dwNumberOfProcessors );
}

CPUArchitecture DetectCPUArchitecture() {
	SYSTEM_INFO si;
	GetSystemInfo( &si );
	switch ( si.wProcessorArchitecture ) {
	case PROCESSOR_ARCHITECTURE_AMD64: return CPUArchitecture::X64;
	case PROCESSOR_ARCHITECTURE_INTEL: return CPUArchitecture::X86;
	case PROCESSOR_ARCHITECTURE_ARM:   return CPUArchitecture::ARM;
	}
	return CPUArchitecture::UNKNOWN;
}

long long DetectInstalledMemory() {
	ULONGLONG size = 0;
	GetPhysicallyInstalledSystemMemory( &size );
	return static_cast< long long >( size );
}

void System::Initialize() {
	// Podpora instrukcnich sad - inicializace statickych promennych
	AvailableSSE();
	AvailableSSE2();
	AvailableSSE3();

	GetProcessorsCount();
	GetCPUArchitecture();
	GetPhysicalMemory();

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

int System::GetProcessorsCount() {
	static const int count = DetectCPUCount();
	return count;
}

CPUArchitecture System::GetCPUArchitecture() {
	static const CPUArchitecture architecture = DetectCPUArchitecture();
	return architecture;
}

long long System::GetPhysicalMemory() {
	static const long long physicalMemory = DetectInstalledMemory();
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