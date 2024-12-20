# About
Project 2 - Ray Tracer

Create your own Ray Tracer (renderer) that can render simple primitive shapes and meshes using ambient and point lighting.

**Part 1 - Setting up the Camera/View Environment**: Setup the basic camera, view plane and scene using the example classes that were provided in class and perform a one pass ray trace to generate an image of a simple scene with several spheres in ambient light to make sure it is working.<br>
**Part 2 - Shading and Shadows**: Implement Lambert and Blinn-Phong Shading with Multiple Lights. Modify your renderer to support Meshes from Lab/Project 1.<br>
**Part 3 - Planar Textures**: Add simple planar texture mapping.

**Code Editor**: Visual Studio<br>
**Library**: OpenGL (openFrameworks)

# Examples
* SA Game Models: https://sa-game-remake.tumblr.com/post/697635182338048000/
  * Assets/Mesh: https://drive.proton.me/urls/VFG903EZWG#eXmnNUPduNoS

# Notes<br>
random notes taken during class

\<image\> API
```
// declared image
ofImage image;

image.allocate(); allocate space for new images, do black and white height x with x 1 bit; color need more parameters
image.setColor(color,x,y)
image.save("<pathname>");  // "images/random.png"
image.load("");            // assumes image in default PROJECT/bin/data if nothing before images
image.getWidth()
image.getHeight()
image.draw(x,y)            // draw after cam.end();
```

Basic 3 functions to have: drawGrid, rayTrace, drawAxis<br>
**drawGrid**: draw pixelated graph on world

Bring program up interactively then ray trace, do not ray trace on the start.

Cam: gives vector and tells camera to look at from this vector

Part 1: Top part of spheres grainy because resolution is low. Increase resolution more than 1200x800 to decrease grainyness.

Part 3: Print few lines of u, v. Convert to i, j. Try a single tile map first.


### TO-DO:
* use Lab 2, drag sphere instead of manually moving coordinates
* sliders for diffuse, ambient, etc. many variables
* project image on to view plane
* make a rayTrace
