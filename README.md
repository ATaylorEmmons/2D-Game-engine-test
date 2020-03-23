# OpenGL test
A test to try out OpenGl for a game.

### Required Libraries:
* GLAD - Function loading library for OpenGL
* GLFW3 - The best windowing library I've found

>Results: I have found that the OpenGl's state machine is rather difficult to maintain. There was a bug that took me 
quite a while to track down due to the nature of how OpenGl requires memory to be uploaded to the GPU. I have moved
to the Vulkan API because the lower level API appeals to my interests and the memory operations are explicit and has
better debugging infastructure so I will have a better idea of when there are bugs.

### Note
* This demo uses instancing rendering to draw many textured squares. It has come to my attention that instance rendering is most useful
for more complex geometry. So, in the end, instance rendering only introduced code complexity with no optimization.
