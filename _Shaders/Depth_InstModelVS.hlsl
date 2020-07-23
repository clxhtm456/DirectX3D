#include "RenderingNode.hlsli"

cbuffer meshIDbuffer : register(b3) //VS
{
    uint meshID;
}

Texture1D transformMap : register(t0);
Texture2DArray animationMap : register(t1);

cbuffer VPBuffer : register(b10)
{
    matrix DepthView;
    matrix DepthProjection;
}

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
    float4 position : SV_Position;
    float2 uv : TEXCOORD;
    float3 tangent : TANGENT;
    float3 binormal : BINORMAL;
    float3 normal : NORMAL;
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

struct DepthInput
{
    float4 Position : SV_POSITION0;
    float4 sPosition : POSITION1;
};

DepthInput VS(VertexInput input)
{
    DepthInput output;

    float4 Pos = input.position;

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
    
    output.Position = mul(Pos, DepthView);
    output.Position = mul(output.Position, DepthProjection);

    output.sPosition = output.Position;

    return output;
}

void PS(DepthInput input)
{
	float depth = input.Position.z / input.Position.w;
}