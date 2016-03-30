#pragma once

#include <assert.h>

#include "Graphics/Vulkan/vk_cpp.h"

struct VKVerifier
{
	VKVerifier() {}
	VKVerifier(const vk::Result& a_result)
	{
		operator=(a_result);
	}
	VKVerifier operator=(const vk::Result& a_result)
	{
		assert(a_result == vk::Result::eSuccess);
		m_result = a_result;
		return VKVerifier();
	}

	vk::Result m_result;
};