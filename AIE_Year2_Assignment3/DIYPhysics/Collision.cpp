#include "Collision.hpp"

#include "Actor.hpp"
#include "Shape.hpp"
#include "Shapes.hpp"

bool Collision::Shape_Shape(Shape *A, Shape *B, Data &data) {

	//	SphereSphere
	if (A->_Type == ShapeType::SPHERE && B->_Type == ShapeType::SPHERE) {
		Sphere *AA = dynamic_cast<Sphere*>(A);
		Sphere *BB = dynamic_cast<Sphere*>(B);
		return Sphere_Sphere(AA, BB, data);
	}

	//	PlaneSphere
	if (A->_Type == ShapeType::PLANE && B->_Type == ShapeType::SPHERE) {
		Plane *AA = dynamic_cast<Plane*>(A);
		Sphere *BB = dynamic_cast<Sphere*>(B);
		return Plane_Sphere(AA, BB, data);
	}
	else if (A->_Type == ShapeType::SPHERE && B->_Type == ShapeType::PLANE) {
		Sphere *AA = dynamic_cast<Sphere*>(A);
		Plane *BB = dynamic_cast<Plane*>(B);
		return Plane_Sphere(BB, AA, data);
	}

	return false;
}

bool Collision::AABB_AABB(AABB *A, AABB *B, Data &data) {

	glm::mat4 ATrans = A->_Parent->_Transform;
	glm::mat4 BTrans = B->_Parent->_Transform;

	glm::vec3 APos = ATrans[3].xyz + A->_Offset;
	glm::vec3 BPos = BTrans[3].xyz + B->_Offset;
	glm::vec3 AMin = APos + -A->_Extents; 
	glm::vec3 AMax = APos + A->_Extents;
	glm::vec3 BMin = BPos + -B->_Extents; 
	glm::vec3 BMax = BPos + B->_Extents;

	if (AMin.x > BMax.x) return false;
	if (AMin.y > BMax.y) return false;
	if (AMin.z > BMax.z) return false;
	if (AMax.x < BMin.x) return false;
	if (AMax.y < BMin.y) return false;
	if (AMax.z < BMin.z) return false;

	//	Get the Collision Normal
	data.Normal = glm::normalize(BPos - APos);
	data.Depth = 0.0f;

	return true;
}
bool Collision::Plane_Plane(Plane *A, Plane *B, Data &data) {
	return false;
}
bool Collision::Sphere_Sphere(Sphere *A, Sphere *B, Data &data) {

	glm::mat4 ATrans = A->_Parent->_Transform;
	glm::mat4 BTrans = B->_Parent->_Transform;

	glm::vec3 APos = ATrans[3].xyz + A->_Offset;
	glm::vec3 BPos = BTrans[3].xyz + B->_Offset;

	////	Optimized?
	//float radius2 = pow(A->_Radius + B->_Radius,2);
	//float distance2 = glm::distance2(APos, BPos);

	//if (distance2 > radius2)
	//	return false;

	//data.Normal = glm::normalize(BPos - APos);
	//data.Depth = radius2 - distance2;

	//	Unoptimized
	float radius = A->_Radius + B->_Radius;
	float distance = glm::distance(APos, BPos);
	if (distance > radius)
		return false;

	data.Normal = glm::normalize(BPos - APos);
	data.Depth = radius - distance;

	return true;
}

bool Collision::Plane_Sphere(Plane *A, Sphere *B, Data &data) {

	glm::mat4 ATrans = A->_Parent->_Transform;
	glm::mat4 BTrans = B->_Parent->_Transform;

	glm::vec3 APos = ATrans[3].xyz + A->_Offset;
	glm::vec3 BPos = BTrans[3].xyz + B->_Offset;

	float distance = glm::abs(glm::dot(BPos, A->_Up) - glm::length(APos));

	if (distance > B->_Radius)
		return false;

	data.Normal = A->_Up;
	data.Depth = B->_Radius - distance;
	return true;
}

//====================================================================

