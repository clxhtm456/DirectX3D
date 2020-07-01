

cbuffer ViewProjection : register(b0)
{
	matrix View;
	matrix Projection;
	matrix InvView;
}

cbuffer World : register(b1)
{
	matrix World;
}

cbuffer Light : register(b0)
{
	float3 lightDirection;
	float specExp;

	float4 ambientLight;

	int isSpecularMap;
	int isNormalMap;
}

cbuffer Material : register(b1)
{
	float4 mAmbient;
	float4 mDiffuse;
	float4 mSpecular;
	float4 mEmissive;
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