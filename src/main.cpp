#pragma once
// CG Author:		Michael Gharbharan
// Creation Date:	September 15, 2017
// Modified By:		Tom Tsiliopoulos 
// Student ID:		100616336
// Date Modified:	February 11, 2018

#define GLEW_STATIC
#include "glew/glew.h"
#pragma comment(lib, "glew32s.lib")
#pragma comment(lib, "FreeImage.lib")
#pragma comment(lib, "nfd_d.lib")

#include "GameObject.h"
#include "AnimationMath.h"
#include "ParticleEmitter.h"
#include "GUIFuncs.h"

// Core Libraries (std::)
#include <iostream>
#include <string>
#include <math.h>
#include <vector>
#include <direct.h>
#include <map> 
#include <fstream>

// 3rd Party Libraries
#include <GLUT/freeglut.h>
#include <TTK/GraphicsUtils.h>
#include <TTK/Camera.h>
#include "imgui/imgui.h"
#include <TTK/OBJMesh.h>
#include <nfd/src/include/nfd.h>

// Sound Engine
#include "SoundEngine.h"
//#include "SoundFile.h"

// Defines and Core variables
#define FRAMES_PER_SECOND 60
const int FRAME_DELAY = 1000 / FRAMES_PER_SECOND; // Milliseconds per frame

// Window size
int windowWidth = 800;
int windowHeight = 600;

// Angle conversions
const float degToRad = 3.14159f / 180.0f;
const float radToDeg = 180.0f / 3.14159f;

float deltaTime = 0.0f; // amount of time since last update (set every frame in timer callback)

// Mouse position in pixels
glm::vec3 mousePosition; // x, y, 0
glm::vec3 mousePositionFlipped; // x, windowHeight - y, 0

// Game Objects
GameObject Head;
GameObject Speaker;
std::map<std::string, std::shared_ptr<TTK::OBJMesh>> meshes;

TTK::Camera camera;

// Sound Objects
SoundEngine		soundEngine;
//FMOD_RESULT     result;

// Sound Channel
FMOD_VECTOR soundPos = { 0.0f, 0.0f, 0.0f };
FMOD_VECTOR soundVel = { 0.0f, 0.0f, 0.0f };

// Switches
bool panEnabled = false;
bool distanceEnabled = false;
bool rollOff = false;

// Sliders
float radius = 5.0f;

// Menu Switches
static bool openSoundFile = false;
static bool exitEditor = false;
static bool displayAbout = false;
static bool resetScene = false;

std::string GetProjectDirectory()
{
	char* cwd = _getcwd(0, 0); // **** microsoft specific ****
	std::string working_directory(cwd);
	std::free(cwd);
	return working_directory;
}

void OpenSoundFile()
{
	const nfdchar_t *defaultPath = NULL;
	nfdchar_t *outPath = NULL;

	std::string currentDirectory = GetProjectDirectory();
	defaultPath = currentDirectory.c_str();

	nfdresult_t nfdresult = NFD_OpenDialog("wav", defaultPath, &outPath);
	if (nfdresult == NFD_OKAY)
	{
		puts("opened new file");

		puts(outPath);

		/*
		if (soundFile) {
			result = soundFile->release();
			soundEngine.FmodErrorCheck(result);
		}
		*/

		if (soundEngine.sound) {
			soundEngine.sound->cleanUp();
		}

		soundEngine.result = soundEngine.system->createSound(outPath, FMOD_3D, 0, &soundEngine.sound->file);
		soundEngine.FmodErrorCheck(soundEngine.result);
		soundEngine.result = soundEngine.sound->file->set3DMinMaxDistance(0.0f, 6.0f);
		soundEngine.FmodErrorCheck(soundEngine.result);
		soundEngine.result = soundEngine.sound->file->setMode(FMOD_LOOP_NORMAL);
		soundEngine.FmodErrorCheck(soundEngine.result);

		soundEngine.result = soundEngine.system->playSound(soundEngine.sound->file, 0, true, &soundEngine.sound->channel);
		soundEngine.FmodErrorCheck(soundEngine.result);
		soundEngine.result = soundEngine.sound->channel->set3DAttributes(&soundPos, &soundVel);
		soundEngine.FmodErrorCheck(soundEngine.result);
		soundEngine.result = soundEngine.sound->channel->setPaused(false);
		soundEngine.FmodErrorCheck(soundEngine.result);
	}
	else if (nfdresult == NFD_CANCEL)
	{
		puts("User pressed cancel.");
	}
	else
	{
		printf("Error: %s\n", NFD_GetError());
	}
}

