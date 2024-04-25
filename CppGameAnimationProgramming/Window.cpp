#include "Window.h"
#include "Logger.h"

bool Window::init(unsigned int width, unsigned int height, std::string title) {
	if (!glfwInit()) {
		Logger::log(1, "%s: glfwInit() error\n", __FUNCTION__);
		return false;
	}

	if (!glfwVulkanSupported()) {
		glfwTerminate();
		Logger::log(1, "%s: Vulkan is not supported\n", __FUNCTION__);
		return false;
	}

	glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
	mApplicationName = title;
	mWindow = glfwCreateWindow(width, height, title.c_str(), nullptr, nullptr);
	if (!mWindow) {
		Logger::log(1, "%s: Could not create window\n", __FUNCTION__);
		glfwTerminate();
		return false;
	}

	// glfwMakeContextCurrent(mWindow); // Use OpenGL
	if (!initVulkan()) {
		Logger::log(1, "%s: Could not init Vulkan\n", __FUNCTION__);
		glfwTerminate();
		return false;
	}

	Logger::log(1, "%s: Window successfully initialized\n", __FUNCTION__);
	return true;
}

bool Window::initVulkan() {
	return true;
}

void Window::mainLoop() {
	glfwSwapInterval(1);
	float color = 0.0f;
	while (!glfwWindowShouldClose(mWindow)) {
		color >= 1.0f ? color = 0.0f : color += 0.01f;
		glClearColor(color, color, color, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);
		glfwSwapBuffers(mWindow);
		glfwPollEvents();
	}
}

void Window::cleanup() {
	Logger::log(1, "%s: Terminating Window\n", __FUNCTION__);
	glfwDestroyWindow(mWindow);
	glfwTerminate();
}