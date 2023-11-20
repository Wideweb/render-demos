#include "libs/common.hlsl"
#include "libs/light.hlsl"
#include "libs/fog.hlsl"

cbuffer cbCommon : register(b0)
{
    float4x4 view;
    float4x4 projection;
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

cbuffer cbMotion : register(b3)
{
    float4x4 prevView;
    float4x4 prevProjection;
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
    float3 PosW         : POSITION0;
    float3 PosV         : POSITION1;
    float4 PosH         : SV_POSITION;
    float4 PrevPosH     : POSITION2;
    float4 CurrPosH     : POSITION3;
    float3 NormalW      : NORMAL;
};

struct OutputData
{
    float4 Color  : SV_Target0;
    float2 Motion : SV_Target1;
};

VertexOut VS(VertexIn vin)
{
	VertexOut vout;

    float4 pos = float4(vin.PosL, 1.0);
    float4 posW = mul(pos, model);
    float4 posV = mul(posW, view);
    float4 posH = mul(posV, projection);

    float4 prevPosV = mul(posW, prevView);
    float4 prevPosH = mul(prevPosV, prevProjection);

    vout.PosW = posW.xyz;
    vout.PosV = posV.xyz;
    vout.PosH = posH;
    vout.PrevPosH = prevPosH;
    vout.CurrPosH = posH;
    vout.NormalW = mul(vin.NormalL, (float3x3) model);
	
    return vout;
}

OutputData PS(VertexOut pin) : SV_Target
{
    OutputData output;

    float3 normal = normalize(pin.NormalW);

    float3 toEye = normalize(viewPos - pin.PosW);
    float4 ambient = ambientLight * diffuseAlbedo;

    const float shininess = 1.0f - roughness;
    Material mat = { diffuseAlbedo, fresnelR0, shininess };

    float4 directLight = ComputeLighting(light, mat, pin.PosW, normal, toEye, 1.0f);

    float4 litColor = ambient + directLight;
    litColor.a = diffuseAlbedo.a;

    output.Color = litColor;// lerp(litColor, FOG_COLOR, FogFactor(pin.PosW, viewPos, time));

    float2 currScreenPos = pin.CurrPosH.xy / pin.CurrPosH.w * 0.5 + float2(0.5, 0.5);
	float2 prevScreenPos = pin.PrevPosH.xy / pin.PrevPosH.w * 0.5 + float2(0.5, 0.5);

	output.Motion = currScreenPos - prevScreenPos;

    return output;
}