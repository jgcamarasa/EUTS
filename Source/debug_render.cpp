#include "debug_render.h"
#include "mesh.h"
#include "shader.h"

struct EUTS_DebugRenderState
{
	EUTS_Shader lineShader;
};

EUTS_DebugRenderState _debugRenderState;

void EUTS_DebugRender_initialize(EUTS_RenderState *renderState)
{
	EUTS_Shader_initialize(&(_debugRenderState.lineShader), renderState,
							L"../../../Resources/Shaders/DebugLineVS.hlsl", L"../../../Resources/Shaders/DebugLinePS.hlsl", 
							SHADER_FLAG_NONE);
}

void EUTS_DebugRender_finalize()
{
	EUTS_Shader_finalize(&(_debugRenderState.lineShader));
}

void EUTS_DebugRender_drawLine(XMFLOAT3 p1, XMFLOAT3 p2, EUTS_RenderState *renderState)
{
	EUTS_Vertex vertices[2];
	vertices[0].position = p1;
	vertices[0].normal = XMFLOAT3();
	vertices[0].texCoord = XMFLOAT2();

	vertices[1].position = p2;
	vertices[1].normal = XMFLOAT3();
	vertices[1].texCoord = XMFLOAT2();

	unsigned int indices[2];
	indices[0] = 0;
	indices[1] = 1;

	EUTS_MeshDescriptor descriptor;
	descriptor.vertexCount = 2;
	descriptor.vertices = vertices;
	descriptor.indexCount = 2;
	descriptor.indices = indices;

	EUTS_Mesh lineMesh;
	EUTS_Mesh_initialize(&lineMesh, renderState, &descriptor);
	EUTS_Mesh_bindLine(&lineMesh, renderState);
	EUTS_Shader_bind(&(_debugRenderState.lineShader), renderState);
	renderState->deviceContext->DrawIndexed(lineMesh.indexCount, 0, 0);
	EUTS_Mesh_finalize(&lineMesh);
}