#include "..\System.h"

class WindowsSystem: public System {
public:
	WindowsSystem();

	// Implementace rozhrani
	virtual void EnumerateCapabilities() override;
	virtual bool CompatibleOS( const OSVersion version ) const override;
	virtual int GetProcessorsCount() const override;
	virtual CPUArchitecture GetCPUArchitecture() const override;
	virtual long long GetPhysicalMemory() const override;
	virtual bool SSE() const override;
	virtual bool SSE2() const override;
	virtual bool SSE3() const override;
	virtual void ResetTime() override;
	virtual double GetTime() override;
	virtual int GetAdaptersCount() const override;
	virtual bool GetAdapterInfo( const int adapterId, AdapterInfo& result ) const override;

private:
	long long performanceFrequency;
	long long performanceCounter;
};