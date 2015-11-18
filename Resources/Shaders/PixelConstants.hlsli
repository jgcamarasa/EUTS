cbuffer LightConstants
{
	float3 sunDirection;
	float3 sunDiffuse;
	float3 ambient;
};

cbuffer RenderTargetConstants
{
	float4 renderTargetParams; // w, h
};