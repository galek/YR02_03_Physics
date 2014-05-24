#ifndef __PhysX_H_
#define __PhysX_H_

#include "Application.h"
#include <glm/glm.hpp>

#include <PxPhysicsAPI.h>
#include <vector>

class myAllocator: public physx::PxAllocatorCallback {
public:
	virtual ~myAllocator() {}
	virtual void* allocate(size_t size, const char* typeName, const char* filename, int line){
		void* pointer = _aligned_malloc(size,16);
		return pointer;
	}
	virtual void deallocate(void* ptr){
		_aligned_free(ptr);
	}
};

// Derived application class that wraps up all globals neatly
class PhysX : public Application {
public:

	PhysX();
	virtual ~PhysX();

protected:

	virtual bool onCreate(int a_argc, char* a_argv[]);
	virtual void onUpdate(float a_deltaTime);
	virtual void onDraw();
	virtual void onDestroy();

	glm::mat4	m_cameraMatrix;
	glm::mat4	m_projectionMatrix;

	void setUpPhysXTutorial();
	void cleanUpPhsyx();
	void upDatePhysx();
	void drawSphere(physx::PxShape* pShape,physx::PxRigidDynamic* actor);
	void drawBox(physx::PxShape* pShape,physx::PxRigidDynamic* actor);
	void AddBullet(const glm::mat4& m4_Camera,float f_Power,float f_radius,float f_density);
	void AddPlane(glm::vec3 v3_Facing);
	void AddBox(const glm::vec3& v3_Transform, const glm::vec3& v3_Dimentions,float f_density);
	void setUpVisualDebugger();
	
	//!- TUTORIAL
	physx::PxFoundation* g_PhysicsFoundation;
	physx::PxPhysics* g_Physics;
	physx::PxScene* g_PhysicsScene;
	physx::PxDefaultErrorCallback gDefaultErrorCallback;
	physx::PxDefaultAllocator gDefaultAllocatorCallback;
	physx::PxSimulationFilterShader gDefaultFilterShader;
	physx::PxMaterial* g_PhysicsMaterial;
	physx::PxCooking* g_PhysicsCooker;
	std::vector<physx::PxRigidDynamic*> g_PhysXActors;
	//!- TUTORIAL
};

#endif // __PhysX_H_