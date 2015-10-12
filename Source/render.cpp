#include "render.h"

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	PAINTSTRUCT ps;
	HDC hdc;

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
	}
	else
	{
		// If windowed then set it to 800x600 resolution.
		screenWidth = 800;
		screenHeight = 600;

		// Place the window in the middle of the screen.
		posX = (GetSystemMetrics(SM_CXSCREEN) - screenWidth) / 2;
		posY = (GetSystemMetrics(SM_CYSCREEN) - screenHeight) / 2;
	}

	window->width = screenWidth;
	window->height = screenHeight;

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

void initD3D11(EUTS_Window *window, EUTS_D3D11Context *context)
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

	for (int i = 0; i < numModes; i++)
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

	context->videoMemoryInMB = adapterDesc.DedicatedVideoMemory / 1024.0f / 1024.0f;

	size_t stringLength;
	wcstombs_s(&stringLength, context->videoCardDescription, 128, adapterDesc.Description, 128);

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
	result = D3D11CreateDeviceAndSwapChain(NULL, D3D_DRIVER_TYPE_HARDWARE, NULL, 0, &featureLevel, 1, D3D11_SDK_VERSION, &swapChainDesc, &(context->swapChain), &(context->device), NULL, &(context->deviceContext));
	assert(!FAILED(result));


	result = context->swapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID*)&backBufferPtr);
	assert(!FAILED(result));

	result = context->device->CreateRenderTargetView(backBufferPtr, NULL, &(context->renderTargetView));
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

	result = context->device->CreateTexture2D(&depthBufferDesc, NULL, &(context->depthStencilBuffer));
	assert(!FAILED(result));

	ZeroMemory(&depthStencilDesc, sizeof(depthStencilDesc));
	depthStencilDesc.DepthEnable = true;
	depthStencilDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
	depthStencilDesc.DepthFunc = D3D11_COMPARISON_LESS;

	depthStencilDesc.StencilEnable = true;
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

	result = context->device->CreateDepthStencilState(&depthStencilDesc, &(context->depthStencilState));
	assert(!FAILED(result));

	context->deviceContext->OMSetDepthStencilState(context->depthStencilState, 1);

	ZeroMemory(&depthStencilViewDesc, sizeof(depthStencilViewDesc));
	depthStencilViewDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	depthStencilViewDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
	depthStencilViewDesc.Texture2D.MipSlice = 0;

	result = context->device->CreateDepthStencilView(context->depthStencilBuffer, &depthStencilViewDesc, &(context->depthStencilView));
	assert(!FAILED(result));

	context->deviceContext->OMSetRenderTargets(1, &(context->renderTargetView), context->depthStencilView);

	rasterDesc.AntialiasedLineEnable = false;
	rasterDesc.CullMode = D3D11_CULL_BACK;
	rasterDesc.DepthBias = 0;
	rasterDesc.DepthBiasClamp = 0.0f;
	rasterDesc.DepthClipEnable = true;
	rasterDesc.FillMode = D3D11_FILL_SOLID;
	rasterDesc.FrontCounterClockwise = false;
	rasterDesc.MultisampleEnable = false;
	rasterDesc.ScissorEnable = false;
	rasterDesc.SlopeScaledDepthBias = 0.0f;

	result = context->device->CreateRasterizerState(&rasterDesc, &(context->rasterState));
	assert(!FAILED(result));

	context->deviceContext->RSSetState(context->rasterState);

	viewport.Width = float(window->width);
	viewport.Height = float(window->height);
	viewport.MinDepth = 0.0f;
	viewport.MaxDepth = 1.0f;
	viewport.TopLeftX = 0.0f;
	viewport.TopLeftY = 0.0f;
	context->deviceContext->RSSetViewports(1, &viewport);

	fieldOfView = 3.141592654f / 4.0f;
	screenAspect = window->width / (float)(window->height);
	context->projectionMatrix = XMMatrixPerspectiveFovLH(fieldOfView, screenAspect, SCREEN_NEAR, SCREEN_DEPTH);
	context->worldMatrix = XMMatrixIdentity();
}

void finalizeD3D11(EUTS_D3D11Context *context)
{
	context->swapChain->SetFullscreenState(false, NULL);
	context->rasterState->Release();
	context->depthStencilView->Release();
	context->depthStencilState->Release();
	context->depthStencilBuffer->Release();
	context->renderTargetView->Release();
	context->deviceContext->Release();
	context->device->Release();
	context->swapChain->Release();
}

