#include "00_Light.fx"

Texture2D GBufferMaps[6];


DepthStencilState Deffered_DepthStencil_State;
RasterizerState Deffered_Rasterizer_State;

///////////////////////////////////////////////////
//PackGBuffer
///////////////////////////////////////////////////



struct VertexOutput_PackGBuffer
{
    float4 Position : SV_Position;
    float2 Screen : Position1;
};


VertexOutput_PackGBuffer VS_Deffered(uint VertexID : SV_VertexID)
{
    VertexOutput_PackGBuffer output;
    
    output.Position = float4(NDC[VertexID].xy, 0, 1);
    output.Screen = output.Position.xy;

    return output;
}

struct PixelOutput_PackGBuffer
{
    float4 Diffuse : SV_Target0;
    float4 Specular : SV_Target1;
    float4 Emissive : SV_Target2;
    float4 Normal : SV_Target3;
    float4 Tangent : SV_Target4;
};


PixelOutput_PackGBuffer PS_PackGBuffer(MeshOutput input)
{
	
    Texture(Material.Diffuse, DiffuseMap, input.Uv);
    NormalMapping(input.Uv, input.Normal, input.Tangent);
    Texture(Material.Specular, SpecularMap, input.Uv);

    PixelOutput_PackGBuffer output;
    
    output.Diffuse = float4(Material.Diffuse);
    output.Specular = Material.Specular;
    output.Emissive = Material.Emissive;
    output.Normal = float4(input.Normal, 1); //-1~1사이 값을 다시 0~1사이 값으로 변환
    output.Tangent = float4(input.Tangent, 1);

    return output;

   // return output;
}

///////////////////////////////////////////////////
//UnPackGBuffer
///////////////////////////////////////////////////


void UnPackGBuffer(inout float4 position, in float2 screen, out MaterialDesc material, out float3 normal, out float3 tangent)
{
    
    material.Ambient = float4(0, 0, 0, 1);
    material.Diffuse = GBufferMaps[1].Load(int3(position.xy, 0));
    material.Specular = GBufferMaps[2].Load(int3(position.xy, 0));
    material.Emissive = GBufferMaps[3].Load(int3(position.xy, 0));

    normal = GBufferMaps[4].Load(int3(position.xy, 0)).rgb;
    tangent = GBufferMaps[5].Load(int3(position.xy, 0)).rgb;
    
    //float linearDepth = Deffered.Perspective.z / (depth + Deffered.Perspective.w);

    //position.xy = screen * Deffered.Perspective.xy * linearDepth;
    //position.z = linearDepth;
    //position.w = 1.0f;
    //position = mul(position, ViewInverse);
    float2 xy = 1.0f / float2(Projection._11, Projection._22);
    float z = Projection._43;
    float w = -Projection._33;
    float depth = GBufferMaps[0].Load(int3(position.xy, 0)).r;

    float linearDepth = z / (depth + w);

    position.xy = screen * xy * linearDepth;
    position.z = linearDepth;
    position.w = 1.0f;
    position = mul(position, ViewInverse);

}

///////////////////////////////////////////////////
//Directional
///////////////////////////////////////////////////

VertexOutput_PackGBuffer VS_Directional(uint VertexID : SV_VertexID)
{
    VertexOutput_PackGBuffer output;
    
    output.Position = float4(NDC[VertexID].xy, 0, 1);
    output.Screen = output.Position.xy;

    return output;
}
void ComputeLight_Deffered(out MaterialDesc output, MaterialDesc material, float3 normal, float3 wPosition)
{
    output.Ambient = 0;
    output.Diffuse = 0;
    output.Specular = 0;
    output.Emissive = 0;

    float3 direction = -GlobalLight.Direction;
    float NdotL = dot(direction, normalize(normal));

    output.Ambient = GlobalLight.Ambient * material.Ambient;
    float3 E = normalize(ViewPosition() - wPosition);
    

    [flatten]
    if (NdotL > 0.0f)
    {
        output.Diffuse = NdotL * material.Diffuse;


        [flatten]
        if (any(material.Specular.rgb))
        {
            float3 R = normalize(reflect(-direction, normal));
            float RdotE = saturate(dot(R, E));

            float specular = pow(RdotE, material.Specular.a);
            output.Specular = specular * material.Specular * GlobalLight.Specular;
        }
    }

    [flatten]
    if (any(material.Emissive.rgb))
    {
        float NdotE = dot(E, normalize(normal));
        
        float emissive = smoothstep(1.0f - material.Emissive.a, 1.0f, 1.0f - saturate(NdotE));
            
        output.Emissive = material.Emissive * emissive;
    }

}

