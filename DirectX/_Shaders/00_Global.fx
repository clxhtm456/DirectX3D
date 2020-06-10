//Common
//-----------------------------------------------
cbuffer CB_PerFrame
{
    matrix View;
    matrix ViewInverse;
    matrix Projection;
    matrix ProjectionInverse;
    matrix VP;

    float4 Culling[4];
    float4 Cliping;
    
    float Time;
};

cbuffer CB_Fog
{
    float4 FogColor;
    float2 FogDistance;
    float FogDensity;
    uint FogType;
};

cbuffer CB_World
{
    matrix World;
};

cbuffer CB_Reflection
{
    matrix Reflection;
};

Texture2D DiffuseMap;
Texture2D SpecularMap;
Texture2D NormalMap;
Texture2D ReflectionMap;
Texture2D RefractionMap;

TextureCube SkyCubeMap;
TextureCube DynamicCubeMap;
cbuffer CB_DynamicCubeMap
{
    matrix CubeView[6];
    matrix CubeProjection;
};

Texture2D ShadowMap;//텍스쳐가 아닌 깊이정보를 받음
SamplerComparisonState ShadowSampler;

static const float2 NDC[4] = { float2(-1, +1), float2(+1, +1), float2(-1, -1), float2(+1, -1) };
static const float PI = 3.14159265f;
static const float G = -0.980f;
static const float G2 = -0.980f * -0.980f;

//Function
//-----------------------------------------------
float4 WorldPosition(float4 position)
{
    return mul(position, World);
}

float4 ViewProjection(float4 position)
{
    position = mul(position, View);
    return mul(position, Projection);
}

float3 WorldNormal(float3 normal)
{
    return mul(normal, (float3x3) World);
}

float3 WorldTangent(float3 tangent)
{
    return mul(tangent, (float3x3) World);
}

float3 ViewPosition()
{
    return ViewInverse._41_42_43;
}

float4 LinearFogBlend(float4 color, float3 wPosition)
{
    float dist = saturate((distance(wPosition, ViewPosition()) - FogDistance.x) / (FogDistance.y + FogDistance.x));

    return float4(lerp(color.rgb, FogColor.rgb, dist), 1);
}

float4 ExpFogBlend(float4 color, float3 wPosition)
{
    float dist = distance(wPosition, ViewPosition());
    dist = dist / FogDistance.y * FogDistance.x;

    float factor = exp(-dist * FogDensity);
    return float4(lerp(FogColor.rgb, color.rgb, factor), 1);
}

float4 Exp2FogBlend(float4 color, float3 wPosition)
{
    float dist = distance(wPosition, ViewPosition());
    dist = dist / FogDistance.y * FogDistance.x;

    float factor = exp(-(dist * FogDensity) * (dist * FogDensity));
    return float4(lerp(FogColor.rgb, color.rgb, factor), 1);
}

float4 CalculateFogColor(float4 color, float3 wPosition)
{
    if(FogType == 0)
        return LinearFogBlend(color, wPosition);
    else if (FogType == 1)
        return ExpFogBlend(color, wPosition);
    else if(FogType == 2)
        return Exp2FogBlend(color, wPosition);

    return color;

}
//Mesh
//-----------------------------------------------
struct MeshOutput
{
    float4 Position : SV_Position0; // Rasterizing
    float4 wvpPosition : Position1;//wvp position
    float4 wvpPosition_Sub : Position2;//sub wvp position(projection texture)
    float3 oPosition : Position3;//original
    float3 wPosition : Position4;//world position
    float4 sPosition : Position5;//shadow position
    float4 gPosition : Position6; //GeometryPosition;

    float2 Uv : Uv0;
    float3 Normal : Normal0;
    float3 Tangent : Tangent0;

    float4 Cull : SV_CullDistance0; //x = left, y = right z = near , w = far
    float4 Clip : SV_ClipDistance0;// 
};

struct MeshGeometryOutput
{
    float4 Position : SV_Position0; // Rasterizing
    float4 wvpPosition : Position1; //wvp position
    float4 wvpPosition_Sub : Position2; //sub wvp position(projection texture)
    float3 oPosition : Position3; //original
    float3 wPosition : Position4; //world position
    float4 sPosition : Position5; //shadow position
    float4 gPosition : Position6; //GeometryPosition;

