#include "Header.hlsli"

struct VertexInput
{
	float4 Position : POSITION;
};


void VS(VertexInput input,	out float4 position : SV_POSITION)
{
	position = ViewProjection(input.Position);
}

float4 PS(float4 input : SV_POSITION) : SV_TARGET
{
    return float4(1, 0, 0, 1);
}