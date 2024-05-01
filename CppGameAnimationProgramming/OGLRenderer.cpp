#include "OGLRenderer.h"

bool OGLRenderer::init(unsigned int width, unsigned int height) {
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
		Logger::log(1, "%s error: failed to initialize GLAD\n", __FUNCTION__);
		return false;
	}
	if (!GLAD_GL_VERSION_4_6) {
		Logger::log(1, "%s error: failed to get at least OpenGL 4.6\n", __FUNCTION__);
		return false;
	}

	if (!mFramebuffer.init(width, height)) {
		return false;
	}
	if (!mTex.loadTexture("crate.png")) {
		Logger::log(1, "%s: texture loading failed\n", __FUNCTION__);
		return false;
	}
	mVertexBuffer.init();
	if (!mShader.loadShaders("basic.vert", "basic.frag")) {
		return false;
	}

	return true;
}

void OGLRenderer::setSize(unsigned int width, unsigned int height) {
	mFramebuffer.resize(width, height);
	glViewport(0, 0, width, height);
}

void OGLRenderer::uploadData(OGLMesh vertexData) {
	mTriangleCount = vertexData.vertices.size();
	mVertexBuffer.uploadData(vertexData);
}

void OGLRenderer::draw() {
	mFramebuffer.bind();
	glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glEnable(GL_CULL_FACE);
	// Draw triangles
	mShader.use();
	mTex.bind();
	mVertexBuffer.bind();
	mVertexBuffer.draw(GL_TRIANGLES, 0, mTriangleCount);
	mVertexBuffer.unbind();
	mTex.unbind();
	mFramebuffer.unbind();
	mFramebuffer.drawToScreen();
}

void OGLRenderer::cleanup() {
	mShader.cleanup();
	mTex.cleanup();
	mVertexBuffer.cleanup();
	mFramebuffer.cleanup();
}