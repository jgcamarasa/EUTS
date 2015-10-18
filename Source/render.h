#pragma once

#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include <d3d11.h>
#include <DirectXMath.h>
#include <d3dcompiler.h>

using namespace DirectX;

#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "dxgi.lib")
#pragma comment(lib, "d3dcompiler.lib")

const bool FULLSCREEN = false;
const bool VSYNC = true;
const float SCREEN_NEAR = 0.1f;
const float SCREEN_DEPTH = 1000.0f;


struct EUTS_Window
{
	HWND handler;		// Window handler
	HINSTANCE instance; // Application instance
	LPCWSTR title;		// Title
	UINT32 width;		// Width
	UINT32 height;		// Height
	bool fullScreen;	// Is fullscreen?
};

void initWindow(EUTS_Window *window);


struct EUTS_RenderState
{
	IDXGISwapChain			*swapChain;
	ID3D11Device			*device;
	ID3D11DeviceContext		*deviceContext;
	ID3D11RenderTargetView	*renderTargetView;
	ID3D11Texture2D			*depthStencilBuffer;
	ID3D11DepthStencilState *depthStencilState;
	ID3D11DepthStencilView	*depthStencilView;
	ID3D11RasterizerState	*rasterState;
	XMMATRIX				projectionMatrix;
	XMMATRIX				worldMatrix;
	EUTS_Window				*window; // This struct DOESN'T have the ownership of this
	float					videoMemoryInMB;
	char					videoCardDescription[128];
};

void initD3D11(EUTS_Window *window, EUTS_RenderState *renderState);

void finalizeD3D11(EUTS_RenderState *renderState);

void beginScene(EUTS_RenderState *renderState);

void endScene(EUTS_RenderState *renderState);


struct EUTS_Camera
{
	float posX, posY, posZ;
	float rotX, rotY, rotZ; // radians
	XMMATRIX viewMatrix;
};

void EUTS_Camera_setPosition(EUTS_Camera *camera, float posX, float posY, float posZ);

void EUTS_Camera_setRotation(EUTS_Camera *camera, float rotX, float rotY, float rotZ);

void EUTS_Camera_update(EUTS_Camera *camera);