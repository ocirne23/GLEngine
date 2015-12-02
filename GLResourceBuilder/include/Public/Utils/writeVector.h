#pragma once

#include "BuilderCore.h"

template <typename T>
uint64 writeVector(std::ostream& a_file, const std::vector<T>& a_vector)
{
	const int size = (int) a_vector.size();
	a_file.write(reinterpret_cast<const char*>(&size), sizeof(int));
	const uint64 vectorSizeBytes = sizeof(a_vector[0]) * size;
	a_file.write(reinterpret_cast<const char*>(&a_vector[0]), vectorSizeBytes);
	return vectorSizeBytes + sizeof(int);
}
