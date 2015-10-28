#pragma once
#include "render.h"

struct EUTS_Mesh // Mesh on GPU
{
	ID3D11Buffer	*vertexBuffer;
	ID3D11Buffer	*indexBuffer;
	unsigned int				vertexCount;
	unsigned int				indexCount;
};

void EUTS_Mesh_initialize(EUTS_Mesh *mesh, EUTS_RenderState *renderState, const char *fileName);

void EUTS_Mesh_finalize(EUTS_Mesh *mesh);

void EUTS_Mesh_bind(EUTS_Mesh *mesh, EUTS_RenderState *renderState);