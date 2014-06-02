#include "PhysX.h"
#include "Gizmos.h"
#include "Utilities.h"
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/ext.hpp>

#define DEFAULT_SCREENWIDTH 1280
#define DEFAULT_SCREENHEIGHT 720
#define TIMESTEP 60.0f

RagdollNode* HumanRagdoll[21] = {
	new RagdollNode(physx::PxQuat(physx::PxPi/2.0f,Z_AXIS)		,-1, 0.5f, 3.0f	, 1.0f , 1.0f, "lowerSpine"),
	new RagdollNode(physx::PxQuat(physx::PxPi,Z_AXIS)			, 0, 0.5f, 1.0f	,-1.0f , 1.0f, "leftPelvis"),
	new RagdollNode(physx::PxQuat(0,Z_AXIS)						, 0, 0.5f, 1.0f	,-1.0f , 1.0f, "rightPelvis"),
	new RagdollNode(physx::PxQuat(physx::PxPi/2.0f+0.2f,Z_AXIS)	, 1, 2.5f, 2.0f	,-1.0f , 1.0f, "leftUpperLeg"),
	new RagdollNode(physx::PxQuat(physx::PxPi/2.0f-0.2f,Z_AXIS)	, 2, 2.5f, 2.0f ,-1.0f , 1.0f, "right upperLeg"),
	new RagdollNode(physx::PxQuat(physx::PxPi/2.0f+0.2f,Z_AXIS)	, 3, 2.5f, 1.75f,-1.0f , 1.0f, "leftLowerLeg"),
	new RagdollNode(physx::PxQuat(physx::PxPi/2.0f-0.2f,Z_AXIS)	, 4, 2.5f, 1.75f,-1.0f , 1.0f, "rightLowerLeg"),
	new RagdollNode(physx::PxQuat(physx::PxPi/2.0f,Y_AXIS)		, 5, 0.7f, 1.5f	,-1.0f , 1.0f, "leftFoot"),
	new RagdollNode(physx::PxQuat(physx::PxPi/2.0f,Y_AXIS)		, 6, 0.7f, 1.5f	,-1.0f , 1.0f, "rightFoot"),
	new RagdollNode(physx::PxQuat(physx::PxPi/2.0f,Z_AXIS)		, 0, 0.5f, 3.0f , 1.0f ,-1.0f, "upperSpine"),
	new RagdollNode(physx::PxQuat(physx::PxPi,Z_AXIS)			, 9, 0.5f, 1.5f , 1.0f , 1.0f, "leftClavicle"),
	new RagdollNode(physx::PxQuat(0,Z_AXIS)						, 9, 0.5f, 1.5f , 1.0f , 1.0f, "rightClavicle"),
	new RagdollNode(physx::PxQuat(physx::PxPi/2.0f,Z_AXIS)		, 9, 0.5f, 1.0f , 1.0f ,-1.0f, "neck"),
	new RagdollNode(physx::PxQuat(physx::PxPi/2.0f,Z_AXIS)		,12, 0.5f, 3.0f , 1.0f ,-1.0f, "head"),
	new RagdollNode(physx::PxQuat(physx::PxPi-.3,Z_AXIS)		,10, 1.5f, 1.5f ,-1.0f , 1.0f, "leftUpperArm"),
	new RagdollNode(physx::PxQuat(0.3,Z_AXIS)					,11, 1.5f, 1.5f ,-1.0f , 1.0f, "rightUpperArm"),
	new RagdollNode(physx::PxQuat(physx::PxPi-.3,Z_AXIS)		,14, 1.5f, 1.0f ,-1.0f , 1.0f, "leftLowerArm"),
	new RagdollNode(physx::PxQuat(0.3,Z_AXIS)					,15, 1.5f, 1.0f ,-1.0f , 1.0f, "rightLowerArm"),
	new RagdollNode(physx::PxQuat(physx::PxPi-.3,Z_AXIS)		,16, 0.5f, 1.5f ,-1.0f , 1.0f, "leftHand"),
	new RagdollNode(physx::PxQuat(0.3,Z_AXIS)					,17, 0.5f, 1.5f ,-1.0f , 1.0f, "rightHand"),
	NULL
};

