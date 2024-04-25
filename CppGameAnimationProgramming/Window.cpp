#include "Window.h"
#include "Logger.h"

bool Window::init(unsigned int width, unsigned int height, std::string title) {
	if (!glfwInit()) {
		return false;
	}
}