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
const int SCREEN_WIDTH = 1280;
const int SCREEN_HEIGHT = 768;


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
	XMMATRIX				modelMatrix;
	EUTS_Window				*window; // This struct DOESN'T have the ownership of this
	float					videoMemoryInMB;
	char					videoCardDescription[128];
};

struct EUTS_RenderTarget
{
	ID3D11Texture2D	*texture;
	ID3D11RenderTargetView	*view;
	ID3D11ShaderResourceView *shaderResourceView;
};

void initD3D11(EUTS_Window *window, EUTS_RenderState *renderState);

void finalizeD3D11(EUTS_RenderState *renderState);

void EUTS_Render_beginFrame(EUTS_RenderState *renderState);

void EUTS_Render_endFrame(EUTS_RenderState *renderState);

void EUTS_Render_setTexture(EUTS_RenderState *renderState, ID3D11ShaderResourceView* texture); // Todo: More textures?

void EUTS_Render_setRenderTarget(EUTS_RenderState *renderState, EUTS_RenderTarget *renderTarget);

void EUTS_Render_setDefaultRenderTarget(EUTS_RenderState *renderState);


void EUTS_RenderTarget_initialize(EUTS_RenderTarget *renderTarget, EUTS_RenderState *state, int width, int height);

void EUTS_RenderTarget_finalize(EUTS_RenderTarget *renderTarget);

void EUTS_RenderTarget_clear(EUTS_RenderTarget *renderTarget, EUTS_RenderState *state, float r, float g, float b, float a);


struct EUTS_Camera
{
	float targetX, targetY, targetZ;
	float rotAngle;
	float heightAngle;
	float zoom;
	XMMATRIX viewMatrix;
};

void EUTS_Camera_setTarget(EUTS_Camera *camera, float x, float y, float z);

void EUTS_Camera_setAngles(EUTS_Camera *camera, float rotAngle, float heightAngle);

void EUTS_Camera_setDistance(EUTS_Camera *camera, float zoom);

void EUTS_Camera_update(EUTS_Camera *camera);