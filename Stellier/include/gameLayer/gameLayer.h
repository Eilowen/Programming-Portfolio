#pragma once
#include <glm/vec2.hpp>

bool initGame();
bool gameLogic(float deltaTime);
void closeGame();

namespace platform
{
	void setRelMousePosition(int x, int y);
	bool isFullScreen();
	void setFullScreen(bool f);
	glm::ivec2 getWindowSize();
	inline int getWindowSizeX() { return getWindowSize().x; }
	inline int getWindowSizeY() { return getWindowSize().y; }
	glm::ivec2 getFrameBufferSize();
	inline int getFrameBufferSizeX() { return getFrameBufferSize().x; }
	inline int getFrameBufferSizeY() { return getFrameBufferSize().y; }
	glm::ivec2 getRelMousePosition();
	void showMouse(bool show);
	bool isFocused();
	bool mouseMoved();
	bool writeEntireFile(const char* name, void* buffer, size_t size);
	bool readEntireFile(const char* name, void* buffer, size_t size);
};
