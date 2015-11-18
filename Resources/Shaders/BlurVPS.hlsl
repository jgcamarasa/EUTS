#include "PixelConstants.hlsli"

Texture2D shaderTexture;
SamplerState samplerState;

static const float kernel[5] = {
	0.0545f, 0.2442f, 0.4026f, 0.2442f, 0.0545f
};

struct PixelInput
{
	float4 position : SV_POSITION;
	float2 texCoord : TEXCOORD;
};



float4 main(PixelInput input) : SV_TARGET
{
	float4 output;
	const float pW = 1.0f / renderTargetParams.y;
	float4 c0 = shaderTexture.Sample(samplerState, input.texCoord - float2(0.0f, -2.0*pW));
		float4 c1 = shaderTexture.Sample(samplerState, input.texCoord - float2(0.0f, -1.0*pW));
		float4 c2 = shaderTexture.Sample(samplerState, input.texCoord);
		float4 c3 = shaderTexture.Sample(samplerState, input.texCoord - float2(0.0f, 1.0*pW));
		float4 c4 = shaderTexture.Sample(samplerState, input.texCoord - float2(0.0f, 2.0*pW));
		output = c0 * kernel[0] + c1 * kernel[1] + c2 * kernel[2] + c3 * kernel[3] + c4 * kernel[4];
	//output.a = 1.0f;
	//output = float4(0.5f, 0.5f, 0.5f, 1.0f);
	return output;
}