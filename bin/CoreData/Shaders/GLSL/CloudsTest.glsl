// Created by inigo quilez - iq/2013
// License Creative Commons Attribution-NonCommercial-ShareAlike 3.0 Unported License.

// Volumetric clouds. It performs level of detail (LOD) for faster rendering

#include "Uniforms.glsl"
#include "Samplers.glsl"
#include "Transform.glsl"
#include "ScreenPos.glsl"
#include "PostProcess.glsl"

varying vec2 vTexCoord;
varying vec2 vScreenPos;

float time = 10.0;

#ifdef COMPILEPS

float noise( in vec3 x )
{
    vec3 p = floor(x);
    vec3 f = fract(x);
    f = f*f*(3.0-2.0*f);
    vec2 uv = (p.xy+vec2(37.0,17.0)*p.z) + f.xy;
    vec2 rg = texture2D( sDiffMap, (uv+ 0.5)/256.0, -200.0 ).yx;
    return -1.0+2.0*mix( rg.x, rg.y, f.z );
}

#endif

void VS()
{

}

void PS()
{
    float n = noise(vec3(0.5, 0.5, 0.5));
    gl_FragColor = vec4(n,n,n,n);
}