void SoundEngineInit() {
	soundEngine.Init(); // initialize Sound Engine
	
	/*
	Load some sounds
	*/

	soundEngine.result = soundEngine.system->createSound("../media/drumloop.wav", FMOD_3D, 0, &soundEngine.sound->file);
	soundEngine.FmodErrorCheck(soundEngine.result);
	soundEngine.result = soundEngine.sound->file->set3DMinMaxDistance(0.5f, 300.0f);
	soundEngine.FmodErrorCheck(soundEngine.result);
	soundEngine.result = soundEngine.sound->file->setMode(FMOD_LOOP_NORMAL);
	soundEngine.FmodErrorCheck(soundEngine.result);

	/*
	Play sounds at ertain positions
	*/

	soundEngine.result = soundEngine.system->playSound(soundEngine.sound->file, 0, true, &soundEngine.sound->channel);
	soundEngine.FmodErrorCheck(soundEngine.result);
	soundEngine.result = soundEngine.sound->channel->set3DAttributes(&soundPos, &soundVel);
	soundEngine.FmodErrorCheck(soundEngine.result);
	soundEngine.result = soundEngine.sound->channel->setPaused(false);
	soundEngine.FmodErrorCheck(soundEngine.result);

}

void ShowMenu()
{
	// setup main menu bar
	if (ImGui::BeginMainMenuBar())
	{
		if (ImGui::BeginMenu("File"))
		{
			ImGui::MenuItem("Open Sound File", NULL, &openSoundFile);
			ImGui::Separator();
			ImGui::MenuItem("Exit", NULL, &exitEditor);
			ImGui::EndMenu();
		}
		if (ImGui::BeginMenu("Edit"))
		{
			ImGui::MenuItem("Reset", NULL, &resetScene);

			ImGui::EndMenu();
		}
		if (ImGui::BeginMenu("Help"))
		{
			ImGui::Separator();
			ImGui::MenuItem("About", NULL, &displayAbout);
			ImGui::EndMenu();
		}
		ImGui::EndMainMenuBar();
	}

	if (openSoundFile) {
		OpenSoundFile();
		openSoundFile = false;
	}
	if (resetScene)
	{
		panEnabled = false;
		distanceEnabled = false;

		resetScene = false;
	}

	if (exitEditor) {
		glutExit();
	}

	if (displayAbout) {
		ImGui::Begin("Game Sound - Assignment 1", &displayAbout, ImGuiWindowFlags_AlwaysAutoResize);
		ImGui::Separator();
		ImGui::Text("Authors:");
		ImGui::Text("Tom Tsiliopoulos - 100616336");
		ImGui::End();
	}
}

void InitializeScene()
{
	SoundEngineInit();

	// Load meshes
	std::string meshPath = "../Assets/Models/";

	std::shared_ptr<TTK::OBJMesh> monkeyMesh = std::make_shared<TTK::OBJMesh>();
	std::shared_ptr<TTK::OBJMesh> speakerMesh = std::make_shared<TTK::OBJMesh>();

	monkeyMesh->loadMesh(meshPath + "Monkey.obj");
	speakerMesh->loadMesh(meshPath + "speaker.obj");

	// assign to meshes map
	meshes["monkey"] = monkeyMesh;
	meshes["speaker"] = speakerMesh;

	Head.mesh = meshes["monkey"];
	Speaker.mesh = meshes["speaker"];

	Head.name = "Head";
	Speaker.name = "Speaker";
}

