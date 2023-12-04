#include "libs/common.hlsl"
#include "libs/light.hlsl"
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

cbuffer cbMaterial : register(b2)
{
	float4 diffuseAlbedo;
    float3 fresnelR0;
    float  roughness;
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
    float3 PosW      : POSITION0;
    float3 PosV      : POSITION1;
    float4 PosH      : SV_POSITION;
    float3 NormalV   : NORMAL;
};

struct OutputData
{
    float4 Color  : SV_Target0;
    float2 Normal : SV_Target1;
};

VertexOut VS(VertexIn vin)
{
	VertexOut vout;

    float4 pos = float4(vin.PosL, 1.0);
    float4 posW = mul(pos, model);
    float4 posV = mul(posW, view);
    float4 posH = mul(posV, projection);

    vout.PosW = posW.xyz;
    vout.PosV = posV.xyz;
    vout.PosH = posH;

    float3 normal = mul(vin.NormalL, (float3x3) model);
    normal = mul(normal, (float3x3) view);

    vout.NormalV = normal;
	
    return vout;
}

OutputData PS(VertexOut pin) : SV_Target
{
    OutputData output;

    output.Color = diffuseAlbedo;
    output.Normal = normalize(pin.NormalV).xy;

    return output;
}