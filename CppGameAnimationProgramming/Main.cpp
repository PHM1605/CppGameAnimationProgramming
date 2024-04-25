#include <memory>
#include "Window.h"
#include "Logger.h"

int main(int argc, char* argv[]) {
	std::unique_ptr<Window> w = std::make_unique<Window>();
	w->init(640, 480, "Test Window");
	return 0;
}