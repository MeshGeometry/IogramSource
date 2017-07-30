#include "Uniforms.glsl"
#include "Samplers.glsl"
#include "Transform.glsl"
#include "ScreenPos.glsl"

varying highp vec2 vScreenPos;

#ifdef COMPILEVS

void VS()
{
    mat4 modelMatrix = iModelMatrix;
    vec3 worldPos = GetWorldPos(modelMatrix);
    gl_Position = GetClipPos(worldPos);
    vScreenPos = GetScreenPosPreDiv(gl_Position);
}

#endif


#ifdef COMPILEPS

// Port from: https://github.com/jsj2008/Zombie-Blobs/blob/278e16229ccb77b2e11d788082b2ccebb9722ace/src/postproc.fs

// see T Möller, 1999: Efficiently building a matrix to rotate one vector to another
mat3 rotateNormalVecToAnother(vec3 f, vec3 t) {
    vec3 v = cross(f, t);
    float c = dot(f, t);
    float h = (1.0 - c) / (1.0 - c * c);
    return mat3(c + h * v.x * v.x, h * v.x * v.y + v.z, h * v.x * v.z - v.y,
                h * v.x * v.y - v.z, c + h * v.y * v.y, h * v.y * v.z + v.x,
                h * v.x * v.z + v.y, h * v.y * v.z - v.x, c + h * v.z * v.z);
}

vec3 normal_from_depth(float depth, highp vec2 texcoords) {
    
    const vec2 offset1 = vec2(0.0, 0.001);
    const vec2 offset2 = vec2(0.001, 0.0);
    
    float depth1 = DecodeDepth(texture2D(sDepthBuffer, texcoords + offset1).rgb);
    float depth2 = DecodeDepth(texture2D(sDepthBuffer, texcoords + offset2).rgb);
    
    vec3 p1 = vec3(offset1, depth1 - depth);
    vec3 p2 = vec3(offset2, depth2 - depth);
    
    vec3 normal = cross(p1, p2);
    normal.z = -normal.z;
    
    return normalize(normal);
}

void PS()
{
    const float STRENGTH = 1.0;

    const float zNear = 1.0;
    const float zFar = 100.0;
    const highp vec2  screenSize = vec2(1024.0, 768.0);
    
    highp vec2 tx = vScreenPos;
    highp vec2 px = vec2(1.0 / screenSize.x, 1.0 / screenSize.y);
    
    float depth = DecodeDepth(texture2D(sDepthBuffer, vScreenPos).rgb);
    vec3  normal = normal_from_depth(depth, vScreenPos);
    
    float radius = 0.6;
    float zRange = radius / (zFar - zNear);
    
    // The inverse matrix of the normal
    mat3 InverseNormalMatrix = rotateNormalVecToAnother(normal, vec3(0.0, 0.0, 1.0));
    
    // result of line sampling
    // See Loos & Sloan: Volumetric Obscurance
    // http://www.cs.utah.edu/~loos/publications/vo/vo.pdf
    float hemi = 0.0;
    float maxi = 0.0;
    
    for (int x = -2; x <= 2; ++x) {
        for (int y = -2; y <= 2; ++y) {
            // Ignore itself
            if (x == 0 && y == 0) continue;

            // Smaller hemisphere
            float rx = 0.3 * x;
            float ry = 0.3 * y;
            float rz = sqrt(1.0 - rx * rx - ry * ry);
            
            highp vec3 screenCoord = vec3(x * px.x, y * px.y, 0.0);
            // just a guess, should sample some constant sized disc in world coords
            highp vec2 coord = tx + (5.0 - 4.0 * depth) * screenCoord.xy;
            screenCoord.z = DecodeDepth(texture2D(sDepthBuffer, coord).rgb);
            screenCoord.z -= depth;

            // Ignore geometries beyond radius
            if (screenCoord.z < -zRange) continue;

            // Transform to normal-oriented hemisphere space
            highp vec3 localCoord = InverseNormalMatrix * screenCoord;
            float dr = localCoord.z / zRange;
            float v = max(min(rz, STRENGTH * dr) + rz, 0);

            maxi += rz;
            hemi += v;
        }
    }

    hemi = hemi / maxi;

    gl_FragColor = vec4(vec3(hemi), 1.0);
}

#endif
