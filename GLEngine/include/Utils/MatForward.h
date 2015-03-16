#pragma once

namespace glm
{
	namespace detail
	{
		template <typename T> struct tmat3x3;
		template <typename T> struct tmat4x4;
	};
	typedef float mediump_float_t;
	typedef mediump_float_t mediump_float;

	typedef detail::tmat3x3<mediump_float> mediump_mat3x3;
	typedef mediump_mat3x3 mat3x3;
	typedef mat3x3 mat3;
	typedef detail::tmat4x4<mediump_float> mediump_mat4x4;
	typedef mediump_mat4x4 mat4x4;
	typedef mat4x4 mat4;
};