#include <mutex>
//#include "Window.h"
#include "Application.h"

std::mutex mutex; 

Application *Application::app = nullptr;

Application::Application() {}

Application::~Application() {}

void Application::Run() {
	if ( app != nullptr ) {
		return;
	}
	app = this;
	
	// start message loop
	while ( ProcessPlatformMessages() ) {
		Update();
	}
	// navrat z message loop, standardni ukonceni aplikace
	app = nullptr;
}

void Application::Update() {
	// overwrite
}

void Application::Exit() {
	mutex.lock();
	if ( app != nullptr ) {
		app->ExitApp();
	}
	mutex.unlock();
}

void Application::Abort() {
	mutex.lock();
	if ( app != nullptr ) {
		app->AbortApp();
	}
	mutex.unlock();
}

void Application::Abort( const String &errorMessage ) {
	mutex.lock();
	if ( app != nullptr ) {
		app->AbortApp( errorMessage );
	}
	mutex.unlock();
}