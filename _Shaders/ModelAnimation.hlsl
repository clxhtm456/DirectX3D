#include "RenderingNode.hlsli"
#include "Shadow.hlsl"

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

    matrix Transform : INSTANCE;
};

struct ShadowVertexInput
{
    float4 Position : POSITION;
    float2 Uv : UV;
    float3 Normal : Normal;
    float3 Tangent : TANGENT;
    float4 blendIndices : BLENDINDICES;
    float4 blendWeights : BLENDWEIGHTS;

    matrix Transform : INSTANCE;
};

ShadowPixelInput VS_Shadow(ShadowVertexInput input)
{
    ShadowPixelInput output;
    matrix boneWorld = SkinWorld(input.blendIndices, input.blendWeights);
    boneWorld = mul(boneWorld, input.Transform);
    
    output.Position = mul(input.Position, boneWorld);
    output.wPosition = output.Position;
    output.Position = VPPosition(output.Position);

    output.Uv = input.Uv;
    output.Normal = mul(input.Normal, (float3x3) boneWorld);
    output.Tangent = mul(input.Tangent, (float3x3) boneWorld);
    output.BiNormal = cross(output.Normal, output.Tangent);
    output.ViewPos = ViewPosition();

    output.sPosition = WorldPosition(input.Position);
    output.sPosition = mul(output.sPosition, ShadowView);
    output.sPosition = mul(output.sPosition, ShadowProjection);

    return output;
}

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

float4 ComputeLight2(float3 normal, float3 viewPos, float4 wPosition)
{
    float4 output;
    float3 lightDirection = -CB_Light.Direction;
    float NdotL = dot(lightDirection, normalize(normal));
    NdotL = 1;

    float4 Ambient = CB_Light.Ambient * CB_Material.Ambient;
    float3 E = normalize(viewPos - wPosition);
    float4 Diffuse = CB_Material.Diffuse * NdotL;
    float4 Specular = float4(0, 0, 0, 0);
    float4 Emissive = float4(0, 0, 0, 0);
	//[flatten]
 //   if (NdotL > 0.0f)
 //   {
	//	[flatten]
 //       if (any(CB_Material.Specular.rgb))
 //       {
 //           float3 R = normalize(reflect(-lightDirection, normal));
 //           float RdotE = saturate(dot(R, E));

 //           float specularVal = pow(RdotE, CB_Material.Specular.a);
 //           Specular = specularVal * CB_Material.Specular;
 //       }
 //   }

	//[flatten]
 //   if (any(CB_Material.Emissive.rgb))
 //   {
 //       float NdotE = dot(E, normalize(normal));

 //       float emissiveVal = smoothstep(1.0f - CB_Material.Emissive.a, 1.0f, 1.0f - saturate(NdotE));

 //       Emissive = CB_Material.Emissive * emissiveVal;
 //   }
    output = Ambient + Diffuse + Specular + Emissive;

    return output;
}

float4 PS(PixelInput input) : SV_TARGET
{
float3 diffuse = diffuseMap.Sample(diffuseSamp, input.Uv).rgb;
    float4 result = ComputeLight2(input.Normal, input.ViewPos, input.wPosition);
	//float4 emissive = ComputeEmissive(input.Normal, input.wPosition);
	//return float4(1, 1, 0, 1);
	return float4(diffuse,1) * result;
}