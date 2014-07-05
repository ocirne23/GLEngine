#version 330 core
#extension GL_ARB_gpu_shader5 : enable

layout(std140) uniform LightData {
	vec4 lightPositions[32];
} lightData;

layout(std140) uniform LightTransform {
	mat4 VPMatrix[32];
} lightTransforms;

layout(triangles, invocations = 32) in;
layout(triangle_strip, max_vertices = 3) out;

in vec3 position[];

/** Used to generate depth buffer for up to 32 lights at a time with a single draw call. */
void main() 
{
	//calculate triangle normal
	vec3 normal = cross(position[2]-position[0], position[0]-position[1]);
	//calculate light angle
	vec3 light = vec3(lightData.lightPositions[gl_InvocationID]) - position[0];

	// backface culling
	if (dot(normal, light) > 0.f) 
	{
		//output triangle based on the lights mvp matrix
		for (int i = 0; i < 3; ++i) 
		{
			gl_Position = lightTransforms.VPMatrix[gl_InvocationID] * vec4(position[i], 1.f);
			gl_Layer = gl_InvocationID;
			EmitVertex();
		}
		EndPrimitive();
	}
}