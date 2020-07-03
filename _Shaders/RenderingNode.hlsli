
//////////////////////////////VSBuffer
struct ViewProjection
{
	matrix View;
	matrix Projection;
	matrix InvView;
};
cbuffer ViewProjection : register(b0)
{
	ViewProjection CB_ViewProjection;
}

struct World
{
	matrix World;
};

cbuffer World : register(b1)
{
	World CB_World;
}
//////////////////////////////PSBuffer
struct Light
{
	float3 lightDirection;
	float specExp;

	float4 ambientLight;

	int isSpecularMap;
	int isNormalMap;
};

cbuffer Light : register(b0)
{
	Light CB_Light;
}

struct MaterialDesc
{
	float4 Ambient;
	float4 Diffuse;
	float4 Specular;
	float4 Emissive;
};
cbuffer Material : register(b1)
{
	MaterialDesc CB_Material;
}
Texture2D diffuseMap : register(t0);
SamplerState diffuseSamp : register(s0);

Texture2D specularMap : register(t1);
SamplerState specularSamp : register(s1);

Texture2D normalMap : register(t2);
SamplerState normalSamp : register(s2);


MaterialDesc MakeMaterial()
{
	MaterialDesc output;
	output.Ambient = float4(0, 0, 0, 0);
	output.Diffuse = float4(0, 0, 0, 0);
	output.Specular = float4(0, 0, 0, 0);
	output.Emissive = float4(0, 0, 0, 0);

	return output;
}

void AddMaterial(inout MaterialDesc result, MaterialDesc val)
{
	result.Ambient += val.Ambient;
	result.Diffuse += val.Diffuse;
	result.Specular += val.Specular;
	result.Emissive += val.Emissive;
}

float3 MaterialToColor(MaterialDesc result)
{
	return (result.Ambient + result.Diffuse + result.Specular + result.Emissive).rgb;
}


float4 WorldPosition(float4 position)
{
	return mul(position, CB_World.World);
}

float4 VPPosition(float4 position)
{
	position = mul(position, CB_ViewProjection.View);
	return mul(position, CB_ViewProjection.Projection);
}

float3 ViewPosition()
{
	return CB_ViewProjection.InvView._41_42_43;
}

//////////////////////////////////
struct PixelInput
{
	float4 Position : SV_POSITION;
	float4 wPosition : POSITION1;
	float2 Uv : UV;
	float3 Normal : Normal;
	float3 Tangent : TANGENT;
	float3 BiNormal : BINORMAL;
	float3 ViewPos : VIEWPOS;
};


float4 ComputeLight(PixelInput input)
{
	float4 output;
	float3 lightDirection = -CB_Light.lightDirection;
	float NdotL = dot(lightDirection, normalize(input.Normal));
    //NdotL = saturate(NdotL);

	float4 Ambient = CB_Light.ambientLight * CB_Material.Ambient;
	float3 E = normalize(input.ViewPos - input.wPosition);
	float4 Diffuse = CB_Material.Diffuse * NdotL;
	float4 Specular = float4(0, 0, 0, 0);
	float4 Emissive = float4(0, 0, 0, 0);
	[flatten]
	if (NdotL > 0.0f)
	{
		[flatten]
		if (any(CB_Material.Specular.rgb))
		{
			float3 R = normalize(reflect(-lightDirection, input.Normal));
			float RdotE = saturate(dot(R, E));

			float specularVal = pow(RdotE, CB_Material.Specular.a);
			Specular = specularVal * CB_Material.Specular;
		}
	}

	[flatten]
	if (any(CB_Material.Emissive.rgb))
	{
		float NdotE = dot(E, normalize(input.Normal));

		float emissiveVal = smoothstep(1.0f - CB_Material.Emissive.a, 1.0f, 1.0f - saturate(NdotE));

		Emissive = CB_Material.Emissive * emissiveVal;
	}
	output = Ambient + Diffuse + Specular + Emissive;

	return output;
}