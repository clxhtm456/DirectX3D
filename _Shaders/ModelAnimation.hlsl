#include "RenderingNode.hlsli"

#define MAX_BONE 256

cbuffer Bones : register(b2)
{
	matrix bones[MAX_BONE];

	int index;
	int isUseBlend;
}

cbuffer BoneIndex : register(b3)
{
	int boneIndex;
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


struct VertexInput
{
	float4 position : POSITION;
	float2 uv : UV;
	float3 normal : NORMAL;
	float3 tangent : TANGENT;
	float4 blendIndices : BLENDINDICES;
	float4 blendWeights : BLENDWEIGHTS;
};

PixelInput VS(VertexInput input)
{
	PixelInput output;
	matrix boneWorld = SkinWorld(input.blendIndices, input.blendWeights);
	//matrix boneWorld = bones[boneIndex];

	output.Position = mul(input.position, boneWorld);
	output.Position = VPPosition(output.Position);
	//output.Position = input.position;

	output.ViewPos = ViewPosition();

	output.Uv = input.uv;

	output.Normal = mul(input.normal, (float3x3) boneWorld);
	output.Tangent = mul(input.tangent, (float3x3) boneWorld);
	output.BiNormal = cross(output.Normal, output.Tangent);

	return output;
}

float4 PS(PixelInput input) : SV_TARGET
{
float3 diffuse = diffuseMap.Sample(diffuseSamp, input.Uv).rgb;
	float4 result = ComputeLight(input.Normal, input.ViewPos, input.wPosition);
	//float4 emissive = ComputeEmissive(input.Normal, input.wPosition);
	//return float4(1, 1, 0, 1);
	return float4(diffuse,1) * result;
}