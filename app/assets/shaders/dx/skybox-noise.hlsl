#include "libs/common.hlsl"
#include "libs/fog.hlsl"

cbuffer cbCommon : register(b0)
{
    float4x4 view;
    float4x4 viewInv;
    float4x4 projection;
    float4x4 projectionInv;
    float3 viewPos;
    float time;
    float4 ambientLight;
    Light light;
};

cbuffer cbObject : register(b1)
{
	float4x4 model;
};

TextureCube cubeMap : register(t0);

struct VertexIn
{
	float3 PosL      : POSITION;
    float3 NormalL   : NORMAL;
    float2 TexCoord  : TEXCOORD;
    float3 Tangent   : TANGENT;
    float3 Bitangent : BITANGENT;
    float4 Color     : COLOR;
};

struct VertexOut
{
    float4 PosH     : SV_POSITION;
    float3 PosW     : POSITION0;
    float3 PosL     : POSITION1;
    float3 TexCoord : TEXCOORD;
    float3 NormalL  : NORMAL;
};

VertexOut VS(VertexIn vin)
{
	VertexOut vout;

    float4x4 fixedView = view;
    fixedView[0][3] = 0;
    fixedView[1][3] = 0;
    fixedView[2][3] = 0;
    fixedView[3][0] = 0;
    fixedView[3][1] = 0;
    fixedView[3][2] = 0;
    fixedView[3][3] = 1;

    float4 posW = mul(float4(vin.PosL, 1.0), model);
    float4 posV = mul(posW, fixedView);
    float4 posH = mul(posV, projection);
    
    vout.PosL = vin.PosL;
    vout.PosH = posH.xyww;
    vout.PosW = posW.xyz;
    vout.TexCoord = vin.PosL;
    vout.NormalL = vin.NormalL;

    return vout;
}

float4 PS(VertexOut pin) : SV_Target
{
    float4 skyColor = float4(0.25f, 0.25f, 1.00f, 1.0f);

    // float noiseSample = Turbulence(pin.PosW * 3.0 + time * 0.05);
    // float mergefactor = pow(noiseSample, 0.5);
    // float heightFactor = mergefactor;

    // float noiseSample = Ridget(pin.PosW * 2.5 + time * 0.05);
    // float mergefactor = pow((noiseSample - 0.5) * 2.0 + 0.5, 4.0);
    // float heightFactor = noiseSample * 0.5;

    // float f0 = FBM(pin.PosW + float3(0.01 * time, 0.01 * time, 0.01 * time));
    // float noiseSample = FBM(pin.PosW + float3(f0, f0, f0) + float3(0.15 * time, 0.15 * time, 0.15 * time));

    float noiseSample = FBM(pin.PosW * 3.0 + float3(time * 0.05, time * 0.05, time * 0.05));
    float mergefactor = pow((noiseSample * 0.5 + 0.5 - 0.5) * 2.0 + 0.5, 1.0);
    float heightFactor = mergefactor;

    float skyFoxfactor = lerp(0.0, mergefactor, smoothstep(0.1, 0.2, pin.PosW.y + heightFactor * 0.1));

    return lerp(FOG_COLOR, skyColor, skyFoxfactor);
}