
// Taken from mts3d forums, from user fredrik.

#include "Uniforms.glsl"
#include "Samplers.glsl"
#include "Transform.glsl"
#include "ScreenPos.glsl"

varying vec2 vTexCoord;
varying vec2 vScreenPos;

//uniform vec2 cWindowSize;
//uniform vec2 cLensCenter;

const vec2 LeftLensCenter = vec2(0.5, 0.5);
const vec2 RightLensCenter = vec2(0.5, 0.5);
const vec2 LeftScreenCenter = vec2(0.5, 0.5);
const vec2 RightScreenCenter = vec2(0.5, 0.5);
const vec2 Scale = vec2(0.1469278, 0.2350845);
const vec2 ScaleIn = vec2(4, 2.5);
const vec4 HmdWarpParam   = vec4(1, 0.2, 0.1, 0);
const float aberr_r = 0.985;
const float aberr_b = 1.015;
// const vec4 HmdWarpParam   = vec4(1, 0.5, 0.05, 0);


void VS()
{
    mat4 modelMatrix = iModelMatrix;
    vec3 worldPos = GetWorldPos(modelMatrix);
    gl_Position = GetClipPos(worldPos);
    vTexCoord = GetQuadTexCoord(gl_Position);
    vScreenPos = GetScreenPosPreDiv(gl_Position);
}


void PS()
{
	
	//float halfX = cWindowSize.x / 2.0;
	//float halfY = cWindowSize.y /2.0;

	vec2 LensCenter = LeftLensCenter;
	vec2 ScreenCenter = LeftScreenCenter;

	// The following two variables need to be set per eye
	// vec2 LensCenter = gl_FragCoord.x < halfX? LeftLensCenter : RightLensCenter;
	// vec2 ScreenCenter = gl_FragCoord.x < halfX ? LeftScreenCenter : RightScreenCenter;

	vec2 oTexCoord = vTexCoord; //gl_FragCoord.xy / cWindowSize;


	vec2 theta = (oTexCoord - LensCenter) * ScaleIn; // Scales to [-1, 1]
	float rSq = theta.x * theta.x + theta.y * theta.y;
	vec2 rvector = theta * (HmdWarpParam.x + HmdWarpParam.y * rSq +
		HmdWarpParam.z * rSq * rSq +
		HmdWarpParam.w * rSq * rSq * rSq);
	vec2 tc_r = LensCenter + aberr_r * Scale * rvector;
	vec2 tc_g = LensCenter +           Scale * rvector;
	vec2 tc_b = LensCenter + aberr_b * Scale * rvector;
	// tc_r.x = gl_FragCoord.x < halfX ? (2.0 * tc_r.x) : (2.0 * (tc_r.x - 0.5));
	// tc_g.x = gl_FragCoord.x < halfX ? (2.0 * tc_g.x) : (2.0 * (tc_g.x - 0.5));
	// tc_b.x = gl_FragCoord.x < halfX ? (2.0 * tc_b.x) : (2.0 * (tc_b.x - 0.5));

	float rval = 0.0;
	float gval = 0.0;
	float bval = 0.0;
	
	// tc_r.y = (1-tc_r.y);
	// tc_g.y = (1-tc_g.y);
	// tc_b.y = (1-tc_b.y);

	rval = texture2D(sDiffMap, tc_r).x;
	gval = texture2D(sDiffMap, tc_g).y;
	bval = texture2D(sDiffMap, tc_b).z;

	if(tc_r.x > 1.0 || tc_r.y > 1.0 || tc_r.x < 0 || tc_r.y < 0)
	{
		rval = 0.0;
	}

	if(tc_g.x > 1.0 || tc_g.y > 1.0 || tc_g.x < 0 || tc_g.y < 0)
	{
		gval = 0.0;
	}

	if(tc_b.x > 1.0 || tc_b.y > 1.0 || tc_b.x < 0 || tc_b.y < 0)
	{
		bval = 0.0;
	}


	gl_FragColor = vec4(rval,gval,bval,1.0);
	//gl_FragColor = vec4(0.1,0.1,0.1,1.0);
}
