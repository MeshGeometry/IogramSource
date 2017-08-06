// Created by sebastien durand - 11/2016
//-------------------------------------------------------------------------------------
// Based on "Type 2 Supernova" by Duke (https://www.shadertoy.com/view/lsyXDK) 
// Sliders from IcePrimitives by Bers (https://www.shadertoy.com/view/MscXzn)
// License: Creative Commons Attribution-NonCommercial-ShareAlike 3.0 Unported License
//-------------------------------------------------------------------------------------
#include "Uniforms.glsl"
#include "Samplers.glsl"
#include "Transform.glsl"
#include "ScreenPos.glsl"
#include "PostProcess.glsl"



varying vec2 vTexCoord;
varying vec2 vScreenPos;

#ifdef COMPILEPS
#define SPIRAL_NOISE_ITER 8

float hash( const in vec3 p ) {
    return fract(sin(dot(p,vec3(127.1,311.7,758.5453123)))*43758.5453123);
}

float pn(in vec3 x) {
    vec3 p = floor(x), f = fract(x);
    f *= f*(3.-f-f);
    vec2 uv = (p.xy+vec2(37.,17.)*p.z) + f.xy,
         rg = texture2D( sDiffMap, (uv+.5)/256., -100.).yx;
    return 2.4*mix(rg.x, rg.y, f.z)-1.;
}

//-------------------------------------------------------------------------------------
// otaviogood's noise from https://www.shadertoy.com/view/ld2SzK
//--------------------------------------------------------------
// This spiral noise works by successively adding and rotating sin waves while increasing frequency.
// It should work the same on all computers since it's not based on a hash function like some other noises.
// It can be much faster than other noise functions if you're ok with some repetition.
const float nudge = 20.;    // size of perpendicular vector
float normalizer = 1.0 / sqrt(1.0 + nudge*nudge);   // pythagorean theorem on that perpendicular to maintain scale
float SpiralNoiseC(vec3 p, vec4 id)
{
    float n = 2.-id.x, // noise amount
          iter = 2.;
    for (int i = 0; i < SPIRAL_NOISE_ITER; i++) {
        // add sin and cos scaled inverse with the frequency
        n += -abs(sin(p.y*iter) + cos(p.x*iter)) / iter;    // abs for a ridged look
        // rotate by adding perpendicular and scaling down
        p.xy += vec2(p.y, -p.x) * nudge;
        p.xy *= normalizer;
        // rotate on other axis
        p.xz += vec2(p.z, -p.x) * nudge;
        p.xz *= normalizer;  
        // increase the frequency
        iter *= id.y + .733733;
    }
    return n;
}

float map(vec3 p, vec4 id) {
    float k = 2.*id.w +.1;
  //  p/=k;
    return k*(.5+ // +.2*cos(20.*iGlobalTime); // pour des bacteries vivantes
              pn(p*8.5)*.12 + 
              SpiralNoiseC(p.zxy*0.4132+333., id)*3.0);
   
}

//-------------------------------------------------------------------------------------
// [iq: https://www.shadertoy.com/view/MsS3Wc]
//-------------------------------------------------------------------------------------
// HSV to RGB conversion (Adaptation)
vec3 hsv2rgb(float x, float y, float z) {   
    return z+z*y*(clamp(abs(mod(x*6.+vec3(0.,4.,2.),6.)-3.)-1.,0.,1.)-1.);
}

//-------------------------------------------------------------------------------------
// Based on "Type 2 Supernova" by Duke (https://www.shadertoy.com/view/lsyXDK) 
//-------------------------------------------------------------------------------------
vec4 renderSuperstructure(vec3 ro, vec3 rd, const vec4 id) {
    const float max_dist=20.;
    // ld, td: local, total density 
    // w: weighting factor
    // t: length of the ray
    // d: distance function
    float ld, td=0., w=0., d, t, noi, lDist,         
          rRef = 2.*id.x,
          h = .05+.25*id.z;
   
    vec3 pos, lpos, lightColor;
       
    t = .15*hash(rd+cElapsedTimePS); 
   
    vec4 sum = vec4(0);
    
    for (int i=0; i<200; i++)  {
        pos = ro + t*rd;
  
        // Loop break conditions.
        if(td>0.9 ||  sum.a > .99 || t>max_dist) break;
        
        // evaluate distance function
        d = map(pos, id);
        d = abs(d)+.07;
        // change this string to control density 
        d = max(d,.05);
        
        // point light calculations
        lpos = mod(pos+2.5,5.)-2.5;  // TODO add random offset
        lDist = max(length(lpos), .001);
    
        noi = pn(0.01*pos);
        lightColor = mix(hsv2rgb(noi,.5,.6), hsv2rgb(noi+.3,.5,.6), smoothstep(rRef*.5,rRef*2.,lDist));
        sum.rgb += (lightColor/exp(lDist*lDist*lDist*.08)/30.)*smoothstep(max_dist,0.,t);

        if (d<h) {
            // compute local density 
            ld = h - d;
            // compute weighting factor 
            w = (1. - td) * ld;
            // accumulate density
            td += w + .005;
            // emission
            sum.rgb += sum.a * sum.rgb * .25 / lDist;   
            // uniform scale density
            // alpha blend in contribution
            sum += td*.05*smoothstep(max_dist,0.,t)*(1. - sum.a);  
        } 
        td += .015;

      //  t += max(d*0.15,0.01);
        t += max(d * .08 * max(min(lDist,d),2.), .01);  // trying to optimize step size
    }
    
    // simple scattering
    sum *= 1. / exp(ld*.2)*.9;
    sum = clamp(sum, 0., 1.);   
    sum.xyz *= sum.xyz*(3.-sum.xyz-sum.xyz);
    return sum;
}

#endif


// ---------------------------------------------------
// Bers : https://www.shadertoy.com/view/MscXzn
// vec4 processSliders(in vec2 uv, out vec4 sliderVal) {
//     sliderVal = texture2D(iChannel1,vec2(0));
//     if(length(uv.xy)>1.) {
//     	return texture2D(iChannel1,uv.xy/iResolution.xy);
//     }
//     return vec4(0);
// }

#define R(p, a) p=cos(a)*p+sin(a)*vec2(p.y, -p.x)



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


    vec4 sliderVal = vec4(0.5,0.4,0.2,0.4);
    vec2 m = vec2(0.5,1.0);
    vec3 ro = vec3(15.+cElapsedTimePS, cos(.1*cElapsedTimePS), 15.+cElapsedTimePS);
    vec3 rd = normalize(vec3(vScreenPos.xy, 1.));


    R(rd.zx, 3.*m.x);
    R(rd.yx, 1.5*m.y);
    R(rd.xz, cElapsedTimePS*0.1);
       
    // Super Structure
    vec4 col = renderSuperstructure(ro, rd, sliderVal);

    //Apply slider overlay
    gl_FragColor = col;
}