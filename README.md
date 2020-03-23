# OpenGL test
A test to try out OpenGl for a game.

### Required Libraries:
* GLAD - Function loading library for OpenGL
* GLFW3 - The best windowing library I've found

>Results: I have found that the OpenGl's state machine is rather difficult to maintain. There was a bug that took me 
quite a while to track down due to the nature of how OpenGl requires memory to be uploaded to the GPU. I have moved
to the Vulkan API because the lower level API appeals to my interests and the memory operations are explicit and has
better debugging infastructure so I will have a better idea of when there are bugs.

>After testing with vulkan: Vulkan does give much more freedom in memory and the debugging is better however, portability is more
difficult. With OpenGL I was able to build on my work machine and copy the .exe to a different windows machine and it would run.
With vulkan there are issues of the library(vulkan.dll) missing from the machine but also there is introduced complexity when it comes to checking for available GPU's with: available queues, enough memory(or writing fallbacks) and even features such as multisampling and
window system integration(i.e. a buffer to write colors to).

I believe the couple of milliseconds per frame I would get out of Vulkan do not outway the extra months of work in ensuring 
compatibility amongst many different devices.

### Note
* This demo uses instancing rendering to draw many textured squares. It has come to my attention that instance rendering is most useful
for more complex geometry. So, in the end, instance rendering only introduced code complexity with no optimization.
