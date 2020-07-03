#include "RenderingNode.hlsli"

float4 PS(PixelInput input) : SV_TARGET0
{
	float4 Red = float4(1, 0, 0, 1);
	//float4 emissive = ComputeEmissive(input.Normal, input.wPosition);
	return Red;
}