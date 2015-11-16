#include "VertexConstants.hlsli"

struct VertexInput
{
	float4 position : POSITION;
	float4 normal : NORMAL;
	float2 texCoord : TEXCOORD;
};

struct PixelInput
{
	float4 position : SV_POSITION;
	float2 texCoord : TEXCOORD;
};


PixelInput main(VertexInput input)
{
	PixelInput output;
	input.position.w = 1.0f; // Because we read only xyz from vertex data
	output.position = input.position;
	output.texCoord = input.texCoord;

	return output;
}