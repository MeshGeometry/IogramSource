#include "Uniforms.glsl"
#include "Samplers.glsl"
#include "Transform.glsl"
#include "ScreenPos.glsl"
#include "Lighting.glsl"

varying vec2 vScreenPos;
varying vec2 vTexCoord;
varying vec2 vScreenPosFull;

#ifdef COMPILEPS

uniform float cBlurClamp;  // max blur amount
uniform float cBias; //aperture - bigger values for shallower depth of field
uniform float cFocus;  // this value comes from ReadDepth script.

float normpdf(in float x, in float sigma)
{
    return 0.39894*exp(-0.5*x*x/(sigma*sigma))/sigma;
}

vec3 OnePassBlur(sampler2D texSampler, vec2 texCoord, float blurScale)
{
    //declare stuff
    const int mSize = 9;
    const int kSize = (mSize-1)/2;
    float kernel[mSize];
    vec3 final_colour = vec3(0.0);
    
    //create the 1-D kernel
    float sigma = 7.0;
    float Z = 0.0;
    for (int j = 0; j <= kSize; ++j)
    {
        kernel[kSize+j] = kernel[kSize-j] = normpdf(float(j), sigma);
    }
    
    //get the normalization factor (as the gaussian has been clamped)
    for (int j = 0; j < mSize; ++j)
    {
        Z += kernel[j];
    }
    
    //read out the texels
    for (int i=-kSize; i <= kSize; ++i)
    {
        for (int j=-kSize; j <= kSize; ++j)
        {
            final_colour += kernel[kSize+j]*kernel[kSize+i]*texture2D(texSampler, (texCoord.xy+ blurScale * vec2(float(i),float(j)))).rgb;

        }
    }
        
        
    return final_colour/(Z*Z);
}


#endif


void VS()
{
    mat4 modelMatrix = iModelMatrix;
    vec3 worldPos = GetWorldPos(modelMatrix);
    gl_Position = GetClipPos(worldPos);
    vScreenPos = GetScreenPosPreDiv(gl_Position);
    vTexCoord = GetQuadTexCoord(gl_Position);
    vScreenPosFull = GetScreenPos(gl_Position).xy;
}

