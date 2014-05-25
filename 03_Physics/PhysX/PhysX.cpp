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

	// Player
	{
		m_Scene->AddCapsule("Player",physx::PxActorType::Enum::eRIGID_DYNAMIC,10,1,2,glm::vec3(0,4,0));
	}

	// Outer Walls
	{
		m_Scene->AddPlane("floor",physx::PxActorType::Enum::eRIGID_STATIC,0,glm::vec3(0,0,0),physx::PxQuat(glm::half_pi<float>(),physx::PxVec3(0,0,1)));
		m_Scene->AddPlane("roof",physx::PxActorType::Enum::eRIGID_STATIC,0,glm::vec3(0,100,0),physx::PxQuat(-glm::half_pi<float>(),physx::PxVec3(0,0,1)));

		m_Scene->AddBox("xPos",physx::PxActorType::Enum::eRIGID_STATIC,0,glm::vec3(1,50,100),glm::vec3( 100,50,0));
		m_Scene->AddBox("xNeg",physx::PxActorType::Enum::eRIGID_STATIC,0,glm::vec3(1,50,100),glm::vec3(-100,50,0));
		m_Scene->AddBox("zPos",physx::PxActorType::Enum::eRIGID_STATIC,0,glm::vec3(100,50,1),glm::vec3(0,50, 100));
		m_Scene->AddBox("zNeg",physx::PxActorType::Enum::eRIGID_STATIC,0,glm::vec3(100,50,1),glm::vec3(0,50,-100));
	}

	// Parkour Ground
	{
		float wx = 50;
		float wz = -50;
		m_Scene->AddBox("parkour1",physx::PxActorType::Enum::eRIGID_STATIC,0,glm::vec3( 5,1,5),glm::vec3(wx + 10.8,6.7,wz + 10));
		m_Scene->AddBox("parkour2",physx::PxActorType::Enum::eRIGID_STATIC,0,glm::vec3(10,1,3),glm::vec3(wx + -3,3,wz + 10),physx::PxQuat(glm::half_pi<float>() / 4,physx::PxVec3(0,0,1)));
	}

	// Linked Actors
	{
		float wx = -50;
		float wz = -50;
		std::string sBoxMesh = "linkedBoxes[~][`]";

		// Our two handles
		m_Scene->AddBox("linkedBoxesTopPositive",physx::PxActorType::Enum::eRIGID_STATIC,0,glm::vec3(0.5f),glm::vec3(wx + 5,25,wz));
		m_Scene->AddBox("linkedBoxesTopNegative",physx::PxActorType::Enum::eRIGID_STATIC,0,glm::vec3(0.5f),glm::vec3(wx - 5,25,wz));

		char buffer[16];
		for (int y = 0; y < 10; y++){
			for (int x = -5; x < 5; x++){
				std::string str = sBoxMesh;

				sprintf(buffer,"%i",y);
				str.replace(str.find("~"),1,buffer);

				sprintf(buffer,"%i",x);
				str.replace(str.find("`"),1,buffer);

				m_Scene->AddBox((char*)str.c_str(),physx::PxActorType::Enum::eRIGID_DYNAMIC,100,glm::vec3(0.5f),glm::vec3(wx + x,y + 0.5f,wz));
			}
		}

		// Does nothing smart, simple gets actors around it, link them
		// LinkDistance wont link same, or non existing actors.
		// Multiple linking is present(if it even exists in PhysX)
		for (int y = 0; y < 10; y++){
			for (int x = -5; x < 5; x++){
				std::string actorme = sBoxMesh;
				sprintf(buffer,"%i",y);actorme.replace(actorme.find("~"),1,buffer);
				sprintf(buffer,"%i",x);actorme.replace(actorme.find("`"),1,buffer);
	
				std::string actorOther = sBoxMesh;
				sprintf(buffer,"%i",y+1);actorOther.replace(actorOther.find("~"),1,buffer);
				sprintf(buffer,"%i",x  );actorOther.replace(actorOther.find("`"),1,buffer);
				m_Scene->linkDistance(m_Scene->getActor((char*)actorme.c_str()),m_Scene->getActor((char*)actorOther.c_str()),0.1f,1.0f,-0.5f);

				actorOther = sBoxMesh;
				sprintf(buffer,"%i",y-1);actorOther.replace(actorOther.find("~"),1,buffer);
				sprintf(buffer,"%i",x  );actorOther.replace(actorOther.find("`"),1,buffer);
				m_Scene->linkDistance(m_Scene->getActor((char*)actorme.c_str()),m_Scene->getActor((char*)actorOther.c_str()),0.1f,1.0f,-0.5f);

				actorOther = sBoxMesh;
				sprintf(buffer,"%i",y  );actorOther.replace(actorOther.find("~"),1,buffer);
				sprintf(buffer,"%i",x+1);actorOther.replace(actorOther.find("`"),1,buffer);
				m_Scene->linkDistance(m_Scene->getActor((char*)actorme.c_str()),m_Scene->getActor((char*)actorOther.c_str()),0.1f,1.0f,-0.5f);

				actorOther = sBoxMesh;
				sprintf(buffer,"%i",y  );actorOther.replace(actorOther.find("~"),1,buffer);
				sprintf(buffer,"%i",x-1);actorOther.replace(actorOther.find("`"),1,buffer);
				m_Scene->linkDistance(m_Scene->getActor((char*)actorme.c_str()),m_Scene->getActor((char*)actorOther.c_str()),0.1f,1.0f,-0.5f);

			}
		}

		// Link to our handles
		m_Scene->linkDistance(m_Scene->getActor("linkedBoxesTopPositive"),m_Scene->getActor("linkedBoxes[9][4]"),1.0f,1.0f,-10.0f);
		m_Scene->linkDistance(m_Scene->getActor("linkedBoxesTopNegative"),m_Scene->getActor("linkedBoxes[9][-5]"),1.0f,1.0f,-10.0f);
	}

	return true;
}

void PhysX::onUpdate(float a_deltaTime) {
	Utility::freeMovement( m_cameraMatrix, a_deltaTime, 50 );
	Gizmos::clear();
	Gizmos::addTransform( glm::mat4(1,0,0,0,0,1,0,0,0,0,1,0,0,0,0,1) );
	
	//!- TUTORIAL
	static bool bLeftPressed = false;
	if (glfwGetMouseButton(m_window,GLFW_MOUSE_BUTTON_1) == GLFW_PRESS) {
		if (!bLeftPressed){
			bLeftPressed = true;
			std::string bullet = "bullet";
			char buffer[5];
			sprintf(buffer,"%i",rand()%100);
			bullet.append(buffer);
			m_Scene->AddSphere((char*)bullet.c_str(),physx::PxActorType::Enum::eRIGID_DYNAMIC,10000,0.5f,m_cameraMatrix[3].xyz,physx::PxQuat(),m_cameraMatrix[2].xyz,100.0f);
		}
	}else{
		bLeftPressed = false;
	}

	m_Scene->controlActor(a_deltaTime,m_cameraMatrix,m_Scene->getActor("Player"),100);

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

