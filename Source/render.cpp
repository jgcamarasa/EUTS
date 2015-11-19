#include "render.h"
#include "imgui\imgui.h"
#include "imgui_impl_dx11.h"

extern LRESULT ImGui_ImplDX11_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	PAINTSTRUCT ps;
	HDC hdc;

	if (ImGui_ImplDX11_WndProcHandler(hWnd, message, wParam, lParam))
		return true;

	switch (message)
	{
	case WM_PAINT:
		hdc = BeginPaint(hWnd, &ps);
		EndPaint(hWnd, &ps);
		break;

	case WM_DESTROY:
		PostQuitMessage(0);
		break;

		// Note that this tutorial does not handle resizing (WM_SIZE) requests,
		// so we created the window without the resize border.

	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}

	return 0;
}


void initWindow(EUTS_Window *window)
{
	WNDCLASSEX wc;
	DEVMODE dmScreenSettings;
	int posX, posY;

	// Get the instance of this application.
	window->instance = GetModuleHandle(NULL);

	// Give the application a name.
	window->title = L"Everyone Under The Sun";

	// Setup the windows class with default settings.
	wc.style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
	wc.lpfnWndProc = WndProc;
	wc.cbClsExtra = 0;
	wc.cbWndExtra = 0;
	wc.hInstance = window->instance;
	wc.hIcon = LoadIcon(NULL, IDI_WINLOGO);
	wc.hIconSm = wc.hIcon;
	wc.hCursor = LoadCursor(NULL, IDC_ARROW);
	wc.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);
	wc.lpszMenuName = NULL;
	wc.lpszClassName = window->title;
	wc.cbSize = sizeof(WNDCLASSEX);

	// Register the window class.
	RegisterClassEx(&wc);

	// Determine the resolution of the clients desktop screen.
	int screenWidth = GetSystemMetrics(SM_CXSCREEN);
	int screenHeight = GetSystemMetrics(SM_CYSCREEN);

	// Setup the screen settings depending on whether it is running in full screen or in windowed mode.

	DWORD style = WS_CLIPSIBLINGS | WS_CLIPCHILDREN | WS_CAPTION | WS_SYSMENU; // Default (non full screen) style
	if (FULLSCREEN)
	{
		// If full screen set the screen to maximum size of the users desktop and 32bit.
		memset(&dmScreenSettings, 0, sizeof(dmScreenSettings));
		dmScreenSettings.dmSize = sizeof(dmScreenSettings);
		dmScreenSettings.dmPelsWidth = (unsigned long)screenWidth;
		dmScreenSettings.dmPelsHeight = (unsigned long)screenHeight;
		dmScreenSettings.dmBitsPerPel = 32;
		dmScreenSettings.dmFields = DM_BITSPERPEL | DM_PELSWIDTH | DM_PELSHEIGHT;

		// Change the display settings to full screen.
		ChangeDisplaySettings(&dmScreenSettings, CDS_FULLSCREEN);

		// Set the position of the window to the top left corner.
		posX = posY = 0;

		style = WS_CLIPSIBLINGS | WS_CLIPCHILDREN | WS_CAPTION | WS_SYSMENU | WS_POPUP;

		window->width = screenWidth;
		window->height = screenHeight;
	}
	else
	{
		// If windowed then set it to 1280x768 resolution.
		screenWidth = SCREEN_WIDTH;
		screenHeight = SCREEN_HEIGHT;

		RECT rect;
		rect.top = 0;
		rect.left = 0;
		rect.right = screenWidth;
		rect.bottom = screenHeight;
		AdjustWindowRectEx(&rect, style, 0, 0);

		window->width = screenWidth;
		window->height = screenHeight;

		screenWidth = rect.right - rect.left;
		screenHeight = rect.bottom - rect.top;

		// Place the window in the middle of the screen.
		posX = (GetSystemMetrics(SM_CXSCREEN) - screenWidth) / 2;
		posY = (GetSystemMetrics(SM_CYSCREEN) - screenHeight) / 2;
	}

	
	window->fullScreen = FULLSCREEN;

	// Create the window with the screen settings and get the handle to it.
	window->handler = CreateWindowEx(WS_EX_APPWINDOW, window->title, window->title,
		style,
		posX, posY, screenWidth, screenHeight, NULL, NULL, window->instance, NULL);

	// Bring the window up on the screen and set it as main focus.
	ShowWindow(window->handler, SW_SHOW);
	SetForegroundWindow(window->handler);
	SetFocus(window->handler);

	// Hide the mouse cursor.
	ShowCursor(true);

	return;
}

