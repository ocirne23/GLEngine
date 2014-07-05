#version 330 core

layout(location = 0) in vec4 in_position;
layout(location = 1) in vec2 in_texcoord;
layout(location = 2) in vec3 in_normal;
layout(location = 3) in vec3 in_tangent;
layout(location = 4) in vec3 in_bitangent;
layout(location = 5) in uint in_materialID;

uniform mat4 u_mvp;
uniform mat4 u_mv;
uniform mat3 u_normalMat;
uniform mat4 u_transform;

out vec3 v_position;
out vec2 v_texcoord;
out vec3 v_normal;
out vec3 v_tangent;
out vec3 v_bitangent;
flat out uint v_materialID;

#ifdef SSAO_ENABLED
out vec3 v_screenSpaceNormal;
#endif

void main()
{
	vec4 pos = u_transform * in_position;
	gl_Position = u_mvp * pos;

	v_position	= (u_mv * pos).xyz;
	v_texcoord	= in_texcoord;
	v_normal	= normalize(u_normalMat * in_normal);
	v_tangent	= normalize(u_normalMat * in_tangent);
	v_bitangent = normalize(u_normalMat * in_bitangent);
	v_materialID = in_materialID;
#ifdef SSAO_ENABLED
	v_screenSpaceNormal = mat3(u_mv) * in_normal;
#endif
}