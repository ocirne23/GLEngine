# GLEngine

A minimalistic C++11 framework to efficiently and easily render various graphics.

This project is split into 3 parts:
* GLApp: An application utilizing the GLEngine library to showcase and test functionality.
* GLEngine: Provides generic functionality like rendering, input and file IO.
* GLResourceBuilder: Preprocesses various assets for efficient runtime usage.

This project is mainly intended as a portfolio piece.

## Code Architecture:
###GLApp:
Is in charge of the main function, gameloop and all threads that are created so the user is always in complete control of the flow of their application.

Manages all the different components and systems to make use of a modified version of the entityx Entity Component System (https://github.com/alecthomas/entityx)

###GLEngine:
The GLEngine class provides static access to lifecycle functions to create the window and render contexts. Various functions can be accessed through the graphics and input public static members.

####Graphics:

The Graphics folder contains basic objects such as PerspectiveCamera, Pixmap,

#####Wrappers:
Various OpenGL object wrappers are available in the Graphics/GL/Wrappers folder, these objects allow the user to easily setup their own rendering pipeline with minimal code.

All the wrapper objects are to be initialized before use with .initialize before use to keep the GL state intact during allocation and construction times to a minimum.

#####Tech:
Various graphics algorithms are available for use in the Graphics/GL/Tech folder, these objects allow for easy implementation of complex graphics techniques such as HBAO and Clustered Shading.
