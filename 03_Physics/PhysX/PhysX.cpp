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

void PhysX::setUpPhysXTutorial(){

	g_PhysicsFoundation = nullptr;
	g_Physics = nullptr;
	g_PhysicsScene = nullptr;
	gDefaultFilterShader = physx::PxDefaultSimulationFilterShader;
	g_PhysicsMaterial = nullptr;
	g_PhysicsCooker = nullptr;

	physx::PxAllocatorCallback *myCallback = new myAllocator();
	g_PhysicsFoundation = PxCreateFoundation(PX_PHYSICS_VERSION,*myCallback, gDefaultErrorCallback);
	g_Physics = PxCreatePhysics(PX_PHYSICS_VERSION, *g_PhysicsFoundation, physx::PxTolerancesScale());
	g_PhysicsCooker = PxCreateCooking(PX_PHYSICS_VERSION, *g_PhysicsFoundation, physx::PxCookingParams(physx::PxTolerancesScale()));
	PxInitExtensions(*g_Physics);
	//create physics material
	g_PhysicsMaterial = g_Physics->createMaterial(0.5f,0.5f,0.6f);
	physx::PxSceneDesc sceneDesc(g_Physics->getTolerancesScale());
	sceneDesc.gravity = physx::PxVec3(0,-30.0f,0);
	sceneDesc.filterShader = gDefaultFilterShader;
	sceneDesc.cpuDispatcher = physx::PxDefaultCpuDispatcherCreate(1);
	g_PhysicsScene = g_Physics->createScene(sceneDesc);

	if(g_PhysicsScene) {
		printf("start physx scene2\n");
	}
}

void PhysX::cleanUpPhsyx() {
	g_PhysicsCooker->release();
	g_PhysicsScene->release();
	g_Physics->release();
	g_PhysicsFoundation->release();
}

void PhysX::upDatePhysx(){
	g_PhysicsScene->simulate( 1/120.f );
	while (g_PhysicsScene->fetchResults() == false) {
	// don’t need to do anything here yet but we still need to do the fetch
	}
	int ran = rand();
	srand(3);
	for (auto actor:g_PhysXActors) {
		physx::PxU32 nShapes = actor->getNbShapes();
		physx::PxShape** shapes = new physx::PxShape*[nShapes];
		actor->getShapes(shapes, nShapes);
		// Render all the shapes in the physx actor (for early tutorials there is just one)
		while (nShapes--) {
			physx::PxGeometryType::Enum type = shapes[nShapes]->getGeometryType();
			switch(type){
				case physx::PxGeometryType::eBOX:
					drawBox(shapes[nShapes],actor);
					break;
				case physx::PxGeometryType::eSPHERE:
					drawSphere(shapes[nShapes],actor);
					break;
			}
		}
		delete [] shapes;
	}
	srand(ran);
}

void PhysX::AddPlane(glm::vec3 v3_Facing){
	//add a plane
	physx::PxTransform pose = physx::PxTransform( physx::PxVec3(0,0,0),physx::PxQuat(physx::PxHalfPi, physx::PxVec3(0.0f, 0.0f, 1.0f)));
	physx::PxRigidStatic* plane = PxCreateStatic(*g_Physics, pose, physx::PxPlaneGeometry(), *g_PhysicsMaterial);
	//add it to the physX scene
	g_PhysicsScene->addActor(*plane);
}

void PhysX::AddBox(const glm::vec3& v3_Transform, const glm::vec3& v3_Dimentions,float f_density) {
	//add a box
	physx::PxBoxGeometry box(v3_Dimentions.x,v3_Dimentions.y,v3_Dimentions.z);
	physx::PxTransform transform(physx::PxVec3(v3_Transform.x,v3_Transform.y,v3_Transform.z));
	physx::PxRigidDynamic* dynamicActor = PxCreateDynamic(*g_Physics, transform, box,*g_PhysicsMaterial, f_density); 
	//add it to the physX scene
	g_PhysicsScene->addActor(*dynamicActor);
	//add it to our copy of the scene
	g_PhysXActors.push_back(dynamicActor);
}

void PhysX::AddBullet(const glm::mat4& m4_Camera,float f_Power,float f_radius,float f_density) {
	//add a box
	physx::PxSphereGeometry sphere(f_radius);
	physx::PxTransform transform(physx::PxVec3(m4_Camera[3].x,m4_Camera[3].y,m4_Camera[3].z));
	physx::PxRigidDynamic* dynamicActor = PxCreateDynamic(*g_Physics, transform, sphere,*g_PhysicsMaterial, f_density); 
	physx::PxVec3 velocity(m4_Camera[2].x * -f_Power,m4_Camera[2].y * -f_Power,m4_Camera[2].z * -f_Power);
	dynamicActor->setLinearVelocity(velocity,true);
	//add it to the physX scene
	g_PhysicsScene->addActor(*dynamicActor);
	//add it to our copy of the scene
	g_PhysXActors.push_back(dynamicActor);
}

