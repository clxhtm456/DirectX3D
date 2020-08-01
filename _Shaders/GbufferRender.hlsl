#include "RenderingNode.hlsli"
#include "Shadow.hlsl"

Texture2D GBufferMaps[6] : register(t9);




void ComputeLight_Deffered(out MaterialDesc output, MaterialDesc material, float3 normal, float3 wPosition)
{
    output.Ambient = 0;
    output.Diffuse = 0;
    output.Specular = 0;
    output.Emissive = 0;

    float3 direction = -CB_Light.Direction;
    float NdotL = dot(direction, normalize(normal));

    output.Ambient = CB_Light.Ambient * material.Ambient;
    float3 E = normalize(ViewPosition() - wPosition);
    
	output.Diffuse = NdotL * float4(1,1,1,1);

    [flatten]
    if (NdotL > 0.0f)
    {
        [flatten]
        if (any(material.Specular.rgb))
        {
            float3 R = normalize(reflect(-direction, normal));
            float RdotE = saturate(dot(R, E));

            float specular = pow(RdotE, material.Specular.a);
			output.Specular = specular*(material.Specular) *CB_Light.Specular;
        }
    }

    [flatten]
    if (any(material.Emissive.rgb))
    {
        float NdotE = dot(E, normalize(normal));

        float emissive = smoothstep(1.0f - material.Emissive.a, 1.0f, 1.0f - saturate(NdotE));
            
        output.Emissive = material.Emissive* emissive;
    }

}

struct VertexOutput_PackGBuffer
{
    float4 Position : SV_POSITION0;
    float2 Screen : POSITION1;
};

void UnPackGBuffer(inout float4 position, in float2 screen, out MaterialDesc material, out float3 normal, out float3 tangent)
{
    
    material.Ambient = float4(0, 0, 0, 1);
    material.Diffuse = GBufferMaps[1].Load(int3(position.xy, 0));
    material.Specular = GBufferMaps[2].Load(int3(position.xy, 0));
	material.Emissive = GBufferMaps[3].Load(int3(position.xy, 0));

    normal = GBufferMaps[4].Load(int3(position.xy, 0));
    tangent = GBufferMaps[5].Load(int3(position.xy, 0));
    
    //float linearDepth = Deffered.Perspective.z / (depth + Deffered.Perspective.w);

    //position.xy = screen * Deffered.Perspective.xy * linearDepth;
    //position.z = linearDepth;
    //position.w = 1.0f;
    //position = mul(position, ViewInverse);
    float2 xy = 1.0f / float2(CB_ViewProjection.Projection._11, CB_ViewProjection.Projection._22);
    float z = CB_ViewProjection.Projection._43;
    float w = -CB_ViewProjection.Projection._33;
    float depth = GBufferMaps[0].Load(int3(position.xy, 0)).r;

    float linearDepth = z / (depth + w);

    position.xy = screen * xy * linearDepth;
    position.z = linearDepth;
    position.w = 1.0f;
    position = mul(position, CB_ViewProjection.InvView);

}



VertexOutput_PackGBuffer VS(uint VertexID : SV_VertexID)
{
    VertexOutput_PackGBuffer output;
    
	const float2 NDC[4] = { float2(-1, +1), float2(+1, +1), float2(-1, -1), float2(+1, -1) };

	/*output.Screen = float2((VertexID << 1) & 2, VertexID & 2);
	output.Position = float4(output.Screen * float2(2.0f, -2.0f) + float2(-1.0f, 1.0f), 0.0f, 1.0f);*/

    output.Position = float4(NDC[VertexID].xy, 0, 1);
    output.Screen = output.Position.xy;

    return output;
}


float4 PS(VertexOutput_PackGBuffer input) : SV_Target0
{
    float4 position = input.Position;
    MaterialDesc material;
    float3 normal;
    float3 tangent;
    UnPackGBuffer(position, input.Screen, material, normal, tangent);

    //float4 color = material.Diffuse * dot(-GlobalLight.Direction, normalize(normal));//����Ʈ �������


	MaterialDesc light = MakeMaterial();

    ComputeLight_Deffered(light, material, normal, position.xyz);

    float4 sPosition = mul(position, ShadowView);
    sPosition = mul(sPosition, ShadowProjection);

	float3 lightV = (light.Diffuse + light.Ambient + light.Specular + light.Emissive).rgb;
	float3 result = material.Diffuse;
    float4 color = float4(result*lightV, 1.0f);
    //float4 color = result.Diffuse;

    /*ShadowPixelInput shadowinput = (ShadowPixelInput) 0;

    shadowinput.sPosition = position;

    color = PS_Shadow(shadowinput, color);*/

    //if (FogType == 0)
    //    color = LinearFogBlend(color, position);
    //else if (FogType == 1)
    //    color = ExpFogBlend(color, position);
    //else if (FogType == 2)
    //    color = Exp2FogBlend(color, position);
    //else
        //color = ExpFogBlend(color, position);
    //if (FogType == 0)
    //{
    //    color = LinearFogBlend(color, position);
    //}else
    //{
    //    color = ExpFogBlend(color, position);

    //}
        

    //ComputePointLight(output, material, normal, position.xyz);
    //AddMaterial(result, output);

    //ComputeSpotLight(output, material, normal, position.xyz);
    //AddMaterial(result, output);
    return color;

}

