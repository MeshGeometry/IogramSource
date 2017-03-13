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
    gl_FragColor = 0.5*(vec4(1,1,1,1) + normal.xyzz);
}
