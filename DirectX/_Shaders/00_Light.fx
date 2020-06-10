
struct LightDesc
{
    float4 Ambient;
    float4 Specular;
    float3 Direction;
    float Padding;
    float3 Position;
};


struct MaterialDesc
{
    float4 Ambient;
    float4 Diffuse;
    float4 Specular;
    float4 Emissive;
};

cbuffer CB_Material
{
    MaterialDesc Material;
};

cbuffer CB_Light
{
    LightDesc GlobalLight;
};

float3 MaterialToColor(MaterialDesc result)
{
    return (result.Ambient + result.Diffuse + result.Specular + result.Emissive).rgb;
}

void AddMaterial(inout MaterialDesc result, MaterialDesc val)
{
    result.Ambient += val.Ambient;
    result.Diffuse += val.Diffuse;
    result.Specular += val.Specular;
    result.Emissive += val.Emissive;
}

MaterialDesc MakeMaterial()
{
    MaterialDesc output;
    output.Ambient = float4(0, 0, 0, 0);
    output.Diffuse = float4(0, 0, 0, 0);
    output.Specular = float4(0, 0, 0, 0);
    output.Emissive = float4(0, 0, 0, 0);
    
    return output;
}

///////////////////////////////////////////////////////////////////////////////

void Texture(inout float4 color, Texture2D t, float2 uv, SamplerState samp)
{
    float4 sampling = t.Sample(samp, uv);
    
    color = color * sampling;
}

void Texture(inout float4 color, Texture2D t, float2 uv)
{
    Texture(color, t, uv, LinearSampler);
}

///////////////////////////////////////////////////////////////////////////////
Texture2D ProjectorMap;

struct ProjectorDesc
{
    Matrix View;
    Matrix Projection;

    float4 color;
};


cbuffer CB_Projector
{
    ProjectorDesc Projector;
};

void VS_Projector(inout float4 wvp, float4 position)
{
    wvp = WorldPosition(position);
    wvp = mul(wvp, Projector.View);
    wvp = mul(wvp, Projector.Projection);
}

void PS_Projection(inout float4 color, float4 wvp)
{
    float3 uvw = 0;
    uvw.x = wvp.x / wvp.w * 0.5f + 0.5f;
    uvw.y = -wvp.y / wvp.w * 0.5f + 0.5f;
    uvw.z = wvp.z / wvp.w;

    [flatten]
    if (saturate(uvw.x) == uvw.x && saturate(uvw.y) == uvw.y && saturate(uvw.z) == uvw.z)
    {
        float4 map = ProjectorMap.Sample(LinearSampler, uvw.xy);
        
        map.rgb *= Projector.color.rgb;
        color = lerp(color, map, map.a);
    }

}


///////////////////////////////////////////////////////////////////////////////

void ComputeLight(out MaterialDesc output, float3 normal, float3 wPosition)
{
    output.Ambient = 0;
    output.Diffuse = 0;
    output.Specular = 0;
    output.Emissive = 0;

    float3 direction = -GlobalLight.Direction;
    float NdotL = dot(direction, normalize(normal));

    output.Ambient = GlobalLight.Ambient * Material.Ambient;
    float3 E = normalize(ViewPosition() - wPosition);
    

    [flatten]
    if (NdotL > 0.0f)
    {
        output.Diffuse = NdotL * Material.Diffuse;


        [flatten]
        if (any(Material.Specular.rgb))
        {
            float3 R = normalize(reflect(-direction, normal));
            float RdotE = saturate(dot(R, E));

            float specular = pow(RdotE, Material.Specular.a);
            output.Specular = specular * Material.Specular * GlobalLight.Specular;
        }
    }

    [flatten]
    if (any(Material.Emissive.rgb))
    {
        float NdotE = dot(E, normalize(normal));
        
        float emissive = smoothstep(1.0f - Material.Emissive.a, 1.0f, 1.0f - saturate(NdotE));
            
        output.Emissive = Material.Emissive * emissive;
    }

}


