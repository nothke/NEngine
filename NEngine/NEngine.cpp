#include "pch.h"
#include <iostream>
//#define GLEW_STATIC
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "ModelReader.h"
#include "Vertex.h"
#include <vector>
#include "imgui/imgui.h"
#include <chrono>
#include <random>

#include "Application.h"
#include "AssetManager.h"
#include "Shader.h"
#include "Mesh.h"
#include "Texture.h"
#include "Renderer.h"
#include "Camera.h"
#include "Conversion.h"
#include "Log.h"
#include "Model.h"
#define PROFILING
#include "instrumentor.h"
#include "GUI.h"
#include "FrustumCull.h"
#include "perlin/PerlinNoise.hpp"
#include "btBulletCollisionCommon.h"
#include "btBulletDynamicsCommon.h"
#include "BulletCollision/CollisionShapes/btHeightfieldTerrainShape.h"
#include "BulletCollision/CollisionDispatch/btGhostObject.h"
#include "BulletDynamics/Character/btKinematicCharacterController.h"
#include "Physics.h"
#include "DebugDraw.h"

#include "soloud.h"
#include "soloud_wav.h"
#include <map>
#include "INIReader.h"
#include "parser.hpp"
#include "FullscreenQuad.h"
#include "FrameBuffer.h"

#define USE_CONSOLE // When changing this you also need to set Linker > System > SubSystem to Console/Windows
#if defined(WIN32) && !defined(USE_CONSOLE)
#include <windows.h>
#endif

#define USE_GUI

#define LOG(x) std::cout << x << std::endl
#define LOGV(x) std::cout << x[0] << ", " << x[1] << ", " << x[2] << std::endl
#define LOGV2(x) std::cout << x[0] << ", " << x[1] << std::endl

bool mouseView = true;

Application app;
Renderer renderer;
Camera camera;
SoLoud::Soloud audio;

// TODO: put these in asset manager
SoLoud::Wav clip;
std::array<SoLoud::Wav*, 6> stepClips;

Shader* mainShader; // not like this with multiple shaders
Shader screenShader;

AssetManager assets(5, 3, 3);

glm::ivec2 targetResolution = { 1024, 768 };

float cameraSpeedInput = 1;
float cameraSpeed = 1;

bool quitKeyPressed = false;
bool drawGUI = false;
bool drawDebug = false;

bool spawnCubeThisFrame = false;

FrameBuffer fb;
FullscreenQuad* quad;

void LockMouse(bool b)
{
	if (b)
	{
		glfwSetInputMode(app.window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

		if (glfwRawMouseMotionSupported())
			glfwSetInputMode(app.window, GLFW_RAW_MOUSE_MOTION, GLFW_TRUE);
	}
	else
	{
		glfwSetInputMode(app.window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);

		if (glfwRawMouseMotionSupported())
			glfwSetInputMode(app.window, GLFW_RAW_MOUSE_MOTION, GLFW_FALSE);
	}
}

// forward declaration for key_callback
void RebuildEverything();

void PlayFootstep()
{
	int i = rand() % stepClips.size();
	// swap pointers
	auto ptr = stepClips[0];
	stepClips[0] = stepClips[i];
	stepClips[i] = ptr;

	auto& clip = *stepClips[0];
	SoLoud::handle handle = audio.play(clip, 0.05f);
}

// Keyboard button press
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	if (action == GLFW_PRESS)
	{
		switch (key)
		{
		case GLFW_KEY_B:
			spawnCubeThisFrame = true;
			break;

			// System
		case GLFW_KEY_T:
		{
			SoLoud::handle handle = audio.play3d(clip, 0, 0, 0, 0, 0, 0, 1);
			//audio.setLooping(handle, true);
		}
		//audio.play(clip);

		break;

		case GLFW_KEY_F:
		{
			PlayFootstep();
		}

		break;

		case GLFW_KEY_LEFT_CONTROL:

			mouseView = !mouseView;

			if (mouseView)
				LockMouse(true);
			else LockMouse(false);

			break;

		case GLFW_KEY_ENTER:
			app.ToggleFullscreen();
			RebuildEverything();

			break;

		case GLFW_KEY_ESCAPE:
			quitKeyPressed = true;

			break;

		case GLFW_KEY_GRAVE_ACCENT:
			drawGUI = !drawGUI;

			break;


		}
	}
}

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
	if (yoffset != 0)
	{
		cameraSpeedInput += (float)(yoffset * 0.1f);
		cameraSpeed = exp(cameraSpeedInput);
	}
}

