//==================================================================================================================
//
// Display a single triangle.
//
//==================================================================================================================



#include <windows.h>
#include "imgui/imgui.h"
#include "imgui/imgui_impl_dx11.h"
#include <dinput.h>

#include "App.h"

//Window handle
static HWND hwnd = NULL;

//Default window dimensions
static const int kDefaultWidth = 1024;
static const int kDefaultHeight = 768;

//Pointer to the initialised d3d11 device
extern ID3D11Device* d3d11Device;

//Prototypes
bool InitializeWindow(HINSTANCE hInstance, int ShowWnd, int width, int height, bool windowed);
int MainLoop(HINSTANCE hInstance);
LRESULT CALLBACK WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
extern LRESULT ImGui_ImplDX11_WndProcHandler(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);



int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nShowCmd)
{
	InitializeWindow(hInstance, nShowCmd, kDefaultWidth, kDefaultHeight, true);
	MainLoop(hInstance);

	return 0;
}



bool InitializeWindow(HINSTANCE hInstance, int ShowWnd, int width, int height, bool windowed)
{
	//Create a new extended windows class
	WNDCLASSEX wc;	
	wc.cbSize = sizeof(WNDCLASSEX);
	wc.style = CS_HREDRAW | CS_VREDRAW;
	wc.lpfnWndProc = WndProc;
	wc.cbClsExtra = NULL;
	wc.cbWndExtra = NULL;
	wc.hInstance = hInstance;
	wc.hIcon = LoadIcon(NULL, IDI_WINLOGO);
	wc.hCursor = LoadCursor(NULL, IDC_ARROW);
	wc.hbrBackground = (HBRUSH)(COLOR_WINDOW + 2);
	wc.lpszMenuName = NULL;
	wc.lpszClassName = L"window";
	wc.hIconSm = LoadIcon(NULL, IDI_WINLOGO);

	RegisterClassEx(&wc);

	//Create the window
	hwnd = CreateWindowEx(
		NULL,
		L"window",
		L"Fireworks Demo",
		WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT, CW_USEDEFAULT,
		width,
		height,
		NULL,
		NULL,
		hInstance,
		NULL
	);

	ShowWindow(hwnd, ShowWnd);
	UpdateWindow(hwnd);

	return true;
}


LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	//ImGui message handler
	if (ImGui_ImplDX11_WndProcHandler(hwnd, msg, wParam, lParam))
		return true;

	switch (msg)
	{
	case WM_SIZE:
	//Window resized event
		if (d3d11Device != NULL && wParam != SIZE_MINIMIZED)
		{
			App::instance().Resize(lParam);
		}
		return 0;

	//Keydown event
	case WM_KEYDOWN:
						// if escape key was pressed, display popup box
		if (wParam == VK_ESCAPE)
		{
			DestroyWindow(hwnd);
		}
		return 0;

	//Window closed event
	case WM_DESTROY:
		PostQuitMessage(0);

		return 0;
	}

	//Pass other messages back to Windows
	return DefWindowProc(hwnd, msg, wParam, lParam);
}



int MainLoop(HINSTANCE hInstance)
{
	MSG msg;
	ZeroMemory(&msg, sizeof(MSG));

	//Initialise application instance
	App& application = App::instance();
	application.Init(hwnd, hInstance);


	while (true)
	{
		//if there was a windows message
		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			if (msg.message == WM_QUIT)
			{
				break;
			}

			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}

		else
		{
			//Update the application
			application.Update();
		}
	}

	return (int)msg.wParam;		//return the message
}
