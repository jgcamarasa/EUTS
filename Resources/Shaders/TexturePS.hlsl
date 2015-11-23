#include "PixelConstants.hlsli"

Texture2D shaderTexture : register(t0);
SamplerState samplerState : register(s0);

struct PixelInput
{
	float4 position : SV_POSITION;
	float4 normal   : NORMAL;
	float2 texCoord : TEXCOORD;
};

float4 main(PixelInput input) : SV_TARGET
{
	float3 lightOutput = float3(0.0f, 0.0f, 0.0f);
	float4 texValue = shaderTexture.Sample(samplerState, input.texCoord);

	float3 sunDirectionNormalized = normalize(sunDirection);
	
	lightOutput += ambient;
	float cosTi = saturate(dot(input.normal.xyz, sunDirectionNormalized));
	lightOutput += sunDiffuse * cosTi;
	
	saturate(lightOutput);

	float4 output = float4(lightOutput, 0.0f) * texValue;
	return output;
}