#pragma once

template <typename T>
uint readVector(FileHandle& a_handle, rde::vector<T>& a_vector, uint a_offset)
{
	int size;
	a_handle.readBytes(reinterpret_cast<char*>(&size), sizeof(int), a_offset);
	a_vector.resize(size);
	a_handle.readBytes(reinterpret_cast<char*>(&a_vector[0]), size * sizeof(a_vector[0]), a_offset + sizeof(int));
	return a_offset + sizeof(int) + size * sizeof(a_vector[0]);
}