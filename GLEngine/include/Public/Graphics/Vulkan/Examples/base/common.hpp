#pragma once

#include <assert.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <time.h>

#include <algorithm>
#include <array>
#include <chrono>
#include <fstream>
#include <functional>
#include <initializer_list>
#include <iostream>
#include <iomanip>
#include <list>
#include <mutex>
#include <random>
#include <set>
#include <string>
#include <fstream>
#include <sstream>
#include <stdexcept>
#include <streambuf>
#include <thread>
#include <vector>

#include <glm/glm.hpp>

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/matrix_inverse.hpp>
#include <glm/gtc/noise.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <glm/gtx/rotate_vector.hpp>
#include <glm/gtx/transform.hpp>
#include <glm/gtx/quaternion.hpp>

using glm::ivec2;
using glm::uvec2;
using glm::vec2;
using glm::vec3;
using glm::vec4;
using glm::mat3;
using glm::mat4;
using glm::quat;

class Rotations {
public:
	static const quat IDENTITY;
	static const quat Y_180;

	//  Helper function returns the positive angle (in radians) between two 3D vectors
	static inline float angleBetween(const glm::vec3& v1, const glm::vec3& v2) {
		return acosf((glm::dot(v1, v2)) / (glm::length(v1) * glm::length(v2)));
	}
};

class Vectors {
public:
	static const vec3 UNIT_X;
	static const vec3 UNIT_Y;
	static const vec3 UNIT_Z;
	static const vec3 UNIT_NEG_X;
	static const vec3 UNIT_NEG_Y;
	static const vec3 UNIT_NEG_Z;
	static const vec3 UNIT_XY;
	static const vec3 UNIT_XZ;
	static const vec3 UNIT_YZ;
	static const vec3 UNIT_XYZ;
	static const vec3 MAX;
	static const vec3 MIN;
	static const vec3 ZERO;
	static const vec3 ONE;
	static const vec3 TWO;
	static const vec3 HALF;
	static const vec3& RIGHT;
	static const vec3& UP;
	static const vec3& FRONT;
	static const vec3 ZERO4;
};


// Image loading 
#include <gli/gli.hpp>

// Vulkan!
#include <vulkan/vulkan.hpp>

