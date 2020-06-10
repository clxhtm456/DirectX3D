#include "00_Global.fx"
#include "00_Deffered.fx"
#include "00_Model.fx"
#include "00_Sky.fx"



/////////////////////////////////////////////////////////////////////////////////////////////////
//PreRender
/////////////////////////////////////////////////////////////////////////////////////////////////

float4 PS(MeshOutput input) : SV_Target
{
    return PS_Shadow(input, PS_AllLight(input));
}

cbuffer CB_Water
{
    float4 ReflectionColor;

    float2 NormalMapTile;
    float WaveTranslation;
    float WaveScale;

    float WaterShiness;
    float WaterAlpha;
};

struct VertexOutput_Water
{
    float4 Position : SV_Position;
    float3 wPosition : Position1;

    float4 ReflectionPosition : Position2;
    float4 RefractionPosition : Position3;

    float2 Uv : Uv;
    float2 Uv2 : Uv1;
};

VertexOutput_Water VS_Water(VertexTexture input)
{
    VertexOutput_Water output;
    output.Position = WorldPosition(input.Position);
    output.wPosition = output.Position.xyz;

    output.Position = ViewProjection(output.Position);

    output.ReflectionPosition = WorldPosition(input.Position);
    output.ReflectionPosition = mul(output.ReflectionPosition, Reflection);
    output.ReflectionPosition = mul(output.ReflectionPosition, Projection);

    output.Uv = input.Uv/NormalMapTile.x;
    output.Uv2 = input.Uv / NormalMapTile.y;

    output.RefractionPosition = output.Position;

    return output;
}

float4 PS_Water(VertexOutput_Water input) : SV_Target
{
    input.Uv.y += WaveTranslation;
    input.Uv2.x += WaveTranslation;
    float4 normalMap = NormalMap.Sample(LinearSampler, input.Uv) * 2.0f- 1.0f;
    float4 normalMap2 = NormalMap.Sample(LinearSampler, input.Uv2) * 2.0f - 1.0f;

    float3 normal = (normalMap.rgb + normalMap2.rgb);

    float2 reflection;
    reflection.x = input.ReflectionPosition.x / input.ReflectionPosition.w * 0.5f + 0.5f;
    reflection.y = -input.ReflectionPosition.y / input.ReflectionPosition.w * 0.5f + 0.5f;
    reflection = reflection + (normal.xy * WaveScale);
    float4 reflectionColor = ReflectionMap.Sample(LinearSampler, reflection);
    //return float4(reflectionColor.rgb, 1);

    float2 refraction;
    refraction.x = input.ReflectionPosition.x / input.ReflectionPosition.w * 0.5f + 0.5f;
    refraction.y = input.ReflectionPosition.y / input.ReflectionPosition.w * 0.5f + 0.5f;
    refraction = refraction + (normal.xy * WaveScale);
    float4 refractionColor = saturate(RefractionMap.Sample(LinearSampler, refraction) + ReflectionColor);

    //return float4(refractionColor.rgb, 1);

    float3 light = GlobalLight.Direction;
    light.y *= -1.0f;
    light.z *= -1.0f;

    //--
    float3 view = normalize(ViewPosition() - input.wPosition);
    float3 heightView = view.yyy;

    float r = (1.2f - 1.0f) / (1.2f + 1.0f);
    float fresnel = max(0, min(1, r + (1 - r) * pow(1 - dot(normal, heightView), 2)));
    //--
    //float3 diffuse = float3(0.0f, 0.0f, 1.0f);
    float3 diffuse = lerp(reflectionColor, refractionColor, fresnel).rgb ;
    float3 R = normalize(reflect(light, normal));
    float specular = saturate(dot(R, view));

    [flatten]
    if(specular > 0.0f)
    {
        specular = pow(specular, WaterShiness);
        diffuse = saturate(diffuse + specular);
    }

    float4 color = CalculateFogColor(float4(diffuse, 1), input.wPosition);
    return float4(diffuse.rgb, WaterAlpha);

}
technique11 T0
{
//Deffered - Depth
    P_RS_VP(P0, FrontCounterClockwise_True, VS_Depth_Mesh, PS_Depth)
    P_RS_VP(P1, FrontCounterClockwise_True, VS_Depth_Model, PS_Depth)
    P_RS_VP(P2, FrontCounterClockwise_True, VS_Depth_Animation, PS_Depth)

//Sky
    P_VP(P3, VS_Scattering, PS_Scattering)
    P_DSS_VP(P4, SkyDepthStencil, VS_Dome, PS_Dome)
    P_BS_VP(P5,AlphaBlend, VS_Moon, PS_Moon)
    P_BS_VP(P6, AlphaBlend, VS_Cloud, PS_Cloud)

//Render
    P_VP(P7, VS_Mesh,PS)
    P_VP(P8, VS_Model, PS)
    P_VP(P9, VS_Animation, PS)

//PreRender - Water
    P_VP(P10,VS_PreRender_Reflection_Dome,PS_Dome)
    P_BS_VP(P11,AlphaBlend, VS_PreRender_Reflection_Moon, PS_Moon)
    P_BS_VP(P12, AlphaBlend, VS_PreRender_Reflection_Cloud, PS_Cloud)
    
    P_VP(P13,VS_PreRender_Reflection_Mesh,PS)
    P_VP(P14, VS_PreRender_Reflection_Model, PS)
    P_VP(P15, VS_PreRender_Reflection_Animation, PS)

//Water
    P_BS_VP(P16, AlphaBlend, VS_Water, PS_Water)
}