void InitInputCallbacks()
{
	//glfwSetInputMode(gameWindow.window, GLFW_STICKY_KEYS, GLFW_TRUE);
	glfwSetKeyCallback(app.window, key_callback);
	glfwSetScrollCallback(app.window, scroll_callback);
}

inline float randv()
{
	return ((double)rand() / (RAND_MAX)) + 1;
}

void RebuildEverything()
{
	renderer.Init();

	// Initialization sequence needs to be kept assets > DebugDraw
	assets.RebuildAll();
	mainShader = &assets.shaders[0];
	//mainShader->Bind();
	screenShader.Recompile();

	fb.fbWidth = app.currentWidth / 4;
	fb.fbHeight = app.currentHeight / 4;

	delete(quad);

	fb.Release();
	fb.Build();

	quad = new FullscreenQuad();

	DebugDraw::RecompileShader();

	InitInputCallbacks();

	camera.SetProjection(90.0f, app.aspectRatio);

#ifdef USE_GUI
	GUI::Shutdown();

	GUI::Init(app.window);
#endif

	LockMouse(true);

	LOG("Rebuilt everything after resolution change");
}

bool KeyPressed(int key)
{
	int state = glfwGetKey(app.window, key);
	return state == GLFW_PRESS;
}

struct ParsedModel
{
	std::string name;
	std::string texture;
	vec3 pos;
	vec3 rot;
	vec3 scl;
	bool createCollider;
};

#if defined(WIN32) && !defined(USE_CONSOLE)
int WINAPI WinMain(
	HINSTANCE hInstance,
	HINSTANCE hPrevInstance,
	LPSTR     lpCmdLine,
	int       nShowCmd)
