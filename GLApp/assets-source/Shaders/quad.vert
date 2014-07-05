#version 330

const vec4 vertices[] = vec4[]( 
	vec4(-1.0, -1.0, 0.5, 1.0),
	vec4( 1.0, -1.0, 0.5, 1.0),
	vec4(-1.0,  1.0, 0.5, 1.0),
	vec4( 1.0,  1.0, 0.5, 1.0) );

const vec2 texcoords[] = vec2[]( 
	vec2(0.0, 0.0),
    vec2(1.0, 0.0),
    vec2(0.0, 1.0),
    vec2(1.0, 1.0) );

out vec2 v_texcoord;

void main()
{
	gl_Position = vertices[gl_VertexID];
	v_texcoord = texcoords[gl_VertexID];
}