void NormalMapping(float2 uv, float3 normal, float3 tangent, SamplerState samp)
{
    float4 map = NormalMap.Sample(samp, uv);

    [flatten]
    if (any(map) == false)
        return;

    float3 N = normalize(normal);
    float3 T = normalize(tangent - dot(tangent, N) * N);
    float3 B = cross(N, T);
    float3x3 TBN = float3x3(T, B, N);


    float3 coord = map.rgb * 2.0f - 1.0f;

    coord = mul(coord, TBN);
    
	Material.Diffuse *= saturate(dot(coord, -GlobalLight.Direction));
}

void NormalMapping(float2 uv, float3 normal, float3 tangent)
{
    NormalMapping(uv, normal, tangent, LinearSampler);

}


///////////////////////////////////////////////////////////////////////////////

struct PointLightDesc
{
    float4 Ambient;
    float4 Diffuse;
    float4 Specualr;
    float4 Emissive;

    float3 Position;
    float Range;

    float Intensity;
    float3 Padding;
};

#define MAX_POINT_LIGHT 128

cbuffer CB_PointLights
{
    uint PointLightCount;
    float3 CB_PointLights_Padding;

    PointLightDesc PointLights[MAX_POINT_LIGHT];
};

void ComputePointLight(inout MaterialDesc output, float3 normal, float3 wPosition)
{
    output = MakeMaterial();
    MaterialDesc result = MakeMaterial();

    normal = normalize(normal);

    for (uint i = 0; i < PointLightCount; i++)
    {
        float3 light = PointLights[i].Position - wPosition;
        float dist = length(light);

        [flatten]
        if (dist > PointLights[i].Range)
            continue;

        light /= dist;

        result.Ambient = Material.Ambient * PointLights[i].Ambient;

        float NdotL = dot(light, normal);
        float3 E = normalize(ViewPosition() - wPosition);

        [flatten]
        if (NdotL > 0.0f)
        {
            float3 R = normalize(reflect(-light, normal));
            float RdotE = saturate(dot(R, E));
            float specular = pow(RdotE, Material.Specular.a);

            result.Diffuse = NdotL * Material.Diffuse * PointLights[i].Diffuse;
            result.Specular = specular * Material.Specular * PointLights[i].Specualr;
        }

        float NdotE = dot(E, normal);
        float emissive = smoothstep(1.0f - Material.Emissive.a, 1.0f, 1.0f - saturate(NdotE));
        result.Emissive = emissive * Material.Emissive * PointLights[i].Emissive;

        //-
        float temp = 1.0f / saturate(dist / PointLights[i].Range);

        float att = temp * temp * (1.0f / max(1 - PointLights[i].Intensity, 1e-8f));

        output.Ambient += result.Ambient * temp;
        output.Diffuse += result.Diffuse * att;
        output.Specular += result.Specular * att;
        output.Emissive += result.Emissive * att;
    }

}


///////////////////////////////////////////////////////////////////////////////

struct SpotLightDesc
{
    float4 Ambient;
    float4 Diffuse;
    float4 Specular;
    float4 Emissive;

    float3 Position;
    float Range;

    float3 Direction;
    float Angle;

    float Intensity;
    float3 Padding;
};

#define MAX_SPOT_LIGHT 128

cbuffer CB_SpotLights
{
    uint SpotLightCount;
    float3 CB_SpotLights_Padding;

    SpotLightDesc SpotLights[MAX_SPOT_LIGHT];
};

