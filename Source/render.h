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
const int BLUR_WIDTH = SCREEN_WIDTH / 16;
const int BLUR_HEIGHT = SCREEN_HEIGHT / 16;

extern bool showDebugGui;

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

struct EUTS_RenderTarget
{
	ID3D11Texture2D	*texture;
	ID3D11RenderTargetView	*view;
	ID3D11ShaderResourceView *shaderResourceView;
	float width;
	float height;
};

struct EUTS_DepthBuffer
{
	ID3D11Texture2D	*texture;
	ID3D11DepthStencilView *view;
};

struct EUTS_RenderState
{
	IDXGISwapChain			*swapChain;
	ID3D11Device			*device;
	ID3D11DeviceContext		*deviceContext;
	ID3D11RenderTargetView	*renderTargetView;
	ID3D11DepthStencilState *depthStencilState;
	EUTS_DepthBuffer		 depthBuffer;
	ID3D11RasterizerState	*rasterState;
	ID3D11BlendState		*blendState;
	XMMATRIX				projectionMatrix;
	XMMATRIX				modelMatrix;
	EUTS_Window				*window; // This struct DOESN'T have the ownership of this
	float					videoMemoryInMB;
	char					videoCardDescription[128];
};



void initD3D11(EUTS_Window *window, EUTS_RenderState *renderState);

void finalizeD3D11(EUTS_RenderState *renderState);

void EUTS_Render_beginFrame(EUTS_RenderState *renderState);

void EUTS_Render_endFrame(EUTS_RenderState *renderState);

void EUTS_Render_bindTexture(EUTS_RenderState *renderState, ID3D11ShaderResourceView* texture, int index); 

void EUTS_Render_unbindTexture(EUTS_RenderState *renderState, int index);

void EUTS_Render_setRenderTarget(EUTS_RenderState *renderState, EUTS_RenderTarget *renderTarget, EUTS_DepthBuffer *depthBuffer);

void EUTS_Render_setDefaultRenderTarget(EUTS_RenderState *renderState);

void EUTS_Render_setViewport(EUTS_RenderState *renderState, float width, float height);

void EUTS_RenderTarget_initialize(EUTS_RenderTarget *renderTarget, EUTS_RenderState *state, int width, int height);

void EUTS_RenderTarget_finalize(EUTS_RenderTarget *renderTarget);

void EUTS_RenderTarget_clear(EUTS_RenderTarget *renderTarget, EUTS_RenderState *state, float r, float g, float b, float a);

void EUTS_DepthBuffer_initialize(EUTS_DepthBuffer *depthBuffer, EUTS_RenderState *state, int width, int height);

void EUTS_DepthBuffer_finalize(EUTS_DepthBuffer *depthBuffer);

void EUTS_DepthBuffer_clear(EUTS_DepthBuffer *depthBuffer, EUTS_RenderState *state);


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