void beginScene(EUTS_D3D11Context *context)
{
	float color[4] = { 0.5f, 0.8f, 1.0f, 1.0f };

	context->deviceContext->ClearRenderTargetView(context->renderTargetView, color);

	context->deviceContext->ClearDepthStencilView(context->depthStencilView, D3D11_CLEAR_DEPTH, 1.0f, 0);
}

void endScene(EUTS_D3D11Context *context)
{
	if (VSYNC)
	{
		context->swapChain->Present(1, 0);
	}
	else
	{
		context->swapChain->Present(0, 0);
	}
}

void render(EUTS_D3D11Context *context)
{
	beginScene(context);
	endScene(context);
}

struct EUTS_Vertex
{
	XMFLOAT3 position;
	XMFLOAT4 color;
};

// intialize a mesh to a triangle...
void EUTS_Mesh_initialize(EUTS_Mesh *mesh, EUTS_D3D11Context *context)
{
	EUTS_Vertex *vertices;
	unsigned long *indices;
	D3D11_BUFFER_DESC vertexBufferDesc, indexBufferDesc;
	D3D11_SUBRESOURCE_DATA vertexData, indexData;
	HRESULT result;

	mesh->vertexCount = 3;
	mesh->indexCount = 3;
	vertices = new EUTS_Vertex[mesh->vertexCount];
	indices = new unsigned long[mesh->indexCount];

	// Load the vertex array with data.
	vertices[0].position = XMFLOAT3(-1.0f, -1.0f, 0.0f);  // Bottom left.
	vertices[0].color = XMFLOAT4(0.0f, 1.0f, 0.0f, 1.0f);

	vertices[1].position = XMFLOAT3(0.0f, 1.0f, 0.0f);  // Top middle.
	vertices[1].color = XMFLOAT4(0.0f, 1.0f, 0.0f, 1.0f);

	vertices[2].position = XMFLOAT3(1.0f, -1.0f, 0.0f);  // Bottom right.
	vertices[2].color = XMFLOAT4(0.0f, 1.0f, 0.0f, 1.0f);

	// Load the index array with data.
	indices[0] = 0;  // Bottom left.
	indices[1] = 1;  // Top middle.
	indices[2] = 2;  // Bottom right.

	vertexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	vertexBufferDesc.ByteWidth = sizeof(EUTS_Vertex)*mesh->vertexCount;
	vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vertexBufferDesc.CPUAccessFlags = 0;
	vertexBufferDesc.MiscFlags = 0;
	vertexBufferDesc.StructureByteStride = 0;

	vertexData.pSysMem = vertices;
	vertexData.SysMemPitch = 0;
	vertexData.SysMemSlicePitch = 0;

	result = context->device->CreateBuffer(&vertexBufferDesc, &vertexData, &(mesh->vertexBuffer));
	assert(!FAILED(result));

	indexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	indexBufferDesc.ByteWidth = sizeof(unsigned long)*mesh->indexCount;
	indexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	indexBufferDesc.CPUAccessFlags = 0;
	indexBufferDesc.MiscFlags = 0;
	indexBufferDesc.StructureByteStride = 0;

	indexData.pSysMem = indices;
	indexData.SysMemPitch = 0;
	indexData.SysMemSlicePitch = 0;

	result = context->device->CreateBuffer(&indexBufferDesc, &indexData, &(mesh->indexBuffer));
	assert(!FAILED(result));

	delete[] vertices;
	delete[] indices;
}

void EUTS_Mesh_finalize(EUTS_Mesh *mesh)
{
	mesh->indexBuffer->Release();
	mesh->vertexBuffer->Release();
}

void EUTS_Mesh_bind(EUTS_Mesh *mesh, EUTS_D3D11Context *context)
{
	unsigned int stride;
	unsigned int offset;

	stride = sizeof(EUTS_Vertex);
	offset = 0;

	context->deviceContext->IASetVertexBuffers(0, 1, &(mesh->vertexBuffer), &stride, &offset);
	context->deviceContext->IASetIndexBuffer(mesh->indexBuffer, DXGI_FORMAT_R32_UINT, 0);
	context->deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
}