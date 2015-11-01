
struct PixelInput
{
	float4 position : SV_POSITION;
	float4 normal   : NORMAL;
	float2 texCoord : TEXCOORD;
	float4 color	: COLOR;
};

float4 main(PixelInput input) : SV_TARGET
{
	return input.color;
}