void PS()
{
    // vec4 color = texture2D(sDiffMap, vScreenPos);

    // float depth = DecodeDepth(texture2D(sDepthBuffer, vScreenPos).rgb);
    // float scale = 0.001;
    // depth *= scale;
    // float blurScale = 0.001;
    // float targetDepth = 1.0;
    // float blurRadius = clamp(abs(depth - targetDepth), 0.0, 1.0);

    // vec3 blurColor = OnePassBlur(sDiffMap, vTexCoord, blurScale * blurRadius);


    // gl_FragColor = vec4(blurColor, 1.0);

    float aspectratio = 800.0/800.0;
    vec2 aspectcorrect = vec2(1.0,aspectratio);
    aspectcorrect *= 0.1;
    
    float depth = 100.0 * DecodeDepth(texture2D(sDepthBuffer, vTexCoord).rgb);
    float factor = ( depth - cFocus );     
    vec2 dofblur = vec2 (clamp( factor * cBias, -cBlurClamp, cBlurClamp ));
    vec4 col = vec4(0.0);
    
    col += texture2D(sDiffMap, vScreenPos);
    col += texture2D(sDiffMap, vScreenPos + (vec2( 0.0,0.4 )*aspectcorrect) * dofblur);
    col += texture2D(sDiffMap, vScreenPos + (vec2( 0.15,0.37 )*aspectcorrect) * dofblur);
    col += texture2D(sDiffMap, vScreenPos + (vec2( 0.29,0.29 )*aspectcorrect) * dofblur);
    col += texture2D(sDiffMap, vScreenPos + (vec2( -0.37,0.15 )*aspectcorrect) * dofblur);    
    col += texture2D(sDiffMap, vScreenPos + (vec2( 0.4,0.0 )*aspectcorrect) * dofblur);   
    col += texture2D(sDiffMap, vScreenPos + (vec2( 0.37,-0.15 )*aspectcorrect) * dofblur);    
    col += texture2D(sDiffMap, vScreenPos + (vec2( 0.29,-0.29 )*aspectcorrect) * dofblur);    
    col += texture2D(sDiffMap, vScreenPos + (vec2( -0.15,-0.37 )*aspectcorrect) * dofblur);
    col += texture2D(sDiffMap, vScreenPos + (vec2( 0.0,-0.4 )*aspectcorrect) * dofblur);  
    col += texture2D(sDiffMap, vScreenPos + (vec2( -0.15,0.37 )*aspectcorrect) * dofblur);
    col += texture2D(sDiffMap, vScreenPos + (vec2( -0.29,0.29 )*aspectcorrect) * dofblur);
    col += texture2D(sDiffMap, vScreenPos + (vec2( 0.37,0.15 )*aspectcorrect) * dofblur); 
    col += texture2D(sDiffMap, vScreenPos + (vec2( -0.4,0.0 )*aspectcorrect) * dofblur);  
    col += texture2D(sDiffMap, vScreenPos + (vec2( -0.37,-0.15 )*aspectcorrect) * dofblur);   
    col += texture2D(sDiffMap, vScreenPos + (vec2( -0.29,-0.29 )*aspectcorrect) * dofblur);   
    col += texture2D(sDiffMap, vScreenPos + (vec2( 0.15,-0.37 )*aspectcorrect) * dofblur);
    
    col += texture2D(sDiffMap, vScreenPos + (vec2( 0.15,0.37 )*aspectcorrect) * dofblur*0.9);
    col += texture2D(sDiffMap, vScreenPos + (vec2( -0.37,0.15 )*aspectcorrect) * dofblur*0.9);        
    col += texture2D(sDiffMap, vScreenPos + (vec2( 0.37,-0.15 )*aspectcorrect) * dofblur*0.9);        
    col += texture2D(sDiffMap, vScreenPos + (vec2( -0.15,-0.37 )*aspectcorrect) * dofblur*0.9);
    col += texture2D(sDiffMap, vScreenPos + (vec2( -0.15,0.37 )*aspectcorrect) * dofblur*0.9);
    col += texture2D(sDiffMap, vScreenPos + (vec2( 0.37,0.15 )*aspectcorrect) * dofblur*0.9);     
    col += texture2D(sDiffMap, vScreenPos + (vec2( -0.37,-0.15 )*aspectcorrect) * dofblur*0.9);   
    col += texture2D(sDiffMap, vScreenPos + (vec2( 0.15,-0.37 )*aspectcorrect) * dofblur*0.9);    
    
    col += texture2D(sDiffMap, vScreenPos + (vec2( 0.29,0.29 )*aspectcorrect) * dofblur*0.7);
    col += texture2D(sDiffMap, vScreenPos + (vec2( 0.4,0.0 )*aspectcorrect) * dofblur*0.7);   
    col += texture2D(sDiffMap, vScreenPos + (vec2( 0.29,-0.29 )*aspectcorrect) * dofblur*0.7);    
    col += texture2D(sDiffMap, vScreenPos + (vec2( 0.0,-0.4 )*aspectcorrect) * dofblur*0.7);  
    col += texture2D(sDiffMap, vScreenPos + (vec2( -0.29,0.29 )*aspectcorrect) * dofblur*0.7);
    col += texture2D(sDiffMap, vScreenPos + (vec2( -0.4,0.0 )*aspectcorrect) * dofblur*0.7);  
    col += texture2D(sDiffMap, vScreenPos + (vec2( -0.29,-0.29 )*aspectcorrect) * dofblur*0.7);   
    col += texture2D(sDiffMap, vScreenPos + (vec2( 0.0,0.4 )*aspectcorrect) * dofblur*0.7);
             
    col += texture2D(sDiffMap, vScreenPos + (vec2( 0.29,0.29 )*aspectcorrect) * dofblur*0.4);
    col += texture2D(sDiffMap, vScreenPos + (vec2( 0.4,0.0 )*aspectcorrect) * dofblur*0.4);   
    col += texture2D(sDiffMap, vScreenPos + (vec2( 0.29,-0.29 )*aspectcorrect) * dofblur*0.4);    
    col += texture2D(sDiffMap, vScreenPos + (vec2( 0.0,-0.4 )*aspectcorrect) * dofblur*0.4);  
    col += texture2D(sDiffMap, vScreenPos + (vec2( -0.29,0.29 )*aspectcorrect) * dofblur*0.4);
    col += texture2D(sDiffMap, vScreenPos + (vec2( -0.4,0.0 )*aspectcorrect) * dofblur*0.4);  
    col += texture2D(sDiffMap, vScreenPos + (vec2( -0.29,-0.29 )*aspectcorrect) * dofblur*0.4);   
    col += texture2D(sDiffMap, vScreenPos + (vec2( 0.0,0.4 )*aspectcorrect) * dofblur*0.4);   
            
    gl_FragColor = col/41.0;
    gl_FragColor.a = 1.0;

    //gl_FragColor = vec4(vScreenPosFull.x, vScreenPosFull.y, 1.0, 1.0);
}