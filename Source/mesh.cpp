#include "mesh.h"
struct EUTS_Vertex
{
	XMFLOAT3 position;
	XMFLOAT4 color;
	XMFLOAT2 texCoord;
};

// intialize a mesh to a triangle...
void EUTS_Mesh_initialize(EUTS_Mesh *mesh, EUTS_RenderState *renderState)
{
	EUTS_Vertex *vertices;
	unsigned long *indices;
	D3D11_BUFFER_DESC vertexBufferDesc, indexBufferDesc;
	D3D11_SUBRESOURCE_DATA vertexData, indexData;
	HRESULT result;

	mesh->vertexCount = 3;
	mesh->indexCount = 3;
	vertices = new EUTS_Vertex[mesh->vertexCount];
	indices = new unsigned long[mesh->indexCount];

	// Load the vertex array with data.
	vertices[0].position = XMFLOAT3(-1.0f, -1.0f, 0.0f);  // Bottom left.
	vertices[0].color = XMFLOAT4(1.0f, 0.0f, 0.0f, 1.0f);
	vertices[0].texCoord = XMFLOAT2(0.0f, 1.0f);

	vertices[1].position = XMFLOAT3(0.0f, 1.0f, 0.0f);  // Top middle.
	vertices[1].color = XMFLOAT4(0.0f, 1.0f, 0.0f, 1.0f);
	vertices[1].texCoord = XMFLOAT2(0.5f, 0.0f);

	vertices[2].position = XMFLOAT3(1.0f, -1.0f, 0.0f);  // Bottom right.
	vertices[2].color = XMFLOAT4(0.0f, 0.0f, 1.0f, 1.0f);
	vertices[2].texCoord = XMFLOAT2(1.0f, 1.0f);

	// Load the index array with data.
	indices[0] = 0;  // Bottom left.
	indices[1] = 1;  // Top middle.
	indices[2] = 2;  // Bottom right.

	vertexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	vertexBufferDesc.ByteWidth = sizeof(EUTS_Vertex)*mesh->vertexCount;
	vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vertexBufferDesc.CPUAccessFlags = 0;
	vertexBufferDesc.MiscFlags = 0;
	vertexBufferDesc.StructureByteStride = 0;

	vertexData.pSysMem = vertices;
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

	indexData.pSysMem = indices;
	indexData.SysMemPitch = 0;
	indexData.SysMemSlicePitch = 0;

	result = renderState->device->CreateBuffer(&indexBufferDesc, &indexData, &(mesh->indexBuffer));
	assert(!FAILED(result));

	delete[] vertices;
	delete[] indices;
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