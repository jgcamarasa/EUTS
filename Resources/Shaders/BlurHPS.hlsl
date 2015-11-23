#include "PixelConstants.hlsli"

Texture2D blurredTexture : register(t0);
SamplerState blurredSampler : register(s0);

Texture2D sceneTexture : register(t1);
SamplerState sceneSampler : register(s1);

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
	float4 blurred;
	float4 scene;
	float4 output;
	float4 c0, c1, c2, c3, c4;
	float r;
	const float pW = 1.0f / renderTargetParams.x;
	c0 = blurredTexture.Sample(blurredSampler, input.texCoord - float2(-2.0*pW, 0.0f));
	c1 = blurredTexture.Sample(blurredSampler, input.texCoord - float2(-1.0*pW, 0.0f));
	c2 = blurredTexture.Sample(blurredSampler, input.texCoord);
	c3 = blurredTexture.Sample(blurredSampler, input.texCoord - float2(1.0*pW, 0.0f));
	c4 = blurredTexture.Sample(blurredSampler, input.texCoord - float2(2.0*pW, 0.0f));
	blurred = c0 * kernel[0] + c1 * kernel[1] + c2 * kernel[2] + c3 * kernel[3] + c4 * kernel[4];
	scene = sceneTexture.Sample(sceneSampler, input.texCoord);

	r = input.texCoord.y * 2.0f - 1.0f;
	r = abs(r);
	r = clamp(r, 0.0f, 1.0f);

	output = float4(blurred.rgb*blurred.a*renderTargetParams.z + scene.rgb, 1.0f);
	//output = float4(r, 0.0f, 0.0f, 1.0f);
	return output;
}