#version 450

#extension GL_ARB_separate_shader_objects : enable
#extension GL_ARB_shading_language_420pack : enable

// Vertex attributes
layout (location = 0) in vec4 inPos;
layout (location = 1) in vec3 inColor;
layout (location = 2) in vec3 inNormal;

// Instanced attributes
layout (location = 4) in vec3 instancePos;
layout (location = 5) in vec3 instanceRot;
layout (location = 6) in float instanceScale;

layout (binding = 0) uniform UBO 
{
	mat4 projection;
	mat4 view;
	float time;
} ubo;

layout (location = 0) out vec3 outNormal;
layout (location = 1) out vec3 outColor;
layout (location = 2) out vec3 outEyePos;
layout (location = 3) out vec3 outLightVec;


vec4 quat_from_axis_angle(vec3 axis, float angle)
{ 
  vec4 qr;
  float half_angle = (angle * 0.5) * 3.14159 / 180.0;
  qr.x = axis.x * sin(half_angle);
  qr.y = axis.y * sin(half_angle);
  qr.z = axis.z * sin(half_angle);
  qr.w = cos(half_angle);
  return qr;
}

vec3 rotate_vertex_position(vec3 v, vec4 q)
{ 
  return v + 2.0 * cross(q.xyz, cross(q.xyz, v) + q.w * v);
}

void main() 
{
	outColor = inColor;
	vec4 q = normalize(quat_from_axis_angle(instanceRot, ubo.time * 100.0 / instanceScale));
	outNormal = rotate_vertex_position(inNormal, q);
	
	vec3 v = rotate_vertex_position(inPos.xyz, q);
	v *= instanceScale;
	vec3 p = rotate_vertex_position(instancePos, q);

	vec4 pos = vec4(v + p, 1.0);
	outEyePos = vec3(ubo.view * pos);
	
	gl_Position = ubo.projection * ubo.view * pos;
	
	vec4 lightPos = vec4(0.0, 0.0, 0.0, 1.0) * ubo.view;
	outLightVec = normalize(lightPos.xyz - outEyePos);
}
