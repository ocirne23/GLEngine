#pragma once

struct IVec4
{
	IVec4() {}
	IVec4(int x, int y, int z, int w) : x(x), y(y), z(z), w(w) {}
	int x = 0;
	int y = 0;
	int z = 0;
	int w = 0;
};

struct Vec4
{
	Vec4() {}
	Vec4(float x, float y, float z, float w) : x(x), y(y), z(z), w(w) {}
	float x = 0.0f;
	float y = 0.0f;
	float z = 0.0f;
	float w = 0.0f;
};

struct Vec3
{
	Vec3() {}
	Vec3(float x, float y, float z) : x(x), y(y), z(z) {}
	float x = 0.0f;
	float y = 0.0f;
	float z = 0.0f;
};

struct Vec2
{
	Vec2() {}
	Vec2(float x, float y) : x(x), y(y) {}
	float x = 0.0f;
	float y = 0.0f;
};