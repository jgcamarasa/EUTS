cbuffer MatrixBuffer
{
	matrix modelMatrix;
	matrix viewMatrix;
	matrix projectionMatrix;
};

struct VertexInput
{
	float4 position : POSITION;
	float4 color : COLOR;
	float2 texCoord : TEXCOORD;
};

struct PixelInput
{
	float4 position : SV_POSITION;
	float4 color : COLOR;
	float2 texCoord : TEXCOORD;
};

PixelInput main(VertexInput input)
{
	PixelInput output;
	input.position.w = 1.0f; // Because we read only xyz from vertex data
	
	output.position = mul(input.position, modelMatrix);
	output.position = mul(output.position, viewMatrix);
	output.position = mul(output.position, projectionMatrix);

	output.color = input.color;
	output.texCoord = input.texCoord;

	return output;
}