#ifndef __PhysicsDIY_H_
#define __PhysicsDIY_H_

#include "Application.h"
#include <glm/glm.hpp>
#include <Shader.h>
#include "VertexBatch.hpp"

// Derived application class that wraps up all globals neatly
class PhysicsDIY : public Application
{
public:

	PhysicsDIY();
	virtual ~PhysicsDIY();

protected:

	virtual bool onCreate(int a_argc, char* a_argv[]);
	virtual void onUpdate(float a_deltaTime);
	virtual void onDraw();
	virtual void onDestroy();

	glm::mat4	m_cameraMatrix;
	glm::mat4	m_projectionMatrix;
	
	//!--TUTORIAL

	Shader *sScene;
	Osiris::VertexBatch *vb;

	//!--TUTORIAL
};

#endif // __PhysicsDIY_H_