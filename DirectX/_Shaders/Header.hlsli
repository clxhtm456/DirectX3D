cbuffer ViewProjection : register(b0)
{
	matrix View;
	matrix Projection;
}

cbuffer World : register(b1)
{
	matrix World;
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