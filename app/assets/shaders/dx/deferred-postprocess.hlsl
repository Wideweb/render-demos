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

Texture2D colorBuffer  : register(t0);
Texture2D normalBuffer : register(t1);
Texture2D depthBuffer  : register(t2);

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
    float4 PosH      : SV_POSITION;
    float3 PosNDC    : POSITION0;
    float2 TexCoord  : TEXCOORD;
};

VertexOut VS(VertexIn vin)
{
	VertexOut vout;

    vout.PosH = float4(vin.PosL * float3(1.0, -1.0, 1.0), 1.0);
    vout.PosNDC = vin.PosL * float3(1.0, -1.0, 1.0);
    vout.TexCoord = vin.TexCoord;
	
    return vout;
}

float4 PS(VertexOut pin) : SV_Target
{
    float4 color  = colorBuffer.Sample(gsamLinearWrap, pin.TexCoord);
    float2 normalEncoded = normalBuffer.Sample(gsamLinearWrap, pin.TexCoord);
    float  depth  = depthBuffer.Sample(gsamLinearWrap, pin.TexCoord);

    float normalZ = sqrt(1.0 - normalEncoded.x * normalEncoded.x - normalEncoded.y * normalEncoded.y);
    float3 normal = float3(normalEncoded, normalZ);

    // const float zNear = 0.1f;
    // const float zFar  = 200.0f;

    // depth = (1 / z - 1 / zNear) / (1 / zFar - 1 / zNear)
    // float linearDepth = (zFar * zNear) / (depth * (zNear - zFar) + zFar);

    // float g = projection[1][1];
    // float s = g / projection[0][0];

    float4 fragClipPos = float4(pin.PosNDC.xy, depth, 1.0);
    float4 fragViewPos = mul(fragClipPos, projectionInv);
    fragViewPos /= fragViewPos.w;
    float3 fragWorldPos = mul(fragViewPos, viewInv).xyz;

    // float3 viewPos = float3(pin.PosNDC.x * linearDepth / projection[0][0], pin.PosNDC.y * linearDepth / projection[1][1], linearDepth);
    // float4 worldPos = mul(float4(viewPos, 1.0), viewInv);
    // float t = linearDepth / viewRay.z;
    // float3 pos = viewRay * t;

    float3 toEye = normalize(viewPos - fragWorldPos);
    float4 ambient = ambientLight * color;

    Material mat = { color, float3(0.01, 0.01, 0.01), 0.5 };

    float4 directLight = ComputeLighting(light, mat, fragWorldPos, normal, toEye, 1.0f);

    float4 litColor = ambient + directLight;
    litColor.a = color.a;

    return litColor;
}