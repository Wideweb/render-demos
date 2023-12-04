#include "libs/common.hlsl"

cbuffer cbCommon : register(b0)
{
    float4x4 projection;
};

cbuffer cbObject : register(b1)
{
	float4x4 model;
    float4 color;
};

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
    float2 TexCoord : TEXCOORD;
    float4 Color    : COLOR;
};

VertexOut VS(VertexIn vin)
{
	VertexOut vout;

    float4 pos  = float4(vin.PosL, 1.0);
    float4 posW = mul(pos, model);
    float4 posH = mul(posW, projection);

    vout.PosH = posH;
    vout.TexCoord = vin.TexCoord;
    vout.Color = vin.Color;
	
    return vout;
}

float4 PS(VertexOut pin) : SV_Target
{
    return color;
}