    float2 Uv : Uv0;
    float3 Normal : Normal0;
    float3 Tangent : Tangent0;

    uint TargetIndex : SV_RenderTargetArrayindex;
};





//Input
//-----------------------------------------------
struct Vertex
{
    float4 Position : POSITION0;
};

struct VertexColor
{
    float4 Postion : POSITION0;
    float4 Color : COLOR0;
};

struct VertexNormal
{
    float4 Position : POSITION0;
    float3 Normal : NORMAL0;
};

struct VertexColorNormal
{
    float4 Position : POSITION0;
    float4 Color : COLOR0;
    float3 Normal : NORMAL0;
};

struct VertexTexture
{
    float4 Position : Position0;
    float2 Uv : Uv0;
};

struct VertexTextrueNormal
{
    float4 Position : Position0;
    float2 Uv : Uv;
    float3 Normal : Normal;
};

//State
//-----------------------------------------------
SamplerState LinearSampler
{
    Filter = MIN_MAG_MIP_LINEAR;

    AddressU = Wrap;
    AddressV = Wrap;
};

RasterizerState FillMode_WireFrame
{
    FillMode = WireFrame;
};

RasterizerState FrontCounterClockwise_True
{
    FrontCounterClockwise = true;
};

RasterizerState Rasterizer_Light
{
    CullMode = Front;
};

DepthStencilState DepthEnable_False
{
    DepthEnable = false;
};

BlendState AlphaBlend
{
    BlendEnable[0] = true;
    DestBlend[0] = INV_SRC_ALPHA;
    SrcBlend[0] = SRC_ALPHA;
    BlendOp[0] = Add;

    SrcBlendAlpha[0] = Zero;
    DestBlendAlpha[0] = Zero;
    BlendOpAlpha[0] = Add;

    RenderTargetWriteMask[0] = 15;
};

BlendState AlphaBlend_AlphaToCoverage
{
    BlendEnable[0] = true;
    AlphaToCoverageEnable = true;
    DestBlend[0] = INV_SRC_ALPHA;
    SrcBlend[0] = SRC_ALPHA;
    BlendOp[0] = Add;

    SrcBlendAlpha[0] = Zero;
    DestBlendAlpha[0] = Zero;
    BlendOpAlpha[0] = Add;

    RenderTargetWriteMask[0] = 15;
};

BlendState Blend_Addtive
{
    BlendEnable[0] = true;
    DestBlend[0] = One;
    SrcBlend[0] = One;
    BlendOp[0] = Add;

    SrcBlendAlpha[0] = One;
    DestBlendAlpha[0] = One;
    BlendOpAlpha[0] = Add;

    RenderTargetWriteMask[0] = 0xF;
};

//Pass
//---------------------------------------------------------------------------------
///////////////////////////////////////////////////////////////////////////////
// Vertex / Pixel
///////////////////////////////////////////////////////////////////////////////
#define P_VP(name, vs, ps) \
pass name \
{ \
    SetVertexShader(CompileShader(vs_5_0, vs())); \
    SetPixelShader(CompileShader(ps_5_0, ps())); \
}

#define P_RS_VP(name, rs, vs, ps) \
pass name \
{ \
    SetRasterizerState(rs); \
    SetVertexShader(CompileShader(vs_5_0, vs())); \
    SetPixelShader(CompileShader(ps_5_0, ps())); \
}

#define P_BS_VP(name, bs, vs, ps) \
pass name \
{ \
    SetBlendState(bs, float4(0, 0, 0, 0), 0xFF); \
    SetVertexShader(CompileShader(vs_5_0, vs())); \
    SetPixelShader(CompileShader(ps_5_0, ps())); \
}

#define P_DSS_VP(name, dss, vs, ps) \
pass name \
{ \
    SetDepthStencilState(dss, 1); \
    SetVertexShader(CompileShader(vs_5_0, vs())); \
    SetPixelShader(CompileShader(ps_5_0, ps())); \
}

