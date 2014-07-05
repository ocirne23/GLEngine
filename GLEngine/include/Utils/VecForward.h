#pragma once

namespace glm 
{
	namespace detail 
	{
		template<typename T> struct tvec2;
		template<typename T> struct tvec3;
		template<typename T> struct tvec4;
	};
	typedef float mediump_float_t;
	typedef mediump_float_t mediump_float;
	typedef signed int mediump_int_t;
	typedef mediump_int_t mediump_int;

	typedef detail::tvec2<mediump_float> mediump_vec2;
	typedef mediump_vec2 vec2;
	typedef detail::tvec3<mediump_float> mediump_vec3;
	typedef mediump_vec3 vec3;
	typedef detail::tvec4<mediump_float> mediump_vec4;
	typedef mediump_vec4 vec4;

	typedef detail::tvec2<mediump_int> mediump_ivec2;
	typedef mediump_ivec2 ivec2;
	typedef detail::tvec3<mediump_int> mediump_ivec3;
	typedef mediump_ivec3 ivec3;
	typedef detail::tvec4<mediump_int> mediump_ivec4;
	typedef mediump_ivec4 ivec4;
};