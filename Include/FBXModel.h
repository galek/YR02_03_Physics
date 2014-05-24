#pragma once

#include "FBXFile.h"
#include "Shader.h"

#include <glm/ext.hpp>

struct OGL_FBXRenderData
{
	unsigned int VBO; // vertex buffer object
	unsigned int IBO; // index buffer object
	unsigned int VAO; // vertex array object
};

class FBXModel
{
public:
	FBXModel();
	FBXModel(const char* a_filename, FBXFile::UNIT_SCALE a_scale = FBXFile::UNITS_CENTIMETER, bool a_loadTextures = true, bool a_loadAnimations = true);
	~FBXModel(void);
	void Update();
	void Draw(Shader* a_Shader,glm::mat4 &a_View);
	bool HasAnimation;
	void Model(glm::mat4 NewModel);
	void Colour(glm::vec4 NewColour);
	glm::mat4 m_ModelMatrix;
private:
	FBXFile* m_fbx;
};

