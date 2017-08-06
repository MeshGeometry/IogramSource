#include "Uniforms.glsl"
#include "Samplers.glsl"
#include "Transform.glsl"
#include "ScreenPos.glsl"

varying vec2 vTexCoord;
varying vec2 vScreenPos;

#ifdef COMPILEPS

uniform vec4 cGradColorA;
uniform vec4 cGradColorB;
uniform float cBlurRadius;
uniform float cBlend;


float normpdf(in float x, in float sigma)
{
	return 0.39894*exp(-0.5*x*x/(sigma*sigma))/sigma;
}

vec3 OnePassBlur(sampler2D texSampler, vec2 texCoord, float blurScale)
{
	//declare stuff
	const int mSize = 7;
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
    gl_Position.z = gl_Position.w;
    vTexCoord = iTexCoord.xy;
    vScreenPos = GetQuadTexCoord(gl_Position);
}

void PS()
{   
	vec4 texCol = texture2D(sDiffMap, vTexCoord);
	texCol = pow(texCol + clamp((cAmbientColor.a - 1.0) * 0.1, 0.0, 0.25), max(vec4(cAmbientColor.a), 1.0)) * clamp(cAmbientColor.a, 0.0, 1.0);
	if(cBlurRadius > 0.0){
	    texCol = vec4(OnePassBlur(sDiffMap, vTexCoord, cBlurRadius), 1.0);
	}
	vec4 mixCol = mix(cGradColorA, cGradColorB, smoothstep(0.4, 0.6, vTexCoord.y));
	vec4 col = mix(mixCol, texCol, cBlend);

	//VIGNETTE
	// vec2 q = vScreenPos;
    //col = pow( col*2., vec4(vec3(1.75), 1.0) );
	// col *= sqrt( 32.0*q.x*q.y*(1.0-q.x)*(1.0-q.y) ); //from iq

    gl_FragColor = col;    
}
