#include "shader.h"
#include <stdio.h>

void EUTS_Shader_initialize(EUTS_Shader *shader, EUTS_RenderState *renderState, WCHAR *vsFilename, WCHAR *psFilename)
{
	HRESULT result;
	ID3DBlob *errorMessage;
	ID3DBlob *vertexShaderBuffer;
	ID3DBlob *pixelShaderBuffer;
	D3D11_INPUT_ELEMENT_DESC layout[2];
	unsigned int numElements;
	D3D11_BUFFER_DESC constantBufferDesc;
	HWND hwnd = renderState->window->handler;

	errorMessage = 0;
	vertexShaderBuffer = 0;
	pixelShaderBuffer = 0;

	result = D3DCompileFromFile(vsFilename, NULL, NULL, "main", "vs_5_0",
		D3D10_SHADER_ENABLE_STRICTNESS, 0, &vertexShaderBuffer, &errorMessage);

	if (FAILED(result))
	{
		if (errorMessage)
		{
			outputShaderErrorMessage(errorMessage, hwnd, vsFilename);
		}
		else
		{
			MessageBox(hwnd, vsFilename, L"Missing Shader File", MB_OK);
		}
	}

	result = D3DCompileFromFile(psFilename, NULL, NULL, "main", "ps_5_0",
		D3D10_SHADER_ENABLE_STRICTNESS, 0, &pixelShaderBuffer, &errorMessage);

	if (FAILED(result))
	{
		if (errorMessage)
		{
			outputShaderErrorMessage(errorMessage, hwnd, psFilename);
		}
		else
		{
			MessageBox(hwnd, psFilename, L"Missing Shader File", MB_OK);
		}
		assert(false);
	}

	result = renderState->device->CreateVertexShader(vertexShaderBuffer->GetBufferPointer(), vertexShaderBuffer->GetBufferSize(), NULL, &(shader->vertexShader));
	assert(!FAILED(result));

	result = renderState->device->CreatePixelShader(pixelShaderBuffer->GetBufferPointer(), pixelShaderBuffer->GetBufferSize(), NULL, &(shader->pixelShader));
	assert(!FAILED(result));

	layout[0].SemanticName = "POSITION";
	layout[0].SemanticIndex = 0;
	layout[0].Format = DXGI_FORMAT_R32G32B32_FLOAT; // float3
	layout[0].InputSlot = 0;
	layout[0].AlignedByteOffset = 0;
	layout[0].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
	layout[0].InstanceDataStepRate = 0;

	layout[1].SemanticName = "COLOR";
	layout[1].SemanticIndex = 0;
	layout[1].Format = DXGI_FORMAT_R32G32B32A32_FLOAT; // float4
	layout[1].InputSlot = 0;
	layout[1].AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;
	layout[1].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
	layout[1].InstanceDataStepRate = 0;

	numElements = sizeof(layout) / sizeof(layout[0]);

	result = renderState->device->CreateInputLayout(layout, numElements, vertexShaderBuffer->GetBufferPointer(), vertexShaderBuffer->GetBufferSize(), &(shader->layout));
	assert(!FAILED(result));

	vertexShaderBuffer->Release();
	pixelShaderBuffer->Release();

	constantBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	constantBufferDesc.ByteWidth = sizeof(EUTS_VSConstantBuffer);
	constantBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	constantBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	constantBufferDesc.MiscFlags = 0;
	constantBufferDesc.StructureByteStride = 0;

	result = renderState->device->CreateBuffer(&constantBufferDesc, NULL, &(shader->constantBuffer));
}

void outputShaderErrorMessage(ID3D10Blob* errorMessage, HWND hwnd, WCHAR* shaderFilename)
{
	char* compileErrors;
	unsigned long long bufferSize;

	// Get a pointer to the error message text buffer.
	compileErrors = (char*)(errorMessage->GetBufferPointer());

	// Get the length of the message.
	bufferSize = errorMessage->GetBufferSize();

	printf("ShaderError:\n%s", compileErrors);

	// Release the error message.
	errorMessage->Release();
	errorMessage = 0;

	// Pop a message up on the screen to notify the user to check the text file for compile errors.
	MessageBox(hwnd, L"Error compiling shader.  Check shader-error.txt for message.", shaderFilename, MB_OK);

	return;
}

void EUTS_Shader_finalize(EUTS_Shader *shader)
{
	shader->constantBuffer->Release();
	shader->layout->Release();
	shader->pixelShader->Release();
	shader->vertexShader->Release();
}

void EUTS_Shader_setParameters(EUTS_Shader *shader, EUTS_RenderState *renderState, XMMATRIX *worldMatrix, XMMATRIX *viewMatrix, XMMATRIX *projectionMatrix)
{
	HRESULT result;
	D3D11_MAPPED_SUBRESOURCE mappedResource;
	EUTS_VSConstantBuffer *dataPtr;
	unsigned int bufferNumber;
	XMMATRIX world, view, projection;

	// It's a requirement to transpose the matrices before passing them to the shader
	world = XMMatrixTranspose(*worldMatrix);
	view = XMMatrixTranspose(*viewMatrix);
	projection = XMMatrixTranspose(*projectionMatrix);

	result = renderState->deviceContext->Map(shader->constantBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	assert(!FAILED(result));

	dataPtr = (EUTS_VSConstantBuffer*)mappedResource.pData;
	dataPtr->world = world;
	dataPtr->view = view;
	dataPtr->projection = projection;

	renderState->deviceContext->Unmap(shader->constantBuffer, 0);

	// Set the position of the constant vuffer in the vertex shader
	bufferNumber = 0;

	renderState->deviceContext->VSSetConstantBuffers(bufferNumber, 1, &(shader->constantBuffer));
}

void EUTS_Shader_bind(EUTS_Shader *shader, EUTS_RenderState *renderState)
{
	renderState->deviceContext->IASetInputLayout(shader->layout);
	renderState->deviceContext->VSSetShader(shader->vertexShader, NULL, 0);
	renderState->deviceContext->PSSetShader(shader->pixelShader, NULL, 0);
}