void ComputeSpotLight(inout MaterialDesc output, float3 normal, float3 wPosition)
{
    output = MakeMaterial();
    MaterialDesc result = MakeMaterial();

    normal = normalize(normal);

    for (uint i = 0; i < SpotLightCount; i++)
    {
        float3 light = SpotLights[i].Position - wPosition;
        float dist = length(light);

        [flatten]
        if (dist > SpotLights[i].Range)
            continue;

        light /= dist;

        result.Ambient = Material.Ambient * SpotLights[i].Ambient;

        float NdotL = dot(light, normal);
        float3 E = normalize(ViewPosition() - wPosition);

        [flatten]
        if (NdotL > 0.0f)
        {
            float3 R = normalize(reflect(-light, normal));
            float RdotE = saturate(dot(R, E));
            float specular = pow(RdotE, Material.Specular.a);

            result.Diffuse = NdotL * Material.Diffuse * SpotLights[i].Diffuse;
            result.Specular = specular * Material.Specular * SpotLights[i].Specular;
        }

        float NdotE = dot(E,normal);
        float emissive = smoothstep(1.0f - Material.Emissive.a, 1.0f, 1.0f - saturate(NdotE));
        result.Emissive = emissive * Material.Emissive * SpotLights[i].Emissive;

        float temp = pow(saturate(dot(-light, SpotLights[i].Direction)), SpotLights[i].Angle);

        float att = temp * (1.0f / max(1 - SpotLights[i].Intensity, 1e-8f));


        output.Ambient += result.Ambient * temp;
        output.Diffuse += result.Diffuse * att;
        output.Specular += result.Specular * att;
        output.Emissive += result.Emissive * att;
    }

}


///////////////////////////////////////////////////////////////////////////////

float4 PS_AllLight(MeshOutput input)
{
    NormalMapping(input.Uv, input.Normal, input.Tangent);
    Texture(Material.Diffuse, DiffuseMap, input.Uv);
    Texture(Material.Specular, SpecularMap, input.Uv);
    
    MaterialDesc result = MakeMaterial();
    MaterialDesc output = MakeMaterial();

    ComputeLight(output, input.Normal, input.wPosition);
    AddMaterial(result, output);

    ComputePointLight(output, input.Normal, input.wPosition);
    AddMaterial(result, output);

    ComputeSpotLight(output, input.Normal, input.wPosition);
    AddMaterial(result, output);

    return float4(MaterialToColor(result), 1.0f);
}

///////////////////////////////////////////////////////////////////////////////

cbuffer CB_Shadow
{
    matrix ShadowView;
    matrix ShadowProjection;

    float2 ShadowMapSize;
    float ShadowBias;

    uint ShadowQuality;
};



float4 PS_Shadow(MeshOutput input, float4 color)
{
    input.sPosition.xyz /= input.sPosition.w;

    [flatten]
    if (input.sPosition.x < -1.0f || input.sPosition.x > 1.0f ||
        input.sPosition.y < -1.0f || input.sPosition.y > 1.0f ||
        input.sPosition.z < 0.0f || input.sPosition.z > 1.0f)
        return color;

    input.sPosition.x = input.sPosition.x * 0.5f + 0.5f;
    input.sPosition.y = -input.sPosition.y * 0.5f + 0.5f;
    input.sPosition.z -= ShadowBias; //z축 직접수정을 통한 피터패닝 수정가능(권장하진않음)

    float depth = 0.0f;
    float factor = 0.0f;

    [branch]
    if (ShadowQuality == 0)
    {
        depth = ShadowMap.Sample(LinearSampler, input.sPosition.xy).r;
        factor = (float) input.sPosition.z <= depth;
    }
    else if (ShadowQuality == 1)
    {
        depth = input.sPosition.z;
        factor = ShadowMap.SampleCmpLevelZero(ShadowSampler, input.sPosition.xy, depth).r;
    }
    else if (ShadowQuality == 2)
    {
        depth = input.sPosition.z;
        float2 size = 1.0f / ShadowMapSize; //그림자맵 한픽셀의 사이즈
        float2 offsets[] =
        {
            float2(-size.x, -size.y), float2(0.0f, -size.y),
            float2(size.x, -size.y), float2(-size.x, 0.0f),
            float2(0.0f, 0.0f), float2(+size.x, 0.0f),
            float2(-size.x, +size.y), float2(0.0f, +size.y),
            float2(size.x, size.y)
        };

        float2 uv = 0;
        float2 sum = 0;

        [unroll(9)]
        for (int i = 0; i < 9; i++)
        {
            uv = input.sPosition.xy + offsets[i];
            sum += ShadowMap.SampleCmpLevelZero(ShadowSampler, uv, depth).r;
        }

        factor = sum / 9.0f;
    }
    
    factor = saturate(factor + depth);

    return float4(color.rgb * factor, 1);
}

