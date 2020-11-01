# Diamond-Engine

Diamond Engine is currently under active development as the "Engine Development" subject project for the CITM Bachelor's degree in Video Game Design and Development. Also hoping to be used as the base engine for the "Project 3" subject on the same degree. 

Diamond Engine is a fast C++ 3D game engine, developed with SDL2 and OpenGL. The engine targets optimal memory management, fast starting and loading speeds and simple use. This will provide a fast, simple and powerful tool to prototype good looking games the fastest way possible.

<p align="center">
  <img src="docs/media/S1.png">
</p>

<!--All the documentation and a lot more can be found in the engine web page here:-->

Diamond Engine is currently under indie development by one person, you can check the repository [HERE](https://github.com/MayKoder/Diamond-Engine)

# Creators

This engine was made by Miquel Suau Gonzàlez: [GitHub: @MayKoder](https://github.com/MayKoder)

# Execution and controls

Simple guide to execute and use the engine:

### Use
- Download the latest release
- Extract the .zip file anywhere
- Open the extracted folder and go to "Diamond-Engine" folder
- Double-click the "Diamond Engine.exe" file

### Controls
- Right click the scene window to rotate the camera like a FPS game.
- While Right clicking on the scene window, use “WASD” to move like a FPS game.
- Use the mouse wheel to zoom in and out.
- Use Alt+LeftClick to orbit around the selected GameObject.
- Press “F” to focus the camera around the selected GameObject.
- Hold SHIFT to duplicate the movement speed.
- Use the mouse wheel click to PAN the camera (Horizontal and Vertical movement)

# Features
- Additional comments for the teacher to understand some part of the engine

 - Take cool screenshots with the "Take Screenshot" button located on the top-middle bar. (Screenshots will ignore editor layer).
 - Optimized scene window rendering, displaying only the needed section of the frame buffer, reducing wasted pixels and improving performance.
 - Style customization, save and load custom styles created with the "Style Settings" tab inside the "Configuration Window" window.
 - Move, rotate and scale models with correct parent-to-child transformations.
 - Default Skybox, for the ones tired of looking at empty black backgrounds. Had to reduce the texture size and will look low-res until shaders are added.
 - Camera pan movement with the mouse wheel click.
 - Use the "Supr" key to delete the selected GameObject and all the children inside it.
 - Enable / Disable components.
 - Focus on memory management, no memory leaks.
 - Use of hyperlinks with ImGui.
 - Drag&Drop from any supported file format from anywhere on the disc, file will be duplicated in the "Assets" folder.
 - Console log have types (Normal, Warning and Error) indicated by colors. 
 - Console logs can stack up if the value is the same and are equal as the last log.
 - Console logs and console can also be cleaned.
 
# Short term TODO list

 - Reduce the use of pointers in small size objects, like Texture class and others.
 - Add shader pipeline.

<br></br>
## MIT License

Copyright (c) 2020 MayKoder

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
