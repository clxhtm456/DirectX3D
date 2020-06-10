#include "00_Global.fx"
#include "00_Light.fx"
#include "00_Model.fx"

int Selected;

[maxvertexcount(18)]
void GS_PreRender(triangle MeshOutput input[3], inout TriangleStream<MeshGeometryOutput> stream)
{
    int vertex = 0;
    MeshGeometryOutput output;

    [unroll(6)]
    for (int i = 0; i < 6; i++)
    {
        output.TargetIndex = i;

        [unroll(3)]
        for (vertex = 0; vertex < 3; vertex++)
        {
            output.Position = mul(input[vertex].gPosition, CubeView[i]);
            output.Position = mul(output.Position, CubeProjection);
            output.wvpPosition = output.Position;
            output.wvpPosition_Sub = output.Position;

            output.wPosition = input[vertex].wPosition;
            output.oPosition = input[vertex].oPosition;
            output.sPosition = input[vertex].sPosition;
            output.gPosition = input[vertex].gPosition;
            output.Normal = input[vertex].Normal;
            output.Tangent = input[vertex].Tangent;
            output.Uv = input[vertex].Uv;

            stream.Append(output);
        }
        stream.RestartStrip();
    }
}

float4 PS_SkyPreRender(MeshGeometryOutput input) : SV_Target0
{
    return SkyCubeMap.Sample(LinearSampler, input.oPosition);
}

float4 PS_PreRender(MeshGeometryOutput input) : SV_Target0
{
    return PS_Shadow(input, PS_AllLight(input));
}

float4 PS(MeshOutput input) : SV_Target
{
    return PS_Shadow(input, PS_AllLight(input));
}

float4 PS_Cube(MeshOutput input ) : SV_Target
{
    float3 normal = normalize(input.Normal);
    float3 view = input.wPosition - ViewPosition();
    float3 reflection = reflect(normalize(view), normal);
    float3 refraction = refract(normalize(view), normal, 0.66f);

    float4 color = 0;
    float4 diffuse = 0;

    [branch]
    if(Selected ==0)
    {
        color = DynamicCubeMap.Sample(LinearSampler, input.oPosition);
        color.a = 0.75f;
    }else if(Selected == 1)
    {
        color = DynamicCubeMap.Sample(LinearSampler, normalize(reflection));
        color.a = 0.75f;
    }
    else if (Selected == 2)
    {
        color = DynamicCubeMap.Sample(LinearSampler, -normalize(refraction));
        color.a = 0.75f;
    }else if(Selected ==3)
    {
        diffuse = PS_AllLight(input);
        color = DynamicCubeMap.Sample(LinearSampler, normalize(reflection));

        color.rgb *= (0.8f + diffuse * 0.75f);
        color.a = 1.0f;

    }
    return color;
}


technique11 T0
{
    //Sky
    P_RS_DSS_VP(P0, FrontCounterClockwise_True, DepthEnable_False, VS_Mesh, PS_Sky)

    //Depth
    P_RS_VP(P1, FrontCounterClockwise_True, VS_Depth_Mesh, PS_Depth)
    P_RS_VP(P2, FrontCounterClockwise_True, VS_Depth_Model, PS_Depth)
    P_RS_VP(P3, FrontCounterClockwise_True, VS_Depth_Animation, PS_Depth)

    //Render
    P_VP(P4, VS_Mesh, PS)
    P_VP(P5, VS_Model, PS)
    P_VP(P6, VS_Animation, PS)

    //Cube PreRender
    P_RS_DSS_VGP(P7, FrontCounterClockwise_True, DepthEnable_False, VS_Mesh, GS_PreRender, PS_SkyPreRender)
    P_VGP(P8, VS_Mesh, GS_PreRender, PS_PreRender)
    P_VGP(P9, VS_Model, GS_PreRender, PS_PreRender)
    P_VGP(P10, VS_Animation, GS_PreRender, PS_PreRender)
    
    //Cube Render
    P_BS_VP(P11, AlphaBlend,VS_Mesh, PS_Cube)
    P_BS_VP(P12, AlphaBlend,VS_Model, PS_Cube)
    P_BS_VP(P13, AlphaBlend,VS_Animation, PS_Cube)
}

//Todo 23