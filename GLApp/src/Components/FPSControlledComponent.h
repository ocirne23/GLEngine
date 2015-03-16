#pragma once

struct FPSControlledComponent
{
	FPSControlledComponent() {}
	FPSControlledComponent(float moveSpeed, float lookSensitivity) : moveSpeed(moveSpeed), lookSensitivity(lookSensitivity) {}
	float moveSpeed = 10.0f;
	float lookSensitivity = 0.7f;
};