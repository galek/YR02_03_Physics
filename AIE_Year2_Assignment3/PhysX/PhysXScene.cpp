#include "PhysXScene.hpp"
#include <Gizmos.h>
#include <Utilities.h>
#include <SOIL.h>

#include <glm\ext.hpp>

#include <gl\glew.h>
#include <GLFW\glfw3.h>

#include <algorithm>
#include <pxtask/PxCudaContextManager.h>
#include <PxTkStream.h>

bool bWaterHit = false;

struct FilterGroup{
	enum Enum{
		ePLAYER = (1 << 0),
		ePLATFORM = (1 << 1),
		eGROUND = (1 << 2)
	};
};

//helper function to set up filtering
void setupFiltering(physx::PxRigidActor* actor, physx::PxU32 filterGroup, physx::PxU32 filterMask){
	physx::PxFilterData filterData;
	filterData.word0 = filterGroup; // word0 = own ID
	filterData.word1 = filterMask; // word1 = ID mask to filter pairs that trigger a contact callback;
	const physx::PxU32 numShapes = actor->getNbShapes();
	physx::PxShape** shapes = (physx::PxShape**)_aligned_malloc(sizeof(physx::PxShape*)*numShapes,16);
	actor->getShapes(shapes, numShapes);
	for(physx::PxU32 i = 0; i < numShapes; i++) {
		physx::PxShape* shape = shapes[i];
		shape->setSimulationFilterData(filterData);
	}
	_aligned_free(shapes);
}

