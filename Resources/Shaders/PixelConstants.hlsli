cbuffer LightConstants : register(b0)
{
	float3 sunDirection;
	float3 sunDiffuse;
	float3 ambient;
};

cbuffer RenderTargetConstants : register(b1)
{
	float4 renderTargetParams; // w, h
};