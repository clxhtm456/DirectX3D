#include "RenderingNode.hlsli"


cbuffer VPBuffer : register(b10)
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

    output.Position = mul(input.Position, CB_World.World);
	output.Position = mul(output.Position, DepthView);
	output.Position = mul(output.Position, DepthProjection);

	output.sPosition = output.Position;

	return output;
}

void PS(DepthInput input)
{
	float depth = input.Position.z / input.Position.w;
}