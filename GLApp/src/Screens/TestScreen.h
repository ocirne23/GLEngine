#pragma once

#include "Core.h"
#include "Input/Key.h"
#include "entityx/entityx.h"

class GLMesh;
class PerspectiveCamera;

class TestScreen
{
public:
    TestScreen();
    ~TestScreen();

    void render(float deltaSec);

private:

    bool keyDown(Key key);

private:

    entityx::EntityX m_entityx;
    GLMesh* m_building = NULL;
    GLMesh* m_skybox = NULL;
    PerspectiveCamera* m_camera = NULL;
};