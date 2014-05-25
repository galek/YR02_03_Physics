#ifndef __PhysX_H_
#define __PhysX_H_

#include "Application.h"
#include <glm/glm.hpp>
#include "PhysXScene.hpp"

// Derived application class that wraps up all globals neatly
class PhysX : public Application {
public:

	PhysX();
	virtual ~PhysX();

protected:

	virtual bool onCreate(int a_argc, char* a_argv[]);
	virtual void onUpdate(float a_deltaTime);
	virtual void onDraw();
	virtual void onDestroy();

	glm::mat4	m_cameraMatrix;
	glm::mat4	m_projectionMatrix;
	PhysXScene	*m_Scene;

};

#endif // __PhysX_H_