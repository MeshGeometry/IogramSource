#include "Uniforms.glsl"
#include "Samplers.glsl"
#include "Transform.glsl"
#include "ScreenPos.glsl"
#include "PostProcess.glsl"

varying vec2 vTexCoord;
varying vec2 vScreenPos;

uniform float cEpsilon;
uniform float cPower;

#define FAR_CLIP 5.0
#define PI 3.1415

const float t = 10.0;

/* Rotations */

void rX(inout vec3 p, float a) {
    vec3 q = p;
    float c = cos(a);
    float s = sin(a);
    p.y = c * q.y - s * q.z;
    p.z = s * q.y + c * q.z;
}

void rY(inout vec3 p, float a) {
    vec3 q = p;
    float c = cos(a);
    float s = sin(a);
    p.x = c * q.x + s * q.z;
    p.z = -s * q.x + c * q.z;
}

void rZ(inout vec3 p, float a) {
    vec3 q = p;
    float c = cos(a);
    float s = sin(a);
    p.x = c * q.x - s * q.y;
    p.y = s * q.x + c * q.y;
}

//mandelbulb distance function from
//http://blog.hvidtfeldts.net/index.php/2011/09/distance-estimated-3d-fractals-v-the-mandelbulb-different-de-approximations/
vec2 mandelbulb(vec3 rp) {
    vec3 z = rp;
	float dr = 1.0;
	float r = 0.0;
    float its = 0.0;
    //apply phase shift
    float phase = mod(t * 0.05, 1.0) * 2.0 * PI;
	for (int i = 0; i < 10 ; i++) {
		r = length(z);
		if (r > 4.0) break;
		
		// convert to polar coordinates
        //phase applied to theta not phi? My maths is poor so I'm not sure where the
        //descrepency comes from
		float theta = acos(z.z / r) + phase;
		float phi = atan(z.y, z.x);
		dr = pow( r, cPower - 1.0) * cPower * dr + 1.0;
		
		// scale and rotate the point
		float zr = pow(r, cPower);
		theta = theta * cPower;
		phi = phi * cPower;
		
		// convert back to cartesian coordinates
		z = zr * vec3(sin(theta) * cos(phi), sin(phi) * sin(theta), cos(theta));
		z += rp;
        its+=1.0;
	}
	return vec2(0.5 * log(r) * r / dr, its);
}

//convenient wrapper function
float lmandelbulb(vec3 rp) {
    vec2 res = mandelbulb(rp);
    return res.x;
}

//IQ - AO
float calcAO(vec3 pos, vec3 nor) {
    float occ = 0.0;
    float sca = 1.0;
    for(int i = 0; i < 5; i++)  {
        float hr = 0.01 + 0.05 * float(i);
        vec3 aopos = pos + nor * hr;
        occ += smoothstep(0.0, 0.7, hr - lmandelbulb(aopos)) * sca;
        sca *= 0.97;
    }
    return clamp(1.0 - 3.0 * occ , 0.0, 1.0);
}

vec3 surfaceNormal(vec3 rp) {
    vec3 dx = vec3(cEpsilon, 0.0, 0.0);
    vec3 dy = vec3(0.0, cEpsilon, 0.0);
    vec3 dz = vec3(0.0, 0.0, cEpsilon);
    return normalize(vec3(lmandelbulb(rp + dx) - lmandelbulb(rp - dx),
                          lmandelbulb(rp + dy) - lmandelbulb(rp - dy),
                          lmandelbulb(rp + dz) - lmandelbulb(rp - dz)));
}

// Shane - Standard hue rotation formula... compacted down a bit.
vec3 rotHue(vec3 p, float a){
    vec2 cs = sin(vec2(1.570796, 0) + a);
    mat3 hr = mat3(0.299,  0.587,  0.114,  0.299,  0.587,  0.114,  0.299,  0.587,  0.114) +
        	  mat3(0.701, -0.587, -0.114, -0.299,  0.413, -0.114, -0.300, -0.588,  0.886) * cs.x +
        	  mat3(0.168,  0.330, -0.497, -0.328,  0.035,  0.292,  1.250, -1.050, -0.203) * cs.y;

    return clamp(p*hr, 0., 1.);
}

vec3 marchScene(vec3 ro, vec3 rd) {
    
    vec3 pc = vec3(0.0); //pixel colour
    float d = 0.0; //distance marched
    vec3 rp = vec3(0.0); //ray position
    
    vec2 curDf = vec2(9999.0, 0.0); //distance , escape iterations
    
    for (int i = 0; i < 90; i++) {
        rp = ro + rd * d;
        curDf = mandelbulb(rp); //nearest surface
        d += curDf.x;
        if (curDf.x < cEpsilon || d > FAR_CLIP) break;
    }
    
    if (d < FAR_CLIP) {
        //simple colouring
        vec3 n = surfaceNormal(rp);
        float ao = calcAO(rp, n);
        vec3 col = rotHue(vec3(1.0, sin(d), cos(d)), mod(t/16., 6.283));
        pc = mix(vec3(1.0) / d, col, curDf.y * 0.15) * ao;
    }
    
    return pc;
}

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
    //coordinate system
    vec2 uv = vTexCoord;
    uv = uv * 2.0 - 1.0;
    //uv.x *= vTexCoord.x / vTexCoord.y;
    
    //camera
    vec3 rd = normalize(vec3(uv, 2.));
    vec3 ro = vec3(0.0, 0.0, -2.5);
    
    //camera rotation
    rY(ro, cElapsedTimePS * 0.1);
    rY(rd, cElapsedTimePS * 0.1);
    rX(ro, 2.8 + sin(cElapsedTimePS) * 0.1);
    rX(rd, 2.8 + sin(cElapsedTimePS) * 0.1);
    
    gl_FragColor = vec4(marchScene(ro, rd) , 1.0);

    //gl_FragColor = vec4(0.5, 1.0, 1.0, 1.0);
}
