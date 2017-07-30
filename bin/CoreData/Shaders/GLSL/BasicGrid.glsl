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

#ifdef COMPILEPS

#extension GL_OES_standard_derivatives : enable

uniform float cGridScale;

float IsAxis(vec2 coord, float lineWidth)
{
    
    // Compute anti-aliased world-space grid lines
    coord *= 0.00001;
    vec2 grid = abs(fract(coord - 0.5) - 0.5) / fwidth(lineWidth * coord);
    float line = min(grid.x, grid.y);

    return line;
}

// This helper function returns 1.0 if the current pixel is on a grid line, 0.0 otherwise
float IsGridLine(vec2 coord, float scale, float lineWidth)
{
    // Compute anti-aliased world-space grid lines
    coord *= scale;
    vec2 grid = abs(fract(coord - 0.5) - 0.5) / fwidth(lineWidth * coord);
    float line = min(grid.x, grid.y);

    line = 1.0 - smoothstep(pow(line, 2.0), 0.0, 1.0);

    return line;
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

}

void PS()
{

    
    float depth = 0.01 / abs(vWorldPos.w);
    depth = min(0.5, depth);

    vec2 coord = vTexCoord * cGridScale;

    float minorLine = IsGridLine(coord, 10.0, 1.0);
    float majorLine = IsGridLine(coord, 1.0, 1.0);

    float sum = 0.5 * (0.7 * minorLine + majorLine);
    float alpha = 0.5 * depth * (minorLine + majorLine);

    //visualize the grid lines directly
    gl_FragColor = vec4(alpha) * cMatDiffColor;
}