void setUIStyle() {
	ImGuiStyle& style = ImGui::GetStyle();

	style.Alpha = 0.8f;
	style.FrameRounding = 3.0f;
	style.Colors[ImGuiCol_Text] = ImVec4(0.00f, 0.00f, 0.00f, 1.00f);
	style.Colors[ImGuiCol_WindowBg] = ImVec4(0.94f, 0.94f, 0.94f, 0.94f);
	style.Colors[ImGuiCol_ChildWindowBg] = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
	style.Colors[ImGuiCol_PopupBg] = ImVec4(1.00f, 1.00f, 1.00f, 0.94f);
	style.Colors[ImGuiCol_Border] = ImVec4(0.00f, 0.00f, 0.00f, 0.39f);
	style.Colors[ImGuiCol_BorderShadow] = ImVec4(1.00f, 1.00f, 1.00f, 0.10f);
	style.Colors[ImGuiCol_FrameBg] = ImVec4(1.00f, 1.00f, 1.00f, 0.94f);
	style.Colors[ImGuiCol_FrameBgHovered] = ImVec4(0.26f, 0.59f, 0.98f, 0.40f);
	style.Colors[ImGuiCol_FrameBgActive] = ImVec4(0.26f, 0.59f, 0.98f, 0.67f);
	style.Colors[ImGuiCol_TitleBg] = ImVec4(0.96f, 0.96f, 0.96f, 1.00f);
	style.Colors[ImGuiCol_TitleBgCollapsed] = ImVec4(1.00f, 1.00f, 1.00f, 0.51f);
	style.Colors[ImGuiCol_TitleBgActive] = ImVec4(0.82f, 0.82f, 0.82f, 1.00f);
	style.Colors[ImGuiCol_MenuBarBg] = ImVec4(0.86f, 0.86f, 0.86f, 1.00f);
	style.Colors[ImGuiCol_ScrollbarBg] = ImVec4(0.98f, 0.98f, 0.98f, 0.53f);
	style.Colors[ImGuiCol_ScrollbarGrab] = ImVec4(0.69f, 0.69f, 0.69f, 1.00f);
	style.Colors[ImGuiCol_ScrollbarGrabHovered] = ImVec4(0.59f, 0.59f, 0.59f, 1.00f);
	style.Colors[ImGuiCol_ScrollbarGrabActive] = ImVec4(0.49f, 0.49f, 0.49f, 1.00f);
	style.Colors[ImGuiCol_ComboBg] = ImVec4(0.86f, 0.86f, 0.86f, 0.99f);
	style.Colors[ImGuiCol_CheckMark] = ImVec4(0.26f, 0.59f, 0.98f, 1.00f);
	style.Colors[ImGuiCol_SliderGrab] = ImVec4(0.24f, 0.52f, 0.88f, 1.00f);
	style.Colors[ImGuiCol_SliderGrabActive] = ImVec4(0.26f, 0.59f, 0.98f, 1.00f);
	style.Colors[ImGuiCol_Button] = ImVec4(0.26f, 0.59f, 0.98f, 0.40f);
	style.Colors[ImGuiCol_ButtonHovered] = ImVec4(0.26f, 0.59f, 0.98f, 1.00f);
	style.Colors[ImGuiCol_ButtonActive] = ImVec4(0.06f, 0.53f, 0.98f, 1.00f);
	style.Colors[ImGuiCol_Header] = ImVec4(0.26f, 0.59f, 0.98f, 0.31f);
	style.Colors[ImGuiCol_HeaderHovered] = ImVec4(0.26f, 0.59f, 0.98f, 0.80f);
	style.Colors[ImGuiCol_HeaderActive] = ImVec4(0.26f, 0.59f, 0.98f, 1.00f);
	style.Colors[ImGuiCol_Column] = ImVec4(0.39f, 0.39f, 0.39f, 1.00f);
	style.Colors[ImGuiCol_ColumnHovered] = ImVec4(0.26f, 0.59f, 0.98f, 0.78f);
	style.Colors[ImGuiCol_ColumnActive] = ImVec4(0.26f, 0.59f, 0.98f, 1.00f);
	style.Colors[ImGuiCol_ResizeGrip] = ImVec4(1.00f, 1.00f, 1.00f, 0.50f);
	style.Colors[ImGuiCol_ResizeGripHovered] = ImVec4(0.26f, 0.59f, 0.98f, 0.67f);
	style.Colors[ImGuiCol_ResizeGripActive] = ImVec4(0.26f, 0.59f, 0.98f, 0.95f);
	style.Colors[ImGuiCol_CloseButton] = ImVec4(0.59f, 0.59f, 0.59f, 0.50f);
	style.Colors[ImGuiCol_CloseButtonHovered] = ImVec4(0.98f, 0.39f, 0.36f, 1.00f);
	style.Colors[ImGuiCol_CloseButtonActive] = ImVec4(0.98f, 0.39f, 0.36f, 1.00f);
	style.Colors[ImGuiCol_PlotLines] = ImVec4(0.39f, 0.39f, 0.39f, 1.00f);
	style.Colors[ImGuiCol_PlotLinesHovered] = ImVec4(1.00f, 0.43f, 0.35f, 1.00f);
	style.Colors[ImGuiCol_PlotHistogram] = ImVec4(0.90f, 0.70f, 0.00f, 1.00f);
	style.Colors[ImGuiCol_PlotHistogramHovered] = ImVec4(1.00f, 0.60f, 0.00f, 1.00f);
	style.Colors[ImGuiCol_TextSelectedBg] = ImVec4(0.26f, 0.59f, 0.98f, 0.35f);
	style.Colors[ImGuiCol_ModalWindowDarkening] = ImVec4(0.20f, 0.20f, 0.20f, 0.35f);
}