void initD3D11(EUTS_Window *window, EUTS_RenderState *renderState)
{
	HRESULT result;
	IDXGIFactory* factory;
	IDXGIAdapter* adapter;
	IDXGIOutput* adapterOutput;
	UINT numModes, numerator, denominator;
	DXGI_MODE_DESC* displayModeList;
	DXGI_ADAPTER_DESC adapterDesc;
	DXGI_SWAP_CHAIN_DESC swapChainDesc;
	ID3D11Texture2D* backBufferPtr;
	D3D11_TEXTURE2D_DESC depthBufferDesc;
	D3D11_DEPTH_STENCIL_DESC depthStencilDesc;
	D3D11_DEPTH_STENCIL_VIEW_DESC depthStencilViewDesc;
	D3D11_RASTERIZER_DESC rasterDesc;
	D3D11_VIEWPORT viewport;
	float fieldOfView, screenAspect;

	// Get video card info (refresh rate, video memory, video card description)
	result = CreateDXGIFactory(__uuidof(IDXGIFactory), (void**)&factory);
	assert(!FAILED(result));

	result = factory->EnumAdapters(0, &adapter); // Enum videocard
	assert(!FAILED(result));

	result = adapter->EnumOutputs(0, &adapterOutput);
	assert(!FAILED(result));

	result = adapterOutput->GetDisplayModeList(DXGI_FORMAT_R8G8B8A8_UNORM, DXGI_ENUM_MODES_INTERLACED, &numModes, NULL);
	assert(!FAILED(result));

	displayModeList = new DXGI_MODE_DESC[numModes];
	result = adapterOutput->GetDisplayModeList(DXGI_FORMAT_R8G8B8A8_UNORM, DXGI_ENUM_MODES_INTERLACED, &numModes, displayModeList);
	assert(!FAILED(result));

	for (unsigned int i = 0; i < numModes; i++)
	{
		if (displayModeList[i].Width == window->width)
		{
			if (displayModeList[i].Height == window->height)
			{
				numerator = displayModeList[i].RefreshRate.Numerator;
				denominator = displayModeList[i].RefreshRate.Denominator;
			}
		}
	}

	result = adapter->GetDesc(&adapterDesc);
	assert(!FAILED(result));

	renderState->videoMemoryInMB = adapterDesc.DedicatedVideoMemory / 1024.0f / 1024.0f;

	size_t stringLength;
	wcstombs_s(&stringLength, renderState->videoCardDescription, 128, adapterDesc.Description, 128);

	delete[] displayModeList;
	adapterOutput->Release();
	adapter->Release();
	factory->Release();

	// Create swapchain
	ZeroMemory(&swapChainDesc, sizeof(swapChainDesc));
	swapChainDesc.BufferCount = 1; // ?
	swapChainDesc.BufferDesc.Width = window->width;
	swapChainDesc.BufferDesc.Height = window->height;

	swapChainDesc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;

	if (VSYNC)
	{
		swapChainDesc.BufferDesc.RefreshRate.Numerator = numerator;
		swapChainDesc.BufferDesc.RefreshRate.Denominator = denominator;
	}
	else
	{
		swapChainDesc.BufferDesc.RefreshRate.Numerator = 0;
		swapChainDesc.BufferDesc.RefreshRate.Denominator = 1;
	}

	swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;

	swapChainDesc.OutputWindow = window->handler;

	swapChainDesc.SampleDesc.Count = 1;
	swapChainDesc.SampleDesc.Quality = 0;

	if (FULLSCREEN)
	{
		swapChainDesc.Windowed = false;
	}
	else
	{
		swapChainDesc.Windowed = true;
	}

	swapChainDesc.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
	swapChainDesc.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;

	swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;

	swapChainDesc.Flags = 0;

	D3D_FEATURE_LEVEL featureLevel;
	featureLevel = D3D_FEATURE_LEVEL_11_0;
	int creationFlags = 0;
#ifdef _DEBUG
	creationFlags |= D3D11_CREATE_DEVICE_DEBUG;
#endif
	result = D3D11CreateDeviceAndSwapChain(NULL, D3D_DRIVER_TYPE_HARDWARE, NULL, creationFlags, &featureLevel, 1, D3D11_SDK_VERSION, &swapChainDesc, &(renderState->swapChain), &(renderState->device), NULL, &(renderState->deviceContext));
	assert(!FAILED(result));


	result = renderState->swapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID*)&backBufferPtr);
	assert(!FAILED(result));

	result = renderState->device->CreateRenderTargetView(backBufferPtr, NULL, &(renderState->renderTargetView));
	assert(!FAILED(result));

	backBufferPtr->Release();

	ZeroMemory(&depthBufferDesc, sizeof(depthBufferDesc));
	depthBufferDesc.Width = window->width;
	depthBufferDesc.Height = window->height;
	depthBufferDesc.MipLevels = 1;
	depthBufferDesc.ArraySize = 1;
	depthBufferDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	depthBufferDesc.SampleDesc.Count = 1;
	depthBufferDesc.SampleDesc.Quality = 0;
	depthBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	depthBufferDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
	depthBufferDesc.CPUAccessFlags = 0;
	depthBufferDesc.MiscFlags = 0;

	result = renderState->device->CreateTexture2D(&depthBufferDesc, NULL, &(renderState->depthStencilBuffer));
	assert(!FAILED(result));

	ZeroMemory(&depthStencilDesc, sizeof(depthStencilDesc));
	depthStencilDesc.DepthEnable = true;
	depthStencilDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
	depthStencilDesc.DepthFunc = D3D11_COMPARISON_LESS;

	depthStencilDesc.StencilEnable = false;
	depthStencilDesc.StencilReadMask = 0xFF;
	depthStencilDesc.StencilWriteMask = 0xFF;

	depthStencilDesc.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	depthStencilDesc.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_INCR;
	depthStencilDesc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
	depthStencilDesc.FrontFace.StencilFunc = D3D11_COMPARISON_ALWAYS;

	depthStencilDesc.BackFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	depthStencilDesc.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP_DECR;
	depthStencilDesc.BackFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
	depthStencilDesc.BackFace.StencilFunc = D3D11_COMPARISON_ALWAYS;

	result = renderState->device->CreateDepthStencilState(&depthStencilDesc, &(renderState->depthStencilState));
	assert(!FAILED(result));

	renderState->deviceContext->OMSetDepthStencilState(renderState->depthStencilState, 1);

	ZeroMemory(&depthStencilViewDesc, sizeof(depthStencilViewDesc));
	depthStencilViewDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	depthStencilViewDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
	depthStencilViewDesc.Texture2D.MipSlice = 0;

	result = renderState->device->CreateDepthStencilView(renderState->depthStencilBuffer, &depthStencilViewDesc, &(renderState->depthStencilView));
	assert(!FAILED(result));

	renderState->deviceContext->OMSetRenderTargets(1, &(renderState->renderTargetView), renderState->depthStencilView);

	rasterDesc.AntialiasedLineEnable = false;
	rasterDesc.CullMode = D3D11_CULL_BACK;
	rasterDesc.DepthBias = 0;
	rasterDesc.DepthBiasClamp = 0.0f;
	rasterDesc.DepthClipEnable = true;
	rasterDesc.FillMode = D3D11_FILL_SOLID;
	rasterDesc.FrontCounterClockwise = false;
	rasterDesc.MultisampleEnable = true;
	rasterDesc.ScissorEnable = false;
	rasterDesc.SlopeScaledDepthBias = 0.0f;

	result = renderState->device->CreateRasterizerState(&rasterDesc, &(renderState->rasterState));
	assert(!FAILED(result));

	renderState->deviceContext->RSSetState(renderState->rasterState);

	D3D11_BLEND_DESC desc;
	ZeroMemory(&desc, sizeof(desc));
	desc.AlphaToCoverageEnable = false;
	desc.RenderTarget[0].BlendEnable = true;
	desc.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA;
	desc.RenderTarget[0].DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
	desc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
	desc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
	desc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ZERO;
	desc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
	desc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
	renderState->device->CreateBlendState(&desc, &(renderState->blendState));
	renderState->deviceContext->OMSetBlendState(renderState->blendState, NULL, 0xffffffff);

	viewport.Width = float(window->width);
	viewport.Height = float(window->height);
	viewport.MinDepth = 0.0f;
	viewport.MaxDepth = 1.0f;
	viewport.TopLeftX = 0.0f;
	viewport.TopLeftY = 0.0f;
	renderState->deviceContext->RSSetViewports(1, &viewport);

	fieldOfView = 3.141592654f / 4.0f;
	screenAspect = window->width / (float)(window->height);
	renderState->projectionMatrix = XMMatrixPerspectiveFovLH(fieldOfView, screenAspect, SCREEN_NEAR, SCREEN_DEPTH);
	renderState->modelMatrix = XMMatrixIdentity();
	
	renderState->window = window;
}

