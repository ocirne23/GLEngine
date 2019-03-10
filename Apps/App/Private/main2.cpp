const float quad[] =
{// Position				Texcoords
	-1.0f, -1.0f, 0.0f,		0.0f, 0.0f,
	1.0f, -1.0f, 0.0f,		1.0f, 0.0f,
	1.0f, 1.0f, 0.0f,		1.0f, 1.0f,
	-1.0f, 1.0f, 0.0f,		0.0f, 1.0f
};

const unsigned int indices[] =
{
	0, 1, 2,
	0, 2, 3
};

int main2()
{

	return 0;
}

// EASTL new defines
#if defined(LIB_BUILD)
void* operator new[](size_t size, const char* pName, int flags, unsigned debugFlags, const char* file, int line)
{
	return new char[size];
}

void* operator new[](size_t size, size_t alignment, size_t alignmentOffset, const char* pName, int flags, unsigned debugFlags, const char* file, int line)
{
	return operator new[](size, pName, flags, debugFlags, file, line);
}
#endif