struct VertexOutput_Directional
{
    float4 Position : SV_Position;
    float2 Screen : Position1;
};

float4 PS_Directional(VertexOutput_Directional input) : SV_Target0
{
    float4 position = input.Position;
    MaterialDesc material;
    float3 normal;
    float3 tangent;
    UnPackGBuffer(position, input.Screen, material, normal, tangent);

    //float4 color = material.Diffuse * dot(-GlobalLight.Direction, normalize(normal));//램버트 조명공식

    MaterialDesc result = MakeMaterial();
    ComputeLight_Deffered(result, material, normal, position.xyz);

    float4 sPosition = mul(position, ShadowView);
    sPosition = mul(sPosition, ShadowProjection);

    float4 color = float4(MaterialToColor(result), 1.0f);
    //color = PS_Shadow(sPosition, color);

    if(ShadowQuality == 4)
        color = PS_Shadow_PCSS(sPosition, color);
    else
        color = PS_Shadow(sPosition, color);

    if (FogType == 0)
        color = LinearFogBlend(color, position);
    else if(FogType == 1)
        color = ExpFogBlend(color, position);
    else if (FogType == 2)
        color = Exp2FogBlend(color, position);
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

//////////////////////////////////////////////////////////////////////////////////////
//PointLights
//////////////////////////////////////////////////////////////////////////////////////


cbuffer CB_Deffered_PointLight
{
    float TessFactor;
    float3 TessDefferedPadding;
    
    matrix PointLight_Projection[MAX_POINT_LIGHT];
    PointLightDesc PointLight_Deffered[MAX_POINT_LIGHT];
};

float4 VS_PointLights() : Position
{
    return float4(0, 0, 0, 1);
}

struct ConstantHullOutput_PointLights
{
    float Edge[4] : SV_TessFactor;
    float Inside[2] : SV_InsideTessFactor;
};

ConstantHullOutput_PointLights ConstHS_PointLights()
{
    ConstantHullOutput_PointLights output;
    

    output.Edge[0] = output.Edge[1] = output.Edge[2] = output.Edge[3] = TessFactor;
    output.Inside[0] = output.Inside[1] = TessFactor;

    return output;
}

struct HullOutput_PointLights
{
    float4 Direction : Position;
};

[domain("quad")]
[partitioning("integer")]
[outputtopology("triangle_ccw")]
[outputcontrolpoints(4)]
[patchconstantfunc("ConstHS_PointLights")]
HullOutput_PointLights HS_PointLights(uint id : SV_PrimitiveID)
{
    HullOutput_PointLights output;

    float4 Direction[2] =
    {
        float4(1, 1, 1, 1),
        float4(-1, 1, -1, 1),
    };
    output.Direction = Direction[id%2];

    return output;
}

struct DomainOutput_PointLights
{
    float4 Position : SV_Position;
    float2 Screen : Uv;
    uint PrimitiveID : Id;
};

[domain("quad")]
DomainOutput_PointLights DS_PointLights
(
    ConstantHullOutput_PointLights input,
    float2 uv : SV_DomainLocation,
    const OutputPatch<HullOutput_PointLights, 4> quad,
    uint id : SV_PrimitiveID
)
{
    DomainOutput_PointLights output;

    float2 clipSpace = uv.xy * 2.0f - 1.0f;

    float2 clipSpaceAbs = abs(clipSpace.xy);
    float maxLength = max(clipSpaceAbs.x, clipSpaceAbs.y);

    float3 direction = normalize(float3(clipSpace.xy, (maxLength - 1.0f)) * quad[0].Direction.xyz);
    float4 position = float4(direction, 1.0f);

    output.Position = mul(position, PointLight_Projection[id/2]);
    output.Screen = output.Position.xyz / output.Position.w;
    output.PrimitiveID = id / 2;

    return output;
}


void ComputePointLight_Deffered(inout MaterialDesc output,uint id, MaterialDesc material, float3 normal, float3 wPosition)
{
    output = MakeMaterial();
    normal = normalize(normal);

    PointLightDesc desc = PointLight_Deffered[id];

    float3 light = desc.Position - wPosition;
    float dist = length(light);
    
    [flatten]
    if (dist > desc.Range)
        return;
    
    light /= dist;
    
    output.Ambient = material.Ambient * desc.Ambient;
    
    float NdotL = dot(light, normal);
    float3 E = normalize(ViewPosition() - wPosition);
    
    [flatten]
    if (NdotL > 0.0f)
    {
        float3 R = normalize(reflect(-light, normal));
        float RdotE = saturate(dot(R, E));
        float specular = pow(RdotE, material.Specular.a);
    
        output.Diffuse = NdotL * material.Diffuse * desc.Diffuse;
        output.Specular = specular * material.Specular * desc.Specualr;
    }
    
    float NdotE = dot(E, normal);
    float emissive = smoothstep(1.0f - material.Emissive.a, 1.0f, 1.0f - saturate(NdotE));
    output.Emissive = emissive * material.Emissive * desc.Emissive;
    
    //-
    float temp = 1.0f / saturate(dist / desc.Range);
    
    float att = temp * temp * (1.0f / max(1 - desc.Intensity, 1e-8f));
    
    output.Ambient = output.Ambient * temp;
    output.Diffuse = output.Diffuse * att;
    output.Specular = output.Specular * att;
    output.Emissive = output.Emissive * att;
   

}

float4 PS_PointLight_Debug(DomainOutput_PointLights input) : SV_Target0
{
    return float4(1, 1, 0, 1);

}

float4 PS_PointLights(DomainOutput_PointLights input) : SV_Target
{
    float4 position = input.Position;

    float3 normal = 0, tangent = 0;
    MaterialDesc material = MakeMaterial();
    UnPackGBuffer(position, input.Screen, material, normal, tangent);

    MaterialDesc result = MakeMaterial();
    ComputePointLight_Deffered(result,input.PrimitiveID, material, normal, position.xyz);

    return float4(MaterialToColor(result), 1.0f);
}
///////////////////////////////////////////////////////////////
//Spot Light
////////////////////////////////////////////////////////////////


cbuffer CB_Deffered_SpotLight
{
    float SpotLight_TessFactor;
    float3 SpotLight_TessDefferedPadding;
    
    float4 SpotLight_Angle[MAX_SPOT_LIGHT];
    matrix SpotLight_Projection[MAX_SPOT_LIGHT];
    SpotLightDesc SpotLight_Deffered[MAX_SPOT_LIGHT];
};

float4 VS_SpotLights() : Position
{
    return float4(0, 0, 0, 1);
}

struct ConstantHullOutput_SpotLights
{
    float Edge[4] : SV_TessFactor;
    float Inside[2] : SV_InsideTessFactor;
};

ConstantHullOutput_SpotLights ConstHS_SpotLights()
{
    ConstantHullOutput_SpotLights output;
    

    output.Edge[0] = output.Edge[1] = output.Edge[2] = output.Edge[3] = SpotLight_TessFactor;
    output.Inside[0] = output.Inside[1] = SpotLight_TessFactor;

    return output;
}

struct HullOutput_SpotLights
{
    float4 Direction : Position;
};

[domain("quad")]
[partitioning("integer")]
[outputtopology("triangle_ccw")]
[outputcontrolpoints(4)]
[patchconstantfunc("ConstHS_SpotLights")]
HullOutput_SpotLights HS_SpotLights(uint id : SV_PrimitiveID)
{
    HullOutput_SpotLights output;

    float4 Direction[2] =
    {
        float4(1, 1, 1, 1),
        float4(-1, 1, -1, 1),
    };
    output.Direction = Direction[id % 2];

    return output;
}

struct DomainOutput_SpotLights
{
    float4 Position : SV_Position;
    float2 Screen : Uv;
    uint PrimitiveID : Id;
};

[domain("quad")]
DomainOutput_SpotLights DS_SpotLights
(
    ConstantHullOutput_SpotLights input,
    float2 uv : SV_DomainLocation,
    const OutputPatch<HullOutput_SpotLights, 4> quad,
    uint id : SV_PrimitiveID
)
{
    DomainOutput_SpotLights output;

    float s = SpotLight_Angle[id].y;
    float c = SpotLight_Angle[id].x;
    float2 clipSpace = uv.xy * float2(2, -2) + float2(-1,1);

    float2 clipSpaceAbs = abs(clipSpace.xy);
    float maxLength = max(clipSpaceAbs.x, clipSpaceAbs.y);

    float cylinder = 0.2f;
    float expentAmount = (1.0f + cylinder);

    float2 clipSpaceCylAbs = saturate(clipSpaceAbs * expentAmount);
    float maxLengthCapsule = max(clipSpaceCylAbs.x, clipSpaceCylAbs.y);
    float2 clipSpaceCyl = sign(clipSpace.xy) * clipSpaceCylAbs;

    float3 halfSpherePosition = normalize(float3(clipSpaceCyl.xy, 1.0f - maxLengthCapsule));
    halfSpherePosition = normalize(float3(halfSpherePosition.xy * s, c));

    float cylOffsetZ = saturate((maxLength * expentAmount - 1.0f) / cylinder);

    float4 position = 0;
    position.xy = halfSpherePosition.xy * (1.0f - cylOffsetZ);
    position.z = halfSpherePosition.z - cylOffsetZ * c;
    position.w = 1.0f;

    output.Position = mul(position, SpotLight_Projection[id]);
    output.Screen = output.Position.xyz / output.Position.w;
    output.PrimitiveID = id;

    return output;
}


void ComputeSpotLight_Deffered(inout MaterialDesc output, uint id, MaterialDesc material, float3 normal, float3 wPosition)
{
    output = MakeMaterial();
    normal = normalize(normal);

    SpotLightDesc desc = SpotLight_Deffered[id];

    float3 light = desc.Position - wPosition;
    float dist = length(light);
    
    [flatten]
    if (dist > desc.Range)
        return;
    
    light /= dist;
    
    output.Ambient = material.Ambient * desc.Ambient;
    
    float NdotL = dot(light, normal);
    float3 E = normalize(ViewPosition() - wPosition);
    
    [flatten]
    if (NdotL > 0.0f)
    {
        float3 R = normalize(reflect(-light, normal));
        float RdotE = saturate(dot(R, E));
        float specular = pow(RdotE, material.Specular.a);
    
        output.Diffuse = NdotL * material.Diffuse * desc.Diffuse;
        output.Specular = specular * material.Specular * desc.Specular;
    }
    
    float NdotE = dot(E, normal);
    float emissive = smoothstep(1.0f - material.Emissive.a, 1.0f, 1.0f - saturate(NdotE));
    output.Emissive = emissive * material.Emissive * desc.Emissive;
    
    //-
    float temp = pow(saturate(dot(-light, desc.Direction)), desc.Angle);

    float att = temp * (1.0f / max(1 - desc.Intensity, 1e-8f));
    
    output.Ambient = output.Ambient * temp;
    output.Diffuse = output.Diffuse * att;
    output.Specular = output.Specular * att;
    output.Emissive = output.Emissive * att;
   

}

float4 PS_SpotLight_Debug(DomainOutput_SpotLights input) : SV_Target0
{
    return float4(0, 1, 1, 1);

}

float4 PS_SpotLights(DomainOutput_SpotLights input) : SV_Target
{
    float4 position = input.Position;

    float3 normal = 0, tangent = 0;
    MaterialDesc material = MakeMaterial();
    UnPackGBuffer(position, input.Screen, material, normal, tangent);

    MaterialDesc result = MakeMaterial();
    ComputeSpotLight_Deffered(result, input.PrimitiveID, material, normal, position.xyz);

    return float4(MaterialToColor(result), 1.0f);
}

