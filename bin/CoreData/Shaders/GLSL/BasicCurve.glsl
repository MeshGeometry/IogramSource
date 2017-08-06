#include "Uniforms.glsl"
#include "Samplers.glsl"
#include "Transform.glsl"
#include "ScreenPos.glsl"
#include "Fog.glsl"

varying vec2 vTexCoord;
varying vec4 vWorldPos;
#ifdef VERTEXCOLOR
    varying vec4 vColor;
#endif

#ifdef COMPILEPS
float DistanceToLineSegment(in vec2 a, in vec2 b, in vec2 p)
{
    vec2 pa = p - a;
    vec2 ba = b - a;
    float h = clamp(dot(pa,ba)/dot(ba,ba), 0.0, 1.0);
    return length(pa-ba*h);
}

float DistanceToLineSegment3D(in vec3 a, in vec3 b, in vec3 p)
{
    vec3 pa = p - a;
    vec3 ba = b - a;
    float h = clamp(dot(pa,ba)/dot(ba,ba), 0.0, 1.0);
    return length(pa-ba*h);
}
#endif

void VS()
{
    mat4 modelMatrix = iModelMatrix;
    vec3 worldPos = GetWorldPos(modelMatrix);
    gl_Position = GetClipPos(worldPos);
    vTexCoord = GetTexCoord(iTexCoord);
    vWorldPos = vec4(worldPos, GetDepth(gl_Position));
    #ifdef VERTEXCOLOR
        vColor = iColor;
    #endif

}

void PS()
{
    // Get material diffuse albedo
    #ifdef DIFFMAP
        vec4 diffColor = cMatDiffColor * texture2D(sDiffMap, vTexCoord);
        #ifdef ALPHAMASK
            if (diffColor.a < 0.5)
                discard;
        #endif
    #else
        vec4 diffColor = cMatDiffColor;
    #endif

    #ifdef VERTEXCOLOR
        diffColor *= vColor;
    #endif

    // Get fog factor
    #ifdef HEIGHTFOG
        float fogFactor = GetHeightFogFactor(vWorldPos.w, vWorldPos.y);
    #else
        float fogFactor = GetFogFactor(vWorldPos.w);
    #endif

    #ifdef DRAWPOINT
        vec2 center = vec2(0.5, 0.5);
        float maxLength = length(center);
        float d = length(vTexCoord - center);
        float factor = smoothstep(0.49, maxLength - 0.2, d);
        gl_FragColor = vec4(diffColor.rgb, diffColor.a*(1.0 - factor));
    #else
        float xOffset = 0.205;
        float fade = 0.01;
        float factor = smoothstep(xOffset, xOffset + fade, vTexCoord.x) - smoothstep(1.0 - xOffset - fade, 1.0 - xOffset, vTexCoord.x);
        gl_FragColor = vec4(diffColor.rgb, diffColor.a*factor);
    #endif
}
