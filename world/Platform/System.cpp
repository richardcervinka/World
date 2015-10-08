#include <Windows.h>
#include <VersionHelpers.h>
#include "System.h"

// staticke promenne singletonu System
long long System::performanceFrequency = 0;
long long System::performanceCounter = 0;
ProcessorArchitecture System::processorArchitecture = ProcessorArchitecture::UNKNOWN;
int System::processorsCount = 1;
bool System::bSSE = false;
bool System::bSSE2 = false;
bool System::bSSE3 = false;
long long System::physicalMemory = 0;

void System::Initialize() {
	// podpora cpu
	bSSE = IsProcessorFeaturePresent( PF_XMMI_INSTRUCTIONS_AVAILABLE ) != 0;
	bSSE2 = IsProcessorFeaturePresent( PF_XMMI64_INSTRUCTIONS_AVAILABLE ) != 0;
	bSSE3 = IsProcessorFeaturePresent( PF_SSE3_INSTRUCTIONS_AVAILABLE ) != 0;

	// architektura cpu
	SYSTEM_INFO si;
	GetSystemInfo( &si );
	switch ( si.wProcessorArchitecture ) {
	case PROCESSOR_ARCHITECTURE_AMD64: processorArchitecture = ProcessorArchitecture::X64; break;
	case PROCESSOR_ARCHITECTURE_INTEL: processorArchitecture = ProcessorArchitecture::X86; break;
	case PROCESSOR_ARCHITECTURE_ARM:   processorArchitecture = ProcessorArchitecture::ARM; break;
	}
	processorsCount = static_cast< int >( si.dwNumberOfProcessors );

	// Velikost nainstalovane pameti
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
	return processorsCount;
}

long long System::PhysicalMemory() {
	return physicalMemory;
}

bool System::AvailableSSE() {
	return bSSE;
}

bool System::AvailableSSE2() {
	return bSSE2;
}

bool System::AvailableSSE3() {
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