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
