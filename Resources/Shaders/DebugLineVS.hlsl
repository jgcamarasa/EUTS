cbuffer MatrixBuffer
{
	matrix modelMatrix;
	matrix viewMatrix;
	matrix projectionMatrix;
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

	output.position = mul(input.position, modelMatrix);
	output.position = mul(output.position, viewMatrix);
	output.position = mul(output.position, projectionMatrix);

	output.normal = float4(0.0f, 0.0f, 0.0f, 0.0f);
	output.texCoord = float2(0.0f, 0.0f);

	return output;
}