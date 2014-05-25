#include "PhysXScene.hpp"
#include <Gizmos.h>
#include <glm\ext.hpp>
#include <GLFW\glfw3.h>

void drawSphere(physx::PxShape* pShape,physx::PxRigidActor* actor) {
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
void drawBox(physx::PxShape* pShape,physx::PxRigidActor* actor) {
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
void drawCapsule(physx::PxShape* pShape,physx::PxRigidActor* actor) {
	//creates a gizmo representation of a capsule using 2 spheres and a cylinder

	glm::vec4 colour(0,0,1,1);  //make our capsule blue
	physx::PxCapsuleGeometry capsuleGeometry;
	float radius = 1; //temporary values whilst we try and get the real value from PhysX
	float halfHeight = 1;
	//get the geometry for this PhysX collision volume
	bool status = pShape->getCapsuleGeometry(capsuleGeometry);
	if(status)
	{
		//this should always happen but just to be safe we check the status flag
		radius = capsuleGeometry.radius; //copy out capsule radius
		halfHeight = capsuleGeometry.halfHeight; //copy out capsule half length
	}
	//get the world transform for the centre of this PhysX collision volume
	physx::PxTransform transform = physx::PxShapeExt::getGlobalPose(*pShape);
	//use it to create a matrix
	physx::PxMat44 m(transform);
	//convert it to an open gl matrix for adding our gizmos
	glm::mat4 M(m.column0.x, m.column0.y, m.column0.z, m.column0.w,
				m.column1.x, m.column1.y, m.column1.z, m.column1.w,
				m.column2.x, m.column2.y, m.column2.z, m.column2.w,
				m.column3.x, m.column3.y, m.column3.z, m.column3.w);
	//get the world position from the PhysX tranform
	glm::vec3 position = glm::vec3(transform.p.x,transform.p.y,transform.p.z); 
	glm::vec4 axis(halfHeight,0,0,0);	//axis for the capsule
	axis = M * axis; //rotate axis to correct orientation
	//add our 2 end cap spheres...
	Gizmos::addSphere(position+axis.xyz,10,10,radius,colour);
	Gizmos::addSphere(position-axis.xyz,10,10,radius,colour);
	//the cylinder gizmo is oriented 90 degrees to what we want so we need to change the rotation matrix...
	glm::mat4 m2 = glm::rotate(M,11/7.0f,0.0f,0.0f,1.0f); //adds an additional rotation onto the matrix
	//now we can use this matrix and the other data to create the cylinder...
	Gizmos::addCylinderFilled(position,radius,halfHeight,10,colour,&m2);
}
void drawPlane(physx::PxShape* pShape,physx::PxRigidActor* actor) {
	//get the geometry for this PhysX collision volume
	physx::PxPlaneGeometry geometry;
	//get the transform for this PhysX collision volume
	physx::PxTransform t = physx::PxShapeExt::getGlobalPose(*pShape);

	physx::PxMat44 m(t);
	glm::mat4 M(m.column0.x, m.column0.y, m.column0.z, m.column0.w,
				m.column1.x, m.column1.y, m.column1.z, m.column1.w,
				m.column2.x, m.column2.y, m.column2.z, m.column2.w,
				m.column3.x, m.column3.y, m.column3.z, m.column3.w);

	glm::vec3 rots = glm::eulerAngles(glm::quat((float)t.q.w,(float)t.q.x,(float)t.q.y,(float)t.q.z));

	M = glm::rotate(M,rots.x,glm::vec3(1,0,0));
	M = glm::rotate(M,rots.y,glm::vec3(0,1,0));
	M = glm::rotate(M,rots.z,glm::vec3(0,0,1));

	glm::vec3 position;
	//get the position out of the transform
	position.x = m.getPosition().x;
	position.y = m.getPosition().y;
	position.z = m.getPosition().z;

	glm::vec3 extents = glm::vec3(100,0.1,100);
	glm::vec4 colour = glm::vec4(rand()%1000 / 1000.0f,rand()%1000 / 1000.0f,rand()%1000 / 1000.0f,1);
	//create our box gizmo
	Gizmos::addAABBFilled(position,extents,colour,&M);
}

PhysXScene::PhysXScene(){
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
PhysXScene::~PhysXScene() {
	g_PhysicsCooker->release();
	g_PhysicsScene->release();
	g_Physics->release();
	g_PhysicsFoundation->release();
}

void PhysXScene::update(){
	g_PhysicsScene->simulate( 1/120.f );
	while (g_PhysicsScene->fetchResults() == false) {}
}

void PhysXScene::draw(){
	int ran = rand();
	srand(3);
	for (auto actor : g_PhysXActors) {
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
				case physx::PxGeometryType::eCAPSULE:
					drawCapsule(shapes[nShapes],actor);
					break;
				case physx::PxGeometryType::ePLANE:
					drawPlane(shapes[nShapes],actor);
					break;
			}
		}
		delete [] shapes;
	}
	srand(ran);
}

void PhysXScene::AddPlane	(char* name, physx::PxActorType::Enum aType, const float& f_density, const glm::vec3& v3_Transform, const physx::PxQuat& px_Quaternion, const glm::vec3& v3_Direction, const float& f_Power){
	//add a plane
	physx::PxTransform transform;
	if (px_Quaternion.isSane()){
		transform = physx::PxTransform( physx::PxVec3(v3_Transform.x,v3_Transform.y,v3_Transform.z),px_Quaternion);
	}else{
		transform = physx::PxTransform( physx::PxVec3(v3_Transform.x,v3_Transform.y,v3_Transform.z));
	}
	physx::PxPlaneGeometry geom;
	physx::PxRigidActor *actor = nullptr;
	switch(aType){
		case physx::PxActorType::eRIGID_DYNAMIC:
			actor = PxCreateDynamic(*g_Physics, transform, geom,*g_PhysicsMaterial, f_density); 
			if (f_Power != 0.0f){
				physx::PxVec3 velocity(v3_Direction.x * -f_Power,v3_Direction.y * -f_Power,v3_Direction.z * -f_Power);
				((physx::PxRigidDynamic*)actor)->setLinearVelocity(velocity,true);
			}
		break;
		case physx::PxActorType::eRIGID_STATIC:
			actor = PxCreateStatic(*g_Physics, transform, geom, *g_PhysicsMaterial);
		break;
	}
	//add it to the physX scene
	if (actor != nullptr){
		g_PhysicsScene->addActor(*actor);
		g_PhysXActors.push_back(actor);
		actor->setName(name);
	}
}

void PhysXScene::AddBox(char* name, physx::PxActorType::Enum aType, const float& f_density, const glm::vec3& v3_Dimentions	, const glm::vec3& v3_Transform, const physx::PxQuat& px_Quaternion, const glm::vec3& v3_Direction, const float& f_Power){
	//add a Box
	physx::PxTransform transform;
	if (px_Quaternion.isSane()){
		transform = physx::PxTransform( physx::PxVec3(v3_Transform.x,v3_Transform.y,v3_Transform.z),px_Quaternion);
	}else{
		transform = physx::PxTransform( physx::PxVec3(v3_Transform.x,v3_Transform.y,v3_Transform.z));
	}
	physx::PxBoxGeometry geom(v3_Dimentions.x,v3_Dimentions.y,v3_Dimentions.z);
	physx::PxRigidActor *actor = nullptr;
	switch(aType){
		case physx::PxActorType::eRIGID_DYNAMIC:
			actor = PxCreateDynamic(*g_Physics, transform, geom,*g_PhysicsMaterial, f_density); 
			if (f_Power != 0.0f){
				physx::PxVec3 velocity(v3_Direction.x * -f_Power,v3_Direction.y * -f_Power,v3_Direction.z * -f_Power);
				((physx::PxRigidDynamic*)actor)->setLinearVelocity(velocity,true);
			}
		break;
		case physx::PxActorType::eRIGID_STATIC:
			actor = PxCreateStatic(*g_Physics, transform, geom, *g_PhysicsMaterial);
		break;
	}
	//add it to the physX scene
	if (actor != nullptr){
		g_PhysicsScene->addActor(*actor);
		g_PhysXActors.push_back(actor);
		actor->setName(name);
	}
}

void PhysXScene::AddSphere(char* name, physx::PxActorType::Enum aType, const float& f_density, const float& f_Radius, const glm::vec3& v3_Transform, const physx::PxQuat& px_Quaternion, const glm::vec3& v3_Direction, const float& f_Power){
	//add a Sphere
	physx::PxTransform transform;
	if (px_Quaternion.isSane()){
		transform = physx::PxTransform( physx::PxVec3(v3_Transform.x,v3_Transform.y,v3_Transform.z),px_Quaternion);
	}else{
		transform = physx::PxTransform( physx::PxVec3(v3_Transform.x,v3_Transform.y,v3_Transform.z));
	}
	physx::PxSphereGeometry geom(f_Radius);
	physx::PxRigidActor *actor = nullptr;
	switch(aType){
		case physx::PxActorType::eRIGID_DYNAMIC:
			actor = PxCreateDynamic(*g_Physics, transform, geom,*g_PhysicsMaterial, f_density); 
			if (f_Power != 0.0f){
				physx::PxVec3 velocity(v3_Direction.x * -f_Power,v3_Direction.y * -f_Power,v3_Direction.z * -f_Power);
				((physx::PxRigidDynamic*)actor)->setLinearVelocity(velocity,true);
			}
		break;
		case physx::PxActorType::eRIGID_STATIC:
			actor = PxCreateStatic(*g_Physics, transform, geom, *g_PhysicsMaterial);
		break;
	}
	//add it to the physX scene
	if (actor != nullptr){
		g_PhysicsScene->addActor(*actor);
		g_PhysXActors.push_back(actor);
		actor->setName(name);
	}
}

void PhysXScene::AddCapsule (char* name, physx::PxActorType::Enum aType, const float& f_density, const float& f_Radius, const float& f_HalfHeight, const glm::vec3& v3_Transform, const physx::PxQuat& px_Quaternion, const glm::vec3& v3_Direction, const float& f_Power){
	//add a Capsule
	physx::PxTransform transform;
	if (px_Quaternion.isSane()){
		transform = physx::PxTransform( physx::PxVec3(v3_Transform.x,v3_Transform.y,v3_Transform.z),px_Quaternion);
	}else{
		transform = physx::PxTransform( physx::PxVec3(v3_Transform.x,v3_Transform.y,v3_Transform.z));
	}
	physx::PxCapsuleGeometry geom(f_Radius,f_HalfHeight);
	physx::PxRigidActor *actor = nullptr;
	switch(aType){
		case physx::PxActorType::eRIGID_DYNAMIC:
			actor = PxCreateDynamic(*g_Physics, transform, geom,*g_PhysicsMaterial, f_density); 
			if (f_Power != 0.0f){
				physx::PxVec3 velocity(v3_Direction.x * -f_Power,v3_Direction.y * -f_Power,v3_Direction.z * -f_Power);
				((physx::PxRigidDynamic*)actor)->setLinearVelocity(velocity,true);
			}
		break;
		case physx::PxActorType::eRIGID_STATIC:
			actor = PxCreateStatic(*g_Physics, transform, geom, *g_PhysicsMaterial);
		break;
	}
	//add it to the physX scene
	if (actor != nullptr){
		g_PhysicsScene->addActor(*actor);
		g_PhysXActors.push_back(actor);
		actor->setName(name);
	}
}

void PhysXScene::controlActor(float a_deltaTime,physx::PxRigidActor* actor,float f_Force){
	if (actor == nullptr){
		return;
	}

	if (actor->getType() != physx::PxActorType::eRIGID_DYNAMIC){
		return;
	}

	physx::PxRigidDynamic *Actor = (physx::PxRigidDynamic*)actor;
	physx::PxTransform pose = Actor->getGlobalPose();
	pose.q = physx::PxQuat(glm::half_pi<float>(),physx::PxVec3(0,0,1));
	Actor->setGlobalPose(pose);

	GLFWwindow *window = glfwGetCurrentContext();
	if (glfwGetKey(window,GLFW_KEY_RIGHT_ALT) == GLFW_PRESS){
		Actor->addForce((physx::PxVec3(0,4000,0) * Actor->getMass() * f_Force) * a_deltaTime);
	}
	if (glfwGetKey(window,GLFW_KEY_UP) == GLFW_PRESS){
		Actor->addForce((physx::PxVec3(1,0,0) * Actor->getMass() * f_Force) * a_deltaTime);
	}else if (glfwGetKey(window,GLFW_KEY_DOWN) == GLFW_PRESS){
		Actor->addForce((physx::PxVec3(-1,0,0) * Actor->getMass() * f_Force) * a_deltaTime);
	}
	if (glfwGetKey(window,GLFW_KEY_RIGHT) == GLFW_PRESS){
		Actor->addForce((physx::PxVec3(0,0,1) * Actor->getMass() *  f_Force) * a_deltaTime);
	}else if (glfwGetKey(window,GLFW_KEY_LEFT) == GLFW_PRESS){
		Actor->addForce((physx::PxVec3(0,0,-1) * Actor->getMass() *  f_Force) * a_deltaTime);
	}
}

void PhysXScene::snapActor(physx::PxRigidActor* actor,glm::vec3 v3_Position){
	if (actor == nullptr){
		return;
	}

	if (actor->getType() != physx::PxActorType::eRIGID_DYNAMIC){
		return;
	}
	physx::PxRigidDynamic *Actor = (physx::PxRigidDynamic*)actor;
	physx::PxTransform pose = Actor->getGlobalPose();
	pose.p = physx::PxVec3(v3_Position.x,v3_Position.y,v3_Position.z);
	pose.q = physx::PxQuat(11/7.0f,physx::PxVec3(0,0,1));
	Actor->setGlobalPose(pose);
}

