matrix World;
matrix View;
matrix Projection;

float3 LightDirection = float3(-1, -1, 1);

struct VertexInput
{
    float4 Position : Position0;
    float3 Normal : Normal;
};

struct VertexOutput
{
    float4 Position : SV_Position;
    float3 Normal : Normal;
    float3 Color : Color0;
};

float3 GetHeightColor(float y)
{

    float3 color = 0.0f;
    
    if(y > 20.0f)
        color = float3(1, 1, 1);
    else if(y > 15.0f)
        color = float3(1, 0, 0);
    else if(y > 10.0f)
        color = float3(0, 1, 0);
    else if (y > 5.0f)
        color = float3(0, 0, 1);
    else if(y >= 0.0f)
        color = float3(0.3, 0.3, 0.3);

    return color;

}

VertexOutput VS(VertexInput input)
{
    VertexOutput output;
    output.Position = mul(input.Position, World);
    output.Color = GetHeightColor(output.Position.y);

    output.Position = mul(output.Position, View);
    output.Position = mul(output.Position, Projection);

    output.Normal = mul(input.Normal, (float3x3)World);

    return output;
}

float4 PS(VertexOutput input) : SV_Target0
{
    float3 diffuse = input.Color;

    float NdotL = dot(normalize(input.Normal), -LightDirection);

    return float4(diffuse * NdotL, 1);
}


RasterizerState RS
{
    FillMode = WireFrame;
};


technique11 T0
{
    pass P0
    {
        SetVertexShader(CompileShader(vs_5_0, VS()));
        SetPixelShader(CompileShader(ps_5_0, PS()));
    }

    pass P1
    {
        SetRasterizerState(RS);

        SetVertexShader(CompileShader(vs_5_0, VS()));
        SetPixelShader(CompileShader(ps_5_0, PS()));
    }
}