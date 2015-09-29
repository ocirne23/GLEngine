#ifndef CAMERA_VARS
#define CAMERA_VARS

layout (std140) uniform CameraVars
{
	uniform mat4 u_vpMatrix;
	uniform mat4 u_viewMatrix;
	uniform mat3 u_normalMatrix; // Should be uploaded as a mat4 because std140 allignment
};

#endif // CAMERA_VARS