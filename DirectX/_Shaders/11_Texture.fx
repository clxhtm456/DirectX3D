matrix World;
matrix View;
matrix Projection;

Texture2D Texture;
uint Address;
uint Filter;

struct VertexInput
{
    float4 Position : Position0;
    float2 Uv : Uv0;
};

struct VertexOutput
{
    float4 Position : SV_Position;
    float2 Uv : Uv0;
};

VertexOutput VS(VertexInput input)
{
    VertexOutput output;
    output.Position = mul(input.Position, World);
    output.Position = mul(output.Position, View);
    output.Position = mul(output.Position, Projection);

    output.Uv = input.Uv;

    return output;
}

SamplerState Sampler;
float4 PS(VertexOutput input) : SV_Target0
{  
    float4 color = Texture.Sample(Sampler, input.Uv);

    if (input.Uv.x < 0.5f)
        color = float4(1, 1, 1, 1);

    return color;
}

//Address
SamplerState Sampler_Address_Wrap
{
    AddressU = Wrap;
    AddressV = Wrap;
};

SamplerState Sampler_Address_Mirror
{
    AddressU = Mirror;
    AddressV = Mirror;
};

SamplerState Sampler_Address_Clamp
{
    AddressU = Clamp;
    AddressV = Clamp;
};

SamplerState Sampler_Address_Border
{
    AddressU = Border;
    AddressV = Border;

    BorderColor = float4(0, 1, 0, 1);
};


float4 PS_Address(VertexOutput input) : SV_Target0
{
    [branch]
    switch (Address)
    {
        case 0 : 
            return Texture.Sample(Sampler_Address_Wrap, input.Uv);
        case 1:
            return Texture.Sample(Sampler_Address_Mirror, input.Uv);
        case 2:
            return Texture.Sample(Sampler_Address_Clamp, input.Uv);
        case 3:
            return Texture.Sample(Sampler_Address_Border, input.Uv);
    }
    
    return float4(0, 0, 0, 1);
}

//Filter
SamplerState Sampler_Filter_Point
{
    Filter = MIN_MAG_MIP_POINT;
};

SamplerState Sampler_Filter_Linear
{
    Filter = MIN_MAG_MIP_LINEAR;
};

SamplerState Sampler_Filter_ANISOTROPIC
{
    Filter = ANISOTROPIC;
};

float4 PS_Filter(VertexOutput input) : SV_Target0
{
    [branch]
    switch (Filter)
    {
        case 0:
            return Texture.Sample(Sampler_Filter_Point, input.Uv);
        case 1:
            return Texture.Sample(Sampler_Filter_Linear, input.Uv);
        case 2:
            return Texture.Sample(Sampler_Filter_ANISOTROPIC, input.Uv);
    }

    return float4(0, 0, 0, 1);
}

technique11 T0
{
    pass P0
    {
        SetVertexShader(CompileShader(vs_5_0, VS()));
        SetPixelShader(CompileShader(ps_5_0, PS()));
    }

    pass P1
    {
        SetVertexShader(CompileShader(vs_5_0, VS()));
        SetPixelShader(CompileShader(ps_5_0, PS_Address()));
    }

    pass P2
    {
        SetVertexShader(CompileShader(vs_5_0, VS()));
        SetPixelShader(CompileShader(ps_5_0, PS_Filter()));
    }
   
}