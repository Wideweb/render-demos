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
    float3 PosL      : POSITION0;
    float3 PosW      : POSITION1;
    float3 PosV      : POSITION2;
    float4 PosNDC    : POSITION4;
    float4 PosH      : SV_POSITION;
    float2 TexCoord  : TEXCOORD;
    float3 ViewL     : POSITION5;
    float3 ViewPosL  : POSITION6;
};

VertexOut VS(VertexIn vin)
{
	VertexOut vout;

    float4 posL = float4(vin.PosL, 1.0);
    float4 posW = mul(posL, model);
    float4 posV = mul(posW, view);
    float4 posH = mul(posV, projection);

    // float4 viewPosL = viewPos - model;

    vout.PosL = posL.xyz;
    vout.PosW = posW.xyz;
    vout.PosV = posV.xyz;
    vout.PosH = posH;
    vout.PosNDC = posH;
    vout.TexCoord = vin.TexCoord;
	
    return vout;
}

float CalculateShaftBrightness(float pz, float3 vdir, float3 viewLocal, float t1, float t2, float currDepth)
{
    float shaftRho0  = 1.0;
    float shaftRho1  = 1.0;
    float shaftSigma = (shaftRho1 - shaftRho0) / 4.0;
    float shaftTau   = -shaftRho0 / shaftSigma;
    float normalizer = 3.0 / (max(shaftRho0, shaftRho1) * 4.0);

    // Read z0 from the structure buffer, calculate t0, and clamp to [t0,1].
    float t0 = 1.0 + currDepth / dot(viewLocal, vdir);
    
    t1 = clamp(t1, t0, 1.0);
    t2 = clamp(t2, t0, 1.0);

    // Limit to range where density is not negative.
    // float tlim = (shaftTau - pz) / vdir.z;
    // if (vdir.z * shaftSigma < 0.0)
    // {
    //     t1 = min(t1, tlim);
    //     t2 = min(t2, tlim);
    // }
    // else
    // {
    //     t1 = max(t1, tlim);
    //     t2 = max(t2, tlim);
    // }

    // pz + vdir.z * t1 = 0.5;
    float tl = (4.0 - pz) / vdir.z;
    float tr = (0.0 - pz) / vdir.z;

    if (vdir.z > 0)
    {
        t1 = clamp(t1, tr, tl);
        t2 = clamp(t2, tr, tl);
    }
    else
    {
        t1 = clamp(t1, tl, tr);
        t2 = clamp(t2, tl, tr);
    }

    // Evaluate density integral, normalize, and square.
    float B = (shaftSigma * (pz + vdir.z * ((t1 + t2) * 0.5)) + shaftRho0) * (t2 - t1) * normalizer;
    return (B * B * dot(vdir, vdir));
}

float CalculateCylinderShaftBrightness(float Rx, float Ry, float3 volumeFragPosLocal, float3 viewLocal, float3 viewPosLocal, float currDepth)
{
    float rx2    = Rx * Rx;
    float ry2    = Ry * Ry;
    float rx2ry2 = rx2 * ry2;

    float3 vdir = viewPosLocal - volumeFragPosLocal;
    float2 v2   = vdir * vdir;
    float2 p2   = volumeFragPosLocal * volumeFragPosLocal;

    // Calculate quadratic coefficients.
    float a = ry2 * v2.x + rx2 * v2.y;
    float b = ry2 * volumeFragPosLocal.x * vdir.x + rx2 * volumeFragPosLocal.y * vdir.y;
    float c = ry2 * p2.x + rx2 * p2.y - rx2ry2;
    float m = sqrt(max(b * b - a * c, 0.0));
    
    // Calculate limits and integrate.
    float t1 = max((-b - m) / (a), 0.0);
    float t2 = max((-b + m) / (a), 0.0);

    return CalculateShaftBrightness(volumeFragPosLocal.z, vdir, viewLocal, t1, t2, currDepth);
}

float CalculateHaloBrightness(float R, float3 volumeFragPosLocal, float3 viewLocal, float3 viewPosLocal, float currDepth)
{
    float R2         = R * R;
    float recipR2    = 1.0 / R2;
    float recip3R2   = recipR2 / 3.0;
    float normalizer = 3.0 / (4.0 * R);

    float3 vdir = viewPosLocal - volumeFragPosLocal;
    
    float v2 = dot(vdir, vdir);
    float p2 = dot(volumeFragPosLocal, volumeFragPosLocal);
    float pv = -dot(volumeFragPosLocal, vdir);
    float m  = sqrt(max(pv * pv - v2 * (p2 - R2), 0.0));

    // vdir.z > 0
    // viewLocal.z < 0

    // Read z0 from the structure buffer.
    float t0 = 1.0 + (currDepth) / dot(viewLocal, vdir);

    // Calculate clamped limits of integration.
    float t1 = clamp((pv - m) / v2, t0, 1.0);
    float t2 = clamp((pv + m) / v2, t0, 1.0);
    float u1 = t1 * t1;
    float u2 = t2 * t2;

    // Evaluate density integral, normalize, and square.
    float B = ((1.0 - p2 * recipR2) * (t2 - t1) + pv * recipR2 * (u2 - u1) - v2 * recip3R2 * (t2 * u2 - t1 * u1)) * normalizer;
    return (B * B * v2);
}

float4 PS(VertexOut pin) : SV_Target
{
    pin.PosNDC = pin.PosNDC / pin.PosNDC.w;
    float2 bufferCoords = pin.PosNDC.xy * 0.5 + 0.5;
    bufferCoords.y = 1.0 - bufferCoords.y;

    float depth = depthBuffer.Sample(gsamLinearWrap, bufferCoords);

    float4 fragClipPos = float4(pin.PosNDC.xy, depth, 1.0);
    float4 fragViewPos = mul(fragClipPos, projectionInv);
    fragViewPos /= fragViewPos.w;

    float3 frontVec = normalize(float3(viewInv[2][0], viewInv[2][1], viewInv[2][2]) * -1.0);

    // float brightness = CalculateHaloBrightness(4.0, pin.PosL, frontVec, viewPos, fragViewPos.z * -1);
    float brightness = CalculateCylinderShaftBrightness(0.5, 0.5, pin.PosL, frontVec, viewPos, fragViewPos.z * -1);

    return float4(1.0, 1.0, 0.1, brightness + 0.1);
}