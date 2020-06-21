#include "Header.hlsli"

struct VertexInput
{
	float4 Position : Position0;
};

struct VertexOutput
{
	float4 Position : SV_Position;
};


VertexOutput VS(VertexInput input)
{
	VertexOutput output;
	output.Position = mul(input.Position, view);
	output.Position = mul(output.Position, projection);

	return output;
}

float4 PS(float4 input : SV_Position) : SV_Target
{
    return float4(1, 0, 0, 1);
}