// This is where we draw stuff
void Update(void)
{
	// Set up scene
	TTK::Graphics::SetBackgroundColour(0.5f, 0.5f, 0.5f);
	TTK::Graphics::ClearScreen();
	TTK::Graphics::SetCameraMode3D(0, 0, windowWidth, windowHeight);
	TTK::Graphics::DrawGrid();
	camera.update();

	Head.update(deltaTime);
	Head.mesh->draw(Head.getLocalToWorldMatrix());

	Speaker.update(deltaTime);
	Speaker.mesh->draw(Speaker.getLocalToWorldMatrix());
	Speaker.setLocalRotationAngleY(180.0f);
	Speaker.setLocalPosition(glm::vec3(0.0f, 0.0f, -radius));
	
	float headX = Head.getLocalPosition().x;
	float headZ = Head.getLocalPosition().z;

	if (panEnabled) {
		// Move the speaker in a counter-clockwise circle
		static float t = 0.0f;
		float x = radius * cos(t);
		float z = radius * sin(-t);
		t += deltaTime;
		
		Speaker.setLocalPosition(glm::vec3(headX + x, 0.0f, headZ + z));
		Speaker.setLocalRotationAngleY(glm::degrees(t) + 90.0f);
	}

	if (distanceEnabled) {
		// Move speaker away from the head and then back again
		static float t = 0.0f;
		float z = sin(t) * 0.8 * radius;
		t += deltaTime;
		
		Speaker.setLocalPosition(glm::vec3(headX, 0.0f, headZ - radius + z));
	}

	if (rollOff) {
		soundEngine.result = soundEngine.sound->channel->set3DMinMaxDistance(2.0f, 6.0f);
		soundEngine.FmodErrorCheck(soundEngine.result);
		soundEngine.result = soundEngine.sound->channel->setMode(FMOD_3D_LINEARROLLOFF);
		soundEngine.FmodErrorCheck(soundEngine.result);
	}
	else {
		soundEngine.result = soundEngine.sound->channel->setMode(FMOD_3D_INVERSEROLLOFF);
		soundEngine.FmodErrorCheck(soundEngine.result);	
	}

	// set channel position
	soundPos.x = Speaker.getLocalPosition().x;
	soundPos.y = Speaker.getLocalPosition().y;
	soundPos.z = Speaker.getLocalPosition().z;

	soundEngine.result = soundEngine.sound->channel->set3DAttributes(&soundPos, &soundVel);
	soundEngine.FmodErrorCheck(soundEngine.result);

	
	// Draw UI
	// You must call this prior to using any imgui functions
	TTK::Graphics::StartUI(windowWidth, windowHeight);
	setUIStyle();

	// show the menu bar
	ShowMenu();

	ImGui::Begin("Inspector");
	// set window to top left corner
	ImGui::SetWindowPos(ImVec2(0, 20), true);

	ImGui::PushItemWidth(100.0f);
	ImGui::DragFloat("Radius: ", &radius, 0.5f, 1.5f, 20.0f);
	ImGui::PopItemWidth();
	ImGui::SameLine();

	if (ImGui::Button("Pan")) {
		panEnabled = !panEnabled;
		distanceEnabled = false;
	}

	ImGui::SameLine();

	if (ImGui::Button("Distance")) {
		distanceEnabled = !distanceEnabled;
		panEnabled = false;
	}

	ImGui::SameLine();

	if (ImGui::Button("Roll-Off")) {
		rollOff = !rollOff;
	}

	ImGui::Text("Hierarchy");
	ImGui::Separator();

	GameObject* headObject = drawHeadHierarchyUI(&Head);

	if (headObject != nullptr)
	{
		ImGui::Text("Properties:");
		ImGui::Separator();

		ImGui::Text("Game Object: %s ", headObject->name.c_str());
		
		ImGui::InputText("Name: ", &headObject->name[0], 255);
		ImGui::DragFloat3("Position: ", &headObject->position.x);
		ImGui::DragFloat3("Rotation: ", &headObject->rotationX);
		ImGui::DragFloat3("Scale: ", &headObject->localScaleX);

		ImGui::Separator();
		ImGui::Text("World Position: %s ", glm::to_string(headObject->getWorldPosition()).c_str());

		// set colour of head to green
		headObject->mesh->setAllColours(glm::vec4(0.0f, 1.0f, 0.0f, 1.0f));
	}

	GameObject* speakerObject = drawSpeakerHierarchyUI(&Speaker);

	if (speakerObject != nullptr)
	{
		ImGui::Text("Properties");
		ImGui::Separator();

		ImGui::Text("Game Object: %s", speakerObject->name.c_str());

		ImGui::InputText("Name", &speakerObject->name[0], 255);
		ImGui::DragFloat3("Position", &speakerObject->position.x);
		ImGui::DragFloat3("Rotation", &speakerObject->rotationX);
		ImGui::DragFloat3("Scale", &speakerObject->localScaleX);

		ImGui::Separator();
		ImGui::Text("World Position: %s", glm::to_string(speakerObject->getWorldPosition()).c_str());

		// set colour of head to red
		speakerObject->mesh->setAllColours(glm::vec4(1.0f, 0.0f, 0.0f, 1.0f));
	}

	// Change Head position
	soundEngine.setListenerPosition(Head.getLocalPosition());

	// You must call this once you are done doing UI stuff
	// This is what actually draws the ui on screen
	ImGui::End();
	TTK::Graphics::EndUI();

	// Swap buffers
	// This is how we tell the program to put the things we just drew on the screen
	glutSwapBuffers();

    soundEngine.Update();
}

