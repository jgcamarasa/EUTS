cbuffer SceneConstants : register(b0)
{
	matrix viewMatrix;
	matrix projectionMatrix;
};

cbuffer ObjectConstants : register(b1)
{
	matrix modelMatrix;
};

cbuffer ColorConstants : register(b2)
{
	float4 color;
};