#define P_RS_DSS_VP(name, rs, dss, vs, ps) \
pass name \
{ \
    SetRasterizerState(rs); \
    SetDepthStencilState(dss, 0); \
    SetVertexShader(CompileShader(vs_5_0, vs())); \
    SetPixelShader(CompileShader(ps_5_0, ps())); \
}

#define P_RS_BS_VP(name, rs, bs, vs, ps) \
pass name \
{ \
    SetRasterizerState(rs); \
    SetBlendState(bs, float4(0, 0, 0, 0), 0xFF); \
    SetVertexShader(CompileShader(vs_5_0, vs())); \
    SetPixelShader(CompileShader(ps_5_0, ps())); \
}

#define P_DSS_BS_VP(name, dss, bs, vs, ps) \
pass name \
{ \
    SetDepthStencilState(dss, 1); \
    SetBlendState(bs, float4(0, 0, 0, 0), 0xFF); \
    SetVertexShader(CompileShader(vs_5_0, vs())); \
    SetPixelShader(CompileShader(ps_5_0, ps())); \
}

#define P_RS_DSS_BS_VP(name, rs, dss, bs, vs, ps) \
pass name \
{ \
    SetRasterizerState(rs); \
    SetDepthStencilState(dss, 1); \
    SetBlendState(bs, float4(0, 0, 0, 0), 0xFF); \
    SetVertexShader(CompileShader(vs_5_0, vs())); \
    SetPixelShader(CompileShader(ps_5_0, ps())); \
}

///////////////////////////////////////////////////////////////////////////////
// Vertex / Geometry / Pixel
///////////////////////////////////////////////////////////////////////////////
#define P_VGP(name, vs, gs, ps) \
pass name \
{ \
    SetVertexShader(CompileShader(vs_5_0, vs())); \
    SetGeometryShader(CompileShader(gs_5_0, gs())); \
    SetPixelShader(CompileShader(ps_5_0, ps())); \
}

#define P_RS_VGP(name, rs, vs, gs, ps) \
pass name \
{ \
    SetRasterizerState(rs); \
    SetVertexShader(CompileShader(vs_5_0, vs())); \
    SetGeometryShader(CompileShader(gs_5_0, gs())); \
    SetPixelShader(CompileShader(ps_5_0, ps())); \
}

#define P_BS_VGP(name, bs, vs, gs, ps) \
pass name \
{ \
    SetBlendState(bs, float4(0, 0, 0, 0), 0xFF); \
    SetVertexShader(CompileShader(vs_5_0, vs())); \
    SetGeometryShader(CompileShader(gs_5_0, gs())); \
    SetPixelShader(CompileShader(ps_5_0, ps())); \
}

#define P_DSS_VGP(name, dss, vs, gs, ps) \
pass name \
{ \
    SetDepthStencilState(dss, 1); \
    SetVertexShader(CompileShader(vs_5_0, vs())); \
    SetGeometryShader(CompileShader(gs_5_0, gs())); \
    SetPixelShader(CompileShader(ps_5_0, ps())); \
}

#define P_RS_DSS_VGP(name, rs, dss, vs, gs, ps) \
pass name \
{ \
    SetRasterizerState(rs); \
    SetDepthStencilState(dss, 1); \
    SetVertexShader(CompileShader(vs_5_0, vs())); \
    SetGeometryShader(CompileShader(gs_5_0, gs())); \
    SetPixelShader(CompileShader(ps_5_0, ps())); \
}

#define P_RS_BS_VGP(name, rs, bs, vs, gs, ps) \
pass name \
{ \
    SetRasterizerState(rs); \
    SetBlendState(bs, float4(0, 0, 0, 0), 0xFF); \
    SetVertexShader(CompileShader(vs_5_0, vs())); \
    SetGeometryShader(CompileShader(gs_5_0, gs())); \
    SetPixelShader(CompileShader(ps_5_0, ps())); \
}

#define P_DSS_BS_VGP(name, dss, bs, vs, gs, ps) \
pass name \
{ \
    SetDepthStencilState(dss, 1); \
    SetBlendState(bs, float4(0, 0, 0, 0), 0xFF); \
    SetVertexShader(CompileShader(vs_5_0, vs())); \
    SetGeometryShader(CompileShader(gs_5_0, gs())); \
    SetPixelShader(CompileShader(ps_5_0, ps())); \
}


