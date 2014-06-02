#include "PhysicsDIY.h"
#include "Gizmos.h"
#include "Utilities.h"
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/ext.hpp>

#include "Scene.hpp"
#include "Shape.hpp"
#include "Actor.hpp"

#define DEFAULT_SCREENWIDTH 1280
#define DEFAULT_SCREENHEIGHT 720
#define TIMESTEP 60.0f

PhysicsDIY::PhysicsDIY(){}
PhysicsDIY::~PhysicsDIY(){}

Osiris::Scene *scene;

void AddSphere(const glm::mat4& m4_Camera,float f_Power){

	Osiris::ActorDynamic *bulletActor = new Osiris::ActorDynamic();
	bulletActor->setTranslate(m4_Camera[3].xyz);
	bulletActor->setVelocity(glm::vec3(m4_Camera[2].x * -f_Power,m4_Camera[2].y * -f_Power,m4_Camera[2].z * -f_Power));
	bulletActor->setFriction(glm::vec3(0.5f));

	Osiris::Sphere *sphere = new Osiris::Sphere();
	sphere->setColour(glm::vec4(rand()%100 / 100.0f,rand()%100 / 100.0f,rand()%100 / 100.0f,1));
	sphere->setRadius(0.1f);
	sphere->setMass(100.0f);
	bulletActor->addShape(sphere);

	scene->addActor(bulletActor);
}

void AddBox(const glm::mat4& m4_Camera,float f_Power){

	Osiris::ActorDynamic *bulletActor = new Osiris::ActorDynamic();
	bulletActor->setTranslate(m4_Camera[3].xyz);
	bulletActor->setVelocity(glm::vec3(m4_Camera[2].x * -f_Power,m4_Camera[2].y * -f_Power,m4_Camera[2].z * -f_Power));
	bulletActor->setFriction(glm::vec3(0.5f));

	Osiris::AxisBox *box = new Osiris::AxisBox();
	box->setColour(glm::vec4(rand()%100 / 100.0f,rand()%100 / 100.0f,rand()%100 / 100.0f,1));
	box->setExtents(glm::vec3(0.9f));
	box->setMass(100.0f);
	bulletActor->addShape(box);

	scene->addActor(bulletActor);
}

bool PhysicsDIY::onCreate(int a_argc, char* a_argv[]) {
	Gizmos::create();
	m_cameraMatrix = glm::inverse( glm::lookAt(glm::vec3(10,10,10),glm::vec3(0,0,0), glm::vec3(0,1,0)) );
	m_projectionMatrix = glm::perspective(glm::pi<float>() * 0.25f, DEFAULT_SCREENWIDTH/(float)DEFAULT_SCREENHEIGHT, 0.1f, 1000.0f);
	glClearColor(0.25f,0.25f,0.25f,1);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);

	//!- ASSIGNMENT
	scene = new Osiris::Scene(TIMESTEP,glm::ivec3(10,10,10),glm::vec3(100));

	Osiris::ActorStatic *planeStatic = new Osiris::ActorStatic();
	Osiris::Plane *plane = new Osiris::Plane();
	plane->setFriction(glm::vec3(0.1f));
	plane->setUp(glm::vec3(0,1,0));
	plane->setColour(glm::vec4(0,0,1,0.1f));
	planeStatic->addShape(plane);
	scene->addActor(planeStatic);

	//scene->setGravity(0.0f);
	//for (int i = 0; i < 10; i++){
	//	Osiris::ActorDynamic *boxActor = new Osiris::ActorDynamic();
	//	boxActor->setTranslate(glm::vec3(rand()%100-50,rand()%10,rand()%100-50));
	//	boxActor->setFriction(glm::vec3(0.1f));
	//
	//	Osiris::AxisBox *box = new Osiris::AxisBox();
	//	box->setColour(glm::vec4(rand()%100 / 100.0f,rand()%100 / 100.0f,rand()%100 / 100.0f,1));
	//	box->setExtents(glm::vec3(0.9f));
	//	box->setMass(100.0f);
	//	boxActor->addShape(box);
	//
	//	scene->addActor(boxActor);
	//}

	//!- ASSIGNMENT

	return true;
}

void PhysicsDIY::onUpdate(float a_deltaTime) {
	Utility::freeMovement( m_cameraMatrix, a_deltaTime, 10 );
	Gizmos::clear();

	//!- ASSIGNMENT

	static bool bMouseButtons1 = false;
	if (glfwGetMouseButton(m_window,GLFW_MOUSE_BUTTON_1) == GLFW_PRESS ){ 
		if (bMouseButtons1 == false){
			bMouseButtons1 = true;
			AddBox(m_cameraMatrix,10);
		}
	}else if (bMouseButtons1 == true){
		bMouseButtons1 = false;
	}
	
	static bool bMouseButtons2 = false;
	if (glfwGetMouseButton(m_window,GLFW_MOUSE_BUTTON_3) == GLFW_PRESS ){
		if (bMouseButtons2 == false){
			bMouseButtons2 = true;
			AddSphere(m_cameraMatrix,10);
		}
	}else if (bMouseButtons2 == true){
		bMouseButtons2 = false;
	}

	static bool bKeyF1 = false;
	static bool bKeyF2 = false;
	static bool bUpdate = true;

	if (glfwGetKey(m_window,GLFW_KEY_F1) == GLFW_PRESS) {
		if (bKeyF1 == false){
			bKeyF1 = true;
			bUpdate = !bUpdate;
			printf("Updating is %s\n",(bUpdate)?"True":"False");
		}
	}else if (bKeyF1 == true){
		bKeyF1 = false;
	}
	
	if (glfwGetKey(m_window,GLFW_KEY_F2) == GLFW_PRESS) {
		if (bKeyF2 == false){
			bKeyF2 = true;
			scene->update(1/TIMESTEP);
		}
	}else if (bKeyF2 == true){
		bKeyF2 = false;
	}

	if (glfwGetKey(m_window,GLFW_KEY_F3) == GLFW_PRESS) {
		scene->clearActors();	
		Osiris::ActorStatic *planeStatic = new Osiris::ActorStatic();
		Osiris::Plane *plane = new Osiris::Plane();
		plane->setUp(glm::vec3(0,1,0));
		plane->setColour(glm::vec4(0,0,1,0.1f));
		planeStatic->addShape(plane);
		scene->addActor(planeStatic);
	}

	if (glfwGetKey(m_window,GLFW_KEY_F4) == GLFW_PRESS) {
		scene->clearGrid();
	}

	if (bUpdate){
		scene->update(a_deltaTime);
	}
	//!- ASSIGNMENT

	if (glfwGetKey(m_window,GLFW_KEY_ESCAPE) == GLFW_PRESS) {
		quit();
	}
}

void PhysicsDIY::onDraw() {
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glm::mat4 viewMatrix = glm::inverse( m_cameraMatrix );

	//!- ASSIGNMENT

	scene->draw();

	//!- ASSIGNMENT

	Gizmos::draw(viewMatrix,m_projectionMatrix);
}

void PhysicsDIY::onDestroy() {
	//!--ASSIGNMENT
	delete scene;
	//!--ASSIGNMENT
	Gizmos::destroy();
}

int main(int argc, char* argv[]) {
	Application* app = new PhysicsDIY();
	if (app->create("AIE - PhysicsDIY",DEFAULT_SCREENWIDTH,DEFAULT_SCREENHEIGHT,argc,argv) == true) {app->run();}
	delete app;
	return 0;
}

