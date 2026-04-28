#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <stb_image/stb_image.h>
#include <stb_truetype/stb_truetype.h>
#include "gl2d/gl2d.h"
#include <iostream>
#include <ctime>
#include "platformTools.h"
#include <raudio.h>
#include "platformInput.h"
#include "otherPlatformFunctions.h"
#include "gameLayer.h"
#include <fstream>
#include <chrono>

#define REMOVE_IMGUI 1

#ifdef _WIN32
#include <Windows.h>
#endif

#undef min
#undef max

bool currentFullScreen = 0;
bool fullScreen = 0;

void keyCallback(GLFWwindow *window, int key, int scancode, int action, int mods)
{
	if ((action == GLFW_REPEAT || action == GLFW_PRESS) && key == GLFW_KEY_BACKSPACE)
		platform::internal::addToTypedInput(8);

	bool state = 0;
	if(action == GLFW_PRESS) state = 1;
	else if(action == GLFW_RELEASE) state = 0;
	else return;

	if(key >= GLFW_KEY_A && key <= GLFW_KEY_Z)
		platform::internal::setButtonState(platform::Button::A + (key - GLFW_KEY_A), state);
	else if (key >= GLFW_KEY_0 && key <= GLFW_KEY_9)
		platform::internal::setButtonState(platform::Button::NR0 + (key - GLFW_KEY_0), state);
	else
	{
		if      (key == GLFW_KEY_SPACE)        platform::internal::setButtonState(platform::Button::Space,    state);
		else if (key == GLFW_KEY_ENTER)        platform::internal::setButtonState(platform::Button::Enter,    state);
		else if (key == GLFW_KEY_ESCAPE)       platform::internal::setButtonState(platform::Button::Escape,   state);
		else if (key == GLFW_KEY_UP)           platform::internal::setButtonState(platform::Button::Up,       state);
		else if (key == GLFW_KEY_DOWN)         platform::internal::setButtonState(platform::Button::Down,     state);
		else if (key == GLFW_KEY_LEFT)         platform::internal::setButtonState(platform::Button::Left,     state);
		else if (key == GLFW_KEY_RIGHT)        platform::internal::setButtonState(platform::Button::Right,    state);
		else if (key == GLFW_KEY_LEFT_CONTROL) platform::internal::setButtonState(platform::Button::LeftCtrl,state);
		else if (key == GLFW_KEY_TAB)          platform::internal::setButtonState(platform::Button::Tab,      state);
	}
}

void mouseCallback(GLFWwindow *window, int key, int action, int mods)
{
	bool state = (action == GLFW_PRESS) ? 1 : (action == GLFW_RELEASE) ? 0 : -1;
	if (state == -1) return;
	if(key == GLFW_MOUSE_BUTTON_LEFT)  platform::internal::setLeftMouseState(state);
	if(key == GLFW_MOUSE_BUTTON_RIGHT) platform::internal::setRightMouseState(state);
}

bool windowFocus = 1;
void windowFocusCallback(GLFWwindow *window, int focused)
{
	windowFocus = focused;
	if (!focused) platform::internal::resetInputsToZero();
}
void windowSizeCallback(GLFWwindow *window, int x, int y) { platform::internal::resetInputsToZero(); }

int mouseMovedFlag = 0;
void cursorPositionCallback(GLFWwindow *window, double xpos, double ypos) { mouseMovedFlag = 1; }
void characterCallback(GLFWwindow *window, unsigned int codepoint)
{
	if (codepoint < 127) platform::internal::addToTypedInput(codepoint);
}

GLFWwindow *wind = 0;

namespace platform
{
	void setRelMousePosition(int x, int y)    { glfwSetCursorPos(wind, x, y); }
	bool isFullScreen()                        { return fullScreen; }
	void setFullScreen(bool f)                 { fullScreen = f; }
	glm::ivec2 getFrameBufferSize()            { int x=0,y=0; glfwGetFramebufferSize(wind,&x,&y); return {x,y}; }
	glm::ivec2 getRelMousePosition()           { double x=0,y=0; glfwGetCursorPos(wind,&x,&y); return {(int)x,(int)y}; }
	glm::ivec2 getWindowSize()                 { int x=0,y=0; glfwGetWindowSize(wind,&x,&y); return {x,y}; }
	void showMouse(bool show)                  { glfwSetInputMode(wind, GLFW_CURSOR, show ? GLFW_CURSOR_NORMAL : GLFW_CURSOR_HIDDEN); }
	bool isFocused()                           { return windowFocus; }
	bool mouseMoved()                          { return mouseMovedFlag; }

