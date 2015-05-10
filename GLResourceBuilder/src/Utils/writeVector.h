#pragma once

template <typename T>
void writeVector(std::ostream& a_file, const std::vector<T>& a_vector)
{
	const int size = (int) a_vector.size();
	a_file.write(reinterpret_cast<const char*>(&size), sizeof(int));
	a_file.write(reinterpret_cast<const char*>(&a_vector[0]), sizeof(a_vector[0]) * size);
}
