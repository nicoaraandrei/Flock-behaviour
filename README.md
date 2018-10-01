# Flock-behaviour

Flock-behaviour is a simulation of a shoal or shoals of fish in an aquarium that display certain steering behaviours such as following a target/leader, escaping/avoiding a predator, aligning with other fishes and avoiding walls.
SDL is used for window creation and input support, GLEW for OpenGL context, GLM for vector and matrix maths and picoPNG to read and use png files.

![](demo.gif)

Prerequisites:
SDL - https://www.libsdl.org/download-2.0.php
GLEW - http://glew.sourceforge.net/
GLM - https://glm.g-truc.net/0.9.9/index.html

Controls:
- Right click to move the blue fish(predator)
- Left click to spawn a new yellow fish
- Middle scroll wheel to zoom in/out
- +/- numpad key to increase/decrease the size of aquarium
