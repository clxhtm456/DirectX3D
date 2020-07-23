#include "RenderingNode.hlsli"
#include "Shadow.hlsl"


cbuffer meshIDbuffer : register(b3)//VS
{
	uint meshID;
}

Texture1D transformMap : register(t0);
Texture2DArray animationMap : register(t1);

struct VertexInput
{
	float4 position : POSITION;
	float2 uv : TEXCOORD;
	float3 normal : NORMAL;
	float3 tangent : TANGENT;
	uint4 boneid : BLENDINDICES;
	float4 weights : BLENDWEIGHTS;

	uint instanceID : SV_InstanceID;

	matrix Transform : INSTANCE;
};

struct VertexOutput
{
    float4 position : SV_POSITION;
    float4 wposition : POSITION1;
	float2 uv : UV;
	float3 normal : NORMAL;
	float3 tangent : TANGENT;
	float3 binormal : BINORMAL;
	float3 viewDir : VIEWDIR;
};


float4x4 hierarchyMatrix(uint id, uint frame, uint instanceID)
{
	return matrix(animationMap.Load(int4(id * 4 + 0, frame, instanceID, 0)),
		animationMap.Load(int4(id * 4 + 1, frame, instanceID, 0)),
		animationMap.Load(int4(id * 4 + 2, frame, instanceID, 0)),
		animationMap.Load(int4(id * 4 + 3, frame, instanceID, 0)));
}

float4x4 transformMatrix(uint instanceID)
{
	return matrix(transformMap.Load(int2(instanceID * 4 + 0, 0)),
		transformMap.Load(int2(instanceID * 4 + 1, 0)),
		transformMap.Load(int2(instanceID * 4 + 2, 0)),
		transformMap.Load(int2(instanceID * 4 + 3, 0)));
}

VertexOutput VS(VertexInput input)
{
	VertexOutput output;

	float4 Pos = input.position;
	float3 Normal = input.normal;
	float3 Tangent = input.tangent;

	//Pos = mul(Pos, hierarchyMatrix(meshID, 0, input.instanceID));

	//[flatten]
	//if (input.weights.x > 0.0f)
	//{
	//	float LastWeight = 1.0f - (input.weights.x + input.weights.y + input.weights.z);

	//	matrix skinWorld = hierarchyMatrix(input.boneid.x, 0, input.instanceID) * input.weights.x
	//		+ hierarchyMatrix(input.boneid.y, 0, input.instanceID) * input.weights.y
	//		+ hierarchyMatrix(input.boneid.z, 0, input.instanceID) * input.weights.z
	//		+ hierarchyMatrix(input.boneid.w, 0, input.instanceID) * LastWeight;

	//	Pos = mul(Pos, skinWorld);

	//	float3x3 normalSkinWorld = float3x3(normalize(skinWorld._11_12_13), normalize(skinWorld._21_22_23), normalize(skinWorld._31_32_33));
	//	Normal = mul(Normal, normalSkinWorld);
	//	Tangent = mul(Tangent, normalSkinWorld);
	//}

	//matrix world = transformMatrix(input.instanceID);
	//Pos = mul(Pos, world);
    Pos = mul(Pos, input.Transform);
	output.viewDir = normalize(ViewPosition() - Pos.xyz);

	//float3x3 normalWorld = float3x3(normalize(world._11_12_13), normalize(world._21_22_23), normalize(world._31_32_33));
    float3x3 normalWorld = float3x3(normalize(input.Transform._11_12_13), normalize(input.Transform._21_22_23), normalize(input.Transform._31_32_33));

	output.position = mul(Pos, CB_ViewProjection.View);
	output.position = mul(output.position, CB_ViewProjection.Projection);
    output.wposition = output.position;

	output.uv = input.uv;

	output.normal = normalize(mul(Normal, normalWorld));
	output.tangent = normalize(mul(Tangent, normalWorld));
	output.binormal = normalize(cross(Normal, Tangent));

	return output;
}