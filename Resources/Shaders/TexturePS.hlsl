Texture2D shaderTexture;
SamplerState samplerState;

struct PixelInput
{
	float4 position : SV_POSITION;
	float2 texCoord : TEXCOORD;
	float4 color : COLOR;
};

float4 main(PixelInput input) : SV_TARGET
{
	float4 output = shaderTexture.Sample(samplerState, input.texCoord);
	return output;
}