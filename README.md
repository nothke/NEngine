## NEngine
Only used for learning about making games in C++ and OpenGL. Not to be used for anything serious.

Uses:
- [GLFW](https://www.glfw.org/)
- [GLEW](http://glew.sourceforge.net/)
- [glm](https://github.com/g-truc/glm)
- [meshoptimizer](https://github.com/zeux/meshoptimizer) by Arseny Kapoulkine
- [stb_image](https://github.com/nothings/stb)
- [FrustumCull.h](https://gist.github.com/podgorskiy/e698d18879588ada9014768e3e82a644) by podgorskiy, based on [this](http://iquilezles.org/www/articles/frustumcorrect/frustumcorrect.htm) by Inigo Quilez
- [instrumentor.h](https://pastebin.com/qw5Neq4U), original by [TheCherno](https://github.com/TheCherno), improved by [David Churchill](https://gist.github.com/davechurchill)

### Requirements
- glm is not included in the repo. Get it from [glm](https://github.com/g-truc/glm) page and copy "glm" folder to (repo location)/NEngine/vendor/glm.

### Useful links:
- [TheCherno's OpenGL tutorials](https://www.youtube.com/playlist?list=PLlrATfBNZ98foTJPJ_Ev03o2oq3-GGOS2)
- [OpenGL common beginner mistakes](https://www.khronos.org/opengl/wiki/Common_Mistakes)
- [My thread on importance of using a mesh optimizer](https://twitter.com/Nothke/status/1240641475286896643)
- [Arseny Kapolkine on finding the best optimization algorhitm](https://zeux.io/2020/01/22/learning-from-data/)
- A glitch I encountered that accidentally demonstrates the difference in vertex thread blocks [without](https://twitter.com/Nothke/status/1241499230197428229) and [with optimiztion](https://twitter.com/Nothke/status/1241499415740846081)