#include "mesh.h"
#include <stdio.h>



void EUTS_MeshDescriptor_load(EUTS_MeshDescriptor *meshDescriptor, const char *fileName)
{
	FILE *file = fopen(fileName, "rb");

	char id[4];
	size_t bytesRead;
	bytesRead = fread(id, sizeof(char), 4, file);
	assert(bytesRead == 4);
	assert(id[0] == 'E');
	assert(id[1] == 'U');
	assert(id[2] == 'T');
	assert(id[3] == 'S');

	bytesRead = fread(id, sizeof(char), 4, file);
	assert(bytesRead == 4);
	assert(id[0] == 'M');
	assert(id[1] == 'E');
	assert(id[2] == 'S');
	assert(id[3] == 'H');

	unsigned int numVerts;
	bytesRead = fread(&numVerts, sizeof(unsigned int), 1, file);
	meshDescriptor->vertexCount = numVerts;
	meshDescriptor->vertices = (EUTS_Vertex*) malloc(sizeof(EUTS_Vertex)*numVerts);
	bytesRead = fread(meshDescriptor->vertices, 1, numVerts*sizeof(EUTS_Vertex), file);
	assert(bytesRead == sizeof(EUTS_Vertex)*numVerts);

	unsigned int numIndices;
	bytesRead = fread(&numIndices, sizeof(unsigned int), 1, file);
	meshDescriptor->indexCount = numIndices;
	meshDescriptor->indices = (unsigned int*)malloc(sizeof(unsigned int)*numIndices);
	bytesRead = fread(meshDescriptor->indices, 1, numIndices*sizeof(unsigned int), file);
	assert(bytesRead == sizeof(unsigned int)*numIndices);

	fclose(file);
}

void EUTS_MeshDescriptor_finalize(EUTS_MeshDescriptor *meshDescriptor)
{
	free(meshDescriptor->vertices);
	free(meshDescriptor->indices);
}

// intialize a mesh to a triangle...
void EUTS_Mesh_initialize(EUTS_Mesh *mesh, EUTS_RenderState *renderState, EUTS_MeshDescriptor *descriptor)
{
	D3D11_BUFFER_DESC vertexBufferDesc, indexBufferDesc;
	D3D11_SUBRESOURCE_DATA vertexData, indexData;
	HRESULT result;

	mesh->vertexCount = descriptor->vertexCount;
	mesh->indexCount = descriptor->indexCount;

	vertexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	vertexBufferDesc.ByteWidth = sizeof(EUTS_Vertex)*mesh->vertexCount;
	vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vertexBufferDesc.CPUAccessFlags = 0;
	vertexBufferDesc.MiscFlags = 0;
	vertexBufferDesc.StructureByteStride = 0;

	vertexData.pSysMem = descriptor->vertices;
	vertexData.SysMemPitch = 0;
	vertexData.SysMemSlicePitch = 0;

	result = renderState->device->CreateBuffer(&vertexBufferDesc, &vertexData, &(mesh->vertexBuffer));
	assert(!FAILED(result));

	indexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	indexBufferDesc.ByteWidth = sizeof(unsigned long)*mesh->indexCount;
	indexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	indexBufferDesc.CPUAccessFlags = 0;
	indexBufferDesc.MiscFlags = 0;
	indexBufferDesc.StructureByteStride = 0;

	indexData.pSysMem = descriptor->indices;
	indexData.SysMemPitch = 0;
	indexData.SysMemSlicePitch = 0;

	result = renderState->device->CreateBuffer(&indexBufferDesc, &indexData, &(mesh->indexBuffer));
	assert(!FAILED(result));
}

void EUTS_Mesh_load(EUTS_Mesh *mesh, EUTS_RenderState *renderState, const char *fileName)
{
	EUTS_MeshDescriptor descriptor;
	EUTS_MeshDescriptor_load(&descriptor, fileName);
	EUTS_Mesh_initialize(mesh, renderState, &descriptor);
	EUTS_MeshDescriptor_finalize(&descriptor);
}

void EUTS_Mesh_finalize(EUTS_Mesh *mesh)
{
	mesh->indexBuffer->Release();
	mesh->vertexBuffer->Release();
}

void EUTS_Mesh_bind(EUTS_Mesh *mesh, EUTS_RenderState *renderState)
{
	unsigned int stride;
	unsigned int offset;

	stride = sizeof(EUTS_Vertex);
	offset = 0;

	renderState->deviceContext->IASetVertexBuffers(0, 1, &(mesh->vertexBuffer), &stride, &offset);
	renderState->deviceContext->IASetIndexBuffer(mesh->indexBuffer, DXGI_FORMAT_R32_UINT, 0);
	renderState->deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
}