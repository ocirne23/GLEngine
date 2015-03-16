#pragma once

class PerspectiveCamera;

struct CameraComponent
{
    CameraComponent() {}
    CameraComponent(PerspectiveCamera* camera) : camera(camera) {}

    PerspectiveCamera* camera = NULL;
};