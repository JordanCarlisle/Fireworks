#include "App.h"
#include "imgui/imgui.h"
#include "imgui/imgui_impl_dx11.h"

ID3D11Device* d3d11Device;
ID3D11DeviceContext* d3d11DevCon;

App::App() : imgui_infoEnabled(true)
{
	
}

App& App::instance()
{
	static App s_instance;
	return s_instance;
}

void App::CreateRenderTarget()
{
	DXGI_SWAP_CHAIN_DESC sd;
	SwapChain->GetDesc(&sd);

	// Create the render target
	ID3D11Texture2D* pBackBuffer;
	D3D11_RENDER_TARGET_VIEW_DESC render_target_view_desc;
	ZeroMemory(&render_target_view_desc, sizeof(render_target_view_desc));
	render_target_view_desc.Format = sd.BufferDesc.Format;
	render_target_view_desc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
	SwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID*)&pBackBuffer);
	d3d11Device->CreateRenderTargetView(pBackBuffer, &render_target_view_desc, &renderTargetView);
	pBackBuffer->Release();

	//Describe our Depth/Stencil Buffer
	D3D11_TEXTURE2D_DESC depthStencilDesc;

	depthStencilDesc.Width = Width;
	depthStencilDesc.Height = Height;
	depthStencilDesc.MipLevels = 1;
	depthStencilDesc.ArraySize = 1;
	depthStencilDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	depthStencilDesc.SampleDesc.Count = 1;
	depthStencilDesc.SampleDesc.Quality = 0;
	depthStencilDesc.Usage = D3D11_USAGE_DEFAULT;
	depthStencilDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
	depthStencilDesc.CPUAccessFlags = 0;
	depthStencilDesc.MiscFlags = 0;

	//Create the Depth/Stencil View
	d3d11Device->CreateTexture2D(&depthStencilDesc, NULL, &depthStencilBuffer);
	d3d11Device->CreateDepthStencilView(depthStencilBuffer, NULL, &depthStencilView);

	//Set our Render Target
	d3d11DevCon->OMSetRenderTargets(1, &renderTargetView, depthStencilView);

	// Create the Viewport
	D3D11_VIEWPORT viewport;
	ZeroMemory(&viewport, sizeof(D3D11_VIEWPORT));

	viewport.TopLeftX = 0;
	viewport.TopLeftY = 0;
	viewport.Width = Width;
	viewport.Height = Height;
	viewport.MinDepth = 0.0f;
	viewport.MaxDepth = 1.0f;

	//Set the Viewport
	d3d11DevCon->RSSetViewports(1, &viewport);
}

void App::Resize(LPARAM lParam)
{
	ImGui_ImplDX11_InvalidateDeviceObjects();
	if (renderTargetView != NULL)
	{
		renderTargetView->Release();
		renderTargetView == NULL;
	}
	if (depthStencilBuffer != NULL)
	{
		depthStencilBuffer->Release();
		depthStencilBuffer = NULL;
	}
	Width = (UINT)LOWORD(lParam);
	Height = (UINT)HIWORD(lParam);
	SwapChain->ResizeBuffers(0, (UINT)LOWORD(lParam), (UINT)HIWORD(lParam), DXGI_FORMAT_UNKNOWN, 0);
	CreateRenderTarget();
	ImGui_ImplDX11_CreateDeviceObjects();
}

App::~App()
{
	ImGui_ImplDX11_Shutdown();
	SwapChain->Release();
	d3d11Device->Release();
	d3d11DevCon->Release();
	renderTargetView->Release();
	depthStencilBuffer->Release();
	depthStencilView->Release();
	rasterizerState->Release();
	if (scene != nullptr)
	{
		delete scene;
	}
	keyboardDevice->Unacquire();
	mouseDevice->Unacquire();
	directInput->Release();

	DSLessEqual->Release();
	RSCullNone->Release();
	Transparency->Release();
}

