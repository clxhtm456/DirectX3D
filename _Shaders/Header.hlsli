cbuffer ViewProjection : register(b8)
{
	matrix View;
	matrix Projection;
}

cbuffer World : register(b9)
{
	matrix World;
}

cbuffer Light : register(b8)
{
	float3 lightDirection;
	float specExp;

	float4 ambientLight;

	int isSpecularMap;
	int isNormalMap;
}

float4 WorldPosition(float4 position)
{
	return mul(position, World);
}

float4 ViewProjection(float4 position)
{
	position = mul(position, View);
	return mul(position, Projection);
}