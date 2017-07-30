#include "Uniforms.glsl"
#include "Samplers.glsl"
#include "Transform.glsl"
#include "ScreenPos.glsl"
#include "Lighting.glsl"
#include "Fog.glsl"

varying vec2 vTexCoord;
varying vec3 vNormal;
varying vec4 vWorldPos;
varying vec4 vScreenPos;
varying vec4 vTangent;
varying vec3 vReflectionVec;

void VS()
{
    mat4 modelMatrix = iModelMatrix;
    vec3 worldPos = GetWorldPos(modelMatrix);
    gl_Position = GetClipPos(worldPos);
    vNormal = GetWorldNormal(modelMatrix);
    vWorldPos = vec4(worldPos, GetDepth(gl_Position));
    vTexCoord = GetTexCoord(iTexCoord);
    vScreenPos = GetScreenPos(gl_Position);
    vReflectionVec = worldPos - cCameraPos;

}

void PS()
{
    float fogFactor = GetFogFactor(vWorldPos.w);
    vec4 diffColor = cMatDiffColor;
    float scale = 0.1;
    float hFactor = pow(scale * vWorldPos.length(),1.0);
    vec3 posColor = vec3(hFactor, 0.1, 1.0 - hFactor);
    gl_FragColor = vec4(GetLitFog(posColor, fogFactor), diffColor.a);
}
