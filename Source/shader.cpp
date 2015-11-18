#include "shader.h"
#include <stdio.h>


void EUTS_ShaderConstants_initialize(EUTS_ShaderConstants *constants, EUTS_RenderState *renderState)
{
	D3D11_BUFFER_DESC constantBufferDesc;
	HRESULT result;

	constantBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	constantBufferDesc.ByteWidth = sizeof(EUTS_VSSceneConstantBuffer);
	constantBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	constantBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	constantBufferDesc.MiscFlags = 0;
	constantBufferDesc.StructureByteStride = 0;

	result = renderState->device->CreateBuffer(&constantBufferDesc, NULL, &(constants->sceneBuffer));

	constantBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	constantBufferDesc.ByteWidth = sizeof(EUTS_VSObjectConstantBuffer);
	constantBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	constantBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	constantBufferDesc.MiscFlags = 0;
	constantBufferDesc.StructureByteStride = 0;

	result = renderState->device->CreateBuffer(&constantBufferDesc, NULL, &(constants->objectBuffer));

	constantBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	constantBufferDesc.ByteWidth = sizeof(EUTS_VSColorConstantBuffer);
	constantBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	constantBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	constantBufferDesc.MiscFlags = 0;
	constantBufferDesc.StructureByteStride = 0;

	result = renderState->device->CreateBuffer(&constantBufferDesc, NULL, &(constants->colorBuffer));

	constantBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	constantBufferDesc.ByteWidth = sizeof(EUTS_PSLightConstantBuffer);
	constantBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	constantBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	constantBufferDesc.MiscFlags = 0;
	constantBufferDesc.StructureByteStride = 0;

	result = renderState->device->CreateBuffer(&constantBufferDesc, NULL, &(constants->lightBuffer));

	constantBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	constantBufferDesc.ByteWidth = sizeof(EUTS_PSRenderTargetConstantBuffer);
	constantBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	constantBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	constantBufferDesc.MiscFlags = 0;
	constantBufferDesc.StructureByteStride = 0;

	result = renderState->device->CreateBuffer(&constantBufferDesc, NULL, &(constants->renderTargetBuffer));
}

void EUTS_ShaderConstants_finalize(EUTS_ShaderConstants *constants)
{
	constants->sceneBuffer->Release();
	constants->objectBuffer->Release();
	constants->colorBuffer->Release();
	constants->lightBuffer->Release();
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
	MessageBox(hwnd, L"Error compiling shader.", shaderFilename, MB_OK);

	return;
}

void EUTS_Shader_finalize(EUTS_Shader *shader)
{
	shader->layout->Release();
	shader->pixelShader->Release();
	shader->vertexShader->Release();
	if (shader->flags & SHADER_FLAG_TEXTURE)
	{
		shader->samplerState->Release();
	}
}

void EUTS_Shader_bind(EUTS_Shader *shader, EUTS_RenderState *renderState)
{
	renderState->deviceContext->IASetInputLayout(shader->layout);
	renderState->deviceContext->VSSetShader(shader->vertexShader, NULL, 0);
	renderState->deviceContext->PSSetShader(shader->pixelShader, NULL, 0);
	if (shader->flags & SHADER_FLAG_TEXTURE)
	{
		renderState->deviceContext->PSSetSamplers(0, 1, &(shader->samplerState));
	}
}