float4 PS_Shadow(float4 position, float4 color)
{
    MeshOutput input = (MeshOutput) 0;

    input.sPosition = position;
    return PS_Shadow(input, color);
}

float4 PS_Shadow(MeshGeometryOutput input , float4 color)
{
    MeshOutput output;
    output.Position = input.Position;
    output.wvpPosition = input.wvpPosition;
    output.wvpPosition_Sub = input.wvpPosition_Sub;
    output.oPosition = input.oPosition;
    output.wPosition = input.wPosition;
    output.sPosition = input.sPosition;
    output.gPosition = input.gPosition;

    output.Uv = input.Uv;
    output.Normal = input.Normal;
    output.Tangent = input.Tangent;

    return PS_Shadow(output, color);
}


float4 PS_AllLight(MeshGeometryOutput input)
{
    MeshOutput output;
    output.Position = input.Position;
    output.wvpPosition = input.wvpPosition;
    output.wvpPosition_Sub = input.wvpPosition_Sub;
    output.oPosition = input.oPosition;
    output.wPosition = input.wPosition;
    output.sPosition = input.sPosition;
    output.gPosition = input.gPosition;

    output.Uv = input.Uv;
    output.Normal = input.Normal;
    output.Tangent = input.Tangent;

    return PS_AllLight(output);
}

// Poisson smapling
static const float2 PoissonDisk[16] =
{
    float2(-0.94201624, -0.39906216),
	float2(0.94558609, -0.76890725),
	float2(-0.094184101, -0.92938870),
	float2(0.34495938, 0.29387760),
	float2(-0.91588581, 0.45771432),
	float2(-0.81544232, -0.87912464),
	float2(-0.38277543, 0.27676845),
	float2(0.97484398, 0.75648379),
	float2(0.44323325, -0.97511554),
	float2(0.53742981, -0.47373420),
	float2(-0.26496911, -0.41893023),
	float2(0.79197514, 0.19090188),
	float2(-0.24188840, 0.99706507),
	float2(-0.81409955, 0.91437590),
	float2(0.19984126, 0.78641367),
	float2(0.14383161, -0.14100790)
};
float4 PS_Shadow_PCSS(float4 position, float4 color)
{
    position.xyz /= position.w;

    [flatten]
    if (position.x < -1.0f || position.x > 1.0f ||
        position.y < -1.0f || position.y > 1.0f ||
        position.z < 0.0f || position.z > 1.0f)
        return color;

    position.x = position.x * 0.5f + 0.5f;
    position.y = -position.y * 0.5f + 0.5f;
    position.z -= ShadowBias;

    float4 avgBlockerDepth = 0;
    float blockerCount = 0;
    float4 d = 0; //Depth
    float4 b = 0; //Factor

    //[unroll]
    for (int y = -2; y <= 2; y += 2)
    {
        //[unroll]
        for (int x = -2; x <= 2; x += 2)
        {
            d = ShadowMap.GatherRed(LinearSampler, position.xy, int2(x, y));
            b = (position.z <= d) ? 0.0f : 1.0f;

            blockerCount += dot(b, 1.0f);
            avgBlockerDepth += dot(b, d);
        }
    }

    if (blockerCount <= 0.0f)
        return color;

    avgBlockerDepth /= blockerCount;

    float ratio = ((position.z - avgBlockerDepth) * 65) / avgBlockerDepth;
    ratio *= ratio;

    float att = 0;
    float2 offset = 0;
    //[unroll]
    for (int i = 0; i < 16; i++)
    {
        offset = ratio * ShadowMapSize.xy * PoissonDisk[i];
        att += ShadowMap.SampleCmpLevelZero(ShadowSampler, position.xy + offset, position.z);
    }

    return float4(color.rgb * (att * 1 / 16), 1);

}