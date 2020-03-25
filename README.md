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
- You need to build bullet. Here is how to do it in case you don't want to spend a day figuring out how to like I did:
	- Clone bullet repo to your PC
	- Open the root with CMAKE (I used cmake GUI, but if you're hardcore you can use the commandline too)
	- cmake: Make sure to select win32 MSVC
	- cmake: Enable USE_MSVC_RUNTIME_LIBRARY_DLL, this is important because if you don't it will give you a LNK2038 error, a missmatch between dynamic and static library, which is weird, didn't understand, but just do it.
	- cmake: Generate
	- Go into your build folder and open BULLET_PHYSICS.sln
	- Right click on ALL_BUILD > Build, it'll take around 5 minutes to build
	- You should build for both Release and Debug, select the other one above in the toolbar and build again.
	- Now in lib folder there will be Release and Debug folders filled with lib files. Copy them to NEngine/Dependencies/bullet/ (make a bullet folder first)
	- Now for include, Bullet doesn't come with separated headers so you'll have to prune them yourself. First go to root bullet repo folder, copy src folder and paste it into NEngine/Dependencies/bullet/.
	- Rename the src folder to include
	- Now in that folder, search for all .cpp, .py and .txt files and delete them so that only .h files are left.
	- That should be it, you should now be able to build NEngine.

### Useful links:
- [TheCherno's OpenGL tutorials](https://www.youtube.com/playlist?list=PLlrATfBNZ98foTJPJ_Ev03o2oq3-GGOS2)
- [OpenGL common beginner mistakes](https://www.khronos.org/opengl/wiki/Common_Mistakes)
- [My thread on importance of using a mesh optimizer](https://twitter.com/Nothke/status/1240641475286896643)
- [Arseny Kapolkine on finding the best optimization algorhitm](https://zeux.io/2020/01/22/learning-from-data/)
- A glitch I encountered that accidentally demonstrates the difference in vertex thread blocks [without](https://twitter.com/Nothke/status/1241499230197428229) and [with optimiztion](https://twitter.com/Nothke/status/1241499415740846081)