void Collide::Shape_Shape(Shape *A, Shape *B, Data data) {

	if (A->_Type == ActorType::STATIC && B->_Type == ActorType::STATIC)
		return;

	//	SphereSphere
	if (A->_Type == ShapeType::SPHERE && B->_Type == ShapeType::SPHERE) {
		Sphere *AA = dynamic_cast<Sphere*>(A);
		Sphere *BB = dynamic_cast<Sphere*>(B);
		return Sphere_Sphere(AA, BB, data);
	}

	//	PlaneSphere
	if (A->_Type == ShapeType::PLANE && B->_Type == ShapeType::SPHERE) {
		Plane *AA = dynamic_cast<Plane*>(A);
		Sphere *BB = dynamic_cast<Sphere*>(B);
		return Plane_Sphere(AA, BB, data);
	}
	else if (A->_Type == ShapeType::SPHERE && B->_Type == ShapeType::PLANE) {
		Sphere *AA = dynamic_cast<Sphere*>(A);
		Plane *BB = dynamic_cast<Plane*>(B);
		return Plane_Sphere(BB, AA, data);
	}
}

void Collide::Sphere_Sphere(Sphere *A, Sphere *B, Data data) {

	//	Relative Velocity
	glm::vec3 relativeVelocity = A->_Parent->_Velocity - B->_Parent->_Velocity;

	//	Velocity Along Normal
	glm::vec3 collisionVector = data.Normal * glm::dot(relativeVelocity, data.Normal);

	glm::vec3 forceVector = collisionVector * 2.0f/(A->_Parent->_InvMass + B->_Parent->_InvMass);
	glm::vec3 seperationVector = data.Normal * data.Depth * 0.505f;

	//	Static Checks
	if (A->_Parent->_Type ==  ActorType::DYNAMIC && B->_Parent->_Type ==  ActorType::STATIC) {
		A->_Parent->_Transform[3].xyz -= seperationVector;
		A->_Parent->ApplyForce(-forceVector * 2.0f);
	}
	else if (A->_Parent->_Type ==  ActorType::STATIC && B->_Parent->_Type ==  ActorType::DYNAMIC) {
		B->_Parent->_Transform[3].xyz += seperationVector;
		B->_Parent->ApplyForce( forceVector * 2.0f);
	} 
	else {
		A->_Parent->_Transform[3].xyz -= seperationVector * 0.505f;
		B->_Parent->_Transform[3].xyz += seperationVector * 0.505f;

		A->_Parent->ApplyForce(-forceVector);
		B->_Parent->ApplyForce( forceVector);
	}
}

void Collide::Plane_Sphere(Plane *A, Sphere *B, Data &data) {

	//	Store Plane Normal
	glm::vec3 planeNormal = A->_Up;

	//	Distance to Plane
	float distance = glm::dot(planeNormal, B->_Parent->_Transform[3].xyz()) - glm::distance(B->_Parent->_Transform[3].xyz(), A->_Parent->_Transform[3].xyz());

	if (distance < 0)
		planeNormal *= -1.0f;

	glm::vec3 forceVector = B->_Parent->_Mass * planeNormal * (glm::dot(planeNormal,B->_Parent->_Velocity));
	glm::vec3 seperationVector = data.Normal * data.Depth;

	//	Static Checks
	if (A->_Parent->_Type ==  ActorType::DYNAMIC && B->_Parent->_Type ==  ActorType::STATIC) {
		A->_Parent->_Transform[3].xyz -= seperationVector;
		A->_Parent->ApplyForce(forceVector * 2.0f);
	}
	else if (A->_Parent->_Type ==  ActorType::STATIC && B->_Parent->_Type ==  ActorType::DYNAMIC) {
		B->_Parent->_Transform[3].xyz += seperationVector;
		B->_Parent->ApplyForce(-forceVector * 2.0f);
	} 
	else {
		A->_Parent->_Transform[3].xyz -= seperationVector * 0.505f;
		B->_Parent->_Transform[3].xyz += seperationVector * 0.505f;

		A->_Parent->ApplyForce( forceVector);
		B->_Parent->ApplyForce(-forceVector);
	}
}