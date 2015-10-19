#pragma once
#include "render.h"

struct EUTS_Shader
{
	ID3D11VertexShader* vertexShader;
	ID3D11PixelShader* pixelShader;
	ID3D11InputLayout* layout;
	ID3D11Buffer* constantBuffer;
};

struct EUTS_VSConstantBuffer
{
	XMMATRIX model;
	XMMATRIX view;
	XMMATRIX projection;
};

void EUTS_Shader_initialize(EUTS_Shader *shader, EUTS_RenderState *renderState, WCHAR *vsFilename, WCHAR *psFilename);

void outputShaderErrorMessage(ID3D10Blob* errorMessage, HWND hwnd, WCHAR* shaderFilename);

void EUTS_Shader_finalize(EUTS_Shader *shader);

void EUTS_Shader_setParameters(EUTS_Shader *shader, EUTS_RenderState *renderState, XMMATRIX *modelMatrix, XMMATRIX *viewMatrix, XMMATRIX *projectionMatrix);

void EUTS_Shader_bind(EUTS_Shader *shader, EUTS_RenderState *renderState);