#include "Uniforms.glsl"
#include "Samplers.glsl"
#include "Transform.glsl"
#include "ScreenPos.glsl"

varying vec2 vScreenPos;

void VS()
{
    mat4 modelMatrix = iModelMatrix;
    vec3 worldPos = GetWorldPos(modelMatrix);
    gl_Position = GetClipPos(worldPos);
    vScreenPos = GetScreenPosPreDiv(gl_Position);
}

void PS()
{
#if 0
    gl_FragColor = texture2D(sEmissiveMap, vScreenPos);
    return;
#endif

    float occlusion = texture2D(sEmissiveMap, vScreenPos).r;
#ifdef AO_ONLY
    vec3 color = vec3(occlusion);
#else
    vec3 color = texture2D(sDiffMap, vScreenPos).rgb * occlusion;
#endif
    gl_FragColor = vec4(color, 1.0);
}