/* function void KeyboardCallbackFunction(unsigned char, int,int)
* Description:
*   - this handles keyboard input when a button is pressed
*/
void KeyboardCallbackFunction(unsigned char key, int x, int y)
{
	ImGuiIO& io = ImGui::GetIO();
	io.KeysDown[(int)key] = true;
	io.AddInputCharacter((int)key); // this is what makes keyboard input work in imgui

	switch (key)
	{
	case 27: // the escape key
		soundEngine.sound->cleanUp();
		soundEngine.CleanUp();
		glutExit();
		break;
	case 'q': // the 'q' key
	case 'Q':
		camera.moveUp();
		break;
	case 'e':
	case 'E':
		camera.moveDown();
	case 'W':
	case 'w':
		camera.moveForward();
		break;
	case 'S':
	case 's':
		camera.moveBackward();
		break;
	case 'A':
	case 'a':
		camera.moveRight();
		break;
	case 'D':
	case 'd':
		camera.moveLeft();
		break;
	case 'P':
	case 'p':
		panEnabled = !panEnabled;
		break;
	case 'R':
	case 'r':
		rollOff = !rollOff;
		break;
	}

	// This is what makes the backspace button work
	int keyModifier = glutGetModifiers();
	switch (keyModifier)
	{
	case GLUT_ACTIVE_SHIFT:
		io.KeyShift = true;
		break;

	case GLUT_ACTIVE_CTRL:
		io.KeyCtrl = true;
		break;

	case GLUT_ACTIVE_ALT:
		io.KeyAlt = true;
		break;
	}
}

/* function void KeyboardUpCallbackFunction(unsigned char, int,int)
* Description:
*   - this handles keyboard input when a button is lifted
*/
void KeyboardUpCallbackFunction(unsigned char key, int x, int y)
{
	ImGuiIO& io = ImGui::GetIO();
	io.KeysDown[key] = false;

	switch (key)
	{
	default:
		break;
	}

	int keyModifier = glutGetModifiers();
	io.KeyShift = false;
	io.KeyCtrl = false;
	io.KeyAlt = false;
	switch (keyModifier)
	{
	case GLUT_ACTIVE_SHIFT:
		io.KeyShift = true;
		break;

	case GLUT_ACTIVE_CTRL:
		io.KeyCtrl = true;
		break;

	case GLUT_ACTIVE_ALT:
		io.KeyAlt = true;
		break;
	}
}

/* function TimerCallbackFunction(int value)
* Description:
*  - this is called many times per second
*  - this enables you to animate things
*  - no drawing, just changing the state
*  - changes the frame number and calls for a redisplay
*  - FRAME_DELAY is the number of milliseconds to wait before calling the timer again
*/
void TimerCallbackFunction(int value)
{
	// Calculate the amount of time since the last frame
	static int elapsedTimeAtLastTick = 0;
	int totalElapsedTime = glutGet(GLUT_ELAPSED_TIME);

	deltaTime = (float)totalElapsedTime - elapsedTimeAtLastTick;
	deltaTime /= 1000.0f;
	elapsedTimeAtLastTick = totalElapsedTime;

	// Re-trigger the display event
	glutPostRedisplay();

	/* this call gives it a proper frame delay to hit our target FPS */
	glutTimerFunc(FRAME_DELAY, TimerCallbackFunction, 0);
}

