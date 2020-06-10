float4 Color = float4(1, 1, 1, 1);

struct VertexInput
{
    float4 Position : Position0;
    float4 Color : Color0;
};

struct VertexOutput
{
    float4 Position : SV_Position;
    float4 Color : Color0;
};

VertexOutput VS(VertexInput input)
{
    VertexOutput output;
    output.Position = input.Position;
    output.Color = input.Color;

    return output;
}

float4 PS(VertexOutput input) : SV_Target0
{
    return input.Color;
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