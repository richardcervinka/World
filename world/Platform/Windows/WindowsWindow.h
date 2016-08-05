#pragma once

//#define UNICODE
//#define _UNICODE

#include <Windows.h>
#include "..\Window.h"

struct CreateWindowsWindowParams {
	// parametry WNDCLASSEX
	UINT		classStyle;
	HICON		hIcon;
	HCURSOR		hCursor;
	LPCWSTR		menuName;
	HICON		hIconSm;
	
	// parametry CreateWindowEx()
	LPCWSTR		windowName;
	DWORD		windowStyle;
	DWORD     	windowExStyle;
	int			x;
	int			y;
	int			width;
	int			height;
	HWND		hParent;
	HMENU		hMenu;
	HINSTANCE	hInstance;
};

class WindowsWindow : public Window {
public:
	WindowsWindow();
	~WindowsWindow();
	
	bool Create( const CreateWindowsWindowParams& params );

	bool CreateAppWindow( const HINSTANCE hInstance, const int clientWidth, const int clientHeight );

	HWND GetHandle() const noexcept;

	// implementace rozhrani Window
	virtual int GetWidth() const override;
	virtual int GetHeight() const override;
	virtual void Resize( const int width, const int height ) noexcept override;
	virtual int GetClientWidth() const override;
	virtual int GetClientHeight() const override;
	virtual void ResizeClient( const int width, const int height ) noexcept override;
	virtual void SetPosition( const int x, const int y ) override;
	virtual void Show() override;
	virtual void Hide() override;
	virtual void SetName( const String& str ) override;
	virtual const String GetName() const override;
	virtual void Update() override;
	virtual void SetBackgroundColor( const Color& color ) override;
	virtual const Color GetBackgroundColor() const override;
	
protected:
	// message handlers
	virtual void OnCreate();
	virtual void OnPaint();
	virtual void OnSize( const int width, const int height ) override;
	virtual void OnDestroy();
	
	// class member window procedure
	virtual LRESULT WndProc( const HWND hwnd, const UINT message, WPARAM wparam, LPARAM lparam );
	
private:
	// generic static window procedure
	static LRESULT CALLBACK WndProcStatic( HWND hwnd, UINT message, WPARAM wparam, LPARAM lparam );
	
private:
	HWND hwnd;
	int width;
	int height;
	int clientWidth;
	int clientHeight;
	Color backgroundColor;
	String name;
};