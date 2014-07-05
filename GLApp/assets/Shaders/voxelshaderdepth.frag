#version 330 core

layout(std140) uniform VoxelTransform2 {
	glm::mat4 matrix;
	glm::vec3 offset;
	glm::mat4 matrix2;
	glm::vec3 offset2;
} transform;