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
varying float vDepth;
varying vec4 vColor;


#ifdef COMPILEPS

#extension GL_OES_standard_derivatives : enable

uniform float cLineWidth;

float edgeFactor(vec3 vBC)
{
    vec3 d = fwidth(cLineWidth * vBC);
    vec3 a3 = smoothstep(vec3(0.0), d*1.5, vBC);
    return min(min(a3.x, a3.y), a3.z);
}

#endif

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
    vDepth = cCameraPos.y;
    vColor = iColor;
}


void PS()
{
    vec3 vBC = vColor.xyz;
    vec4 lineCol = vec4(1.0, 1.0, 1.0, (1.0-pow(edgeFactor(vBC), 2.0)));
    float dotVal = dot(vNormal, vReflectionVec);
    dotVal = (dotVal < 0.0) ? 1.0 : 0.2;
    lineCol.a *= dotVal;
    gl_FragColor = lineCol * cMatDiffColor;
}