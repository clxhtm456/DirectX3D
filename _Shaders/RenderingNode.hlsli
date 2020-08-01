
//////////////////////////////VSBuffer
struct ViewProjection
{
	matrix View;
	matrix Projection;
	matrix InvView;
};
cbuffer ViewProjection : register(b0)//VS,PS
{
	ViewProjection CB_ViewProjection;
}

struct World
{
	matrix World;
};

cbuffer World : register(b1)//VS
{
	World CB_World;
}
//////////////////////////////PSBuffer
struct LightDesc
{
	float4 Ambient;
	float4 Specular;
	float3 Direction;
	float Padding;
	float3 Position;

	matrix LightView;
	matrix LightProjection;
};

cbuffer Light : register(b1)//PS
{
	LightDesc CB_Light;
}

struct BMaterialDesc
{
	float4 Diffuse;
	float4 Ambient;
	float4 Specular;
	float4 Emissive;
	float4 Transparent;
	float4 Reflective;

	float Bumpscaling;
	float Opacity;
	float Shininess;
	float Shininessstrength;
	float Transparentfactor;
	float Reflectivity;

	float Refracti;
	int Opaque;
	int Reflector;

	int HasDiffuseMap;
	int HasSpecularMap;
	int HasAmbientMap;
	int HasEmissiveMap;
	int HasHeightMap;
	int HasNormalMap;
	int HasShininessMap;
	int HasOpacityMap;
	int HasDisplacementMap;
	int HasLightMapMap;
	int HasReflectionMap;
	int HasBasecolorMap;
	int HasNormalcameraMap;
	int HasEmissioncolorMap;
	int HasMetalnessMap;
	int HasDiffuseroughnessMap;
	int HasAmbientocculsionMap;
};

struct MaterialDesc
{
	float4 Diffuse;
	float4 Ambient;
	float4 Specular;
	float4 Emissive;
};
cbuffer Material : register(b2)//PS
{
	BMaterialDesc CB_Material;
}
Texture2D diffuseMap : register(t0);
SamplerState linearSamp : register(s0);

Texture2D specularMap : register(t1);

Texture2D normalMap : register(t2);

//Pixel Shader Texture
Texture2D emissiveMap : register(t3);
Texture2D heightMap : register(t4);
Texture2D ambientMap : register(t5);
Texture2D shininessMap : register(t6);
Texture2D opacityMap : register(t7);
Texture2D displacementMap : register(t8);
Texture2D lightMapMap : register(t9);
Texture2D reflectionMap : register(t10);
//pbr
Texture2D basecolorMap : register(t11);
Texture2D normalcameraMap : register(t12);
Texture2D emissioncolorMap : register(t13);
Texture2D metalnessMap : register(t14);
Texture2D diffuseroughnessMap : register(t15);
Texture2D ambientocculsionMap : register(t16);


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



float4 ComputeLight(float3 normal, float3 viewPos, float4 wPosition)
{
	float4 output;
	float3 lightDirection = -CB_Light.Direction;
	float NdotL = dot(lightDirection, normalize(normal));
	//NdotL += 1;
    //NdotL = saturate(NdotL);

	float4 Ambient = CB_Light.Ambient * CB_Material.Ambient;
	float3 E = normalize(viewPos - wPosition);
	float4 Diffuse = CB_Material.Diffuse * NdotL;
	float4 Specular = float4(0, 0, 0, 0);
	float4 Emissive = float4(0, 0, 0, 0);
	[flatten]
	if (NdotL > 0.0f)
	{
		[flatten]
		if (any(CB_Material.Specular.rgb))
		{
			float3 R = normalize(reflect(-lightDirection, normal));
			float RdotE = saturate(dot(R, E));

			float specularVal = pow(RdotE, CB_Material.Specular.a);
			Specular = specularVal * CB_Material.Specular;
		}
	}

	[flatten]
	if (any(CB_Material.Emissive.rgb))
	{
		float NdotE = dot(E, normalize(normal));

		float emissiveVal = smoothstep(1.0f - CB_Material.Emissive.a, 1.0f, 1.0f - saturate(NdotE));

		Emissive = CB_Material.Emissive * emissiveVal;
	}
	output = Ambient + Diffuse + Specular + Emissive;

	return output;
}

void ComputeLight_Material(out MaterialDesc output, float3 normal, float3 wPosition)
{
	output.Ambient = 0;
	output.Diffuse = 0;
	output.Specular = 0;
	output.Emissive = 0;

	float3 direction = -CB_Light.Direction;
	float NdotL = dot(direction, normalize(normal));

	output.Ambient = CB_Light.Ambient * CB_Material.Ambient;
	float3 E = normalize(ViewPosition() - wPosition);

	output.Diffuse = NdotL * CB_Material.Diffuse;

	[flatten]
	if (NdotL > 0.0f)
	{
		[flatten]
		if (any(CB_Material.Specular.rgb))
		{
			float3 R = normalize(reflect(-direction, normal));
			float RdotE = saturate(dot(R, E));

			float specular = pow(RdotE, CB_Material.Specular.a);
			output.Specular = specular*(CB_Material.Specular) * CB_Light.Specular;
		}
	}

	[flatten]
	if (any(CB_Material.Emissive.rgb))
	{
		float NdotE = dot(E, normalize(normal));

		float emissive = smoothstep(1.0f - CB_Material.Emissive.a, 1.0f, 1.0f - saturate(NdotE));

		output.Emissive = CB_Material.Emissive* emissive;
	}

}

void NormalMapping(inout float4 diffuse, float2 uv, float3 normal, float3 tangent, SamplerState samp)
{
	float4 map = normalMap.Sample(samp, uv);
	float3 direction = -CB_Light.Direction;
	float4 result = float4(0, 0, 0, 0);

	[flatten]
	if (any(map) == false)
		return;

	float3 N = normalize(normal);
	float3 T = normalize(tangent - dot(tangent, N) * N);
	float3 B = cross(N, T);
	float3x3 TBN = float3x3(T, B, N);


	float3 coord = map.rgb * 2.0f - 1.0f;

	coord = mul(coord, TBN);

	diffuse *= saturate(dot(coord, direction));
}

void NormalMapping(inout float4 diffuse, float2 uv, float3 normal, float3 tangent)
{
	NormalMapping(diffuse, uv, normal, tangent, linearSamp);

}

void Texture(inout float4 color, Texture2D t, float2 uv, SamplerState samp)
{
	float4 sampling = t.Sample(samp, uv);

	color = sampling;
}

void Texture(inout float4 color, Texture2D t, float2 uv)
{
	Texture(color, t, uv, linearSamp);
}
