#include "RenderingNode.hlsli"
#include "Shadow.hlsl"

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
	float3 lightDirection = -CB_Light.Direction;
	float NdotL = dot(lightDirection, normalize(input.normal));

    float3 tranparentColor = { 1.0f, 1.0f, 1.0f };
    float alpha = 1.0f;
	[flatten]
    if (!CB_Material.Opaque)
    {
		[flatten]
        if (CB_Material.HasOpacityMap)
        {
            tranparentColor = opacityMap.Sample(linearSamp, input.uv).xyz;
            tranparentColor = saturate(tranparentColor * CB_Material.Transparentfactor);
        }
        else
        {
            tranparentColor = saturate((CB_Material.Opacity - CB_Material.Transparent) * CB_Material.Transparentfactor);
        }
        alpha = dot(tranparentColor, float3(1.0f, 1.0f, 1.0f));
		[flatten]
        if (alpha < 0.001f)
            discard;
    }
    float3 diffuseColor = { 0.f, 0.f, 0.f };
	[flatten]
    if (CB_Material.HasDiffuseMap)
    {
        diffuseColor = diffuseMap.Sample(linearSamp, input.uv).xyz;
    }
    else
        diffuseColor = CB_Material.Diffuse;

	diffuseColor *= NdotL;
    float3 specularColor = { 0.f, 0.f, 0.f };
	[flatten]
    if (CB_Material.HasSpecularMap)
    {
        specularColor = specularMap.Sample(linearSamp, input.uv).xyz;
        specularColor = saturate(specularColor * CB_Material.Specular);
    }
    else
        specularColor = CB_Material.Specular;

    float3 ambientColor = { 0.f, 0.f, 0.f };
	[flatten]
    if (CB_Material.HasAmbientMap)
    {
        ambientColor = ambientMap.Sample(linearSamp, input.uv).xyz;
    }
    else
        ambientColor = CB_Material.Ambient;

    float3 emissiveColor = { 0.f, 0.f, 0.f };
	[flatten]
    if (CB_Material.HasEmissiveMap)
    {
        emissiveColor = emissiveMap.Sample(linearSamp, input.uv).xyz;
        emissiveColor = saturate(emissiveColor * CB_Material.Emissive);
    }
    else
        emissiveColor = CB_Material.Emissive;

	[flatten]
    if (CB_Material.Reflector)
    {
        float3 reflectionColor = reflectionMap.Sample(linearSamp, input.uv).xyz;
        reflectionColor = reflectionColor * CB_Material.Reflective;
    }


    float4 bumps = normalMap.Sample(linearSamp, input.uv);
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


    float3 textColor = d2dMap.Sample(linearSamp, input.uv).xyz;

	//float3 result = diffuseColor * Lit.y + specularColor * Lit.y * Lit.z + ambientColor + emissiveColor;
	float3 result = diffuseColor * (max(dot(Ln, Nb), 0.0f) + specularColor * SimpleCookTorrance(Nb, Ln, Vn, float(1.0f / (0.01f + CB_Material.Shininess)))) + ambientColor + emissiveColor;
    //float3 result = diffuseColor * CB_Material.Diffuse * max(dot(Hn, Nb), 0.1f) * saturate(dot(Vn, Ln) + 1.8f) + specularColor * SimpleCookTorrance(Nb, Ln, Vn, float(1.5f / (0.01f + sqrt(CB_Material.Shininess)))) * CB_Material.Shininess * 2.0f * saturate(dot(Vn, Ln) + 0.9f) + ambientColor + emissiveColor + textColor;


    result = saturate(result * tranparentColor);

    return float4(result, alpha);

   // return float4(diffuseMap.Sample(linearSamp, input.uv).xyz, 1.0f);
}