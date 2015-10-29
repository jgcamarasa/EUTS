
struct PixelInput
{
	float4 position : SV_POSITION;
	float4 normal   : NORMAL;
	float2 texCoord : TEXCOORD;
};

float4 main(PixelInput input) : SV_TARGET
{
	float4 output = float4(1.0f, 0.0f, 0.0f, 1.0f);
	return output;
}