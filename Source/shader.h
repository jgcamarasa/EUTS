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
	ID3D11SamplerState* samplerState;
};

struct EUTS_ShaderConstants
{
	ID3D11Buffer* sceneBuffer;
	ID3D11Buffer* objectBuffer;
	ID3D11Buffer* colorBuffer;
};

struct EUTS_VSSceneConstantBuffer
{
	XMMATRIX view;
	XMMATRIX projection;
};

struct EUTS_VSObjectConstantBuffer
{
	XMMATRIX model;
};

struct EUTS_VSColorConstantBuffer
{
	XMFLOAT4 color;
};

void EUTS_ShaderConstants_initialize(EUTS_ShaderConstants *constants, EUTS_RenderState *renderState);

void EUTS_ShaderConstants_finalize(EUTS_ShaderConstants *constants);

void outputShaderErrorMessage(ID3D10Blob* errorMessage, HWND hwnd, WCHAR* shaderFilename);

void EUTS_Shader_initialize(EUTS_Shader *shader, EUTS_RenderState *renderState, WCHAR *vsFilename, WCHAR *psFilename, int flags);

void EUTS_Shader_finalize(EUTS_Shader *shader);

void EUTS_Shader_bind(EUTS_Shader *shader, EUTS_RenderState *renderState);

void EUTS_ShaderConstants_setSceneMatrices(EUTS_ShaderConstants *constants, EUTS_RenderState *renderState, XMMATRIX *viewMatrix, XMMATRIX *projectionMatrix);

void EUTS_ShaderConstants_setModelMatrix(EUTS_ShaderConstants *constants, EUTS_RenderState *renderState, XMMATRIX *modelMatrix);

void EUTS_ShaderConstants_setColor(EUTS_ShaderConstants *constants, EUTS_RenderState *renderState, XMFLOAT4 *color);