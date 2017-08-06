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
    vec3 X = dFdx(vWorldPos.xyz);
    vec3 Y = dFdy(vWorldPos.xyz);
    vec3 normal = normalize(cross(X,Y));
    float fogFactor = GetFogFactor(vWorldPos.w);
    vec4 diffColor = cMatDiffColor;

    // float dotWithUp = dot(normal, vec3(0,1,0));
    // float targetVal = clamp(dotWithUp, 0.9, 1.0);
    // float remap = (targetVal - 0.9)/0.1;
    // vec3 dotColor = vec3(1.0 - remap, 0.1, remap);
    gl_FragColor = vec4(GetLitFog(normal.xyz, fogFactor), diffColor.a);
}
