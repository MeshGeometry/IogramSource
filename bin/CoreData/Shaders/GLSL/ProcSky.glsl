//  @class ProcSky
//  @brief Procedural Sky component for Urho3D
//  @author carnalis <carnalis.j@gmail.com>
//  @license MIT License
//  @copyright
//  Permission is hereby granted, free of charge, to any person obtaining a copy
//  of this software and associated documentation files (the "Software"), to deal
//  in the Software without restriction, including without limitation the rights
//  to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
//  copies of the Software, and to permit persons to whom the Software is
//  furnished to do so, subject to the following conditions:
//
//  The above copyright notice and this permission notice shall be included in
//  all copies or substantial portions of the Software.
//
//  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
//  IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
//  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
//  AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
//  LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
//  OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
//  THE SOFTWARE.

#include "Uniforms.glsl"
#include "ScreenPos.glsl"
#include "Transform.glsl"
#line 4

varying vec2 vTexCoord;

void VS() {
  mat4 modelMatrix = iModelMatrix;
  vec3 worldPos = GetWorldPos(modelMatrix);
  gl_Position = GetClipPos(worldPos);
  #ifndef GL_ES
  gl_Position.z = gl_Position.w;
  #else
  // On OpenGL ES force Z slightly in front of far plane to avoid clipping artifacts due to inaccuracy
  gl_Position.z = 0.999 * gl_Position.w;
  #endif
  vTexCoord = GetQuadTexCoord(gl_Position);
  // Flip GLSL texture coord vertically.
  //vTexCoord.y = 1.0 - vTexCoord.y;
}


#if defined(COMPILEPS)

uniform mat4 cInvProj;
uniform mat3 cInvViewRot;
uniform vec3 cLightDir;
uniform vec3 cKr;
uniform float cRayleighBrightness, cMieBrightness, cSpotBrightness, cScatterStrength, cRayleighStrength, cMieStrength, cRayleighCollectionPower, cMieCollectionPower, cMieDistribution;

float surfaceHeight = 0.99; // < 1
float intensity = 1.8;
const int stepCount = 16;

vec3 GetWorldNormal() {
  vec2 fragCoord = vTexCoord;
  fragCoord = (fragCoord - 0.5) * 2.0;
  vec4 deviceNormal = vec4(fragCoord, 0.0, 1.0);
  vec3 eyeNormal = normalize((cInvProj * deviceNormal).xyz);
  vec3 worldNormal = normalize(cInvViewRot * eyeNormal);
  return worldNormal;
}

float AtmosphericDepth(vec3 pos, vec3 dir) {
  float a = dot(dir, dir);
  float b = 2.0 * dot(dir, pos);
  float c = dot(pos, pos) - 1.0;
  float det = b * b - 4.0 * a * c;
  float detSqrt = sqrt(det);
  float q = (-b - detSqrt) / 2.0;
  float t1 = c / q;
  return t1;
}

float Phase(float alpha, float g) {
  float a = 3.0 * (1.0 - g * g);
  float b = 2.0 * (2.0 + g * g);
  float c = 1.0 + alpha * alpha;
  float d = pow(1.0 + g * g - 2.0 * g * alpha, 1.5);
  return (a / b) * (c / d);
}

float HorizonExtinction(vec3 pos, vec3 dir, float radius) {
  float u = dot(dir, -pos);
  if(u < 0.0) {
    return 1.0;
  }
  vec3 near = pos + u * dir;
  if(length(near) < radius) {
    return 0.0;
  } else {
    vec3 v2 = normalize(near) * radius - pos;
    float diff = acos(dot(normalize(v2), dir));
    return smoothstep(0.0, 1.0, pow(diff * 2.0, 3.0));
  }
}

vec3 Absorb(float dist, vec3 color, float factor) {
  return color - color * pow(cKr, vec3(factor / dist));
}

#endif // defined(COMPILEPS)

void PS() {
  vec3 lightDir = cLightDir;
  lightDir.z *= -1.0; // Invert world Z for Urho.
  vec3 eyeDir = GetWorldNormal();
  float alpha = clamp(dot(eyeDir, lightDir), 0, 1);
  float rayleighFactor = Phase(alpha, -0.01) * cRayleighBrightness;
  float mieFactor = Phase(alpha, cMieDistribution) * cMieBrightness;
  float spot = smoothstep(0.0, 15.0, Phase(alpha, 0.9995)) * cSpotBrightness;
  vec3 eyePos = vec3(0.0, surfaceHeight, 0.0);
  float eyeDepth = AtmosphericDepth(eyePos, eyeDir);
  float stepLength = eyeDepth / float(stepCount);
  float eyeExtinction = HorizonExtinction(eyePos, eyeDir, surfaceHeight - 0.15);

  vec3 rayleighCollected = vec3(0.0, 0.0, 0.0);
  vec3 mieCollected = vec3(0.0, 0.0, 0.0);

  for(int i = 0; i < stepCount; ++i) {
    float sampleDistance = stepLength * float(i);
    vec3 pos = eyePos + eyeDir * sampleDistance;
    float extinction = HorizonExtinction(pos, lightDir, surfaceHeight - 0.35);
    float sampleDepth = AtmosphericDepth(pos, lightDir);
    vec3 influx = Absorb(sampleDepth, vec3(intensity), cScatterStrength) * extinction;
    rayleighCollected += Absorb(sampleDistance, cKr * influx, cRayleighStrength);
    mieCollected += Absorb(sampleDistance, influx, cMieStrength);
  }

  rayleighCollected = (rayleighCollected * eyeExtinction * pow(eyeDepth, cRayleighCollectionPower)) / float(stepCount);
  mieCollected = (mieCollected * eyeExtinction * pow(eyeDepth, cMieCollectionPower)) / float(stepCount);

  vec3 color = vec3(spot*mieCollected + mieFactor*mieCollected + rayleighFactor*rayleighCollected);
  color += vec3(0.1,0.2,0.3);
  gl_FragColor = vec4(color, 1.0);
}
