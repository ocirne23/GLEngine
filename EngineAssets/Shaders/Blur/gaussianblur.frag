#include "../globals.glsl"

/* REQUIRED DEFINES /* !NOT IN GLOBAL DEFINES!
VALTYPE      (e.g: vec2)
VALACCESSOR  (e.g: rg)
*/

in vec2 v_texcoord;

layout(binding = BLUR_TEXTURE_BINDING_POINT) uniform FBOSampler u_blurTex;

uniform vec2 u_pixelOffset;

layout(location = 0) out vec3 out_val;   

void main()
{
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // Kernel width 35 x 35
    //
    const int stepCount = 9;
    //
    const float weights[stepCount] ={
       0.10855,
       0.13135,
       0.10406,
       0.07216,
       0.04380,
       0.02328,
       0.01083,
       0.00441,
       0.00157
    };
    const float offsets[stepCount] ={
       0.66293,
       2.47904,
       4.46232,
       6.44568,
       8.42917,
       10.41281,
       12.39664,
       14.38070,
       16.36501
    };
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    
	VALTYPE blurredVal = VALTYPE(0.0);
	
    for( int i = 0; i < stepCount; i++ )                                                                            
    {                                                                                                               
        vec2 texCoordOffset = offsets[i] * u_pixelOffset;                                                            
        VALTYPE val = singleSampleFBO(u_blurTex, v_texcoord + texCoordOffset).VALACCESSOR + singleSampleFBO(u_blurTex, v_texcoord - texCoordOffset).VALACCESSOR; 
        blurredVal += weights[i] * val;                                                                                
    }
    out_val = blurredVal;
}