physx::PxFilterFlags myFliterShader(physx::PxFilterObjectAttributes attributes0, physx::PxFilterData filterData0,physx::PxFilterObjectAttributes attributes1, physx::PxFilterData filterData1,physx::PxPairFlags& pairFlags, const void* constantBlock, physx::PxU32 constantBlockSize){
	// let triggers through
	if(physx::PxFilterObjectIsTrigger(attributes0) || physx::PxFilterObjectIsTrigger(attributes1)){
		pairFlags = physx::PxPairFlag::eTRIGGER_DEFAULT;
		return physx::PxFilterFlag::eDEFAULT;
	}
	// generate contacts for all that were not filtered above
	pairFlags = physx::PxPairFlag::eCONTACT_DEFAULT;
	// trigger the contact callback for pairs (A,B) where
	// the filtermask of A contains the ID of B and vice versa.
	if((filterData0.word0 & filterData1.word1) && (filterData1.word0 & filterData0.word1))
		pairFlags |= physx::PxPairFlag::eNOTIFY_TOUCH_FOUND;
	return physx::PxFilterFlag::eDEFAULT;
}

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
	glm::vec4 colour = glm::vec4(rand()%100 / 100.0f,rand()%100 / 100.0f,rand()%100 / 100.0f,1);
	//create our box gizmo
	Gizmos::addSphere(position,3,3,radius,colour,&M);
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
	glm::vec4 colour = glm::vec4(rand()%100 / 100.0f,rand()%100 / 100.0f,rand()%100 / 100.0f,1);
	//create our box gizmo
	Gizmos::addAABBFilled(position,extents,colour,&M);
}
void drawCapsule(physx::PxShape* pShape,physx::PxRigidActor* actor) {
	//creates a gizmo representation of a capsule using 2 spheres and a cylinder

	glm::vec4 colour(rand()%100 / 100.0f,rand()%100 / 100.0f,rand()%100 / 100.0f,1);  //make our capsule blue
	physx::PxCapsuleGeometry capsuleGeometry;
	float radius = 1; //temporary values whilst we try and get the real value from PhysX
	float halfHeight = 1;
	//get the geometry for this PhysX collision volume
	bool status = pShape->getCapsuleGeometry(capsuleGeometry);
	if(status) {
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
	Gizmos::addSphere(position+axis.xyz,4,4,radius,colour);
	Gizmos::addSphere(position-axis.xyz,4,4,radius,colour);
	//the cylinder gizmo is oriented 90 degrees to what we want so we need to change the rotation matrix...
	glm::mat4 m2 = glm::rotate(M,glm::half_pi<float>(),0.0f,0.0f,1.0f); //adds an additional rotation onto the matrix
	//now we can use this matrix and the other data to create the cylinder...
	Gizmos::addCylinderFilled(position,radius,halfHeight,4,colour,&m2);
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

	glm::vec3 extents = glm::vec3(100,0.0001f,100);
	glm::vec4 colour = glm::vec4(rand()%100 / 100.0f,rand()%100 / 100.0f,rand()%100 / 100.0f,1);
	//create our box gizmo
	Gizmos::addAABBFilled(position,extents,colour,&M);
}

PhysXScene::PhysXScene(float f_TicksPerSecond,int i_ThreadCount){
	fTicksPerSecond = f_TicksPerSecond;
	g_PhysicsFoundation = nullptr;
	g_Physics = nullptr;
	g_PhysicsScene = nullptr;
	gDefaultFilterShader = myFliterShader;//physx::PxDefaultSimulationFilterShader;
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
	sceneDesc.gravity = physx::PxVec3(0,-9.8f,0);
	sceneDesc.filterShader = gDefaultFilterShader;
	sceneDesc.cpuDispatcher = physx::PxDefaultCpuDispatcherCreate(i_ThreadCount);

	//physx::pxtask::CudaContextManagerDesc cudaContextManagerDesc;
    //physx::pxtask::CudaContextManager *mCudaContextManager = physx::pxtask::createCudaContextManager(*g_PhysicsFoundation, cudaContextManagerDesc, nullptr);
    //if( mCudaContextManager ){
    //    if( mCudaContextManager->contextIsValid() ){
    //        sceneDesc.gpuDispatcher = mCudaContextManager->getGpuDispatcher();
	//		printf("Cuda context created!\n");
    //    }
    //}

	g_PhysicsScene = g_Physics->createScene(sceneDesc);

	if(g_PhysicsScene) {
		printf("Start physx scene.\n");
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

	// Here we would make a different overloaded class for each different type of callback
	physx::PxSimulationEventCallback* mycollisionCallBack = new MycollisionCallBack(); //instantiate ourclass to overload call backs
	g_PhysicsScene->setSimulationEventCallback(mycollisionCallBack); //tell the scene to use our call back class
}

void PhysXScene::Reload(float f_TicksPerSecond,int i_ThreadCount){

	g_PhysXActors.clear();
	g_PhysXJoints.clear();
	g_PhysXActorsRagDolls.clear();
	g_PhysXActorsCloths.clear();

	g_PhysicsCooker->release();
	g_PhysicsScene->release();
	g_Physics->release();

	fTicksPerSecond = f_TicksPerSecond;
	g_Physics = nullptr;
	g_PhysicsScene = nullptr;
	gDefaultFilterShader = myFliterShader;//physx::PxDefaultSimulationFilterShader;
	g_PhysicsMaterial = nullptr;
	g_PhysicsCooker = nullptr;

	g_Physics = PxCreatePhysics(PX_PHYSICS_VERSION, *g_PhysicsFoundation, physx::PxTolerancesScale());
	g_PhysicsCooker = PxCreateCooking(PX_PHYSICS_VERSION, *g_PhysicsFoundation, physx::PxCookingParams(physx::PxTolerancesScale()));
	PxInitExtensions(*g_Physics);
	//create physics material
	g_PhysicsMaterial = g_Physics->createMaterial(0.5f,0.5f,0.6f);
	physx::PxSceneDesc sceneDesc(g_Physics->getTolerancesScale());
	sceneDesc.gravity = physx::PxVec3(0,-9.8f,0);
	sceneDesc.filterShader = gDefaultFilterShader;
	sceneDesc.cpuDispatcher = physx::PxDefaultCpuDispatcherCreate(i_ThreadCount);

	//physx::pxtask::CudaContextManagerDesc cudaContextManagerDesc;
    //physx::pxtask::CudaContextManager *mCudaContextManager = physx::pxtask::createCudaContextManager(*g_PhysicsFoundation, cudaContextManagerDesc, nullptr);
    //if( mCudaContextManager ){
    //    if( mCudaContextManager->contextIsValid() ){
    //        sceneDesc.gpuDispatcher = mCudaContextManager->getGpuDispatcher();
	//		printf("Cuda context created!\n");
    //    }
    //}

	g_PhysicsScene = g_Physics->createScene(sceneDesc);

	if(g_PhysicsScene) {
		printf("Start physx scene.\n");
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

	physx::PxSimulationEventCallback* mycollisionCallBack = new MycollisionCallBack(); //instantiate ourclass to overload call backs
	g_PhysicsScene->setSimulationEventCallback(mycollisionCallBack); //tell the scene to use our call back class
}
PhysXScene::~PhysXScene() {
	g_PhysicsCooker->release();
	g_PhysicsScene->release();
	g_Physics->release();
	g_PhysicsFoundation->release();
}

void PhysXScene::update(){
	g_PhysicsScene->simulate( 1 / fTicksPerSecond );
	while (g_PhysicsScene->fetchResults() == false) {}

	for (auto actor : g_PhysXActorsCloths) {
		physx::PxClothReadData* data = actor.second->m_cloth->lockClothReadData();
		for ( unsigned int i = 0 ; i < actor.second->m_clothVertexCount; ++i){
			actor.second->m_clothPositions[i] = glm::vec3(data->particles[i].pos.x,data->particles[i].pos.y,data->particles[i].pos.z);
		}
		data->unlock();
	}
}
void PhysXScene::draw(const glm::mat4 *m4_ViewProjection){
	int ran = rand();
	srand(3);
	// draw all our actors
	for (auto actor : g_PhysXActors) {
		physx::PxU32 nShapes = actor.second->getNbShapes();
		physx::PxShape** shapes = new physx::PxShape*[nShapes];
		actor.second->getShapes(shapes, nShapes);
		while (nShapes--) {
			physx::PxGeometryType::Enum type = shapes[nShapes]->getGeometryType();
			switch(type){
				case physx::PxGeometryType::eBOX:
					drawBox(shapes[nShapes],actor.second);
					break;
				case physx::PxGeometryType::eSPHERE:
					drawSphere(shapes[nShapes],actor.second);
					break;
				case physx::PxGeometryType::eCAPSULE:
					drawCapsule(shapes[nShapes],actor.second);
					break;
				case physx::PxGeometryType::ePLANE:
					drawPlane(shapes[nShapes],actor.second);
					break;
			}
		}
		delete [] shapes;
	}
	// all our ragdolls
	for(auto articulation : g_PhysXActorsRagDolls) {
		physx::PxU32 nLinks = articulation.second->getNbLinks();
		physx::PxArticulationLink** links = new physx::PxArticulationLink*[nLinks];
		articulation.second->getLinks(links, nLinks);
		while (nLinks--)
		{
			physx::PxArticulationLink* link = links[nLinks];
			physx::PxU32 nShapes = link->getNbShapes();
			physx::PxShape** shapes = new physx::PxShape*[nShapes];
			link->getShapes(shapes, nShapes);
			while (nShapes--)
			{
			physx::PxGeometryType::Enum type = shapes[nShapes]->getGeometryType();
				switch(type){
				case physx::PxGeometryType::eBOX:
					drawBox(shapes[nShapes],link);
					break;
				case physx::PxGeometryType::eSPHERE:
					drawSphere(shapes[nShapes],link);
					break;
				case physx::PxGeometryType::eCAPSULE:
					drawCapsule(shapes[nShapes],link);
					break;
				case physx::PxGeometryType::ePLANE:
					drawPlane(shapes[nShapes],link);
					break;
				}
			}
		}
		delete [] links;
	}
	// all our joints
	for (auto joint : g_PhysXJoints) {
		physx::PxRigidActor *actor1;
		physx::PxRigidActor *actor2;

		joint.second->getActors(actor1,actor2);

		glm::vec3 pos1 = glm::vec3(actor1->getGlobalPose().p.x,actor1->getGlobalPose().p.y,actor1->getGlobalPose().p.z);
		glm::vec3 pos2 = glm::vec3(actor2->getGlobalPose().p.x,actor2->getGlobalPose().p.y,actor2->getGlobalPose().p.z);

		Gizmos::addLine(pos1,pos2,glm::vec4(1.0f));
	}

	for (auto actor : g_PhysXActorsCloths) {
		glUseProgram(actor.second->m_shader);
		int location = glGetUniformLocation(actor.second->m_shader, "projectionView");
		glUniformMatrix4fv(location, 1, GL_FALSE, glm::value_ptr(*m4_ViewProjection));
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, actor.second->m_texture);
		// update vertex positions from the cloth
		glBindBuffer(GL_ARRAY_BUFFER, actor.second->m_clothVBO);
		glBufferSubData(GL_ARRAY_BUFFER, 0, actor.second->m_clothVertexCount * sizeof(glm::vec3), actor.second->m_clothPositions);
		// disable face culling so that we can draw it double-sided
		glDisable(GL_CULL_FACE);
		// bind and draw the cloth
		glBindVertexArray(actor.second->m_clothVAO);
		glDrawElements(GL_TRIANGLES, actor.second->m_clothIndexCount, GL_UNSIGNED_INT, 0);
		glEnable(GL_CULL_FACE);
	}

	srand(ran);
}

physx::PxRigidActor* PhysXScene::AddPlane(char* name, physx::PxActorType::Enum aType, const float& f_density, const glm::vec3& v3_Transform, const physx::PxQuat& px_Quaternion, const glm::vec3& v3_Direction, const float& f_Power){
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
		unsigned long long l = getHash(std::string(name));
		if (getActor(name) == nullptr){
			g_PhysicsScene->addActor(*actor);
			g_PhysXActors[l] = (actor);
			actor->setName(name);
		}else{
			printf("ERROR : %s %i \n",name,l);
		}
	}
	setupFiltering(actor,FilterGroup::eGROUND,FilterGroup::ePLAYER);
	return actor;
}
physx::PxRigidActor* PhysXScene::AddBox(char* name, physx::PxActorType::Enum aType, const float& f_density, const glm::vec3& v3_Dimentions	, const glm::vec3& v3_Transform, const physx::PxQuat& px_Quaternion, const glm::vec3& v3_Direction, const float& f_Power){
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
		unsigned long long l = getHash(std::string(name));
		if (getActor(l) == nullptr){
			g_PhysicsScene->addActor(*actor);
			g_PhysXActors[l] = (actor);
			actor->setName(name);
		}else{
			printf("ERROR : %s %i \n",name,l);
		}
	}
	setupFiltering(actor,FilterGroup::ePLATFORM,FilterGroup::ePLAYER);
	return actor;
}
physx::PxRigidActor* PhysXScene::AddSphere(char* name, physx::PxActorType::Enum aType, const float& f_density, const float& f_Radius, const glm::vec3& v3_Transform, const physx::PxQuat& px_Quaternion, const glm::vec3& v3_Direction, const float& f_Power){
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
		unsigned long long l = getHash(std::string(name));
		if (getActor(name) == nullptr){
			g_PhysicsScene->addActor(*actor);
			g_PhysXActors[l] = (actor);
			actor->setName(name);
		}else{
			printf("ERROR : %s %i \n",name,l);
		}
	}
	setupFiltering(actor,FilterGroup::ePLATFORM,FilterGroup::ePLAYER);
	return actor;
}
physx::PxRigidActor* PhysXScene::AddCapsule (char* name, physx::PxActorType::Enum aType, const float& f_density, const float& f_Radius, const float& f_HalfHeight, const glm::vec3& v3_Transform, const physx::PxQuat& px_Quaternion, const glm::vec3& v3_Direction, const float& f_Power){
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
		unsigned long long l = getHash(std::string(name));
		if (getActor(name) == nullptr){
			g_PhysicsScene->addActor(*actor);
			g_PhysXActors[l] = (actor);
			actor->setName(name);
		}else{
			printf("ERROR : %s %i \n",name,l);
		}
	}
	setupFiltering(actor,FilterGroup::ePLATFORM,FilterGroup::ePLAYER);
	return actor;
}
physx::PxArticulation* PhysXScene::AddRagdoll(char* name, RagdollNode** nodeArray, physx::PxTransform worldPos, float scaleFactor){
	physx::PxArticulation* ragDollArticulation = g_Physics->createArticulation();
	physx::PxMaterial* ragdollMaterial = g_Physics->createMaterial(0.4f,0.4f,1.0f);
	RagdollNode** currentNode = nodeArray;

	while(*currentNode != NULL) {
		RagdollNode* currentNodePtr = *currentNode;
		RagdollNode* parentNode;

		float radius = currentNodePtr->radius * scaleFactor;
		float halfLength = currentNodePtr->halfLength * scaleFactor;
		float childHalfLength = radius + halfLength;
		float parentHalfLength;

		int parentIdx = currentNodePtr->parentNodeIdx;
		physx::PxArticulationLink* parentLinkPtr = NULL;
		currentNodePtr->scaledGobalPos = worldPos.p;
		if(parentIdx!= -1) {
			parentNode = *(nodeArray + parentIdx);
			parentLinkPtr = parentNode->linkPtr;
			parentHalfLength = (parentNode->radius + parentNode->halfLength) * scaleFactor;

			physx::PxVec3 currentRelative = currentNodePtr->childLinkPos * currentNodePtr->globalRotation.rotate(physx::PxVec3(childHalfLength,0,0));
			physx::PxVec3 parentRelative = - currentNodePtr->parentLinkPos * parentNode->globalRotation.rotate(physx::PxVec3(parentHalfLength,0,0));
			currentNodePtr->scaledGobalPos = parentNode->scaledGobalPos- (parentRelative + currentRelative);
		}

		physx::PxTransform linkTransform = physx::PxTransform(currentNodePtr->scaledGobalPos,currentNodePtr->globalRotation) ;
		physx::PxArticulationLink* link = ragDollArticulation->createLink(parentLinkPtr, linkTransform);

		std::string partname = name;
		partname.append("_");
		partname.append(currentNodePtr->name);
		link->setName(partname.c_str());

		currentNodePtr->linkPtr = link;
		float jointSpace = .01f;
		float capsuleHalfLength = (halfLength>jointSpace?halfLength-jointSpace:0)+.01f; 
		physx::PxCapsuleGeometry capsule(radius,capsuleHalfLength);
		link->createShape(capsule,*ragdollMaterial);
		physx::PxRigidBodyExt::updateMassAndInertia(*link, 50.0f);

		if(parentIdx!= -1) {
			physx::PxArticulationJoint *joint = link->getInboundJoint();
			physx::PxQuat frameRotation = parentNode->globalRotation.getConjugate() * currentNodePtr->globalRotation;
			physx::PxTransform parentConstraintFrame = physx::PxTransform(physx::PxVec3(currentNodePtr->parentLinkPos * parentHalfLength,0,0),frameRotation);
			physx::PxTransform thisConstraintFrame = physx::PxTransform(physx::PxVec3(currentNodePtr->childLinkPos * childHalfLength,0,0));
			joint->setParentPose(parentConstraintFrame);
			joint->setChildPose(thisConstraintFrame);

			joint->setSpring(20);
			joint->setDamping(100);
			joint->setSwingLimitEnabled(true);
			joint->setSwingLimit(0.4f,0.4f);
			joint->setTwistLimit(0.4f,0.4f);
			joint->setTwistLimitEnabled(true);
		}
		currentNode++;
	}

	g_PhysicsScene->addArticulation(*ragDollArticulation);

	long long l = getHash(name);
	g_PhysXActorsRagDolls[l] = ragDollArticulation;

	return ragDollArticulation;
}
void PhysXScene::AddCloth(char* name,ClothData *ClothIn,float f_springSize, unsigned int ui_SpringRows, unsigned int ui_SprtingCols){
	if (ClothIn == nullptr){return;}
	// shifting grid position for looks
	float halfWidth = ui_SpringRows * f_springSize * 0.5f;
	// generate vertices for a grid with texture coordinates
	ClothIn->m_clothVertexCount = ui_SpringRows * ui_SprtingCols;
	ClothIn->m_clothPositions = new glm::vec3[ClothIn->m_clothVertexCount];
	glm::vec2* clothTextureCoords = new glm::vec2[ClothIn->m_clothVertexCount];
	for (unsigned int r = 0; r < ui_SpringRows; ++r){
		for (unsigned int c = 0; c < ui_SprtingCols; ++c){
			ClothIn->m_clothPositions[r * ui_SprtingCols + c].x = ClothIn->clothPosition.x + f_springSize * c;
			ClothIn->m_clothPositions[r * ui_SprtingCols + c].y = ClothIn->clothPosition.y;
			ClothIn->m_clothPositions[r * ui_SprtingCols + c].z = ClothIn->clothPosition.z + f_springSize * r - halfWidth;
			clothTextureCoords[r * ui_SprtingCols + c].x = 1.0f - r / (ui_SpringRows - 1.0f);
			clothTextureCoords[r * ui_SprtingCols + c].y = 1.0f - c / (ui_SprtingCols - 1.0f);
		}
	}
	// set up indices for a grid
	ClothIn->m_clothIndexCount = (ui_SpringRows-1) * (ui_SprtingCols-1) * 2 * 3;
	unsigned int* indices = new unsigned int[ ClothIn->m_clothIndexCount ];
	unsigned int* index = indices;
	for (unsigned int r = 0; r < (ui_SpringRows-1); ++r){
		for (unsigned int c = 0; c < (ui_SprtingCols-1); ++c){
			// indices for the 4 quad corner vertices
			unsigned int i0 = r * ui_SprtingCols + c;
			unsigned int i1 = i0 + 1;
			unsigned int i2 = i0 + ui_SprtingCols;
			unsigned int i3 = i2 + 1;
			// every second quad changes the triangle order
			if ((c + r) % 2){
				*index++ = i0; *index++ = i2; *index++ = i1;
				*index++ = i1; *index++ = i2; *index++ = i3;
			}else{
				*index++ = i0; *index++ = i2; *index++ = i3;
				*index++ = i0; *index++ = i3; *index++ = i1;
			}
		}
	}

	// set up opengl data for the grid, with the positions as dynamic
	glGenVertexArrays(1, &ClothIn->m_clothVAO);
	glBindVertexArray(ClothIn->m_clothVAO);
	glGenBuffers(1, &ClothIn->m_clothIBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ClothIn->m_clothIBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, ClothIn->m_clothIndexCount * sizeof(unsigned int), indices, GL_STATIC_DRAW);
	glGenBuffers(1, &ClothIn->m_clothVBO);
	glBindBuffer(GL_ARRAY_BUFFER, ClothIn->m_clothVBO);
	glBufferData(GL_ARRAY_BUFFER, ClothIn->m_clothVertexCount * (sizeof(glm::vec3)), 0, GL_DYNAMIC_DRAW);
	glEnableVertexAttribArray(0); // position
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), (char*)0);
	glGenBuffers(1, &ClothIn->m_clothTextureVBO);
	glBindBuffer(GL_ARRAY_BUFFER, ClothIn->m_clothTextureVBO);
	glBufferData(GL_ARRAY_BUFFER, ClothIn->m_clothVertexCount * (sizeof(glm::vec2)), clothTextureCoords, GL_STATIC_DRAW);
	glEnableVertexAttribArray(1); // texture
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(glm::vec2), (char*)0);
	glBindVertexArray(0);

	unsigned int vs = Utility::loadShader("shaders/03_Physics/16_SoftBodies.vert", GL_VERTEX_SHADER);
	unsigned int fs = Utility::loadShader("shaders/03_Physics/16_SoftBodies.frag", GL_FRAGMENT_SHADER);
	ClothIn->m_shader = Utility::createProgram(vs,0,0,0,fs);
	glDeleteShader(vs);
	glDeleteShader(fs);
	ClothIn->m_texture = SOIL_load_OGL_texture("textures/random.png",SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_INVERT_Y);

	// set up the cloth description
	physx::PxClothMeshDesc clothDesc;
	clothDesc.setToDefault();
	clothDesc.points.count = ClothIn->m_clothVertexCount;
	clothDesc.points.stride = sizeof(glm::vec3);
	clothDesc.points.data = ClothIn->m_clothPositions;
	clothDesc.triangles.count = ClothIn->m_clothIndexCount / 3;
	clothDesc.triangles.stride = sizeof(unsigned int) * 3;
	clothDesc.triangles.data = indices;
	// cook the geometry into fabric
	PxToolkit::MemoryOutputStream buf;
	if (g_PhysicsCooker->cookClothFabric(clothDesc, physx::PxVec3(0,-9.8f,0), buf) == false) {
		return;
	}
	//$(SolutionDir)../3rdParty/PhysX/Samples/PxToolkit/lib/win32
	PxToolkit::MemoryInputData data(buf.getData(), buf.getSize());
	physx::PxClothFabric* fabric = g_Physics->createClothFabric(data);

	//Coll data
	//physx::PxClothCollisionData cd;
	//cd.setToDefault();
	//cd.numSpheres = 32;
	//cd.pairIndexBuffer = 0;
	//ClothIn->Spheres = new physx::PxClothCollisionSphere[32];
	//for (int x = 0; x < 32; x++){
	//	ClothIn->Spheres[x].pos = physx::PxVec3(rand()%10,5 + rand()%5,-5 + rand()%10);
	//	ClothIn->Spheres[x].radius = 0.5f + rand()%100 / 100.0f;
	//}
	//cd.spheres = ClothIn->Spheres;

	// set up the particles for each vertex
	physx::PxClothParticle* particles = new physx::PxClothParticle[ ClothIn->m_clothVertexCount ];
	for ( unsigned int i = 0 ; i < ClothIn->m_clothVertexCount ; ++i ){
	particles[i].pos = physx::PxVec3( ClothIn->m_clothPositions[i].x, ClothIn->m_clothPositions[i].y, ClothIn->m_clothPositions[i].z );

	// set weights (0 means static)
	if ( ClothIn->m_clothPositions[i].x == ClothIn->clothPosition.x )
		particles[i].invWeight = 0;
	else
		particles[i].invWeight = 2.f;
	}

	// create the cloth then setup the spring properties
	ClothIn->m_cloth = g_Physics->createCloth(physx::PxTransform( physx::PxVec3(ClothIn->clothPosition.x,ClothIn->clothPosition.y,ClothIn->clothPosition.z) ),*fabric, particles, physx::PxClothCollisionData(), physx::PxClothFlag::eSWEPT_CONTACT);
	// we need to set some solver configurations
	if (ClothIn->m_cloth != nullptr){
		physx::PxClothPhaseSolverConfig bendCfg;
		bendCfg.solverType = physx::PxClothPhaseSolverConfig::eFAST;
		bendCfg.stiffness = 1.0f;
		bendCfg.stretchStiffness = 1.0f;
		ClothIn->m_cloth->setPhaseSolverConfig(physx::PxClothFabricPhaseType::eBENDING, bendCfg);
		ClothIn->m_cloth->setPhaseSolverConfig(physx::PxClothFabricPhaseType::eSTRETCHING, bendCfg);
		ClothIn->m_cloth->setPhaseSolverConfig(physx::PxClothFabricPhaseType::eSHEARING, bendCfg);
		ClothIn->m_cloth->setPhaseSolverConfig(physx::PxClothFabricPhaseType::eSTRETCHING_HORIZONTAL, bendCfg);
		ClothIn->m_cloth->setDampingCoefficient(1.0f);
	}

	if (ClothIn->m_cloth != nullptr){
		unsigned long long l = getHash(std::string(name));
		g_PhysicsScene->addActor(*ClothIn->m_cloth);
		g_PhysXActorsCloths[l] = ClothIn;
		ClothIn->m_cloth->setName(name);
	}

	delete[] particles;
	delete[] clothTextureCoords;
	delete[] indices;
}

