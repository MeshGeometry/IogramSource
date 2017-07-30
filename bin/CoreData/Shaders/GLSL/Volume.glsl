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
varying vec3 vCameraPos;

uniform float StepLength = 0.005;
uniform float Threshold = 0.001;
uniform vec3 LightPosition = vec3(0, 2, -1);
uniform vec3 LightColor = vec3(0.8, 0.8, 0.9);
uniform vec3 DiffuseMaterial = vec3(0.3, 0.3, 0.3);
uniform mat3 NormalMatrix;
uniform vec3 bMin = vec3(0,0,0);
uniform vec3 bMax = vec3(1,1,1);

#ifdef COMPILEPS

float hash( const in vec3 p ) {
    return fract(sin(dot(p,vec3(127.1,311.7,758.5453123)))*43758.5453123);
}


float lookup(vec3 coord)
{
    float d = texture3D(sZoneVolumeMap, coord).x;
    //d = (coord.y > 0.5) ? 0 : d;
    return d;
}


bool IntersectBox(in vec3 ro, in vec3 rd, out float t0, out float t1)
{
    vec3 invR = 1.0 / rd;
    vec3 tbot = invR * (bMin-ro);
    vec3 ttop = invR * (bMax-ro);
    vec3 tmin = min(ttop, tbot);
    vec3 tmax = max(ttop, tbot);
    vec2 t = max(tmin.xx, tmin.yz);
    t0 = max(t.x, t.y);
    t = min(tmax.xx, tmax.yz);
    t1 = min(t.x, t.y);
    return t0 <= t1;
}


vec3 mapPointToBoxSpace(vec3 pt)
{
    //hard code box dims
    // vec3 bMin = vec3(0,0,0);
    // vec3 bMax = vec3(1, 1, 1);

    float x = clamp((pt.x - bMin.x)/(bMax.x - bMin.x), 0.0, 1.0);
    float y = clamp((pt.y - bMin.y)/(bMax.y - bMin.y), 0.0, 1.0);
    float z = clamp((pt.z - bMin.z)/(bMax.z - bMin.z), 0.0, 1.0);

    return vec3(x,y,z);
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
    vCameraPos = cCameraPos;

}

void PS()
{
    //get ray origin and direction
    vec3 ro = vCameraPos.xyz;
    vec3 rd = normalize(vWorldPos.xyz - vCameraPos);

    //intersect with rendering box for actual ray start and end
    float tnear, tfar;
    IntersectBox(ro, rd, tnear, tfar);
    if (tnear < 0.0) tnear = 0.0;

    vec3 rayStart = ro + rd * tnear;
    vec3 rayStop = ro + rd * tfar;
    
    //discard if bounding box is degenerate
     if (rayStart == rayStop) {
        discard;
        return;
    }

    //define ray
    vec3 ray = rayStop - rayStart;
    float rayLength = length(ray);
    vec3 stepVector = StepLength * ray/rayLength;

    //jitter start pos
    rayStart += stepVector * 0.5*hash(ro);;

    //proceed with casting
    vec3 pos = rayStart;
    vec4 dst = vec4(0);
    int safetyCounter = 0;
    while (dst.a < 1 && rayLength > 0.001 && safetyCounter < 1000) {
        
        //safety is on
        safetyCounter++;
        
        //WIP - make sure sample position is within 0 - 1
        pos = mapPointToBoxSpace(pos);

        //read the volume texture
        float V = lookup(pos);

        //if we are still accumulating density
        if (V > Threshold) {

            //do some half stepping to reduce slice artifacts
            vec3 s = -stepVector * 0.5;
            pos += s; V = lookup(pos);
            if (V > Threshold) s *= 0.5; else s *= -0.5;
            pos += s; V = lookup(pos);

            //we have accumulated enough density, render!
            if (V > Threshold) {

                //compute gradient
                float L = StepLength;
                float E = lookup(pos + vec3(L,0,0));
                float N = lookup(pos + vec3(0,L,0));
                float U = lookup(pos + vec3(0,0,L));

                //compute normal
                vec3 normal = normalize(vec3(E - V, N - V, U - V));
                
                //get light position
                vec3 light = LightPosition;

                //get light factor
                float df = dot(-normal, light);

                //define object color from light and material
                vec3 color = 0.9 * df * LightColor * clamp(normal, 0.2, 0.3) +  DiffuseMaterial;

                //make sure alpha is 1.0
                vec4 src = vec4(color, 1.0);
                
                //reverse density accumlation trick
                dst = (1.0 - dst.a) * src + dst;

                break;
            }
        }

        pos += stepVector;
        rayLength -= StepLength;
    }

    gl_FragColor = dst;
}

