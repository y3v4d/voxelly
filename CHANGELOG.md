# Version 0.0.2 - 04/12/2025

Added Emscripten and ES 3.2 compilation support, new tools, better controls.

- Added Emscripten and ES 3.2 compilation support
- Replaced GLFW with SDL3 for better compatibility with Emscripten
- Added more proper tool system with Place, Erase, Brush, Line, Move tools
- Added tool shape support with Sphere and Cube shape
- Added axis lock support
- Integrated SDL_main callback approach
- Added executable path and data path getters to FileSystem
- Fixed invalid data size for UIRenderer shared VAO
- Added WorldMesh class to create and manage World chunk meshes
- Added Ray class with utility functions
- Added getRayFromScreenCoords helper Camera function
- Converted Shader in assets to Asset child
- Converted Image in assets to Asset child
- Added negative number guards in all World functions
- Renamed all uniforms to u_<Name> format
- Moved entire game initialization and logic to Game class
- Added block counter to Chunk
- Added format to internal format conversion system to support more precise formats required for OpenGL ES textures
- Replaced separate .vert, .frag shader files with unified custom .glsl format to make cross-compiling OpenGL and OpenGL ES easier.
- Added draw call counter in RendererGL
- WebGL RendererGL::setWireframe has no implementation
- Moved glad initialization from RendererGL to Window
- Added native and web CMakePresets
- Added unused ui.graph and basic-sprite.mat assets

# Version 0.0.1 - 19/11/2025

Initial semi-working version of Voxelly with the following changes:

- Added new better looking font
- Added World serialization, saving and loading
- Added Transform2D, used for elements meant for 2d
- Refactored Sprite to store a handle to the gpu texture and desired uvs
- Added UIRenderer system to easily render quads and sprite
- Added support for polygon offset fill option in Renderer
- Added more utility functions to Image
- Added MeshBuilder createPlane utility
- Added some of the mouse and keyboard Window event support
- Added zoom support to camera
- Added AssetManager for more standarised asset loading
- Added dynamic palette generation instead of static image
- Improved grid fragment shader
- Added basic-2d shader for sprites
- Improved controls

# Prototype - 05/11/2025

First prototype release of Voxelly, a voxel-based game engine. This version includes basic block manipulation. Initial support for textures and lighting is also included.
