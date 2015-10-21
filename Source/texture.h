#pragma once
#include "render.h"

// Represents a texture in GPU memory
struct EUTS_Texture
{
	int width;
	int height;
	int bytesPerPixel;
	ID3D11Texture2D* texture;
	ID3D11ShaderResourceView* textureView;
#ifdef _DEBUG
	char fileName[100]; // for debugging purposes?
#endif
};

void EUTS_Texture_load(EUTS_Texture *texture, EUTS_RenderState *renderState, const char *fileName);
void EUTS_Texture_delete(EUTS_Texture *texture);
