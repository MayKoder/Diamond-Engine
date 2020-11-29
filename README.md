# Diamond-Engine

Diamond Engine is currently under active development as the "Engine Development" subject project for the CITM Bachelor's degree in Video Game Design and Development. Also hoping to be used as the base engine for the "Project 3" subject on the same degree. 

Diamond Engine is a fast C++ 3D game engine, developed with SDL2 and OpenGL. The engine targets optimal memory management, fast starting and loading speeds and simple use. This will provide a fast, simple and powerful tool to prototype good looking games the fastest way possible.

<p align="center">
  <img src="docs/media/S1.png">
</p>

<!--All the documentation and a lot more can be found in the engine web page here:-->

Diamond Engine is currently under indie development by one person, you can check the repository [HERE](https://github.com/MayKoder/Diamond-Engine)

<br></br>
# Creators

This engine was made by Miquel Suau Gonzàlez: [GitHub: @MayKoder](https://github.com/MayKoder)

<br></br>
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

<br></br>
# Features

Engine v0.1
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
 
Engine v0.2
 - Resource manager will make sure that assets are loaded only once in memory, and unloaded when they are not used anymore.
 - Play/Pause/Stop/Step buttons. You can now press PLAY, move, remove or do anything to the scene, and when you stop playing you will recover the original state of the scene. This system is also ready to be used with a deltaTime an a timeScale for future simulations.
 - You can now change a GameObject's parent by draggin it on top of the desired parent on the Hierarchy. Trabsformations will be respected with every parent change.
 - Create empty GameObjects with right-click. If a GameObject is selected, the new empty GO will be added as a child.
 - Meshes can be changed by dragging a different mesh from the asset window to the mesh panel inside the mesh component. 
 - Textures can be changed by dragging any texture from the assets window to the texture display inside the material component. 
 - Models can be loaded as GameObject by dragging them from the assets window to the scene window. 
 - Components and GameObject can be removed and deactivated. 
 - Components can be added from the inspector display with the "Add component" drop-down.
 - Resource manager process can be visualized on the console and the "Loaded resources" tab. 
 - Library folder can be removed and the code will regenerate all the data with the same Id's to keep saved scenes valid. 
 - **Innovation:** You can now delete assets from the asset window and the library and meta files will be deleted as well. 
 - **Innovation:** Multiple scene display, enjoy the views from the game camera and move it around. No debug info will be displayed on the game camera and you can render the editor and the game camera at the same time. 
 
<br></br>
# Short-Term TODO list:

 - Resource manager should update files if the last modified date is changed, but if a file is changed while the engine is closed, this update won't happen until the file is changed again with the editor running. 
 - Add icons to every button possible, images are better than text. 
 - Clear out outdated code comments. 
 - .model file optimizations, saving a lot of useless data. 

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