/* function WindowReshapeCallbackFunction()
* Description:
*  - this is called whenever the window is resized
*  - and sets up the projection matrix properly
*/
void WindowReshapeCallbackFunction(int w, int h)
{
	/* Update our Window Properties */
	windowWidth = w;
	windowHeight = h;
}

// This is called when a mouse button is clicked
void MouseClickCallbackFunction(int button, int state, int x, int y)
{
	ImGui::GetIO().MouseDown[0] = !state;

	mousePosition.x = x;
	mousePosition.y = y;

	mousePositionFlipped.x = x;
	mousePositionFlipped.y = windowHeight - y;
}

void SpecialInputCallbackFunction(int key, int x, int y)
{
	float movementSpeed = 5.0f; // how fast should the object move
	glm::vec3 currentHeadPosition = Head.getLocalPosition();

	switch (key)
	{
	case GLUT_KEY_UP:
		currentHeadPosition.z += movementSpeed * deltaTime;
		break;
	case GLUT_KEY_DOWN:
		currentHeadPosition.z -= movementSpeed * deltaTime;
		break;
	case GLUT_KEY_LEFT:
		currentHeadPosition.x += movementSpeed * deltaTime;
		break;
	case GLUT_KEY_RIGHT:
		currentHeadPosition.x -= movementSpeed * deltaTime;
		break;
	}

	Head.setLocalPosition(currentHeadPosition);
}

// Called when the mouse is clicked and moves
void MouseMotionCallbackFunction(int x, int y)
{
	if (mousePosition.length() > 0 && !ImGui::GetIO().WantCaptureMouse) // wantCaptureMouse is true if the mouse is within an imgui element
		camera.processMouseMotion(x, y, mousePosition.x, mousePosition.y, deltaTime);

	ImGui::GetIO().MousePos = ImVec2((float)x, (float)y);

	mousePosition.x = x;
	mousePosition.y = y;

	mousePositionFlipped.x = x;
	mousePositionFlipped.y = windowHeight - y;
}

// Called when the mouse is moved inside the window
void MousePassiveMotionCallbackFunction(int x, int y)
{
	ImGui::GetIO().MousePos = ImVec2((float)x, (float)y);

	mousePositionFlipped.x = x;
	mousePositionFlipped.y = windowHeight - y;
}

/* function main()
* Description:
*  - this is the main function
*  - does initialization and then calls glutMainLoop() to start the event handler
*/
int main(int argc, char **argv)
{
	/* initialize the window and OpenGL properly */

	// Request an OpenGL 4.4 compatibility
	// A compatibility context is needed to use the provided rendering utilities 
	// which are written in OpenGL 1.1
	glutInitContextVersion(4, 4); 
	glutInitContextProfile(GLUT_COMPATIBILITY_PROFILE);
	glutInit(&argc, argv);
	glutInitWindowSize(windowWidth, windowHeight);
	glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE);
	glutCreateWindow("Game Sound - Assignment 1");

	//Init GLEW
	GLenum err = glewInit();
	if (err != GLEW_OK)
	{
		std::cout << "TTK::InitializeTTK Error: GLEW failed to init" << std::endl;
	}
	printf("OpenGL version: %s, GLSL version: %s\n", glGetString(GL_VERSION), glGetString(GL_SHADING_LANGUAGE_VERSION));

	/* set up our function callbacks */
	glutDisplayFunc(Update);
	glutKeyboardFunc(KeyboardCallbackFunction);
	glutKeyboardUpFunc(KeyboardUpCallbackFunction);
	glutReshapeFunc(WindowReshapeCallbackFunction);
	glutMouseFunc(MouseClickCallbackFunction);
	glutMotionFunc(MouseMotionCallbackFunction);
	glutPassiveMotionFunc(MousePassiveMotionCallbackFunction);
	glutTimerFunc(1, TimerCallbackFunction, 0);
	glutSpecialFunc(SpecialInputCallbackFunction);

	// Init GL
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);

	/* Start Game Loop */
	deltaTime = (float)glutGet(GLUT_ELAPSED_TIME);
	deltaTime /= 1000.0f;

	InitializeScene();

	// Init IMGUI
	TTK::Graphics::InitImGUI();

	glutMainLoop();

	return 0;
}