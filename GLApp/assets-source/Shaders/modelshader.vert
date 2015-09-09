#include "Shaders/cameravars.glsl"

layout(location = 0) in vec3 in_position;
layout(location = 1) in vec2 in_texcoord;
layout(location = 2) in vec3 in_normal;
layout(location = 3) in vec3 in_tangent;
layout(location = 4) in vec3 in_bitangent;
layout(location = 5) in uint in_materialID;

uniform mat4 u_modelMatrix;

out vec3 v_position;
out vec2 v_texcoord;
out vec3 v_normal;
out vec3 v_tangent;
out vec3 v_bitangent;
flat out uint v_materialID;

void main()
{
	vec4 pos = u_modelMatrix * vec4(in_position, 1.0);
	gl_Position = u_vpMatrix * pos;
	
	v_position	 = (u_viewMatrix * pos).xyz;
	v_texcoord	 = in_texcoord;
	v_normal	 = normalize(u_normalMatrix * in_normal);
	v_tangent	 = normalize(u_normalMatrix * in_tangent);
	v_bitangent  = normalize(u_normalMatrix * in_bitangent);
	v_materialID = in_materialID;
}