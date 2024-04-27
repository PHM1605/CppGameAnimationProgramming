class OGLRenderer {
public:
	bool init(unsigned int width, unsigned int height);
	void setSize(unsigned int width, unsigned int height);
	void cleanup();
	void uploadData(OGLMesh vertexData);
	void draw();
};