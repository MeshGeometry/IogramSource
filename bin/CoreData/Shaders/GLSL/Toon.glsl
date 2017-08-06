#include "Uniforms.glsl"
#include "Samplers.glsl"
#include "Transform.glsl"
#include "ScreenPos.glsl"
#include "Lighting.glsl"

varying vec2 vScreenPos;

precision mediump float;

void VS()
{
  mat4 modelMatrix = iModelMatrix;
  vec3 worldPos = GetWorldPos(modelMatrix);
  gl_Position = GetClipPos(worldPos);
  vScreenPos = GetScreenPosPreDiv(gl_Position);
}

void PS()
{
  float ResS = 1440.;
  float ResT = 1440.;
  float MagTol = 0.2;
  float Quantize = 10.;
	
  vec3 irgb = texture2D(sDiffMap, vScreenPos).rgb;
  vec2 stp0 = vec2(1./ResS, 0.);
  vec2 st0p = vec2(0., 1./ResT);
  vec2 stpp = vec2(1./ResS, 1./ResT);
  vec2 stpm = vec2(1./ResS, -1./ResT);
	
  const vec3 W = vec3(0.2125, 0.7154, 0.0721);
  float i00 =   dot(texture2D(sDiffMap, vScreenPos).rgb, W);
  float im1m1 =	dot(texture2D(sDiffMap, vScreenPos-stpp).rgb, W);
  float ip1p1 = dot(texture2D(sDiffMap, vScreenPos+stpp).rgb, W);
  float im1p1 = dot(texture2D(sDiffMap, vScreenPos-stpm).rgb, W);
  float ip1m1 = dot(texture2D(sDiffMap, vScreenPos+stpm).rgb, W);
  float im10 = 	dot(texture2D(sDiffMap, vScreenPos-stp0).rgb, W);
  float ip10 = 	dot(texture2D(sDiffMap, vScreenPos+stp0).rgb, W);
  float i0m1 = 	dot(texture2D(sDiffMap, vScreenPos-st0p).rgb, W);
  float i0p1 = 	dot(texture2D(sDiffMap, vScreenPos+st0p).rgb, W);
	
  //H and V sobel filters
  float h = -1.*im1p1 - 2.*i0p1 - 1.*ip1p1 + 1.*im1m1 + 2.*i0m1 + 1.*ip1m1;
  float v = -1.*im1m1 - 2.*im10 - 1.*im1p1 + 1.*ip1m1 + 2.*ip10 + 1.*ip1p1;
  float mag = length(vec2(h, v));
	
  if(mag > MagTol){
    gl_FragColor = vec4(0.1, 0.1, 0.1, 1.);
  }else{
    irgb.rgb *= Quantize;
    irgb.rgb += vec3(.5,.5,.5);
    ivec3 intrgb = ivec3(irgb.rgb);
    irgb.rgb = vec3(intrgb)/Quantize;
    //gl_FragColor = vec4(irgb, 1.);
    gl_FragColor = texture2D(sDiffMap, vScreenPos);
  }
}