PhysX::PhysX(){}
PhysX::~PhysX(){}

bool PhysX::onCreate(int a_argc, char* a_argv[]) {

	Gizmos::create(SHRT_MAX * 2,SHRT_MAX * 2);
	m_cameraMatrix = glm::inverse( glm::lookAt(glm::vec3(0,10,0),glm::vec3(-50,10,-50), glm::vec3(0,1,0)) );
	m_projectionMatrix = glm::perspective(glm::pi<float>() * 0.25f, DEFAULT_SCREENWIDTH/(float)DEFAULT_SCREENHEIGHT, 0.1f, 1000.0f);
	glClearColor(0.25f,0.25f,0.25f,1);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);
	
	m_Scene = new PhysXScene(TIMESTEP,8);

	// Outer Walls
	{
		m_Scene->AddPlane("floor",physx::PxActorType::Enum::eRIGID_STATIC,0,glm::vec3(0,0,0),physx::PxQuat(glm::half_pi<float>(),physx::PxVec3(0,0,1)));
		m_Scene->AddPlane("roof",physx::PxActorType::Enum::eRIGID_STATIC,0,glm::vec3(0,100,0),physx::PxQuat(-glm::half_pi<float>(),physx::PxVec3(0,0,1)));

		m_Scene->AddBox("xPos",physx::PxActorType::Enum::eRIGID_STATIC,0,glm::vec3(1,50,100),glm::vec3( 100,50,0));
		m_Scene->AddBox("xNeg",physx::PxActorType::Enum::eRIGID_STATIC,0,glm::vec3(1,50,100),glm::vec3(-100,50,0));
		m_Scene->AddBox("zPos",physx::PxActorType::Enum::eRIGID_STATIC,0,glm::vec3(100,50,1),glm::vec3(0,50, 100));
		m_Scene->AddBox("zNeg",physx::PxActorType::Enum::eRIGID_STATIC,0,glm::vec3(100,50,1),glm::vec3(0,50,-100));
	}

	// Ragdoll
	{
		m_Scene->AddRagdoll("Ragdoll1",HumanRagdoll,physx::PxTransform(physx::PxVec3( 6.0f ,3.0f, 0.0f)),0.1f);
	}

	// Playground
	{
		m_Scene->AddCapsule("PlayerBody",physx::PxActorType::Enum::eRIGID_DYNAMIC,10,1,2,glm::vec3(50,4,-50));
		m_Scene->AddBox("Playground1",physx::PxActorType::Enum::eRIGID_STATIC,0,glm::vec3( 5,1,5),glm::vec3(60.8f,6.7,-60));
		m_Scene->AddBox("Playground2",physx::PxActorType::Enum::eRIGID_STATIC,0,glm::vec3(10,1,3),glm::vec3(47,3,-60),physx::PxQuat(glm::half_pi<float>() / 4,physx::PxVec3(0,0,1)));
	}

	// Water
	{
		float wx = -50;
		float wz = 50;

		ACTOR a = m_Scene->AddBox("WaterPad1",physx::PxActorType::Enum::eRIGID_STATIC,0,glm::vec3(2,0.5,2),glm::vec3(wx,0.25,wz));
		m_Scene->createTrigger(a);

		m_Scene->AddBox("WaterWall1",physx::PxActorType::Enum::eRIGID_STATIC,0,glm::vec3(10,4, 1),glm::vec3(wx     ,2,wz + 10));
		m_Scene->AddBox("WaterWall2",physx::PxActorType::Enum::eRIGID_STATIC,0,glm::vec3(10,4, 1),glm::vec3(wx     ,2,wz - 10));
		m_Scene->AddBox("WaterWall3",physx::PxActorType::Enum::eRIGID_STATIC,0,glm::vec3( 1,4,10),glm::vec3(wx + 10,2,wz     ));
		m_Scene->AddBox("WaterWall4",physx::PxActorType::Enum::eRIGID_STATIC,0,glm::vec3( 1,4,10),glm::vec3(wx - 10,2,wz     ));

		PxParticleFluid* pf;
		// create particle system in PhysX SDK
		// set immutable properties.
		PxU32 maxParticles = 5000;
		bool perParticleRestOffset = false;
		pf = m_Scene->physics()->createParticleFluid(maxParticles, perParticleRestOffset);
		pf->setRestParticleDistance(1.0f);
		pf->setDynamicFriction(0.1f);
		pf->setStaticFriction(0.1f);
		pf->setContactOffset(2.0f);
		pf->setDamping(0.1f);
		pf->setParticleMass(1.0f);
		pf->setRestitution(0.0f);
		pf->setParticleReadDataFlag(PxParticleReadDataFlag::eDENSITY_BUFFER,true);
		pf->setParticleBaseFlag(PxParticleBaseFlag::eCOLLISION_TWOWAY,true);
		if (pf) {
			m_Scene->scene()->addActor(*pf);
			particleFluidEmitter = new ParticleFluidEmitter(maxParticles,PxVec3(wx,2,wz),pf,0.001f);
		}

		maxParticles = 4000;
		perParticleRestOffset = false;
		PxParticleSystem* ps;
		// create particle system in PhysX SDK
		ps = m_Scene->physics()->createParticleSystem(maxParticles, perParticleRestOffset);
		//various flags we can set/clear to get different effects
		//  ps->setParticleBaseFlag(PxParticleBaseFlag::eCOLLISION_TWOWAY,true);
		//	ps->setDamping(.01);
		//	ps->setRestitution(2);
			ps->setActorFlag(PxActorFlag::eDISABLE_GRAVITY,true);
		// add particle system to scene, if creation was successful
		if (ps)
		{
			m_Scene->scene()->addActor(*ps);
			//create a particle emiter to make particles for our scene
			particleEmitter = new ParticleEmitter(maxParticles,PxVec3(0,50,0),ps,.002f);
		}

	}

	// Shooting range
	{
		float wx = 50;
		float wz = 50;

		glm::vec3 size		(1.0f,0.25f,2.0f);
		glm::vec3 extent	(2.0f,2.0f ,5.0f);
		float stride = 2;
		for (float x = size.x * 0.5f; x < extent.x; x += size.x){
			for (float z = size.z * 0.5f; z < extent.z; z += size.z){
				for (float y = size.y * 0.5f; y < extent.y; y += size.y){
					std::string shooting = "shooting";
					char buffer[32];
					sprintf(buffer,"[%i]",(int)(x * 100));shooting.append(buffer);
					sprintf(buffer,"[%i]",(int)(y * 100));shooting.append(buffer);
					sprintf(buffer,"[%i]",(int)(z * 100));shooting.append(buffer);
					m_Scene->AddBox((char*)shooting.c_str(),physx::PxActorType::Enum::eRIGID_DYNAMIC,100,glm::vec3(size * 0.5f),glm::vec3(wx + x, y, wz + (extent.z * 0.5f) - z));
				}
			}
		}
		for (float z = size.z * 0.5f; z < extent.z; z += size.z * stride){
			std::string shooting = "shooting_stride";
			char buffer[32];
			sprintf(buffer,"[%i]",(int)(z * 100));shooting.append(buffer);
			m_Scene->AddBox((char*)shooting.c_str(),physx::PxActorType::Enum::eRIGID_DYNAMIC,10,glm::vec3(size.yzx * 0.5f),glm::vec3(wx + 0.4f, extent.y * (size.y * 2) + (size.z * 1.75f),wz +  (extent.z * 0.5f) - z));
		}
	}

	// Linked Actors
	{
		float wx = -50;
		float wz = -50;
		std::string sBoxMesh = "linkedBoxes[~][`]";
		glm::vec3 size(1.3f,1.3f,0.1f);
		glm::ivec2 stride(5,3);
		float height = 8;
		m_Scene->AddBox("Playground3",physx::PxActorType::Enum::eRIGID_STATIC,0,glm::vec3(10,1,6),glm::vec3(wx + 28,3,wz),physx::PxQuat(-glm::half_pi<float>() / 4,physx::PxVec3(0,0,1)));
		m_Scene->AddBox("Playground4",physx::PxActorType::Enum::eRIGID_STATIC,0,glm::vec3(1,40,10),glm::vec3(wx - 18,20,wz));
		// Our handles
		m_Scene->AddBox("linkedBoxesTopPP",physx::PxActorType::Enum::eRIGID_STATIC,0,glm::vec3(0.5f) ,glm::vec3(wx + (stride.x * 2.8),height,wz + (stride.y * 2.8)));
		m_Scene->AddBox("linkedBoxesTopPN",physx::PxActorType::Enum::eRIGID_STATIC,0,glm::vec3(0.5f) ,glm::vec3(wx + (stride.x * 2.8),height,wz - (stride.y * 2.8)));
		m_Scene->AddBox("linkedBoxesTopNP",physx::PxActorType::Enum::eRIGID_STATIC,0,glm::vec3(0.5f) ,glm::vec3(wx - (stride.x * 2.8),height,wz + (stride.y * 2.8)));
		m_Scene->AddBox("linkedBoxesTopNN",physx::PxActorType::Enum::eRIGID_STATIC,0,glm::vec3(0.5f) ,glm::vec3(wx - (stride.x * 2.8),height,wz - (stride.y * 2.8)));

		char buffer[16];
		for (int y = -stride.x; y <= stride.x; y++){
			for (int x = -stride.y; x <= stride.y; x++){
				std::string str = sBoxMesh;
				sprintf(buffer,"%i",y);str.replace(str.find("~"),1,buffer);
				sprintf(buffer,"%i",x);str.replace(str.find("`"),1,buffer);
				m_Scene->AddBox((char*)str.c_str(),physx::PxActorType::Enum::eRIGID_DYNAMIC,100,size,glm::vec3(wx + (y * 2),height,wz + (x * 2)));
			}
		}

		// Does nothing smart, simple gets actors around it, link them
		// LinkDistance wont link same, or non existing actors.
		// Multiple linking is present(if it even exists in PhysX)
		for (int y = -stride.x; y <= stride.x; y++){
			for (int x = -stride.y; x <= stride.y; x++){
				std::string actorme = sBoxMesh;
				sprintf(buffer,"%i",y);actorme.replace(actorme.find("~"),1,buffer);
				sprintf(buffer,"%i",x);actorme.replace(actorme.find("`"),1,buffer);
		
				std::string actorOther = sBoxMesh;
				sprintf(buffer,"%i",y+1);actorOther.replace(actorOther.find("~"),1,buffer);
				sprintf(buffer,"%i",x  );actorOther.replace(actorOther.find("`"),1,buffer);
				m_Scene->linkDistance(m_Scene->getActor((char*)actorme.c_str()),physx::PxTransform(physx::PxVec3(0,-size.y,0)),m_Scene->getActor((char*)actorOther.c_str()),physx::PxTransform(physx::PxVec3(0,size.y,0)),0.0f,0.1f);
		
				actorOther = sBoxMesh;
				sprintf(buffer,"%i",y-1);actorOther.replace(actorOther.find("~"),1,buffer);
				sprintf(buffer,"%i",x  );actorOther.replace(actorOther.find("`"),1,buffer);
				m_Scene->linkDistance(m_Scene->getActor((char*)actorme.c_str()),physx::PxTransform(physx::PxVec3(0,size.y,0)),m_Scene->getActor((char*)actorOther.c_str()),physx::PxTransform(physx::PxVec3(0,-size.y,0)),0.0f,0.1f);
		
				actorOther = sBoxMesh;
				sprintf(buffer,"%i",y  );actorOther.replace(actorOther.find("~"),1,buffer);
				sprintf(buffer,"%i",x+1);actorOther.replace(actorOther.find("`"),1,buffer);
				m_Scene->linkDistance(m_Scene->getActor((char*)actorme.c_str()),physx::PxTransform(physx::PxVec3(-size.x,0,0)),m_Scene->getActor((char*)actorOther.c_str()),physx::PxTransform(physx::PxVec3(size.y,0,0)),0.0f,0.1f);
		
				actorOther = sBoxMesh;
				sprintf(buffer,"%i",y  );actorOther.replace(actorOther.find("~"),1,buffer);
				sprintf(buffer,"%i",x-1);actorOther.replace(actorOther.find("`"),1,buffer);
				m_Scene->linkDistance(m_Scene->getActor((char*)actorme.c_str()),physx::PxTransform(physx::PxVec3(size.x,0,0)),m_Scene->getActor((char*)actorOther.c_str()),physx::PxTransform(physx::PxVec3(-size.x,0,0)),0.0f,0.1f);
			}
		}
		
		// Link to our handles
		m_Scene->linkDistance(m_Scene->getActor("linkedBoxesTopPP"),physx::PxTransform(physx::PxVec3(0,0,0)),m_Scene->getActor("linkedBoxes[5][3]"),physx::PxTransform(physx::PxVec3(0,0,0)),1.0f,1.0f);
		m_Scene->linkDistance(m_Scene->getActor("linkedBoxesTopPN"),physx::PxTransform(physx::PxVec3(0,0,0)),m_Scene->getActor("linkedBoxes[5][-3]"),physx::PxTransform(physx::PxVec3(0,0,0)),1.0f,1.0f);

		m_Scene->linkDistance(m_Scene->getActor("linkedBoxesTopNP"),physx::PxTransform(physx::PxVec3(0,0,0)),m_Scene->getActor("linkedBoxes[-5][3]"),physx::PxTransform(physx::PxVec3(0,0,0)),1.0f,1.0f);
		m_Scene->linkDistance(m_Scene->getActor("linkedBoxesTopNN"),physx::PxTransform(physx::PxVec3(0,0,0)),m_Scene->getActor("linkedBoxes[-5][-3]"),physx::PxTransform(physx::PxVec3(0,0,0)),1.0f,1.0f);
	}

	return true;
}

