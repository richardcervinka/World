#include "RenderInterface.h"
#include "DX11\DX11RenderInterface.h"

Device *CreateDX11Device( const CreateDX11DeviceParams &params ) {
	DX11Device *device = new DX11Device();
	if ( device == nullptr ) {
		return nullptr;
	}
	if ( !device->Create( params ) ) {
		delete device;
		return nullptr;
	}
	return device;
}

// RenderInterfaceObject 

RenderInterfaceObject::RenderInterfaceObject() {
	references = 1;
}

RenderInterfaceObject::~RenderInterfaceObject() {}

void RenderInterfaceObject::Release() {
	references -= 1;
	if ( references <= 0 ) {
		delete this;
	}
}

void RenderInterfaceObject::AddRef() {
	references += 1;
}
