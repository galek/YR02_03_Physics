#include "PhysicsDIY.h"
#include "Gizmos.h"
#include "Utilities.h"
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/ext.hpp>

#define DEFAULT_SCREENWIDTH 1280
#define DEFAULT_SCREENHEIGHT 720
#define CUBE_COUNT 100

PhysicsDIY::PhysicsDIY(){}
PhysicsDIY::~PhysicsDIY(){}

bool PhysicsDIY::onCreate(int a_argc, char* a_argv[]) {
	Gizmos::create();
	m_cameraMatrix = glm::inverse( glm::lookAt(glm::vec3(10,10,10),glm::vec3(0,0,0), glm::vec3(0,1,0)) );
	m_projectionMatrix = glm::perspective(glm::pi<float>() * 0.25f, DEFAULT_SCREENWIDTH/(float)DEFAULT_SCREENHEIGHT, 0.1f, 1000.0f);
	glClearColor(0.25f,0.25f,0.25f,1);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);
	return true;
}

void PhysicsDIY::onUpdate(float a_deltaTime) {
	Utility::freeMovement( m_cameraMatrix, a_deltaTime, 10 );
	Gizmos::clear();
	//!--TUTORIAL
	
	Gizmos::addAABBFilled(glm::vec3(0),glm::vec3(1),glm::vec4(1));

	//!--TUTORIAL
	if (glfwGetKey(m_window,GLFW_KEY_ESCAPE) == GLFW_PRESS) {
		quit();
	}
}

void PhysicsDIY::onDraw() {
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glm::mat4 viewMatrix = glm::inverse( m_cameraMatrix );
	//!--TUTORIAL

	//!--TUTORIAL	
	Gizmos::draw(viewMatrix,m_projectionMatrix);
}

void PhysicsDIY::onDestroy() {
	//!--TUTORIAL
	Gizmos::destroy();
	//!--TUTORIAL
}

int main(int argc, char* argv[]) {
	Application* app = new PhysicsDIY();
	if (app->create("AIE - PhysicsDIY",DEFAULT_SCREENWIDTH,DEFAULT_SCREENHEIGHT,argc,argv) == true) {app->run();}
	delete app;
	return 0;
}

