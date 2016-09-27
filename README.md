# GLEngine

A minimalistic C++11/14 framework to easily and efficiently render various 3D graphics.
* Requires Visual Studio 2015
* Before compiling extract GLEngine/libs.rar (github 50MB file size limit)
* Before running GLApp, run GLResourceBuilder

This project is split into 3 parts:
* GLApp: An application utilizing the GLEngine library to showcase and test functionality.
* GLEngine: Provides generic functionality like rendering, input and file IO.
* GLResourceBuilder: Preprocesses various assets for efficient runtime usage.

This project is mainly intended as a portfolio piece.

**Functionality:**
* A physically based renderer/pipeline
* Clustered Forward Shading for many pointlights.
* HBAO rendered at half resolution, bilateral blur at full resolution.
* Bloom
* HDR / Tonemapping
* FXAA / MSAA
* Basic PCF shadows
* Asset preprocessor using Assimp & custom file format for fast startup times
* CEGUI library integration (UI)
* Efficient texture packing making it possible to render entire scenes with different materials in one draw call

The primairy goal of the code style is to reduce and make spotting errors easier. For example, rows of repetitive/similar assignments will be alligned to spot missing/duplicate/wrong lines.

Some interesting files to look at: 
* [main.cpp](GLApp/src/main.cpp)
* [GLRenderer.cpp](GLEngine/src/Graphics/GL/Scene/GLRenderer.cpp)
* [TestScreen.cpp](GLApp/src/TestScreen.cpp)
* [DBScene.cpp](GLEngine/src/Database/Assets/DBScene.cpp)
* [DBMesh.cpp](GLEngine/src/Database/Assets/DBMesh.cpp)
* [GLScene.cpp](GLEngine/src/Graphics/GL/Scene/GLScene.cpp)
* [GLMesh.cpp](Graphics/GL/Scene/GLMesh.cpp)
* [modelshader.vert](GLApp/assets/Shaders/modelshader.vert)
* [modelshader.frag](GLApp/assets/Shaders/modelshader.frag)
* [ClusteredShading.cpp](GLEngine/src/Graphics/GL/Tech/ClusteredShading.cpp)

![da](https://dl.dropboxusercontent.com/u/18555381/Permanent/glengine280916.png)
