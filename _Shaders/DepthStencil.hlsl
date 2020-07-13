#include "RenderingNode.hlsli"

cbuffer Bones : register(b2)
{
    matrix bones[256];

    int index;
    int isUseBlend;
}
matrix SkinWorld(float4 indices, float4 weights)
{
    matrix transform = 0;
    transform += mul(weights.x, bones[(uint) indices.x]);
    transform += mul(weights.y, bones[(uint) indices.y]);
    transform += mul(weights.z, bones[(uint) indices.z]);
    transform += mul(weights.w, bones[(uint) indices.w]);

    return transform;
}


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
    float4 blendIndices : BLENDINDICES;
    float4 blendWeights : BLENDWEIGHTS;

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

    //matrix boneWorld = SkinWorld(input.blendIndices, input.blendWeights);

    //input.Transform = mul(input.Transform, boneWorld);

	output.Position = mul(input.Position,input.Transform);
	output.Position = mul(output.Position, DepthView);
	output.Position = mul(output.Position, DepthProjection);

	output.sPosition = output.Position;

	return output;
}

void PS(DepthInput input)
{
	float depth = input.Position.z / input.Position.w;
}