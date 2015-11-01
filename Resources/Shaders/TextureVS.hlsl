cbuffer SceneBuffer
{
	matrix viewMatrix;
	matrix projectionMatrix;
};

cbuffer ObjectBuffer
{
	matrix modelMatrix;
};

struct VertexInput
{
	float4 position : POSITION;
	float4 normal : NORMAL;
	float2 texCoord : TEXCOORD;
};

struct PixelInput
{
	float4 position : SV_POSITION;
	float4 normal   : NORMAL;
	float2 texCoord : TEXCOORD;
};


PixelInput main(VertexInput input)
{
	PixelInput output;
	input.position.w = 1.0f; // Because we read only xyz from vertex data
	input.normal.w = 1.0f;

	output.position = mul(input.position, modelMatrix);
	output.position = mul(output.position, viewMatrix);
	output.position = mul(output.position, projectionMatrix);

	output.normal = input.normal;
	output.texCoord = input.texCoord;

	return output;
}