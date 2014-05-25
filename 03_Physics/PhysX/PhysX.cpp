#include "PhysX.h"
#include "Gizmos.h"
#include "Utilities.h"
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/ext.hpp>

#define DEFAULT_SCREENWIDTH 1280
#define DEFAULT_SCREENHEIGHT 720

PhysX::PhysX(){}
PhysX::~PhysX(){}

bool PhysX::onCreate(int a_argc, char* a_argv[]) {
	Gizmos::create(SHRT_MAX * 2,SHRT_MAX * 2);
	m_cameraMatrix = glm::inverse( glm::lookAt(glm::vec3(10,10,10),glm::vec3(0,0,0), glm::vec3(0,1,0)) );
	m_projectionMatrix = glm::perspective(glm::pi<float>() * 0.25f, DEFAULT_SCREENWIDTH/(float)DEFAULT_SCREENHEIGHT, 0.1f, 1000.0f);
	glClearColor(0.25f,0.25f,0.25f,1);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);
	
	m_Scene = new PhysXScene();
	m_Scene->AddCapsule("Player",physx::PxActorType::Enum::eRIGID_DYNAMIC,10,1,2,glm::vec3(0,4,0));
	//m_Scene->AddPlane("floor",physx::PxActorType::Enum::eRIGID_STATIC,100,glm::vec3(0,0,0),physx::PxQuat(glm::half_pi<float>(),physx::PxVec3(0,0,1)));

	m_Scene->AddBox("box",physx::PxActorType::Enum::eRIGID_STATIC,100,glm::vec3(100,0.1f,100),glm::vec3(0,0,0));

	m_Scene->AddBox("box",physx::PxActorType::Enum::eRIGID_STATIC,100,glm::vec3(1,10,100),glm::vec3( 100,5,0));
	m_Scene->AddBox("box",physx::PxActorType::Enum::eRIGID_STATIC,100,glm::vec3(1,10,100),glm::vec3(-100,5,0));
	m_Scene->AddBox("box",physx::PxActorType::Enum::eRIGID_STATIC,100,glm::vec3(100,10,1),glm::vec3(0,5, 100));
	m_Scene->AddBox("box",physx::PxActorType::Enum::eRIGID_STATIC,100,glm::vec3(100,10,1),glm::vec3(0,5,-100));

	m_Scene->AddBox("box",physx::PxActorType::Enum::eRIGID_STATIC,100,glm::vec3(5,1,5),glm::vec3(10,5,10));
	m_Scene->AddBox("box",physx::PxActorType::Enum::eRIGID_STATIC,100,glm::vec3(5,1,5),glm::vec3( 1.5,1,10),physx::PxQuat(glm::half_pi<float>() / 4,physx::PxVec3(0,0,1)));

	//for (int y = 0; y < 10; y++){
	//	for (int x = -5; x < 5; x++){
	//		for (int z = -5; z < 5; z++){
	//			m_Scene->AddBox("box",physx::PxActorType::Enum::eRIGID_DYNAMIC,100,glm::vec3(0.5f),glm::vec3(-50+x,y + 0.5f,-50+z));
	//		}
	//	}
	//}

	return true;
}

void PhysX::onUpdate(float a_deltaTime) {
	Utility::freeMovement( m_cameraMatrix, a_deltaTime, 10 );
	Gizmos::clear();
	Gizmos::addTransform( glm::mat4(1,0,0,0,0,1,0,0,0,0,1,0,0,0,0,1) );
	for ( int i = 0 ; i < 21 ; ++i ){
		Gizmos::addLine( glm::vec3(-10 + i, 0, 10), glm::vec3(-10 + i, 0, -10),  i == 10 ? glm::vec4(1,1,1,1) : glm::vec4(0,0,0,1) );
		Gizmos::addLine( glm::vec3(10, 0, -10 + i), glm::vec3(-10, 0, -10 + i),  i == 10 ? glm::vec4(1,1,1,1) : glm::vec4(0,0,0,1) );
	}
	
	//!- TUTORIAL
	static bool bLeftPressed = false;
	if (glfwGetMouseButton(m_window,GLFW_MOUSE_BUTTON_1) == GLFW_PRESS) {
		if (!bLeftPressed){
			bLeftPressed = true;
			m_Scene->AddSphere("bullet",physx::PxActorType::Enum::eRIGID_DYNAMIC,10000,0.5f,m_cameraMatrix[3].xyz,physx::PxQuat(),m_cameraMatrix[2].xyz,100.0f);
		}
	}else{
		bLeftPressed = false;
	}

	m_Scene->controlActor(a_deltaTime,m_Scene->getActor("Player"),50);

	m_Scene->update();
	//!- TUTORIAL
	
	if (glfwGetKey(m_window,GLFW_KEY_ESCAPE) == GLFW_PRESS) {
		quit();
	}
}

void PhysX::onDraw() {
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glm::mat4 viewMatrix = glm::inverse( m_cameraMatrix );
		
	m_Scene->draw();

	Gizmos::draw(viewMatrix, m_projectionMatrix);
}

void PhysX::onDestroy(){
	//!- TUTORIAL
	delete m_Scene;
	//!- TUTORIAL
	Gizmos::destroy();
}

int main(int argc, char* argv[]) {
	Application* app = new PhysX();
	if (app->create("AIE - PhysX",DEFAULT_SCREENWIDTH,DEFAULT_SCREENHEIGHT,argc,argv) == true){
		app->run();
	}
	delete app;
	return 0;
}