#else
int main()
#endif
{
	// TODO: Parse command line arguments

	Instrumentor::Instance().beginSession("Game Session", "../results.json");

	app.fullscreen = true;
	//app.windowedWidth = 1024;
	//app.windowedHeight = 768;
	if (app.Init()) return -1;

#ifdef USE_GUI
	GUI::Init(app.window);
#endif

	// Ground noise
	siv::PerlinNoise pnoise = siv::PerlinNoise();
	const float freq = 0.092451f;
	const float gain = 2;
	const int octaves = 4;

	// Physics
	Physics physics;

	//auto groundShape = physics.AddShape(new btBoxShape(btVector3(btScalar(50.), btScalar(50.), btScalar(50.))));
	//auto groundBody = physics.CreateBody(groundShape, 0, btVector3(0, -50, 0), btQuaternion::getIdentity());

	//auto unitCubeShape = physics.AddShape(new btBoxShape(btVector3(1, 1, 1)));
	//auto monkeyBody = physics.CreateBody(unitCubeShape, 1, btVector3(2, 100, -20), btQuaternion(30, 20, 30));

	//auto sphereShape = physics.AddShape(new btSphereShape(1));
	//auto sphereBody = physics.CreateBody(sphereShape, 0, btVector3(0, 0, 0), btQuaternion::getIdentity());


	/*
	float heights[32 * 32];
	for (size_t y = 0; y < 32; y++)
	{
		for (size_t x = 0; x < 32; x++)
		{
			heights[y * 32 + x] = pnoise.accumulatedOctaveNoise2D(x * freq, y * freq, octaves) * gain;
		}
	}*/

	//auto terrainShape = physics.AddShape(new btHeightfieldTerrainShape(32, 32, &heights, 20, 1, true, false));

	audio.init();

	// Sounds
	auto mess = clip.load("res/sfx/tram_joint_1.wav");

	for (size_t i = 0; i < stepClips.size(); i++)
	{
		stepClips[i] = new SoLoud::Wav();
		const char* c = ("res/sfx/step_sand" + std::to_string(i + 1) + ".wav").c_str();
		stepClips[i]->load(c);
	}

	SoLoud::Wav mus_2;
	mus_2.load("res/sfx/mus_2.wav");
	auto mus_handle = audio.play3d(mus_2, -48.45f, -9.39f, 28.02f);
	audio.set3dSourceAttenuation(mus_handle, 1, 0.05f);
	audio.setLooping(mus_handle, true);

	SoLoud::Wav pinaten;
	pinaten.load("res/sfx/pinaten.ogg");
	auto pinaten_handle = audio.play3d(pinaten, 10000, 10000, 10000);
	audio.set3dSourceAttenuation(pinaten_handle, 1, 0.1f);
	audio.setLooping(pinaten_handle, true);

	vec3 coffeePos = { 15.0f, -3.76f, 44.21f };
	SoLoud::Wav coffeeClip;
	coffeeClip.load("res/sfx/coffee.ogg");
	auto coffee = audio.play3d(coffeeClip, -coffeePos.x, -coffeePos.y, -coffeePos.z);
	audio.set3dSourceAttenuation(coffee, 1, 0.1f);
	audio.setLooping(coffee, true);

	vec3 mausoleumpos = { 47.97, 8.54, -27.4 };

	vec3 miccpos = { 42.12, 8.94, -27.94 };
	miccpos = { 0, 10000, 0 };

	//audio.play(clip);

	// Meshes
	//Mesh plainMesh = assets.CreateMesh("res/models/plain.ply");


	Mesh plainMesh;
	{
		std::vector<Vertex> vertices;
		std::vector<unsigned int> indices;
		ModelReader::LoadFromPly("res/models/plain.ply", vertices, indices);

		for (auto& v : vertices)
		{
			v.posy = pnoise.accumulatedOctaveNoise2D(v.posx * freq, v.posz * freq, octaves) * gain;
		}
		plainMesh = assets.CreateMesh(vertices, indices, "plain_generated");
	}

	Mesh monkeyMesh = assets.CreateMesh("res/models/suza.ply");
	//Mesh grassMesh = assets.CreateMesh("res/models/grasso.ply");
	Mesh skyMesh = assets.CreateMesh("res/models/skysphere.ply");
	Mesh cubeMesh = assets.CreateMesh("res/models/cube.ply");
	Mesh houseMesh = assets.CreateMesh("res/models/farmhouse.ply");
	assets.CreateMesh("res/models/mausoleum.ply");

	assets.CreateMesh("res/models/birch.ply");
	Mesh road = assets.CreateMesh("res/models/hillyroad_road.ply");
	Mesh hillGrass = assets.CreateMesh("res/models/hillyroad_grass.ply");

	assets.CreateMesh("res/models/mihaus.ply");
	assets.CreateTexture("res/models/house_a.png");

	assets.CreateMesh("res/models/stonewall.ply");
	assets.CreateTexture("res/models/stonewall.png");

	Mesh miccelMesh = assets.CreateMesh("res/models/miccel.ply");
	Texture miccelTex = assets.CreateTexture("res/models/miccel.png");

	auto col = physics.CreateMeshCollider(road);
	physics.AddShape(col);

	auto grassShape = physics.AddShape(physics.CreateMeshCollider(hillGrass));
	physics.CreateBody(grassShape, 0, btVector3(0, 0, 0), btQuaternion::getIdentity());

	std::cout << "End mesh gen" << std::endl;

	// Shaders
	mainShader = &assets.CreateShader("res/texture.glsl");
	mainShader->Bind();

	screenShader = assets.CreateShader("res/quad.glsl");

	// Textures
	//Texture grass3DTex = assets.CreateTexture("res/models/grasso.png");
	Texture grassPlainTex = assets.CreateTexture("res/models/grass.png", Texture::Filtering::Nearest, Texture::EdgeMode::Wrap);
	Texture redCube = assets.CreateTexture("res/models/redsquare.png");
	Texture whiteCube = assets.CreateTexture("res/models/whitesquare.png");
	Texture houseTex = assets.CreateTexture("res/models/farmhouse_a.png");

	assets.CreateTexture("res/models/tree_birch.png");
	assets.CreateTexture("res/models/tarmac.png");
	assets.CreateTexture("res/models/concrete.png");

	auto character = physics.CreateCharacter(btVector3(50, 10, 0));

	// Miccel
	auto miccelCharacter = physics.CreateCharacter(btVector3(0, 10, 0));

	// Buffer
	fb.fbWidth = app.currentWidth / 4;
	fb.fbHeight = app.currentHeight / 4;

	fb.Build();

	quad = new FullscreenQuad();

	DebugDraw::Init();

	renderer.Init();

	// FPS input setup
	InitInputCallbacks();

	LockMouse(true);

	const float mouseSensitivity = 0.2f;

	glm::vec3 lastCamPos = vec3(0);
	glm::vec2 playerInput;
	glm::vec2 rotation = glm::vec2(0, 0);
	glm::vec2 lastMousePos;
	float footstepDistance = 0;

	{
		// Initialize mouse position
		double lastMousePosX, lastMousePosY;
		glfwGetCursorPos(app.window, &lastMousePosX, &lastMousePosY);
		lastMousePos = glm::vec2(lastMousePosX, lastMousePosY);
	}

	// Timing
	double lastFrameTime = glfwGetTime();

	// Camera
	camera.SetProjection(60.0f, app.aspectRatio);
	bool constrainCameraToGround = true;

	const glm::vec3 RIGHT = glm::vec3(1, 0, 0);
	const glm::vec3 UP = glm::vec3(0, 1, 0);
	const glm::vec3 FORWARD = glm::vec3(0, 0, 1);

	// Data passed to shader
	ImVec4 color1(0.919f, 1.000f, 0.828f, 1.000f);
	ImVec4 color2(0.887f, 0.594f, 0.300f, 1.000f);
	vec4 shader_FogParams = vec4(10.0f, 0.5f, 4.0f, 10.0f);

	// SCENE

	std::vector<Model> objects;
	objects.reserve(1024);

	// Sky
	//Model sky({ 0,0,0 }, skyMesh);
	//sky.SetScale(vec3(100));
	//objects.push_back(sky);

	// Plain
	//Model t({ 0,0,0 }, plainMesh, grassPlainTex);
	//objects.push_back(t);

	// Second plain
	//Model t2({ 0, -4, 0 }, plainMesh, grassPlainTex);
	//t2.SetScale(5);
	//objects.push_back(t2);

	/*
	const int monkeys = 10;
	for (size_t y = 0; y < monkeys; y++)
	{
		for (size_t x = 0; x < monkeys; x++)
		{
			Model m({ x * 2, y * 2, -10 }, monkeyMesh);
			objects.push_back(m);
		}
	}*/

	// Grass
	/*
	const int grassCount = 160;
	double PI = glm::pi<double>();
	for (size_t y = 0; y < grassCount; y++)
	{
		for (size_t x = 0; x < grassCount; x++)
		{
			vec3 pos(
				((-grassCount / 2.0f) + x + randv()) * 0.5f, 0,
				((-grassCount / 2.0f) + y + randv()) * 0.5f);

			pos.y = pnoise.accumulatedOctaveNoise2D(pos.x * freq, pos.z * freq, octaves) * gain;

			Model m(pos, grassMesh, grass3DTex);
			float a = randv() * PI;
			m.SetRotation(vec3(0, a, 0));
			objects.push_back(m);
		}
	}*/

	// rigidbody monkey
	/*
	Model trbMonkey({ 0,0,0 }, monkeyMesh, grassPlainTex);
	objects.push_back(trbMonkey);
	Model& rbMonkey = objects[objects.size() - 1];
	physics.BindBodyToModel(monkeyBody, rbMonkey);
	*/

	// cubes
	/*
	for (size_t y = 0; y < 10; y++)
	{
		float xoff = y % 2 == 0 ? 0.5f : 0;
		for (size_t x = 0; x < 4; x++)
		{
			auto body = physics.CreateBody(unitCubeShape, 100, btVector3((x + xoff) * 2, 5 + y * 2, -10), btQuaternion::getIdentity());
			Model cube({ 0,0,0 }, cubeMesh, whiteCube);
			objects.push_back(cube);
			Model& cubeRef = objects[objects.size() - 1];
			physics.BindBodyToModel(body, cubeRef);
		}
	}*/

	//Model farmhouse = Model({ 0,-1,0 }, houseMesh, houseTex);
	//objects.push_back(farmhouse);

	/*
	INIReader reader("../test.ini");

	if (reader.ParseError() != 0)
	{
		std::cout << "Can't load";
	}
	else
	{

		std::cout << reader.GetInteger("HEADER", "warmup", 0) << std::endl;
	}*/

	// Parse scene CSV
	std::ifstream f("res/scene.csv");
	aria::csv::CsvParser parser = aria::csv::CsvParser(f);

	std::vector<ParsedModel> parsedModels;
	for (auto& row : parser)
	{
		ParsedModel m;

		m.name = row[0];
		m.texture = row[1];
		m.pos = { stof(row[2]), stof(row[3]), stof(row[4]) };
		m.rot = { stof(row[5]), stof(row[6]), stof(row[7]) };
		m.scl = { stof(row[8]), stof(row[9]), stof(row[10]) };
		m.createCollider = stoi(row[11]);

		parsedModels.push_back(m);
	}

	std::map<std::string, btCollisionShape*> shapeMap;

	// Add models from CSV
	for (auto& m : parsedModels)
	{
		int i = assets.GetMeshIndex(m.name);
		if (i >= 0)
		{
			vec3 pos = m.pos;
			pos.x = -m.pos.x;

			Model model = Model(pos, assets.GetMesh(i));
			model.SetRotation(vec3(radians(m.rot.x), radians(-m.rot.y), radians(m.rot.z)));

			model.SetScale(m.scl);
			auto opt_tex = assets.GetTexture(m.texture.c_str());
			if (opt_tex.has_value())
				model.texture = &opt_tex.value().get();
			objects.push_back(model);

			if (m.createCollider)
			{
				mat4 matrix = model.LocalToWorld();

				if (shapeMap.count(m.name) > 0)
				{
					physics.CreateBody(shapeMap[m.name], 0, (btScalar*)&matrix[0]);
				}
				else
				{
					auto mcol = physics.CreateMeshCollider(model.mesh);
					shapeMap.emplace(m.name, mcol);
					physics.CreateBody(mcol, 0, (btScalar*)&matrix[0]);
					LOG("Creating collider for " << m.name);
				}
			}
		}
	}

	objects.push_back(Model(vec3(0), miccelMesh, miccelTex));
	auto& miccel = objects[objects.size() - 1];
	miccel.SetScale(1.5f);
	miccel.SetPosition({ 0, 100, 0 });
	miccel.UpdateModelMatrix();

	float smallBoxSize = 0.5f;
	btCollisionShape* smallBoxShape = physics.AddShape(new btBoxShape(btVector3(smallBoxSize, smallBoxSize, smallBoxSize)));

	std::map<const btCollisionObject*, std::string> namePair;
	namePair[nullptr] = "Shit!";

	bool miccelSpawned = false;

	// GAME LOOP
	while (!glfwWindowShouldClose(app.window))
	{
		PROFILE_SCOPE("Game Loop");

		// Time
		const double time = glfwGetTime();
		const float dt = (float)(time - lastFrameTime);
		lastFrameTime = time;

		// Input
		glfwPollEvents();

		playerInput.x = (float)(KeyPressed(GLFW_KEY_A) ? -1 : KeyPressed(GLFW_KEY_D) ? 1 : 0);
		playerInput.y = (float)(KeyPressed(GLFW_KEY_W) ? -1 : KeyPressed(GLFW_KEY_S) ? 1 : 0);

		double mousePosX, mousePosY;
		glfwGetCursorPos(app.window, &mousePosX, &mousePosY);
		auto mousePos = glm::vec2(mousePosX, mousePosY);

		glm::vec2 mouseDelta = mousePos - lastMousePos;
		lastMousePos = mousePos;

		if (mouseView)
		{
			glm::vec2 delta = mouseDelta * mouseSensitivity * (1.0f / 60.0f);
			rotation += delta;
		}

		const float rad90 = 1.56f;
		rotation.y = glm::clamp(rotation.y, -rad90, rad90);

		// Camera
		camera.SetInputRotation(rotation);
		camera.UpdateRotation();
		camera.MoveRelative(playerInput * dt * cameraSpeed);

		/*
		if (constrainCameraToGround)
		{
			camera.position.y = -pnoise.accumulatedOctaveNoise2D(-camera.position.x * freq, -camera.position.z * freq, octaves) * gain;
			camera.position.y -= 1;
		}*/

		// override camera position from character
		vec3 campos;
		if (constrainCameraToGround)
		{
			campos = from(character->getGhostObject()->getWorldTransform().getOrigin()) + UP;
			campos = -campos;
			camera.position = campos;
			camera.Update();

			// Character move
			character->setWalkDirection(from(
				-camera.right * playerInput.x +
				-camera.forward * playerInput.y) * 0.03f);
		}
		else
			camera.Update();

		if (length(mausoleumpos - camera.position) < 5)
		{
			miccelSpawned = true;
		}

		// Miccel
		if (miccelSpawned)
		{
			miccpos = from(miccelCharacter->getGhostObject()->getWorldTransform().getOrigin());
			vec3 walkDir = -camera.position - miccpos + vec3(0.001f);
			walkDir.y = 0;
			walkDir = normalize(walkDir);

			LOGV(walkDir);
			miccelCharacter->setWalkDirection(from(walkDir * 0.025f));
			miccel.SetPosition(miccpos);
			miccel.SetRotation(quatLookAt(walkDir, UP));
			miccel.UpdateModelMatrix();
			audio.set3dSourcePosition(pinaten_handle, miccpos.x, miccpos.y, miccpos.z);

			if (length(-miccpos - camera.position) < 2)
			{
				LOG(" ");
				LOG("You ded");
				break;
			}
		}

		if (KeyPressed(GLFW_KEY_B))
		{
			auto cubeRB = physics.CreateBody(smallBoxShape, 20, from(-camera.position), btQuaternion::getIdentity());
			namePair[cubeRB] = "Red Box";
			//auto model = Model(vec3(0), cubeMesh, redCube);
			Model& model = objects.emplace_back(vec3(0), cubeMesh, redCube);
			//Model& model2 = objects[objects.size() - 1];
			model.SetScale(smallBoxSize);

			physics.BindBodyToModel(cubeRB, model);

			cubeRB->setLinearVelocity(from(camera.forward * 2.0f));

			spawnCubeThisFrame = false;
		}

		if (KeyPressed(GLFW_KEY_M))
			miccelSpawned = true;

		auto hit = physics.Raycast(camera.position, camera.forward, 10);

		if (hit.hasHit())
		{
			DebugDraw::Ray(from(hit.m_hitPointWorld), from(hit.m_hitNormalWorld), { 0, 1, 0, 1 });
			DebugDraw::Line(from(hit.m_hitPointWorld) + RIGHT, from(hit.m_hitPointWorld) + UP, { 1,0,0,1 });

			const btCollisionObject* hitobj = hit.m_collisionObject;

			if (namePair.count(hitobj) > 0)
			{
				LOG(namePair[hitobj]);
			}
		}

		DebugDraw::AABB(vec3(10), vec3(8), { 0, 1, 1, 1 });
		DebugDraw::Circle(vec3(10), 2, { 0,0,1.0f }, vec4(1, 0, 0, 1), 32);
		DebugDraw::Circle(vec3(10), 1, { 0,1,1 }, vec4(0, 0, 0, 1), 32);
		DebugDraw::Circle(vec3(10), 1, { 1.0f,0.0f,0.1f }, vec4(0, 0, 1, 1), 32);

		DebugDraw::Cross(vec3(10, 10, 10), 0.5f, vec4(1, 1, 0, 1));

		DebugDraw::Sphere(vec3(10), 6, vec4(0.5f, 1, 0.5f, 1));

		// bullet simulate
		{
			PROFILE_SCOPE("Physics step");
			physics.Step(dt);
		}

		{
			PROFILE_SCOPE("Physics update");
			physics.UpdateModels();
		}

		float distancePassed = glm::length(camera.position - lastCamPos);
		lastCamPos = camera.position;

		audio.set3dListenerParameters(
			-camera.position.x, -camera.position.y, -camera.position.z,
			camera.forward.x, camera.forward.y, camera.forward.z,
			0, 1, 0);

		audio.update3dAudio();

		footstepDistance += distancePassed;
		if (footstepDistance > 0.7f)
		{
			PlayFootstep();
			footstepDistance = 0;
		}

		//-------------
		//  Rendering
		//-------------

		fb.Bind();

		renderer.Clear(from(color1));

		Shader shader = *mainShader;
		shader.Bind();

		// imgui read values
		glm::vec4 inputColor1 = from(color1);
		shader.SetVector("_InputColor1", inputColor1);

		glm::vec4 inputColor2 = from(color2);
		shader.SetVector("_InputColor2", inputColor2);

		shader.SetVector("_FogParams", shader_FogParams);

		shader.SetVPMatrix(camera.v);
		shader.SetMatrix("_P", camera.p);
		shader.SetVector("_CamPos", vec4(camera.position, 1));
		shader.SetFloat("_Time", (float)time);
		Frustum frustum = Frustum(camera.vp);

		// Draw calls
		{
			PROFILE_SCOPE("Draw");
			Mesh* meshPtr = nullptr;
			Texture* texPtr = nullptr;

			for (Model& go : objects)
			{
				if (!go.IsVisible(frustum))
					continue;

				if (meshPtr != &go.mesh)
				{
					go.mesh.Bind();
					meshPtr = &go.mesh;
				}

				if (texPtr != go.texture)
				{
					if (go.texture != nullptr)
					{
						go.texture->Bind(0);
						shader.SetInt("_Texture", 0);
						shader.Bind();
					}
					else
						texPtr->Unbind();

					texPtr = go.texture;
				}

				shader.SetMMatrix(go.LocalToWorld());
				renderer.DrawMesh(go.mesh);
			}

			// unbind last texture
			if (texPtr != nullptr)
				texPtr->Unbind();
		}

		if (drawDebug)
			DebugDraw::Render(camera.vp);
		else
			DebugDraw::Clear();

		glViewport(0, 0, app.currentWidth, app.currentHeight);

		{
			// unbind vertices pre framebuffer
			GLCall(glBindVertexArray(0));
			GLCall(glBindBuffer(GL_ARRAY_BUFFER, 0));

			fb.DrawPre();

			screenShader.Bind();
			quad->Bind();

			fb.BindTexture();

			quad->Draw();

			quad->Unbind();
		}

		// imgui
		bool applyResolution = false;
#ifdef USE_GUI
		if (drawGUI)
		{
			PROFILE_SCOPE("ImGui");

			GUI::NewFrame();

			//ImGui::ShowDemoWindow();

			{
				ImGui::Begin("NEngine");

				ImGui::ColorEdit3("color 1", (float*)&color1);
				ImGui::ColorEdit3("color 2", (float*)&color2);

				ImGui::Checkbox("Constrain camera to ground", &constrainCameraToGround);
				ImGui::Checkbox("Draw debug lines", &drawDebug);

				ImGui::Text("Fog");
				ImGui::SliderFloat("Fog range", &shader_FogParams.x, 0, 1000, "%.3f", 2.0f);
				ImGui::SliderFloat("Fog power", &shader_FogParams.y, 0, 1);
				ImGui::SliderFloat("Fog height offset", &shader_FogParams.z, -10, 10);
				ImGui::SliderFloat("Fog height mult", &shader_FogParams.w, 0, 10);

				ImGui::Text("Window");
				ImGui::DragInt2("Resolution", &targetResolution[0], 4);
				if (ImGui::Button("Apply"))
				{
					applyResolution = true;
				}

				if (ImGui::Button("Rescale"))
				{
					app.RescaleTo(targetResolution.x, targetResolution.y);
				}

				ImGui::SameLine();
				ImGui::Checkbox("Fullscreen", &app.fullscreen);

				if (ImGui::Button("Recompile Shaders"))
					assets.ReloadShaders();

				// Analitics
				ImGui::Text("DT: %.3f ms, FPS: %.1f, AVG: %.1f", dt, 1.0f / dt, ImGui::GetIO().Framerate);

				ImGui::Text("Player pos: %.2f, %.2f, %.2f", camera.position.x, camera.position.y, camera.position.z);

				ImGui::End();
			}

			GUI::Render();
		}
#endif


		app.SwapBuffers();

		if (applyResolution)
		{
			app.ChangeResolution(targetResolution.x, targetResolution.y);
			RebuildEverything();
		}

		if (quitKeyPressed)
			break;
	}

#ifdef USE_GUI
	GUI::Shutdown();
#endif

	Mesh& mesh = assets.GetMesh("grasso");
	//mesh.boundsMax = vec3(0, 0, 0);

	for (size_t i = 0; i < stepClips.size(); i++)
	{
		delete stepClips[i];
	}

	delete(quad);
	fb.Release();

	audio.deinit();

	assets.Dispose();

	app.Terminate();

	Instrumentor::Instance().endSession();

	return 0;
}