void PhysX::setUpVisualDebugger() {
	// check if PvdConnection manager is available on this platform
	if (NULL == g_Physics->getPvdConnectionManager())
		return;
	// setup connection parameters
	const char* pvd_host_ip = "127.0.0.1"; // IP of the PC which is running PVD
	int port = 5425; // TCP port to connect to, where PVD is listening
	unsigned int timeout = 100; // timeout in milliseconds to wait for PVD to respond,
	// consoles and remote PCs need a higher timeout.
	physx::PxVisualDebuggerConnectionFlags connectionFlags = physx::PxVisualDebuggerConnectionFlag::Debug | physx::PxVisualDebuggerConnectionFlag::Profile | physx::PxVisualDebuggerConnectionFlag::Memory;
	// and now try to connect
	physx::PxVisualDebuggerExt::createConnection(g_Physics->getPvdConnectionManager(),pvd_host_ip, port, timeout, connectionFlags);
	// pvd_host_ip, port, timeout, connectionFlags));
}

bool PhysX::onCreate(int a_argc, char* a_argv[]) {
	Gizmos::create(SHRT_MAX * 2,SHRT_MAX * 2);
	m_cameraMatrix = glm::inverse( glm::lookAt(glm::vec3(10,10,10),glm::vec3(0,0,0), glm::vec3(0,1,0)) );
	m_projectionMatrix = glm::perspective(glm::pi<float>() * 0.25f, DEFAULT_SCREENWIDTH/(float)DEFAULT_SCREENHEIGHT, 0.1f, 1000.0f);
	glClearColor(0.25f,0.25f,0.25f,1);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);
	
	//!- TUTORIAL
	setUpPhysXTutorial();
	setUpVisualDebugger();
	AddPlane(glm::vec3(0,1,0));
	for (int y = 0; y < 25; y++){
		for (int x = -5; x < 5; x++){
			for (int z = -5; z < 5; z++){
				AddBox(glm::vec3(x,y + 0.5f,z),glm::vec3(0.5f),100);
			}
		}
	}
	//!- TUTORIAL

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
			AddBullet(m_cameraMatrix,100,0.5f,10000);
		}
	}else{
		bLeftPressed = false;
	}

	upDatePhysx();
	//!- TUTORIAL
	
	if (glfwGetKey(m_window,GLFW_KEY_ESCAPE) == GLFW_PRESS) {
		quit();
	}
}

void PhysX::onDraw() {
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glm::mat4 viewMatrix = glm::inverse( m_cameraMatrix );
	
	//!- TUTORIAL
	for (auto child : g_PhysXActors){

	}
	//!- TUTORIAL
	
	Gizmos::draw(viewMatrix, m_projectionMatrix);
}

void PhysX::onDestroy(){
	//!- TUTORIAL
	cleanUpPhsyx();
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

void PhysX::drawSphere(physx::PxShape* pShape,physx::PxRigidDynamic* actor) {
	//get the geometry for this PhysX collision volume
	physx::PxSphereGeometry geometry;
	float radius;
	bool status = pShape->getSphereGeometry(geometry);
	if(status){
		radius = geometry.radius;
	}
	//get the transform for this PhysX collision volume
	physx::PxMat44 m(physx::PxShapeExt::getGlobalPose(*pShape));
	glm::mat4 M(m.column0.x, m.column0.y, m.column0.z, m.column0.w,
				m.column1.x, m.column1.y, m.column1.z, m.column1.w,
				m.column2.x, m.column2.y, m.column2.z, m.column2.w,
				m.column3.x, m.column3.y, m.column3.z, m.column3.w);
	glm::vec3 position;
	//get the position out of the transform
	position.x = m.getPosition().x;
	position.y = m.getPosition().y;
	position.z = m.getPosition().z;
	glm::vec4 colour = glm::vec4(rand()%1000 / 1000.0f,rand()%1000 / 1000.0f,rand()%1000 / 1000.0f,1);
	//create our box gizmo
	Gizmos::addSphere(position,5,5,radius,colour,&M);
}

void PhysX::drawBox(physx::PxShape* pShape,physx::PxRigidDynamic* actor) {
	//get the geometry for this PhysX collision volume
	physx::PxBoxGeometry geometry;
	float width = 1, height = 1, length = 1;
	bool status = pShape->getBoxGeometry(geometry);
	if(status){
		width = geometry.halfExtents.x;
		height = geometry.halfExtents.y;
		length = geometry.halfExtents.z;
	}
	//get the transform for this PhysX collision volume
	physx::PxMat44 m(physx::PxShapeExt::getGlobalPose(*pShape));
	glm::mat4 M(m.column0.x, m.column0.y, m.column0.z, m.column0.w,
				m.column1.x, m.column1.y, m.column1.z, m.column1.w,
				m.column2.x, m.column2.y, m.column2.z, m.column2.w,
				m.column3.x, m.column3.y, m.column3.z, m.column3.w);
	glm::vec3 position;
	//get the position out of the transform
	position.x = m.getPosition().x;
	position.y = m.getPosition().y;
	position.z = m.getPosition().z;
	glm::vec3 extents = glm::vec3(width,height,length);
	glm::vec4 colour = glm::vec4(rand()%1000 / 1000.0f,rand()%1000 / 1000.0f,rand()%1000 / 1000.0f,1);
	//create our box gizmo
	Gizmos::addAABBFilled(position,extents,colour,&M);
}