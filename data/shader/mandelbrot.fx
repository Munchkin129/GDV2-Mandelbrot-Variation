// Based on the following Tutorials
// https://iquilezles.org/articles/distancefractals/
// https://www.shadertoy.com/view/4ddSWf
// https://www.shadertoy.com/view/4tt3Dn

// -----------------------------------------------------------------------------
// Define the constant buffers.
// -----------------------------------------------------------------------------
cbuffer VSBuffer : register(b0)         // Register the vertex constant buffer on slot 0
{
    float4x4 g_ScreenMatrix;
};

cbuffer PSBuffer : register(b0)         // Register the pixel constant buffer on slot 0 (no collision to VSBuffer because of different shader stage)
{
    float4 g_PSConstant01;
};

#define g_Time       g_PSConstant01.x
#define g_Resolution g_PSConstant01.yz

// -----------------------------------------------------------------------------
// Define input and output data of the vertex shader.
// -----------------------------------------------------------------------------
struct VSInput
{
    float3 m_Position : POSITION;
};

struct PSInput
{
    float4 m_Position : SV_POSITION;
};

// -----------------------------------------------------------------------------
// Functions
// -----------------------------------------------------------------------------

float2 cpow(float2 c, float exponent)
{
    if (abs(c.x) < 1e-5 && abs(c.y) < 1e-5) {
        return float2(0.0f, 0.0f);
    }
    else {
        float cAbs = length(c);
        float2  cLog = float2(log(cAbs), atan2(c.y, c.x));
        float2  cMul = exponent * cLog;
        float expReal = exp(cMul.x);
        return float2(expReal * cos(cMul.y), expReal * sin(cMul.y));
    }
}

// -----------------------------------------------------------------------------
// Vertex Shader
// -----------------------------------------------------------------------------
PSInput VSShader(VSInput _Input)
{
    float4 WSPosition;

    PSInput Output = (PSInput)0;

    Output.m_Position = mul(float4(_Input.m_Position, 1.0f), g_ScreenMatrix);

    return Output;
}

// -----------------------------------------------------------------------------
// Pixel Shader
// -----------------------------------------------------------------------------

float4 PSShader(PSInput _Input) : SV_Target
{
    float2 p = -1.0f + 2.0f * _Input.m_Position.xy / g_Resolution.xy;
    p.x *= g_Resolution.x / g_Resolution.y;

    // Animation	
    float n = 4.0f - 2.0f * cos(0.225f * g_Time);
    float zoo = 1.25;
    float2 c = float2(0.0f ,0.0f) + p * zoo;

    // Iterate
    float2 z = float2(0.0f, 0.0f);
    float glow = 0.0f;
    float m2 = 0.0f;
    float2 dz = float2(0.0f, 0.0f);
    for (int i = 0; i < 256; i++)
    {
        if (m2 > 1024.0f) break;

        // Z' -> n·Z^(n-1)·Z' + 1
        float2 chain = n * cpow(z,n - 1.0f);
        dz = float2(chain.x*dz.x-chain.y*dz.y, chain.x*dz.y + chain.y*dz.x) + float2(1.0f,0.0f);

        // Z -> Z^n + c			
        z = cpow(z, n) + c;

        m2 = dot(z,z);

        // Glow
        glow = m2 / 1024.0f;
    }

    // Distance	
    // d(c) = |Z|·log|Z|/|Z'|
    float d = 0.5f * sqrt(m2 / dot(dz,dz)) * log(m2);


    // Coloring based on Distance
    d = clamp(8.0f * d / zoo, 0.0f, 1.0f);
    d = pow(d, 0.25f);

    return float4(d, d, glow, 1.0f);
}