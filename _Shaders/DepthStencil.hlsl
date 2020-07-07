#include "RenderingNode.hlsli"

cbuffer VPBuffer : register(b2)
{
	matrix DepthView;
	matrix DepthProjection;
}
//cbuffer WorldBuffer : register(b1)
//{
//	matrix DepthWorld;
//}

struct VertexInput
{
	float4 Position : POSITION;

	matrix Transform : INSTANCE;
};

struct DepthInput
{
	float4 Position : SV_POSITION0;
	float4 sPosition : POSITION1;
};


DepthInput VS(VertexInput input)
{
	DepthInput output;

	input.Position.w = 1.0f;

	output.Position = mul(input.Position,input.Transform);
	output.Position = mul(output.Position, DepthView);
	output.Position = mul(output.Position, DepthProjection);

	output.sPosition = output.Position;

	return output;
}

float4 PS(DepthInput input) : SV_TARGET
{
	float depth = input.Position.z / input.Position.w;

	return float4(depth, depth, depth, 1.0f);
}