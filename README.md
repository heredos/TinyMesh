# Modgeom - Tinymesh
## Introduction
### context
This is a fork of [tinymesh](https://github.com/eric-galin/TinyMesh) by eric galin.  
It was made as a class project for the ID3D M2 ModGeom class of year 2026-2027.  
This README serves as the handout, and mostly contains information relevant to this class.  
### AI disclosure
In the parts that were modified from the original TinyMesh, some of it was by me, some of it was done using free AI tools (Mimo2.6 on openCode, to be precise, i don't have the money for more). This is neither an endorsement nor a publicity for these tools, and would've rather done everything myself if i had the time to.  

As for the places AI was used, I must state that AI was used ONLY to create, update and fix parts related to QT and the graph editor specifically, and I did my best to keep its code relatively clean, undestandable, reusable and extensible by using existing libraries (in this case [QtNodes](https://github.com/paceholder/nodeeditor)). I explicitely prevented it from modifying any of the files in `Include/implicits` and from reading and writing to `Source/implicits`, or else it wouldn't make any sense to do this work from a learning standpoint.

### Graph interface
![a showcase of the graph editor](Illustrations/rings.png)  

The Interface has been rewworked to allow me to iterate more quickly without having to modify the main function and recompile when i wanted to try a new shape. This way, any existing code can be turned into a node, which can then be used alongside other nodes to form shapes, and check quickly that it works in different scenarios.  
The mesh output node is the final node of a graph, it takes a signed field as input, and shows a render of the 3D poligonization of that field at O. Each mesh output node can be computed independently by clicking on its recompute button. the camera can also be rotated by using ctrl+left click. to unlock more movement options, one can double-click on the preview of that node to open a bigger rendered window of the shape, with the same controls as TinyMesh.
All the other nodes wil be detailled in their own section later in this document.  
The graph also gives an estimation of the computation time by sampling 4096 pseudorandom points using a fixed seed. This gives an estimation of the cost of each node, which is detailed as follows:
 - self: the time that this node took to compute (the total time - children time)
 - incl: the inclusive time, how much time this, and all the previous nodes took to compute
 - share: the share of the computation that this node holds among all the nodes in the graph
 - build: how long it took to turn this graph node into an actual object (mostly interesting for the blobField node)

 and then, there are also some times that are specific to the mesh output node:
 - mesh: the time it took to poligonize the mesh
 - graph: the time it took to benchmark the entire node graph

 note the the graph time and the bencmarking only occur when clicking the top-left `[Recompute]` button, the benchmarking does not occur otherwise.

### Code 

## Windows
- First, you must install Qt6 (https://www.qt.io/download, use the open source version). Install the msvc2019_64 compiler only (*Note: other compilers can work too*).
- Set the "QTDIR64" environment variable to your Qt installation folder (example: "E:\Code\Qt\6.3.0\msvc2019_64")
- Set the "GLEW_DIR" environment variable to the path "XXX/TinyMesh/Libs/", with XXX the path to your repository.
- Run the script TinyMesh/Script/AppTinyMeshDeploy.bat

Finally, open the Visual Studio solution files with Visual Studio 2022 Community, or open the QtCreatorProject.pro with QtCreator. Compile/Build and execute, everything should work.
*Note: For other IDE, you will have to use the provided CMakeLists.txt to generate the solution files yourself.*

## Linux
- First, you must install Qt6 (https://www.qt.io/download, use the open source version). Install the gcc compiler only (*Note: other compilers can work too*).
- Install the following packages
```
sudo apt-get install g++ libglew-dev freeglut3-dev
```
Finally, open the QtCreatorProject.pro with QtCreator. Click on configure, then build and execute, everything should work.
*Note: For other IDE, you will have to use the provided CMakeLists.txt to generate the solution files yourself.*

## Mac OS/X
- First, you must install Qt6 (https://www.qt.io/download, use the open source version). Install the gcc compiler only (*Note: other compilers can work too*).
-  Install the following packages (*Note: Depending on your Mac OS/X version, other packages might be needed. Shoot me an email if anything is missing.*
```
brew install glew
```
Finally, open the QtCreatorProject.pro with QtCreator. Click on configure, then build and execute, everything should work.
*Note: For other IDE, you will have to use the provided CMakeLists.txt to generate the solution files yourself.*

## Additional notes
Optionally, you can use your own code (without Qt) to do the windowing and rendering part. In this case, you can extract the following files, which don't have any dependencies apart from the C++ standard library:
 - box.h/.cpp
 - camera.h/.cpp
 - color.h
 - implicits.h/.cpp
 - mathematics.h
 - mesh.h/.cpp (*You must remove the Mesh::LoadObj and Mesh::SaveObj functions, which depends on Qt*)
 - meshcolor.h/.cpp
 - ray.h/.cpp