void finalizeD3D11(EUTS_RenderState *renderState)
{
	renderState->swapChain->SetFullscreenState(false, NULL);
	renderState->rasterState->Release();
	renderState->depthStencilView->Release();
	renderState->depthStencilState->Release();
	renderState->depthStencilBuffer->Release();
	renderState->renderTargetView->Release();
	renderState->deviceContext->Release();
	renderState->device->Release();
	renderState->swapChain->Release();
}

void EUTS_Render_beginFrame(EUTS_RenderState *renderState)
{
	float color[4] = { 0.5f, 0.8f, 1.0f, 1.0f };

	renderState->deviceContext->OMSetBlendState(renderState->blendState, NULL, 0xffffffff);
	renderState->deviceContext->RSSetState(renderState->rasterState);
	renderState->deviceContext->OMSetDepthStencilState(renderState->depthStencilState, 1);
	//renderState->deviceContext->ClearRenderTargetView(renderState->renderTargetView, color);
	renderState->deviceContext->ClearDepthStencilView(renderState->depthStencilView, D3D11_CLEAR_DEPTH, 1.0f, 0);
}

void EUTS_Render_endFrame(EUTS_RenderState *renderState)
{
	if (VSYNC)
	{
		renderState->swapChain->Present(1, 0);
	}
	else
	{
		renderState->swapChain->Present(0, 0);
	}
}

