#include "RenderingNode.hlsli"
#include "Shadow.hlsl"

SamplerState samplerstate : register(s3);

cbuffer colors : register(b3)//PS
{
	float3 diffuse; // : packoffset(c0.x);엔비디아 샘플에 있는거 참고해 보자
	float bumpscaling;

	float3 ambient;
	float opacity;

	float3 specular;
	float shininess;

	float3 emissive;
	float shininessstrength;

	float3 tranparent;
	float transparentfactor;

	float3 reflective;
	float reflectivity;

	float refracti;

	int opaque;
	int reflector;

	int hasDiffuseMap = 0;

	int hasSpecularMap = 0;
	int hasAmbientMap = 0;
	int hasEmissiveMap = 0;
	int hasHeightMap = 0;

	int hasNormalMap = 0;
	int hasShininessMap = 0;
	int hasOpacityMap = 0;
	int hasDisplacementMap = 0;

	int hasLightMapMap = 0;
	int hasReflectionMap = 0;
	int hasBasecolorMap = 0;
	int hasNormalcameraMap = 0;

	int hasEmissioncolorMap = 0;
	int hasMetalnessMap = 0;
	int hasDiffuseroughnessMap = 0;
	int hasAmbientocculsionMap = 0;

}

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

Texture2D d2dMap : register(t20);

struct PixelInput
{
	float4 position : SV_Position;
	float2 uv : TEXCOORD;
	float3 tangent : TANGENT;
	float3 binormal : BINORMAL;
	float3 normal : NORMAL;
	float3 viewDir : VIEWDIR;
};

float fresnel(float3 eyeVec, float3 normal, float R0)
{
	float kOneMinusEdotN = 1 - abs(dot(eyeVec, normal)); // note: abs() makes 2-sided materials work

	// raise to 5th power
	float result = kOneMinusEdotN * kOneMinusEdotN;
	result = result * result;
	result = result * kOneMinusEdotN;
	result = R0 + (1 - R0) * result;

	return result;
}

// Kelemen and Szirmay-Kalos' approximation to Cook-Torrance model 
//--------------------------------------------------------------------------------------
float SimpleCookTorrance(float3 N, float3 L, float3 V, float m)
{
	float result = 0;
	float LN = dot(L, N);
	if (LN > -0.2)
	{
		float3 h = L + V; // unnormalized half vector 
		float3 H = normalize(h);
		float NH = dot(N, H);
		NH = NH * NH;
		float PH = exp((1. - 1. / NH) / (m * m)) * 1.0 / (m * m * NH * NH);
		float F = fresnel(V, H, 0.028f);
		float frSpec = max(PH * F / dot(h, h), 0.0);
		result = LN * frSpec;
	}
	return result;
}


float4 PS(PixelInput input) : SV_Target
{
	float3 tranparentColor = { 1.0f, 1.0f, 1.0f };
	float alpha = 1.0f;
	[flatten]
	if (!opaque)
	{
		[flatten]
		if (hasOpacityMap)
		{
			tranparentColor = opacityMap.Sample(samplerstate, input.uv).xyz;
			tranparentColor = saturate(tranparentColor * transparentfactor);
		}
		else
		{
			tranparentColor = saturate((opacity - tranparent) * transparentfactor);
		}
		alpha = dot(tranparentColor, float3(1.0f, 1.0f, 1.0f));
		[flatten]
		if (alpha < 0.1f)
			discard;
	}
	float3 diffuseColor = { 0.f, 0.f, 0.f };
	[flatten]
	if (hasDiffuseMap)
	{
		diffuseColor = diffuseMap.Sample(samplerstate, input.uv).xyz;
	}
	else
		diffuseColor = diffuse;

	float3 specularColor = { 0.f, 0.f, 0.f };
	[flatten]
	if (hasSpecularMap)
	{
		specularColor = specularMap.Sample(samplerstate, input.uv).xyz;
		specularColor = saturate(specularColor * specular);
	}
	else
		specularColor = specular;

	float3 ambientColor = { 0.f, 0.f, 0.f };
	[flatten]
	if (hasAmbientMap)
	{
		ambientColor = ambientMap.Sample(samplerstate, input.uv).xyz;
	}
	else
		ambientColor = ambient;

	float3 emissiveColor = { 0.f, 0.f, 0.f };
	[flatten]
	if (hasEmissiveMap)
	{
		emissiveColor = emissiveMap.Sample(samplerstate, input.uv).xyz;
		emissiveColor = saturate(emissiveColor * emissive);
	}
	else
		emissiveColor = emissive;

	[flatten]
	if (reflector)
	{
		float3 reflectionColor = reflectionMap.Sample(samplerstate, input.uv).xyz;
		reflectionColor = reflectionColor * reflective;
	}


	float4 bumps = normalMap.Sample(samplerstate, input.uv);
	bumps = float4(2 * bumps.xyz - float3(1, 1, 1), 0);

	// lighting
	float3 Nn = normalize(input.normal);
	float3 Tn = normalize(input.tangent);
	float3 Bn = normalize(input.binormal);

	float3x3 TangentToWorld = float3x3(Tn, Bn, Nn);
	//bumps.xy *= bumpscaling;
	float3 Nb = mul(bumps.xyz, TangentToWorld);
	Nb = normalize(Nb);

	float3 Vn = input.viewDir;
	float3 Ln = normalize(float3(-1.0f, 1.0f, -1.0f));
	float3 Hn = normalize(Vn + Ln);


	float3 textColor = d2dMap.Sample(samplerstate, input.uv).xyz;

	//float3 result = diffuseColor * Lit.y + specularColor * Lit.y * Lit.z + ambientColor + emissiveColor;
	//float3 result = diffuseColor * (max(dot(Ln, Nb), 0.0f) + specularColor * SimpleCookTorrance(Nb, Ln, Vn, float(1.0f / (0.01f + shininess)))) + ambientColor + emissiveColor;
	float3 result = diffuseColor * diffuse * max(dot(Hn, Nb), 0.1f) * saturate(dot(Vn, Ln) + 1.8f) + specularColor * SimpleCookTorrance(Nb, Ln, Vn, float(1.5f / (0.01f + sqrt(shininess)))) * shininess * 2.0f * saturate(dot(Vn, Ln) + 0.9f) + ambientColor + emissiveColor + textColor;

	result = saturate(result * tranparentColor);
	return float4(result, alpha);
}