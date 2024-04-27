#include "OGLRenderer.h"

bool OGLRenderer::init(unsigned int width, unsigned int height) {
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
		return false;
	}
	if (!GLAD_GL_VERSION_4_6) {
		return false;
	}

	if (!mFrameBuffer.init(width, height)) {
		return false;
	}

	if (!mTex.loadTexture("textures/crate.png")) {
		return false;
	}

	mVertexBuffer.init();

	if (!mShader.loadShaders("shader/basic.vert", "shader/basic.frag")) {
		return false;
	}

	return true;
}