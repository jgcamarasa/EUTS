#pragma once
#include "render.h"

struct EUTS_Mesh
{
	ID3D11Buffer	*vertexBuffer;
	ID3D11Buffer	*indexBuffer;
	int				vertexCount;
	int				indexCount;
};

void EUTS_Mesh_initialize(EUTS_Mesh *mesh, EUTS_RenderState *renderState);

void EUTS_Mesh_finalize(EUTS_Mesh *mesh);

void EUTS_Mesh_bind(EUTS_Mesh *mesh, EUTS_RenderState *renderState);