void EUTS_Render_setTexture(EUTS_RenderState *renderState, ID3D11ShaderResourceView* texture)
{
	renderState->deviceContext->PSSetShaderResources(0, 1, &texture);
}

void EUTS_Render_setRenderTarget(EUTS_RenderState *renderState, EUTS_RenderTarget *renderTarget)
{
	renderState->deviceContext->OMSetRenderTargets(1, &(renderTarget->view), renderTarget->depthStencilView);
	EUTS_Render_setViewport(renderState, renderTarget->width, renderTarget->height);
}

void EUTS_Render_setDefaultRenderTarget(EUTS_RenderState *renderState)
{
	renderState->deviceContext->OMSetRenderTargets(1, &(renderState->renderTargetView), renderState->depthStencilView);
	EUTS_Render_setViewport(renderState, renderState->window->width, renderState->window->height);
}

void EUTS_Render_setViewport(EUTS_RenderState *renderState, float width, float height)
{
	D3D11_VIEWPORT viewport;
	viewport.Width = float(width);
	viewport.Height = float(height);
	viewport.MinDepth = 0.0f;
	viewport.MaxDepth = 1.0f;
	viewport.TopLeftX = 0.0f;
	viewport.TopLeftY = 0.0f;
	renderState->deviceContext->RSSetViewports(1, &viewport);
}