void EUTS_Shader_initialize(EUTS_Shader *shader, EUTS_RenderState *renderState, WCHAR *vsFilename, WCHAR *psFilename, int flags)
{
	HRESULT result;
	ID3DBlob *errorMessage;
	ID3DBlob *vertexShaderBuffer;
	ID3DBlob *pixelShaderBuffer;
	D3D11_INPUT_ELEMENT_DESC layout[3];
	unsigned int numElements;
	HWND hwnd = renderState->window->handler;
	D3D11_SAMPLER_DESC samplerDesc;

	shader->flags = flags; // Store the flags for later use

	errorMessage = 0;
	vertexShaderBuffer = 0;
	pixelShaderBuffer = 0;

	result = D3DCompileFromFile(vsFilename, NULL, D3D_COMPILE_STANDARD_FILE_INCLUDE, "main", "vs_5_0",
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

	result = D3DCompileFromFile(psFilename, NULL, D3D_COMPILE_STANDARD_FILE_INCLUDE, "main", "ps_5_0",
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

	layout[1].SemanticName = "NORMAL";
	layout[1].SemanticIndex = 0;
	layout[1].Format = DXGI_FORMAT_R32G32B32_FLOAT; // float3
	layout[1].InputSlot = 0;
	layout[1].AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;
	layout[1].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
	layout[1].InstanceDataStepRate = 0;

	layout[2].SemanticName = "TEXCOORD";
	layout[2].SemanticIndex = 0;
	layout[2].Format = DXGI_FORMAT_R32G32_FLOAT; // float4
	layout[2].InputSlot = 0;
	layout[2].AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;
	layout[2].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
	layout[2].InstanceDataStepRate = 0;

	numElements = sizeof(layout) / sizeof(layout[0]);

	result = renderState->device->CreateInputLayout(layout, numElements, vertexShaderBuffer->GetBufferPointer(), vertexShaderBuffer->GetBufferSize(), &(shader->layout));
	assert(!FAILED(result));

	vertexShaderBuffer->Release();
	pixelShaderBuffer->Release();


	if (flags & SHADER_FLAG_TEXTURE)
	{
		samplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
		samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
		samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
		samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
		samplerDesc.MipLODBias = 0.0f;
		samplerDesc.MaxAnisotropy = 1;
		samplerDesc.ComparisonFunc = D3D11_COMPARISON_ALWAYS;
		samplerDesc.BorderColor[0] = 0;
		samplerDesc.BorderColor[1] = 0;
		samplerDesc.BorderColor[2] = 0;
		samplerDesc.BorderColor[3] = 0;
		samplerDesc.MinLOD = 0;
		samplerDesc.MaxLOD = D3D11_FLOAT32_MAX;

		result = renderState->device->CreateSamplerState(&samplerDesc, &(shader->samplerState));
		assert(!FAILED(result));
	}
	
}

void EUTS_ShaderConstants_setSceneMatrices(EUTS_ShaderConstants *constants, EUTS_RenderState *renderState, XMMATRIX *viewMatrix, XMMATRIX *projectionMatrix)
{
	HRESULT result;
	D3D11_MAPPED_SUBRESOURCE mappedResource;
	EUTS_VSSceneConstantBuffer *dataPtr;
	unsigned int bufferNumber;
	XMMATRIX view, projection;

	// It's a requirement to transpose the matrices before passing them to the shader
	view = XMMatrixTranspose(*viewMatrix);
	projection = XMMatrixTranspose(*projectionMatrix);

	result = renderState->deviceContext->Map(constants->sceneBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	assert(!FAILED(result));

	dataPtr = (EUTS_VSSceneConstantBuffer*)mappedResource.pData;
	dataPtr->view = view;
	dataPtr->projection = projection;

	renderState->deviceContext->Unmap(constants->sceneBuffer, 0);

	// Set the position of the constant buffer in the vertex shader
	bufferNumber = 0;

	renderState->deviceContext->VSSetConstantBuffers(bufferNumber, 1, &(constants->sceneBuffer));
}

void EUTS_ShaderConstants_setModelMatrix(EUTS_ShaderConstants *constants, EUTS_RenderState *renderState, XMMATRIX *modelMatrix)
{
	HRESULT result;
	D3D11_MAPPED_SUBRESOURCE mappedResource;
	EUTS_VSObjectConstantBuffer *dataPtr;
	unsigned int bufferNumber;
	XMMATRIX model;

	// It's a requirement to transpose the matrices before passing them to the shader
	model = XMMatrixTranspose(*modelMatrix);

	result = renderState->deviceContext->Map(constants->objectBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	assert(!FAILED(result));

	dataPtr = (EUTS_VSObjectConstantBuffer*)mappedResource.pData;
	dataPtr->model = model;

	renderState->deviceContext->Unmap(constants->objectBuffer, 0);

	// Set the position of the constant buffer in the vertex shader
	bufferNumber = 1;

	renderState->deviceContext->VSSetConstantBuffers(bufferNumber, 1, &(constants->objectBuffer));
}

void EUTS_ShaderConstants_setColor(EUTS_ShaderConstants *constants, EUTS_RenderState *renderState, XMFLOAT4 *color)
{
	HRESULT result;
	D3D11_MAPPED_SUBRESOURCE mappedResource;
	EUTS_VSColorConstantBuffer *dataPtr;
	unsigned int bufferNumber;

	result = renderState->deviceContext->Map(constants->colorBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	assert(!FAILED(result));

	dataPtr = (EUTS_VSColorConstantBuffer*)mappedResource.pData;
	dataPtr->color = *color;

	renderState->deviceContext->Unmap(constants->colorBuffer, 0);

	// Set the position of the constant buffer in the vertex shader
	bufferNumber = 2;

	renderState->deviceContext->VSSetConstantBuffers(bufferNumber, 1, &(constants->colorBuffer));
}

void EUTS_ShaderConstants_setLightParameters(EUTS_ShaderConstants *constants, EUTS_RenderState *renderState, XMFLOAT4 *sunDirection, XMFLOAT4 *sunDiffuse, XMFLOAT4 *ambient)
{
	HRESULT result;
	D3D11_MAPPED_SUBRESOURCE mappedResource;
	EUTS_PSLightConstantBuffer *dataPtr;
	unsigned int bufferNumber;

	result = renderState->deviceContext->Map(constants->lightBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	assert(!FAILED(result));

	dataPtr = (EUTS_PSLightConstantBuffer*)mappedResource.pData;
	dataPtr->sunDirection = *sunDirection;
	dataPtr->sunDiffuse = *sunDiffuse;
	dataPtr->ambient = *ambient;

	renderState->deviceContext->Unmap(constants->lightBuffer, 0);

	// Set the position of the constant buffer in the pixel shader
	bufferNumber = 0;

	renderState->deviceContext->PSSetConstantBuffers(bufferNumber, 1, &(constants->lightBuffer));
}

void EUTS_ShaderConstants_setRenderTargetParameters(EUTS_ShaderConstants *constants, EUTS_RenderState *renderState, float width, float height)
{
	HRESULT result;
	D3D11_MAPPED_SUBRESOURCE mappedResource;
	EUTS_PSRenderTargetConstantBuffer *dataPtr;
	unsigned int bufferNumber;
	XMMATRIX view, projection;


	result = renderState->deviceContext->Map(constants->renderTargetBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	assert(!FAILED(result));

	dataPtr = (EUTS_PSRenderTargetConstantBuffer*)mappedResource.pData;
	dataPtr->params.x = width;
	dataPtr->params.y = height;
	dataPtr->params.z = 0.0f;
	dataPtr->params.w = 0.0f;

	renderState->deviceContext->Unmap(constants->renderTargetBuffer, 0);

	// Set the position of the constant buffer in the vertex shader
	bufferNumber = 1;

	renderState->deviceContext->PSSetConstantBuffers(bufferNumber, 1, &(constants->renderTargetBuffer));
}