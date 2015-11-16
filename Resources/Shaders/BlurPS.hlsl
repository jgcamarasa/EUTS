#include "PixelConstants.hlsli"

Texture2D shaderTexture;
SamplerState samplerState;

struct PixelInput
{
	float4 position : SV_POSITION;
	float2 texCoord : TEXCOORD;
};

float4 main(PixelInput input) : SV_TARGET
{
	float4 texValue = shaderTexture.Sample(samplerState, input.texCoord);
	texValue.a = 1.0f;
	texValue.b = texValue.r	;
	return texValue;
}