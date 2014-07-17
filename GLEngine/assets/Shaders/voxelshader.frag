#version 330 core

uniform sampler2DArray u_texArr;

layout (std140) uniform PerFrameData
{
	mat4 u_mvp;
	vec4 u_camPos;		//xyz used
	vec4 u_fogColor;	//rgb used
	float u_fogStart;
	float u_fogEnd;
	float padding;
	float padding2;
};

in VS_OUT
{
	vec3 texCoord;
	float vertexAO;
	float eyeDist;
} in_fs;

out vec4 out_color;

float computeLinearFogFactor()
{
   float factor = (in_fs.eyeDist - u_fogStart) / (u_fogEnd - u_fogStart);
   factor = clamp(factor, 0.0, 1.0);
   return factor;            
}

void main()
{
    float fogFactor = computeLinearFogFactor();

	vec4 texCol = texture(u_texArr, in_fs.texCoord);
	vec4 blockCol = vec4((texCol.rgb) * in_fs.vertexAO, texCol.a);
	vec4 combinedCol = mix(blockCol, u_fogColor, fogFactor);
	out_color = combinedCol;
}