void App::Init(HWND outputWindow, HINSTANCE hInstance)
{
	hwnd = outputWindow;
	RECT rect;
	if (GetWindowRect(outputWindow, &rect))
	{
		Width = rect.right - rect.left;
		Height = rect.bottom - rect.top;
	}

	ClearColorRGBA[0] = 0.0f;// 0.57f;
	ClearColorRGBA[1] = 0.0f; //0.65f;
	ClearColorRGBA[2] = 0.0f; //0.72f;
	ClearColorRGBA[3] = 1.0f;

	DXGI_SWAP_CHAIN_DESC swapChainDesc;
	{
		ZeroMemory(&swapChainDesc, sizeof(DXGI_SWAP_CHAIN_DESC));
		swapChainDesc.BufferCount = 2;
		swapChainDesc.BufferDesc.Width = 0;
		swapChainDesc.BufferDesc.Height = 0;
		swapChainDesc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
		swapChainDesc.BufferDesc.RefreshRate.Numerator = 60;
		swapChainDesc.BufferDesc.RefreshRate.Denominator = 1;
		swapChainDesc.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;
		swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
		swapChainDesc.OutputWindow = outputWindow;
		swapChainDesc.SampleDesc.Count = 1;
		swapChainDesc.SampleDesc.Quality = 0;
		swapChainDesc.Windowed = TRUE;
		swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
	}

	//Create SwapChain
	D3D11CreateDeviceAndSwapChain(NULL, D3D_DRIVER_TYPE_HARDWARE, NULL, D3D11_CREATE_DEVICE_DEBUG, NULL, NULL,
		D3D11_SDK_VERSION, &swapChainDesc, &SwapChain, &d3d11Device, NULL, &d3d11DevCon);

	D3D11_RASTERIZER_DESC rasterizerDesc;
	ZeroMemory(&rasterizerDesc, sizeof(D3D11_RASTERIZER_DESC));
	rasterizerDesc.FillMode = D3D11_FILL_SOLID;
	rasterizerDesc.CullMode = D3D11_CULL_BACK;
	rasterizerDesc.FrontCounterClockwise = TRUE;
	d3d11Device->CreateRasterizerState(&rasterizerDesc, &rasterizerState);

	rasterizerDesc.CullMode = D3D11_CULL_NONE;
	d3d11Device->CreateRasterizerState(&rasterizerDesc, &RSCullNone);

	D3D11_DEPTH_STENCIL_DESC dssDesc;
	ZeroMemory(&dssDesc, sizeof(D3D11_DEPTH_STENCIL_DESC));
	dssDesc.DepthEnable = true;
	dssDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
	dssDesc.DepthFunc = D3D11_COMPARISON_LESS_EQUAL;

	d3d11Device->CreateDepthStencilState(&dssDesc, &DSLessEqual);

	D3D11_BLEND_DESC blendDesc;
	ZeroMemory(&blendDesc, sizeof(blendDesc));

	D3D11_RENDER_TARGET_BLEND_DESC rtbd;
	ZeroMemory(&rtbd, sizeof(rtbd));

	rtbd.BlendEnable = true;
	rtbd.SrcBlend = D3D11_BLEND_SRC_ALPHA;
	rtbd.DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
	rtbd.BlendOp = D3D11_BLEND_OP_ADD;
	rtbd.SrcBlendAlpha = D3D11_BLEND_ONE;
	rtbd.DestBlendAlpha = D3D11_BLEND_ZERO;
	rtbd.BlendOpAlpha = D3D11_BLEND_OP_ADD;
	rtbd.RenderTargetWriteMask = D3D10_COLOR_WRITE_ENABLE_ALL;

	blendDesc.AlphaToCoverageEnable = false;
	blendDesc.RenderTarget[0] = rtbd;

	d3d11Device->CreateBlendState(&blendDesc, &Transparency);

	d3d11DevCon->OMSetBlendState(Transparency, 0, 0xffffffff);

	CreateRenderTarget();

	//imgui stuff
	ImGui_ImplDX11_Init(outputWindow, d3d11Device, d3d11DevCon);

	// Create the Viewport
	D3D11_VIEWPORT viewport;
	ZeroMemory(&viewport, sizeof(D3D11_VIEWPORT));

	viewport.TopLeftX = 0;
	viewport.TopLeftY = 0;
	viewport.Width = Width;
	viewport.Height = Height;
	viewport.MinDepth = 0.0f;
	viewport.MaxDepth = 1.0f;

	//Set the Viewport
	d3d11DevCon->RSSetViewports(1, &viewport);

	d3d11DevCon->RSSetState(rasterizerState);

	//direct input stuff
	auto r = DirectInput8Create(hInstance, DIRECTINPUT_VERSION, IID_IDirectInput8, (void**)&directInput, NULL);
	r = directInput->CreateDevice(GUID_SysKeyboard, &keyboardDevice, NULL);
	r = directInput->CreateDevice(GUID_SysMouse, &mouseDevice, NULL);

	r = keyboardDevice->SetDataFormat(&c_dfDIKeyboard);
	r = keyboardDevice->SetCooperativeLevel(outputWindow, DISCL_FOREGROUND | DISCL_NONEXCLUSIVE);

	r = mouseDevice->SetDataFormat(&c_dfDIMouse);
	r = mouseDevice->SetCooperativeLevel(outputWindow, DISCL_NONEXCLUSIVE | DISCL_FOREGROUND);
	
	ZeroMemory(&input, sizeof(input_map));

	
	resetButton = false;

	scene = new Scene();
	scene->Init();

	//timing stuff
	fps = 0;
	dt = 0.0f;

	LARGE_INTEGER pf;
	QueryPerformanceFrequency(&pf);
	performaceFreq = pf.QuadPart;
	QueryPerformanceCounter(&lastCounter);
	QueryPerformanceCounter(&sCounter);
	QueryPerformanceCounter(&tCounter);
	lastCycleCounter = __rdtsc();
}