physx::PxJoint* PhysXScene::linkFixed(physx::PxRigidActor* px_Actor1, physx::PxTransform pxt_Transform1, physx::PxRigidActor* px_Actor2 , physx::PxTransform pxt_Transform2){
	if (px_Actor1 == nullptr || px_Actor2 == nullptr){return nullptr;}
	if (px_Actor1 == px_Actor2){return nullptr;}
	physx::PxFixedJoint *joint = PxFixedJointCreate(*g_Physics, px_Actor1, pxt_Transform1, px_Actor2, pxt_Transform2);
	std::string name = px_Actor1->getName();
	name.append(px_Actor2->getName());
	long long l = getHash(name);
	g_PhysXJoints[l] = joint;
	return joint;
}
physx::PxJoint* PhysXScene::linkDistance(physx::PxRigidActor* px_Actor1, physx::PxTransform pxt_Transform1, physx::PxRigidActor* px_Actor2 , physx::PxTransform pxt_Transform2, float f_Min, float f_Max, float f_Spring, float f_Damping){
	if (px_Actor1 == nullptr || px_Actor2 == nullptr){return nullptr;}
	if (px_Actor1 == px_Actor2){return nullptr;}
	physx::PxDistanceJoint *joint = PxDistanceJointCreate(*g_Physics, px_Actor1, pxt_Transform1, px_Actor2, pxt_Transform2);
	joint->setMaxDistance(f_Max);
	joint->setDistanceJointFlag(physx::PxDistanceJointFlag::eMAX_DISTANCE_ENABLED, true);
	joint->setMinDistance(f_Min);
	joint->setDistanceJointFlag(physx::PxDistanceJointFlag::eMIN_DISTANCE_ENABLED, true);
	if (f_Spring != 0.0f){
		joint->setSpring(f_Spring);
		joint->setDamping(f_Damping);
		joint->setDistanceJointFlag(physx::PxDistanceJointFlag::eSPRING_ENABLED, true);
	}
	std::string name = px_Actor1->getName();
	name.append(px_Actor2->getName());
	long long l = getHash(name);
	g_PhysXJoints[l] = joint;
	return joint;
}
physx::PxJoint* PhysXScene::linkSherical(physx::PxRigidActor* px_Actor1, physx::PxTransform pxt_Transform1, physx::PxRigidActor* px_Actor2 , physx::PxTransform pxt_Transform2, float f_YLimit, float f_Zlimit, float f_ContactDistance){
	if (px_Actor1 == nullptr || px_Actor2 == nullptr){return nullptr;}
	if (px_Actor1 == px_Actor2){return nullptr;}
	physx::PxSphericalJoint *joint = PxSphericalJointCreate(*g_Physics, px_Actor1, pxt_Transform1, px_Actor2, pxt_Transform2);
	joint->setLimitCone(physx::PxJointLimitCone(f_YLimit,f_Zlimit,f_ContactDistance));
	joint->setSphericalJointFlag(physx::PxSphericalJointFlag::eLIMIT_ENABLED, true);
	std::string name = px_Actor1->getName();
	name.append(px_Actor2->getName());
	long long l = getHash(name);
	g_PhysXJoints[l] = joint;
	return joint;
}
physx::PxJoint* PhysXScene::linkRevolute(physx::PxRigidActor* px_Actor1, physx::PxTransform pxt_Transform1, physx::PxRigidActor* px_Actor2 , physx::PxTransform pxt_Transform2, float f_Lower, float f_Upper, float f_ContactDistance){
	if (px_Actor1 == nullptr || px_Actor2 == nullptr){return nullptr;}
	if (px_Actor1 == px_Actor2){return nullptr;}
	physx::PxRevoluteJoint *joint = PxRevoluteJointCreate(*g_Physics, px_Actor1, pxt_Transform1, px_Actor2, pxt_Transform2);
	joint->setLimit(physx::PxJointLimitPair(f_Lower,f_Upper,f_ContactDistance));
	joint->setRevoluteJointFlag(physx::PxRevoluteJointFlag::eLIMIT_ENABLED, true);
	std::string name = px_Actor1->getName();
	name.append(px_Actor2->getName());
	long long l = getHash(name);
	g_PhysXJoints[l] = joint;
	return joint;
}
physx::PxJoint* PhysXScene::linkPrismatic(physx::PxRigidActor* px_Actor1, physx::PxTransform pxt_Transform1, physx::PxRigidActor* px_Actor2 , physx::PxTransform pxt_Transform2, float f_Lower, float f_Upper, float f_ContactDistance){
	if (px_Actor1 == nullptr || px_Actor2 == nullptr){return nullptr;}
	if (px_Actor1 == px_Actor2){return nullptr;}
	physx::PxPrismaticJoint *joint = PxPrismaticJointCreate(*g_Physics, px_Actor1, pxt_Transform1, px_Actor2, pxt_Transform2);
	joint->setLimit(physx::PxJointLimitPair(f_Lower,f_Upper,f_ContactDistance));
	joint->setPrismaticJointFlag(physx::PxPrismaticJointFlag::eLIMIT_ENABLED, true);
	std::string name = px_Actor1->getName();
	name.append(px_Actor2->getName());
	long long l = getHash(name);
	g_PhysXJoints[l] = joint;
	return joint;
}
physx::PxJoint* PhysXScene::linkD6(physx::PxRigidActor* px_Actor1, physx::PxTransform pxt_Transform1, physx::PxRigidActor* px_Actor2, physx::PxTransform pxt_Transform2){
	if (px_Actor1 == nullptr || px_Actor2 == nullptr){
		return nullptr;}
	if (px_Actor1 == px_Actor2){
		return nullptr;}
	physx::PxD6Joint *joint = PxD6JointCreate(*g_Physics, px_Actor1, pxt_Transform1, px_Actor2, pxt_Transform2);

	std::string name = px_Actor1->getName();
	name.append(px_Actor2->getName());
	long long l = getHash(name);
	g_PhysXJoints[l] = joint;
	return joint;
}