float timer = 0.0f;
void PhysX::onUpdate(float a_deltaTime) {
	timer -= a_deltaTime;
	if (timer <= 0.0f){
		timer = 1/TIMESTEP;
		Utility::freeMovement( m_cameraMatrix, a_deltaTime, 50 );
		Gizmos::clear();
		Gizmos::addTransform( glm::mat4(1,0,0,0,0,1,0,0,0,0,1,0,0,1,0,1) );

		static float fTimer = 0.0f;
		static int bulletnumber = 0; 
		if (glfwGetMouseButton(m_window,GLFW_MOUSE_BUTTON_1) == GLFW_PRESS) {
			if (fTimer <= 0){
				fTimer = 1/15.0f;
				std::string bullet = "bullet_";
				char buffer[32];
				sprintf(buffer,"%i",bulletnumber++);
				bullet.append(buffer);
				m_Scene->AddSphere((char*)bullet.c_str(),physx::PxActorType::Enum::eRIGID_DYNAMIC,100,0.5f,m_cameraMatrix[3].xyz,physx::PxQuat(0,0,0,0),m_cameraMatrix[2].xyz,100.0f);
			}
		}
		fTimer -= a_deltaTime;

		particleEmitter->upDate(a_deltaTime); // Not really an update, more of a removal check
		if (bWaterHit){ particleFluidEmitter->upDate(a_deltaTime); }
		m_Scene->controlActor(a_deltaTime,m_cameraMatrix,m_Scene->getActor("PlayerBody"),100);
		m_Scene->update();
		
		if (glfwGetKey(m_window,GLFW_KEY_ESCAPE) == GLFW_PRESS) {
			quit();
		}
	
		static bool once = false;
		if (glfwGetKey(m_window,GLFW_KEY_F11) == GLFW_PRESS) {
			if (!once){
				once = true;
				printf("Actual FPS : %f \nTimeStep : 1/%i\n",1/a_deltaTime,(int)TIMESTEP);
			}
		}else{
			once = false;
		}
	}
}

void PhysX::onDraw() {
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glm::mat4 viewMatrix = glm::inverse( m_cameraMatrix );
		
	m_Scene->draw();
	particleFluidEmitter->renderParticles();
	particleEmitter->renderParticles();

	Gizmos::draw(viewMatrix, m_projectionMatrix);
}

void PhysX::onDestroy(){
	//!- ASSIGNMENT
	delete m_Scene;
	//!- ASSIGNMENT
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

