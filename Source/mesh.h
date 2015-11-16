#pragma once
#include "render.h"

struct EUTS_Vertex
{
	XMFLOAT3 position;
	XMFLOAT3 normal;
	XMFLOAT2 texCoord;
};

struct EUTS_MeshDescriptor // Mesh on RAM
{
	unsigned int vertexCount;
	EUTS_Vertex *vertices;
	unsigned int indexCount; //faces * 3
	unsigned int *indices;
};


struct EUTS_Mesh // Mesh on GPU
{
	ID3D11Buffer	*vertexBuffer;
	ID3D11Buffer	*indexBuffer;
	unsigned int				vertexCount;
	unsigned int				indexCount;
};

void EUTS_Mesh_initialize(EUTS_Mesh *mesh, EUTS_RenderState *renderState, EUTS_MeshDescriptor *meshDescriptor);

void EUTS_Mesh_load(EUTS_Mesh *mesh, EUTS_RenderState *renderState, const char *fileName);

void EUTS_Mesh_loadQuad(EUTS_Mesh *mesh, EUTS_RenderState *renderState);

void EUTS_Mesh_finalize(EUTS_Mesh *mesh);

void EUTS_Mesh_bind(EUTS_Mesh *mesh, EUTS_RenderState *renderState);

void EUTS_Mesh_bindLine(EUTS_Mesh *mesh, EUTS_RenderState *renderState);