void PhysXScene::createTrigger(physx::PxRigidActor* px_Actor1){
	physx::PxShape* objectShape;
	px_Actor1->getShapes(&objectShape, 1);
	objectShape->setFlag(physx::PxShapeFlag::eSIMULATION_SHAPE, false);
	objectShape->setFlag(physx::PxShapeFlag::eTRIGGER_SHAPE, true);
}

void PhysXScene::controlActor(float a_deltaTime, const glm::mat4& m_camera, physx::PxRigidActor* actor, float f_Force){
	if (actor == nullptr){return;}
	if (actor->getType() != physx::PxActorType::eRIGID_DYNAMIC){return;}

	physx::PxRigidDynamic *Actor = (physx::PxRigidDynamic*)actor;
	setupFiltering(Actor,FilterGroup::ePLAYER,FilterGroup::eGROUND | FilterGroup::ePLATFORM); //set up the collisionfiltering for our player
	physx::PxTransform pose = Actor->getGlobalPose();
	pose.q = physx::PxQuat(glm::half_pi<float>(),physx::PxVec3(0,0,1));
	Actor->setGlobalPose(pose);

	glm::vec4 vForward = m_camera[2];
	glm::vec4 vRight = m_camera[0];

	GLFWwindow *window = glfwGetCurrentContext();
	if (glfwGetKey(window,GLFW_KEY_RIGHT_CONTROL) == GLFW_PRESS){
		Actor->addForce((physx::PxVec3(0,10,0) * Actor->getMass() * f_Force) * a_deltaTime);
	}else if (glfwGetKey(window,GLFW_KEY_RIGHT_ALT) == GLFW_PRESS){
		Actor->setLinearVelocity(physx::PxVec3(0),false);
	}
	if (glfwGetKey(window,GLFW_KEY_UP) == GLFW_PRESS){
		Actor->addForce((physx::PxVec3(vForward.x * -10,0,vForward.z * -10) * Actor->getMass() * f_Force) * a_deltaTime);
	}else if (glfwGetKey(window,GLFW_KEY_DOWN) == GLFW_PRESS){
		Actor->addForce((physx::PxVec3(vForward.x * 10,0,vForward.z * 10) * Actor->getMass() * f_Force) * a_deltaTime);
	}
	if (glfwGetKey(window,GLFW_KEY_RIGHT) == GLFW_PRESS){
		Actor->addForce((physx::PxVec3(vRight.x * 10,0,vRight.z * 10) * Actor->getMass() *  f_Force) * a_deltaTime);
	}else if (glfwGetKey(window,GLFW_KEY_LEFT) == GLFW_PRESS){
		Actor->addForce((physx::PxVec3(vRight.x * -10,0,vRight.z * -10) * Actor->getMass() *  f_Force) * a_deltaTime);
	}
}
void PhysXScene::snapActor(physx::PxRigidActor* actor,glm::vec3 v3_Position){
	if (actor == nullptr){return;}
	if (actor->getType() != physx::PxActorType::eRIGID_DYNAMIC){return;}
	physx::PxRigidDynamic *Actor = (physx::PxRigidDynamic*)actor;
	physx::PxTransform pose = Actor->getGlobalPose();
	pose.p = physx::PxVec3(v3_Position.x,v3_Position.y,v3_Position.z);
	pose.q = physx::PxQuat(glm::half_pi<float>(),physx::PxVec3(0,0,1));
	Actor->setGlobalPose(pose);
}

