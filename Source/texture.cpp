#include "texture.h"
#include <stdio.h>
#include "render.h"
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"


void EUTS_Texture_load(EUTS_Texture *texture, EUTS_RenderState *renderState, const char *fileName)
{
	int width, height, components;
	unsigned char *data = stbi_load(fileName, &width, &height, &components, 0);
	assert(data != NULL);
	assert(components == 4); // TODO: suport RGB?
	texture->width = width;
	texture->height = height;
	texture->bytesPerPixel = components;

	D3D11_TEXTURE2D_DESC textureDesc;
	HRESULT result;
	D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc;

	textureDesc.Height = height;
	textureDesc.Width = width;
	textureDesc.MipLevels = 0;
	textureDesc.ArraySize = 1;
	textureDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	textureDesc.SampleDesc.Count = 1;
	textureDesc.SampleDesc.Quality = 0;
	textureDesc.Usage = D3D11_USAGE_DEFAULT;
	textureDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_RENDER_TARGET;
	textureDesc.CPUAccessFlags = 0;
	textureDesc.MiscFlags = D3D11_RESOURCE_MISC_GENERATE_MIPS;

	result = renderState->device->CreateTexture2D(&textureDesc, NULL, &(texture->texture));
	assert(!FAILED(result));

	int rowPitch = (width * 4);
	renderState->deviceContext->UpdateSubresource(texture->texture, 0, NULL, data, rowPitch, 0);

	srvDesc.Format = textureDesc.Format;
	srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	srvDesc.Texture2D.MostDetailedMip = 0;
	srvDesc.Texture2D.MipLevels = -1;

	result = renderState->device->CreateShaderResourceView(texture->texture, &srvDesc, &(texture->textureView));
	assert(!FAILED(result));
	
	renderState->deviceContext->GenerateMips(texture->textureView);

	stbi_image_free(data);
}

// Should we really be able to finalize a texture?
void EUTS_Texture_delete(EUTS_Texture *texture)
{
	texture->textureView->Release();
	texture->texture->Release();
}