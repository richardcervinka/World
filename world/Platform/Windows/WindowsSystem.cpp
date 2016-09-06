#include <Windows.h>
#include <VersionHelpers.h>
#include <dxgi1_2.h>
#include <cstdint>
#include "WindowsSystem.h"
#include "Framework/String.h"

// WindowsSystem

WindowsSystem::WindowsSystem() {
	performanceFrequency = 0;
	performanceCounter = 0;
}

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

long long EnumerateInstalledMemory() {
	ULONGLONG size = 0;
	GetPhysicallyInstalledSystemMemory( &size );
	return static_cast< long long >( size );
}

int EnumerateAdaptersCount() {
	HRESULT hresult = 0;

	IDXGIFactory1* factory = nullptr;
	hresult = CreateDXGIFactory1( __uuidof( IDXGIFactory1 ), reinterpret_cast< void** >( &factory ) );
	if ( FAILED( hresult ) ) {
		return 0;
	}
	int count = 0;
	IDXGIAdapter* adapter = nullptr;
	while( factory->EnumAdapters( count, &adapter ) != DXGI_ERROR_NOT_FOUND ) {
		adapter->Release();
		count++; 
	}
	factory->Release();
	return count;
}

void WindowsSystem::EnumerateCapabilities() {
	// inicializace statickych lokaknich promennych
	SSE();
	SSE2();
	SSE3();
	GetProcessorsCount();
	GetCPUArchitecture();
	GetPhysicalMemory();
	GetAdaptersCount();

	// casovac
	LARGE_INTEGER frequency;
	QueryPerformanceFrequency( &frequency );
	performanceFrequency = static_cast< long long >( frequency.QuadPart );
	ResetTime();
}

bool WindowsSystem::CompatibleOS( const OSVersion version ) const {
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

int WindowsSystem::GetProcessorsCount() const {
	static const int count = DetectCPUCount();
	return count;
}

CPUArchitecture WindowsSystem::GetCPUArchitecture() const {
	static const CPUArchitecture architecture = DetectCPUArchitecture();
	return architecture;
}

long long WindowsSystem::GetPhysicalMemory() const {
	static const long long physicalMemory = EnumerateInstalledMemory();
	return physicalMemory;
}

bool WindowsSystem::SSE() const {
	static const bool sse = IsProcessorFeaturePresent( PF_XMMI_INSTRUCTIONS_AVAILABLE ) != 0;
	return sse;
}

bool WindowsSystem::SSE2() const {
	static const bool sse2 = IsProcessorFeaturePresent( PF_XMMI64_INSTRUCTIONS_AVAILABLE ) != 0;
	return sse2;
}

bool WindowsSystem::SSE3() const {
	static const bool sse3 = IsProcessorFeaturePresent( PF_SSE3_INSTRUCTIONS_AVAILABLE ) != 0;
	return sse3;
}

void WindowsSystem::ResetTime() {
	LARGE_INTEGER pc;
	QueryPerformanceCounter( &pc );
	performanceCounter = pc.QuadPart;
}

double WindowsSystem::GetTime() {
	LARGE_INTEGER pc;
	QueryPerformanceCounter( &pc );
	LONGLONG countDelta = pc.QuadPart - performanceFrequency;
	performanceCounter = pc.QuadPart;
	return double( double( countDelta ) / double( performanceFrequency ) );
}

int WindowsSystem::GetAdaptersCount() const {
	static const int count = EnumerateAdaptersCount();
	return count;
}

bool WindowsSystem::GetAdapterInfo( const int adapterId, AdapterInfo& result ) const {
	HRESULT hresult = 0;

	IDXGIFactory1* factory = nullptr;
	hresult = CreateDXGIFactory1( __uuidof( IDXGIFactory1 ), reinterpret_cast< void** >( &factory ) );
	if ( FAILED( hresult ) ) {
		return false;
	}
	IDXGIAdapter* adapter = nullptr;
	hresult = factory->EnumAdapters( static_cast< UINT >( adapterId ), &adapter );
	if ( FAILED( hresult ) ) {
		factory->Release();
		return false;
	}
	DXGI_ADAPTER_DESC adapterDesc;
	hresult = adapter->GetDesc( &adapterDesc );
	if ( FAILED( hresult ) ) {
		adapter->Release();
		factory->Release();
		return false;
	}
	// enum outputs count
	int outputsCount = 0;
	IDXGIOutput* output = nullptr;
	while( adapter->EnumOutputs( outputsCount, &output ) != DXGI_ERROR_NOT_FOUND ) {
		output->Release();
		outputsCount += 1;
	}
	adapter->Release();
	factory->Release();

	// ulozit vysledek
	WcharToChar16(
		static_cast< wchar_t* >( adapterDesc.Description ),
		result.description,
		sizeof( adapterDesc.Description ) / sizeof( adapterDesc.Description[ 0 ] )
	);
	result.description[ 127 ] = u'\0';
	result.id = adapterId;
	result.outputsCount = outputsCount;
	result.vendorId = static_cast< uint32_t >( adapterDesc.VendorId );
	result.dedicatedVideoMemory = static_cast< uint64_t >( adapterDesc.DedicatedVideoMemory );
	result.dedicatedSystemMemory = static_cast< uint64_t >( adapterDesc.DedicatedSystemMemory );
	result.sharedSystemMemory = static_cast< uint64_t >( adapterDesc.SharedSystemMemory );

	return true;
}