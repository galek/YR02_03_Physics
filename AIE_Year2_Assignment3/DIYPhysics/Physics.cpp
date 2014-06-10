#include "Physics.h"
#include "Gizmos.h"
#include "Utilities.h"
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/ext.hpp>

#include "DIYPhysics.hpp"

#define DEFAULT_SCREENWIDTH 1280
#define DEFAULT_SCREENHEIGHT 720

Physics::Physics(){}
Physics::~Physics(){}

bool Physics::onCreate(int a_argc, char* a_argv[]) {
	Gizmos::create();
	m_cameraMatrix = glm::inverse( glm::lookAt(glm::vec3(10,10,10),glm::vec3(0,15,0), glm::vec3(0,1,0)) );
	m_projectionMatrix = glm::perspective(glm::pi<float>() * 0.25f, DEFAULT_SCREENWIDTH/(float)DEFAULT_SCREENHEIGHT, 0.1f, 1000.0f);
	glClearColor(0.25f,0.25f,0.25f,1);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);

	//! Assignment 3 - DIY Physics
	scene = new SceneManager();

	ActorStatic *actor = new ActorStatic(glm::vec3(0));
	actor->AddShape(new Plane(glm::vec3(0,1,0), glm::vec3(0), glm::vec4(0,1,0,1)));
	scene->AddActor(actor);

	int size = 10;
	Actor *other = nullptr;
	for (int i = 0; i < size; i++) {
		Actor *actor;

		float y = 15 * (1.0f - i/(float)size);
		if (i != 0) {
			actor = new ActorDynamic(glm::vec3(0,y,0), 10.0f, glm::vec3(0));
		}else{ 
			actor = new ActorStatic(glm::vec3(0,y,0));
		}

		actor->AddShape(new Sphere(0.5f, glm::vec3(0), glm::vec4(1,0,0,1)));
		scene->AddActor(actor);

		if (i % size) {
			springs.push_back(new SpringJoint(other, actor, 1.0f, 0.1f, 1.0f));
		}
		other = actor;
	}
	//! Assignment 3 - DIY Physics

	return true;
}

void Physics::onUpdate(float a_deltaTime) {
	Utility::freeMovement( m_cameraMatrix, a_deltaTime, 10 );
	Gizmos::clear();
	Gizmos::addTransform( glm::mat4(1,0,0,0,0,1,0,0,0,0,1,0,0,0,0,1) );

	//! Assignment 3 - DIY Physics
	if (glfwGetMouseButton(m_window,GLFW_MOUSE_BUTTON_1) == GLFW_PRESS) {
		if (pressed == false) {
			glm::vec3 Pos = m_cameraMatrix[3].xyz;
			glm::vec3 Dir = -m_cameraMatrix[2].xyz();
			ActorDynamic *actor = new ActorDynamic(Pos, 100.0f, Dir * 50.0f);
			actor->AddShape(new Sphere(2.0f, glm::vec3(0), glm::vec4(0,0,1,1)));
			scene->AddActor(actor);
			pressed = true;
		}
	} else {
		pressed = false;
	}

	scene->Update();
	for (auto spring : springs){
		spring->Update();
	}
	//! Assignment 3 - DIY Physics

	if (glfwGetKey(m_window,GLFW_KEY_ESCAPE) == GLFW_PRESS){ quit(); } 
}

void Physics::onDraw() {	
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glm::mat4 viewMatrix = glm::inverse( m_cameraMatrix );
	
	//! Assignment 3 - DIY Physics

	for (auto spring : springs){
		spring->Draw();
	}
	scene->Draw();

	//! Assignment 3 - DIY Physics

	Gizmos::draw(viewMatrix, m_projectionMatrix);
}

void Physics::onDestroy() {
	//! Assignment 3 - DIY Physics
	delete scene;
	//! Assignment 3 - DIY Physics
	Gizmos::destroy();
}

int main(int argc, char* argv[]) {
	Application* app = new Physics();
	if (app->create("AIE - Physics",DEFAULT_SCREENWIDTH,DEFAULT_SCREENHEIGHT,argc,argv) == true){app->run();}		
	delete app;
	return 0;
}