cbuffer cbData : register(b0)
{
    float2 colorBufferSize;
};

Texture2D<float4> colorBuffer     : register(t0);
Texture2D<float2> motionBuffer    : register(t1);
RWTexture2D<float4> outputBuffer  : register(u0);

[numthreads(16, 16, 1)]
void CS(uint3 dispatchThreadId : SV_DispatchThreadID)
{
    float2 fragmentMotion = motionBuffer[dispatchThreadId.xy] * colorBufferSize * float2(1.0, -1.0) * 5.0;

    float halfSteps = length(fragmentMotion) * 0.5;
    halfSteps = min(halfSteps, 10.0);

    float4 fragmentColor = colorBuffer[dispatchThreadId.xy];
    float accumulator = 1.0;
    for (float i = -halfSteps + 0.5; i < halfSteps - 0.5; i += 1.0)
    {
        float t = i / halfSteps;
        float weight = 1.0 - abs(t);

        float2 stepIndex = dispatchThreadId.xy + fragmentMotion * t;
        stepIndex = clamp(stepIndex, float2(0.0, 0.0), colorBufferSize - float2(1.0, 1.0));

        fragmentColor += colorBuffer[stepIndex] * weight;
        accumulator += weight;
    }

    fragmentColor /= accumulator;

    outputBuffer[dispatchThreadId.xy] = fragmentColor;
}