void App::Update()
{

	keyboardDevice->Acquire();
	mouseDevice->Acquire();

	mouseDevice->GetDeviceState(sizeof(DIMOUSESTATE), &mouseState);

	keyboardDevice->GetDeviceState(sizeof(keyboardState), (LPVOID)&keyboardState);

	input.w = keyboardState[DIK_W] & 0x80;
	input.a = keyboardState[DIK_A] & 0x80;
	input.s = keyboardState[DIK_S] & 0x80;
	input.d = keyboardState[DIK_D] & 0x80;
	input.cuu = keyboardState[DIK_UP] & 0x80;
	input.cud = keyboardState[DIK_DOWN] & 0x80;
	input.cul = keyboardState[DIK_LEFT] & 0x80;
	input.cur = keyboardState[DIK_RIGHT] & 0x80;
	input.shift = keyboardState[DIK_LSHIFT] & 0x80;
	input.alt = keyboardState[DIK_LALT] & 0x80;
	input.del = keyboardState[DIK_DELETE] & 0x80;
	input.axis[0] = mouseState.lX;
	input.axis[1] = mouseState.lY;
	input.lmb = mouseState.rgbButtons[0] & 0x80;
	input.rmb = mouseState.rgbButtons[1] & 0x80;

	//picking stuff
	POINT mousePos;
	GetCursorPos(&mousePos);
	ScreenToClient(hwnd, &mousePos);

	input.cursor[0] = 2.f * (float)mousePos.x / (float)Width - 1.0f;
	input.cursor[1] = 1.0f - 2.f * (float)mousePos.y / (float)Height;

	input.Width = Width;
	input.Height = Height;

	ImGui_ImplDX11_NewFrame();
	

	scene->Update(dt, input);


	/* imgui panel */
	ImGui::Begin("Info", &imgui_infoEnabled);

	ImGui::Text("Ms/f: %.6f", msElapsed);
	ImGui::Text("Frames/s: %.2f", fps);
	ImGui::Text("cycles/f: %d", cyclesElapsed);
	ImGui::Text("Window: %d x %d", Width, Height);
	auto resetClicked = ImGui::Button("Reset", ImVec2(80.0f, 30.0f));
	if (resetClicked)
	{
		input.sCycle = 0;
	}
	ImGui::End();
	/* end imgui panel */

	d3d11DevCon->ClearRenderTargetView(renderTargetView, ClearColorRGBA);
	d3d11DevCon->ClearDepthStencilView(depthStencilView, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);

	//update and draw scene
	scene->Render();

	ImGui::Render();

	//Present the backbuffer to the screen
	SwapChain->Present(0, 0);

	/* performance measurement */
	endCycleCounter = __rdtsc();
	QueryPerformanceCounter(&endCounter);

	uint64_t elapsed = endCounter.QuadPart - lastCounter.QuadPart;

	float time = (float)(endCounter.QuadPart - sCounter.QuadPart) / (float)performaceFreq;
	float tTime = (float)(endCounter.QuadPart - tCounter.QuadPart) / (float)performaceFreq;
	dt = (float)elapsed / (float)performaceFreq;
	if (time > 0.06f)
	{
		msElapsed = 1000.0f * dt;

		fps = (float)performaceFreq / (float)elapsed;
		cyclesElapsed = endCycleCounter - lastCycleCounter;

		sCounter = endCounter;
	}

	if (tTime > .2f)
	{
		input.sCycle++;
		tCounter = endCounter;
	}

	lastCycleCounter = endCycleCounter;
	lastCounter = endCounter;
	/* end performance measurement */
}
