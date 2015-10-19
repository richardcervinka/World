#include <mutex>
#include "Application.h"

std::mutex mutex; 

Application *Application::app = nullptr;

Application::Application() {}

Application::~Application() {
	if ( app == this ) {
		app = nullptr;
	}
}

void Application::Run() {
	std::unique_lock< std::mutex > mutexul( mutex, std::defer_lock );
	
	mutexul.lock();
	if ( app != nullptr ) {
		return;
	}
	app = this;
	mutexul.unlock();
	
	// start message loop
	while ( ProcessPlatformMessages() ) {
		//Update();
	}
	// navrat z message loop, standardni ukonceni aplikace
	mutexul.lock();
	app = nullptr;
	mutexul.unlock();
}
/*
void Application::Update() {
	// overwrite
}
*/
void Application::Exit() {
	std::lock_guard< std::mutex > mutexlg( mutex );
	if ( app != nullptr ) {
		app->ExitApp();
		app = nullptr;
	}
}

void Application::Abort() {
	std::lock_guard< std::mutex > mutexlg( mutex );
	if ( app != nullptr ) {
		app->AbortApp();
	}
}

void Application::Abort( const String &errorMessage ) {
	std::lock_guard< std::mutex > mutexlg( mutex );
	if ( app != nullptr ) {
		app->AbortApp( errorMessage );
	}
}