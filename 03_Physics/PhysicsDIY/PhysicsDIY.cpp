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
	m_cameraMatrix = glm::inverse( glm::lookAt(glm::vec3(10,10,10),glm::vec3(0,0,0), glm::vec3(0,1,0)) );
	m_projectionMatrix = glm::perspective(glm::pi<float>() * 0.25f, DEFAULT_SCREENWIDTH/(float)DEFAULT_SCREENHEIGHT, 0.1f, 1000.0f);
	glClearColor(0.25f,0.25f,0.25f,1);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);
	
	//!--TUTORIAL
	sScene = new Shader("shaders/Gizmo/Gizmo.vert","shaders/Gizmo/Gizmo.frag",0,0,"shaders/Gizmo/Gizmo.geom");
	sScene->SetAttribs(4,0,"Position",1,"Information",2,"Colour",3,"Bitset");
	
	//!--TUTORIAL
	vb = new Osiris::VertexBatch();
	vb->SetShader(sScene->id);
	for (int i = 0; i < CUBE_COUNT; i++) {
		vb->Add(new Osiris::Gizmo::Box(glm::vec3(rand()%100 - 50,0,rand()%100 - 50),glm::vec3(1),glm::vec4(1,0,0,1) + glm::vec4(rand()%10 / 10.0f)));
	}
	vb->Add(new Osiris::Gizmo::Plane(glm::vec3(0),glm::vec3(5),glm::vec4(0,1,0,1)));
	vb->Add(new Osiris::Gizmo::Point(glm::vec3(3),glm::vec4(0,0,1,1)));
	vb->Add(new Osiris::Gizmo::Sphere(glm::vec3(5),1.0f,12,12,glm::vec4(0,0,1,1)));
	vb->Update();
	//!--TUTORIAL
	return true;
}

void PhysicsDIY::onUpdate(float a_deltaTime) {
	Utility::freeMovement( m_cameraMatrix, a_deltaTime, 10 );

	//!--TUTORIAL
	static bool bf1 = false;
	if (glfwGetKey(m_window,GLFW_KEY_F1) == GLFW_PRESS) {
		if (!bf1){
			bf1 = true;
			printf("%i elements\n",vb->getCount());
			printf("%f\n",1/Utility::getDeltaTime());
		}
	}else{
		if (bf1){
			bf1 = false;
		}
	}

	if (glfwGetKey(m_window,GLFW_KEY_F2) == GLFW_PRESS) {
		for (int i = 0; i < CUBE_COUNT; i++) {
			vb->Add(new Osiris::Gizmo::Box(glm::vec3(rand()%100 - 50,rand()%100,rand()%100 - 50),glm::vec3(1),glm::vec4(1,0,0,1) + glm::vec4(rand()%10 / 10.0f)));
		}
	}
	
	if (glfwGetKey(m_window,GLFW_KEY_F3) == GLFW_PRESS) {
		vb->Update();
	}

	if (glfwGetKey(m_window,GLFW_KEY_F4) == GLFW_PRESS) {
		const Osiris::GizmoMap *map = vb->getMap();
		for (auto giz : *map){
			giz.second->move();
		}
	}

	//!--TUTORIAL
	
	if (glfwGetKey(m_window,GLFW_KEY_ESCAPE) == GLFW_PRESS) {
		quit();
	}
}

void PhysicsDIY::onDraw() {
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glm::mat4 viewMatrix = glm::inverse( m_cameraMatrix );
	
	//!--TUTORIAL
	sScene->SetUniform("ViewProjection","m4fv",1,false,glm::value_ptr(m_projectionMatrix * viewMatrix));
	vb->Draw();
	//!--TUTORIAL	
}

void PhysicsDIY::onDestroy() {
	//!--TUTORIAL
	delete sScene;
	delete vb;
	//!--TUTORIAL
	
	Gizmos::destroy();
}

int main(int argc, char* argv[]) {
	Application* app = new PhysicsDIY();
	
	if (app->create("AIE - PhysicsDIY",DEFAULT_SCREENWIDTH,DEFAULT_SCREENHEIGHT,argc,argv) == true) {
		app->run();
	}
	delete app;
	return 0;
}

