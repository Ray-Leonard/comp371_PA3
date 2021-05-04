Project Title: COMP371 Assignment2


***********NOTES************
1. This project was completed under MacOS environment using Xcode. 
2. The grid unit in this project is set to be 0.25f unit. Meaning one grid square is 0.25f in length. 
3. Texture of screen with dynamically changing textures approx every 10s


*********How to compile and run**************
0. open up COMP371_assignment2 folder
1. setup an Xcode environment with glew, glfw and glm properly installed and linked.
2. put all .cpp and .h files under source directory
3. put "resources" "assets" folder which contains pictures of textures under textures directory, vertex shader and fragment shader code under source directory.
4. compile and run on main.cpp


*********Function Keys****************

=====Camera movements=======
Right click - move in x direction to pan by moving mouse
Middle click - move in y direction to tilt by moving mouse
Left click - move into / out of scene by moving mouse

====== World Rotation ========
up - counter-clockwise rotation of world around y axis
down - clockwise rotation of world around y axis
right - clockwise world rotation around z axis
left - anti-clockwise world rotation around z axis


======Model selection and Model manipulations=======
1 - select 1st set of model "JI40" to manipulate
2 - select 2nd set of model "LE48" to manipulate
3 - select 3rd set of model "ZN46" to manipulate
4 - select 4th set of model "YN40" to manipulate
5 - select 5th set of model "FG47" to manipulate

0 - reset camera/models/world rotations back to initial position

(once a model is selected, the following manipulation keys will only affect the selected model)

W(shift + w) - move model forward
S(shift + s) - move model backward
A(shift + a) - move model left
D(shift + d) - move model right
U(shift + u) - scale model up
J(shift + j) - scale model down
u - scale model up by scaleDelta each press (defined in program)
j -  scale model down by scaleDelta each press (defined in program)
a - rotate model counter-clockwise around its y axis 
d - rotate model clockwise around its y axis
q - rorate model counter-clockwise by rotateDelta (defined in program) each press 
e -  rotate model clockwise by rotateDelta (defined in program) each press
z - continuous forward movement around vertical shear transformation
Z(shift + z) - continuous forward movement around lateral shear transformation
m - continuous forward movement around vertical shear transformation
M(shift + m) - continuous forward movement around lateral shear transformation


v - small movement(cos(10)) around vertical shear transformation
n - small movement(cos(10) around vertical shear transformation
c - small movement(cos(10) around lateral shear transformation
b - small movement(cos(10) around lateral shear transformation



====== Other Switches ======
x - toggling texture on/off
o - toggling shadow on/off
t - render triangles mode
l - render lines mode
p - render points mode
Space - place all models randomly on the grid 



********* References ***********




