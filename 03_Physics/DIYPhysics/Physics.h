#pragma once

#include "Application.h"
#include <glm/glm.hpp>
#include <vector>

class SpringJoint;
class SceneManager;

class Physics : public Application {
public:

	Physics();
	virtual ~Physics();

protected:

	virtual bool onCreate(int a_argc, char* a_argv[]);
	virtual void onUpdate(float a_deltaTime);
	virtual void onDraw();
	virtual void onDestroy();

	glm::mat4	m_cameraMatrix;
	glm::mat4	m_projectionMatrix;

	SceneManager *scene;
	std::vector<SpringJoint*> springs;
	bool pressed;

};

