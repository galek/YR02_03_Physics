#pragma once

#include <glm\ext.hpp>

// FORWARD DECLARED
class Shape;
class AABB;
class Plane;
class Sphere;
// FORWARD DECLARED

// Collision Data
struct Data {
	glm::vec3 Normal;
	float Depth;
};

// Collision Detection
namespace Collision {

	bool Shape_Shape(Shape *A, Shape *B, Data &data);

	//	Regular
	bool AABB_AABB(AABB *A, AABB *B, Data &data);

	bool Plane_Plane(Plane *A, Plane *B, Data &data);

	bool Sphere_Sphere(Sphere *A, Sphere *B, Data &data);

	//	Irregular
	bool Plane_Sphere(Plane *A, Sphere *B, Data &data);
};

// Collision Responce
namespace Collide {
	void Shape_Shape(Shape *A, Shape *B, Data data);

	void Sphere_Sphere(Sphere *A, Sphere *B, Data data);

	void Plane_Sphere(Plane *A, Sphere *B, Data &data);
};