///////////////////////////////////////////////////////////////////////////////
// Vertex / Tessellation / Pixel
///////////////////////////////////////////////////////////////////////////////
#define P_VTP(name, vs, hs, ds, ps) \
pass name \
{ \
    SetVertexShader(CompileShader(vs_5_0, vs())); \
    SetHullShader(CompileShader(hs_5_0, hs())); \
    SetDomainShader(CompileShader(ds_5_0, ds())); \
    SetPixelShader(CompileShader(ps_5_0, ps())); \
}

#define P_RS_VTP(name, rs, vs, hs, ds, ps) \
pass name \
{ \
    SetRasterizerState(rs); \
    SetVertexShader(CompileShader(vs_5_0, vs())); \
    SetHullShader(CompileShader(hs_5_0, hs())); \
    SetDomainShader(CompileShader(ds_5_0, ds())); \
    SetPixelShader(CompileShader(ps_5_0, ps())); \
}

#define P_BS_VTP(name, bs, vs, hs, ds, ps) \
pass name \
{ \
    SetBlendState(bs, float4(0, 0, 0, 0), 0xFF); \
    SetVertexShader(CompileShader(vs_5_0, vs())); \
    SetHullShader(CompileShader(hs_5_0, hs())); \
    SetDomainShader(CompileShader(ds_5_0, ds())); \
    SetPixelShader(CompileShader(ps_5_0, ps())); \
}

#define P_DSS_VTP(name, dss, vs, hs, ds, ps) \
pass name \
{ \
    SetDepthStencilState(dss, 1); \
    SetVertexShader(CompileShader(vs_5_0, vs())); \
    SetHullShader(CompileShader(hs_5_0, hs())); \
    SetDomainShader(CompileShader(ds_5_0, ds())); \
    SetPixelShader(CompileShader(ps_5_0, ps())); \
}

#define P_RS_DSS_VTP(name, rs, dss, vs, hs, ds, ps) \
pass name \
{ \
    SetRasterizerState(rs); \
    SetDepthStencilState(dss, 1); \
    SetVertexShader(CompileShader(vs_5_0, vs())); \
    SetHullShader(CompileShader(hs_5_0, hs())); \
    SetDomainShader(CompileShader(ds_5_0, ds())); \
    SetPixelShader(CompileShader(ps_5_0, ps())); \
}

#define P_RS_BS_VTP(name, rs, bs, vs, hs, ds, ps) \
pass name \
{ \
    SetRasterizerState(rs); \
    SetBlendState(bs, float4(0, 0, 0, 0), 0xFF); \
    SetVertexShader(CompileShader(vs_5_0, vs())); \
    SetHullShader(CompileShader(hs_5_0, hs())); \
    SetDomainShader(CompileShader(ds_5_0, ds())); \
    SetPixelShader(CompileShader(ps_5_0, ps())); \
}

#define P_DSS_BS_VTP(name, dss, bs, vs, hs, ds, ps) \
pass name \
{ \
    SetDepthStencilState(dss, 1); \
    SetBlendState(bs, float4(0, 0, 0, 0), 0xFF); \
    SetVertexShader(CompileShader(vs_5_0, vs())); \
    SetHullShader(CompileShader(hs_5_0, hs())); \
    SetDomainShader(CompileShader(ds_5_0, ds())); \
    SetPixelShader(CompileShader(ps_5_0, ps())); \
}

#define P_RS_DSS_BS_VTP(name, rs, dss, bs, vs, hs, ds, ps) \
pass name \
{ \
    SetRasterizerState(rs); \
    SetDepthStencilState(dss, 1); \
    SetBlendState(bs, float4(0, 0, 0, 0), 0xFF); \
    SetVertexShader(CompileShader(vs_5_0, vs())); \
    SetHullShader(CompileShader(hs_5_0, hs())); \
    SetDomainShader(CompileShader(ds_5_0, ds())); \
    SetPixelShader(CompileShader(ps_5_0, ps())); \
}