void EUTS_RenderTarget_initialize(EUTS_RenderTarget *renderTarget, EUTS_RenderState *state, int width, int height)
{
	D3D11_TEXTURE2D_DESC textureDesc;
	D3D11_TEXTURE2D_DESC depthBufferDesc;
	D3D11_RENDER_TARGET_VIEW_DESC renderTargetViewDesc;
	D3D11_DEPTH_STENCIL_VIEW_DESC depthStencilViewDesc;
	D3D11_SHADER_RESOURCE_VIEW_DESC shaderResourceViewDesc;
	HRESULT result;

	// Initialize the render target texture description.
	ZeroMemory(&textureDesc, sizeof(textureDesc));

	// Setup the render target texture description.
	textureDesc.Width = width;
	textureDesc.Height = height;
	textureDesc.MipLevels = 1;
	textureDesc.ArraySize = 1;
	textureDesc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
	textureDesc.SampleDesc.Count = 1;
	textureDesc.Usage = D3D11_USAGE_DEFAULT;
	textureDesc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE; // ?
	textureDesc.CPUAccessFlags = 0;
	textureDesc.MiscFlags = 0;

	result = state->device->CreateTexture2D(&textureDesc, NULL, &(renderTarget->texture));
	assert(!FAILED(result));

	renderTargetViewDesc.Format = textureDesc.Format;
	renderTargetViewDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
	renderTargetViewDesc.Texture2D.MipSlice = 0;

	result = state->device->CreateRenderTargetView(renderTarget->texture, &renderTargetViewDesc, &(renderTarget->view));
	assert(!FAILED(result));

	ZeroMemory(&depthBufferDesc, sizeof(depthBufferDesc));
	depthBufferDesc.Width = width;
	depthBufferDesc.Height = height;
	depthBufferDesc.MipLevels = 1;
	depthBufferDesc.ArraySize = 1;
	depthBufferDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	depthBufferDesc.SampleDesc.Count = 1;
	depthBufferDesc.SampleDesc.Quality = 0;
	depthBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	depthBufferDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
	depthBufferDesc.CPUAccessFlags = 0;
	depthBufferDesc.MiscFlags = 0;

	result = state->device->CreateTexture2D(&depthBufferDesc, NULL, &(renderTarget->depthStencilBuffer));
	assert(!FAILED(result));

	ZeroMemory(&depthStencilViewDesc, sizeof(depthStencilViewDesc));
	depthStencilViewDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	depthStencilViewDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
	depthStencilViewDesc.Texture2D.MipSlice = 0;

	result = state->device->CreateDepthStencilView(renderTarget->depthStencilBuffer, &depthStencilViewDesc, &(renderTarget->depthStencilView));
	assert(!FAILED(result));

	shaderResourceViewDesc.Format = textureDesc.Format;
	shaderResourceViewDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	shaderResourceViewDesc.Texture2D.MostDetailedMip = 0;
	shaderResourceViewDesc.Texture2D.MipLevels = 1;

	result = state->device->CreateShaderResourceView((renderTarget->texture), &shaderResourceViewDesc, &(renderTarget->shaderResourceView));
	assert(!FAILED(result));

	renderTarget->width = width;
	renderTarget->height = height;
}

void EUTS_RenderTarget_finalize(EUTS_RenderTarget *renderTarget)
{
	renderTarget->shaderResourceView->Release();
	renderTarget->depthStencilView->Release();
	renderTarget->depthStencilBuffer->Release();
	renderTarget->view->Release();
	renderTarget->texture->Release();
}

void EUTS_RenderTarget_clear(EUTS_RenderTarget *renderTarget, EUTS_RenderState *state, float r, float g, float b, float a)
{
	float color[4];
	color[0] = r;
	color[1] = g;
	color[2] = b;
	color[3] = a;

	state->deviceContext->ClearRenderTargetView(renderTarget->view, color);
	state->deviceContext->ClearDepthStencilView(renderTarget->depthStencilView, D3D11_CLEAR_DEPTH, 1.0f, 0);
}



void EUTS_Camera_setTarget(EUTS_Camera *camera, float x, float y, float z)
{
	camera->targetX = x;
	camera->targetY = y;
	camera->targetZ = z;
}

void EUTS_Camera_setAngles(EUTS_Camera *camera, float rotAngle, float heightAngle)
{
	camera->rotAngle = rotAngle;
	camera->heightAngle = heightAngle;
}

void EUTS_Camera_setDistance(EUTS_Camera *camera, float zoom)
{
	camera->zoom = zoom;
}

void EUTS_Camera_update(EUTS_Camera *camera)
{
	XMFLOAT3 up, position, lookAt;
	XMVECTOR upVector, positionVector, lookAtVector;
	float yaw, pitch, roll;
	XMMATRIX rotationMatrix;

	up.x = 0.0f;
	up.y = 1.0f;
	up.z = 0.0f;

	upVector = XMLoadFloat3(&up);

	position.x = camera->zoom;
	position.y = 0.0f;
	position.z = 0.0f;
	positionVector = XMLoadFloat3(&position);

	XMVECTOR rotVector = XMLoadFloat3(&XMFLOAT3(0.0f, 1.0f, 0.0f));
	XMMATRIX rotMatrix = XMMatrixRotationAxis(rotVector, camera->rotAngle);
	rotVector = XMLoadFloat3(&XMFLOAT3(0.0f, 0.0f, 1.0f));
	rotVector = XMVector4Transform(rotVector, rotMatrix);

	rotMatrix *= XMMatrixRotationAxis(rotVector, camera->heightAngle);
	positionVector = XMVector4Transform(positionVector, rotMatrix);

	lookAt.x = camera->targetX;
	lookAt.y = camera->targetY;
	lookAt.z = camera->targetZ;

	lookAtVector = XMLoadFloat3(&lookAt);

	camera->viewMatrix = XMMatrixLookAtLH(positionVector, lookAtVector, upVector);
}