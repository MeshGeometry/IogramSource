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
    vec3 col = vec3(0.5, 0.7, 0.7);
    vec3 eye = normalize(vReflectionVec);
    float dotFact = pow(1-dot(normal, eye), -1)/7.0;
    vec3 lightCol = vec3(0.8, 0.8, 0.8) * clamp(eye, vec3(0.5, 0.2, 0.2), vec3(0.8, 0.3, 0.8));
    col = dotFact * lightCol  * col + 0.2 * col;
    gl_FragColor = col.xyzz;
}
