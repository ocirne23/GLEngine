#pragma once

#include "BuilderCore.h"

#include <vector>

class Mesh
{
public:

	struct Vertex
	{
		struct vec3 { float x, y, z; };
		struct vec2 { float x, y; };

		Vertex() {};
		Vertex(aiVector3D& a_position, aiVector3D& a_texcoords, aiVector3D& a_normal, aiVector3D& a_tangents, aiVector3D& a_bitangents, uint a_materialID)
			: position((vec3&) a_position), texcoords((vec2&) a_texcoords), normal((vec3&) a_normal)
			, tangents((vec3&) a_tangents), bitangents((vec3&) a_bitangents), materialID(a_materialID)
		{}
		vec3 position;
		vec2 texcoords;
		vec3 normal;
		vec3 tangents;
		vec3 bitangents;
		uint materialID;
	};

	std::vector<Vertex> m_vertices;
	std::vector<uint> m_indices;
};