	bool writeEntireFile(const char *name, void *buffer, size_t size)
	{
		std::ofstream f(name, std::ios::binary);
		if(!f.is_open()) return 0;
		f.write((char*)buffer, size);
		return 1;
	}
	bool readEntireFile(const char *name, void *buffer, size_t size)
	{
		std::ifstream f(name, std::ios::binary);
		if (!f.is_open()) return 0;
		f.read((char*)buffer, size);
		return 1;
	}
};

int main()
{
#ifdef _WIN32
#ifdef _MSC_VER
#if PRODUCTION_BUILD == 0
	AllocConsole();
	(void)freopen("conin$",  "r", stdin);
	(void)freopen("conout$", "w", stdout);
	(void)freopen("conout$", "w", stderr);
	std::cout.sync_with_stdio();
#endif
#endif
#endif

	permaAssertComment(glfwInit(), "err initializing glfw");
	glfwWindowHint(GLFW_SAMPLES, 4);  // 4x MSAA

#ifdef __APPLE__
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, 1);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
#endif

	// Start with a larger window
	int w = 1280;
	int h = 720;
	wind = glfwCreateWindow(w, h, "Space Shooter", nullptr, nullptr);
	glfwMakeContextCurrent(wind);
	glfwSwapInterval(1);

	glfwSetKeyCallback(wind, keyCallback);
	glfwSetMouseButtonCallback(wind, mouseCallback);
	glfwSetWindowFocusCallback(wind, windowFocusCallback);
	glfwSetWindowSizeCallback(wind, windowSizeCallback);
	glfwSetCursorPosCallback(wind, cursorPositionCallback);
	glfwSetCharCallback(wind, characterCallback);

	permaAssertComment(gladLoadGLLoader((GLADloadproc)glfwGetProcAddress), "err initializing glad");

	// Enable MSAA
	glEnable(GL_MULTISAMPLE);

	gl2d::init();

	InitAudioDevice();

	if (!initGame()) return 0;

	auto stop = std::chrono::high_resolution_clock::now();

	while (!glfwWindowShouldClose(wind))
	{
		auto start = std::chrono::high_resolution_clock::now();
		float deltaTime = (std::chrono::duration_cast<std::chrono::nanoseconds>(start - stop)).count() / 1000000000.0;
		stop = std::chrono::high_resolution_clock::now();
		float augmentedDeltaTime = deltaTime;
		if (augmentedDeltaTime > 1.f / 10) augmentedDeltaTime = 1.f / 10;

		if (!gameLogic(augmentedDeltaTime))
		{
			closeGame();
			return 0;
		}

		// Fullscreen toggle
		if (platform::isFocused() && currentFullScreen != fullScreen)
		{
			static int lastW = w, lastH = h, lastPosX = 0, lastPosY = 0;
			if (fullScreen)
			{
				lastW = w; lastH = h;
				glfwGetWindowPos(wind, &lastPosX, &lastPosY);
				auto monitor = getCurrentMonitor(wind);
				const GLFWvidmode* mode = glfwGetVideoMode(monitor);
				glfwSetWindowMonitor(wind, monitor, 0, 0, mode->width, mode->height, mode->refreshRate);
				currentFullScreen = 1;
			}
			else
			{
				glfwSetWindowMonitor(wind, nullptr, lastPosX, lastPosY, lastW, lastH, 0);
				currentFullScreen = 0;
			}
		}

		mouseMovedFlag = 0;
		platform::internal::updateAllButtons(deltaTime);
		platform::internal::resetTypedInput();

		glfwSwapBuffers(wind);
		glfwPollEvents();
	}

	closeGame();
}
