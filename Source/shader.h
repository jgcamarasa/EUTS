#pragma once
#include "render.h"

//** PROVISIONAL **// // May change depending on needs. Maybe separate completely each shader?
// Type of the shader depending on its input (nº samplers, command buffers...)
// Two different shaders can share a type, if they have the same input
enum EUTS_ShaderFlags
{
	SHADER_FLAG_NONE			= 0x0000,
	SHADER_FLAG_TEXTURE			= 0x0001, 
};

// Some of the members won't be actually used, depending on shader type. Ugly, but for now...
struct EUTS_Shader
{
	int flags;
	ID3D11VertexShader* vertexShader;
	ID3D11PixelShader* pixelShader;
	ID3D11InputLayout* layout;
	ID3D11Buffer* constantBuffer;
	ID3D11SamplerState* samplerState;
};

struct EUTS_VSMatrixConstantBuffer
{
	XMMATRIX model;
	XMMATRIX view;
	XMMATRIX projection;
};

void outputShaderErrorMessage(ID3D10Blob* errorMessage, HWND hwnd, WCHAR* shaderFilename);

void EUTS_Shader_initialize(EUTS_Shader *shader, EUTS_RenderState *renderState, WCHAR *vsFilename, WCHAR *psFilename, int flags);

void EUTS_Shader_finalize(EUTS_Shader *shader);

void EUTS_Shader_bind(EUTS_Shader *shader, EUTS_RenderState *renderState);

void EUTS_Shader_setMatrices(EUTS_Shader *shader, EUTS_RenderState *renderState, XMMATRIX *modelMatrix, XMMATRIX *viewMatrix, XMMATRIX *projectionMatrix);