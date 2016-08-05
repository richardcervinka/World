#include <mutex>
#include "Application.h"

std::mutex mutex; 

// the app
Application* app = nullptr;

Application::~Application() {
	if ( app == this ) {
		app = nullptr;
	}
}

void Application::Run() {
	std::unique_lock< std::mutex > lock( mutex, std::defer_lock );
	
	// set global app pointer
	lock.lock();
	if ( app != nullptr ) {
		return;
	}
	app = this;
	lock.unlock();
	
	// start message loop
	while ( ProcessMessages() ) {
		Update();
	}
	// navrat z message loop, standardni ukonceni aplikace
	lock.lock();
	app = nullptr;
	lock.unlock();
}

void Application::Update() noexcept {
	// render
}

void Application::Exit() {
	std::lock_guard< std::mutex > lock( mutex );
	if ( app != nullptr ) {
		app->ExitApp();
		app = nullptr;
	}
}

void Application::Abort() {
	std::lock_guard< std::mutex > lock( mutex );
	if ( app != nullptr ) {
		app->AbortApp();
		app = nullptr;
	}
}

void Application::Abort( const String& errorMessage ) {
	std::lock_guard< std::mutex > mutexlg( mutex );
	if ( app != nullptr ) {
		app->AbortApp( errorMessage );
		app = nullptr;
	}
}