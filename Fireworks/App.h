#pragma once

#include <Windows.h>
#include <d3d11.h>
#include <d3dx11.h>
#include <d3dx10.h>
#include <dinput.h>
#include "Scene.h"
//#include <DirectXMath.h>



class App
{
	HWND hwnd;

	UINT Width;
	UINT Height;

	FLOAT ClearColorRGBA[4];

	IDXGISwapChain			*SwapChain;
	ID3D11RenderTargetView	*renderTargetView;
	ID3D11DepthStencilView* depthStencilView;
	ID3D11Texture2D* depthStencilBuffer;
	ID3D11RasterizerState* rasterizerState;

	ID3D11BlendState* Transparency;
	ID3D11DepthStencilState* DSLessEqual;
	ID3D11RasterizerState* RSCullNone;

	Scene* scene;

	LARGE_INTEGER lastCounter;
	LARGE_INTEGER sCounter;
	LARGE_INTEGER tCounter;
	LARGE_INTEGER endCounter;
	uint64_t lastCycleCounter;
	uint64_t endCycleCounter;
	uint64_t cyclesElapsed;
	uint64_t performaceFreq;
	float msElapsed;
	float fps;
	float dt;

	IDirectInput8* directInput;
	IDirectInputDevice8* keyboardDevice;
	IDirectInputDevice8* mouseDevice;

	DIMOUSESTATE mouseState;
	BYTE keyboardState[256];

	input_map input;



	//imgui stuff
	bool imgui_infoEnabled;
	bool resetButton;

	App();
	~App();

	void CreateRenderTarget();

public:
	static App& instance();
	
	void Resize(LPARAM lParam);

	void Init(HWND outputWindow, HINSTANCE hInstance);
	void Update();
	ID3D11DepthStencilState* GetLessEqualState() { return DSLessEqual; }
	ID3D11RasterizerState* GetCullNone() { return RSCullNone; }
	ID3D11RasterizerState* GetCull() { return rasterizerState;  }
};