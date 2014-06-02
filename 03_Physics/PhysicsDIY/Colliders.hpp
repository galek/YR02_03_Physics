#pragma once

#include <glm\glm.hpp>

namespace Osiris{

	//!--COLLISIONS

	class Shape;
	class Sphere;
	class AxisBox;
	class Box;
	class Plane;
	class Ray;
	class Line;
	class Mesh;
	class Actor;

	typedef bool (*CollisionCheck)(Shape*,Shape*);

	void Actor_Actor(Actor *a_Actor1,Actor *a_Actor2,glm::vec3 v3_CollisionPoint);
	bool Shape_Shape(Shape *s_Shape1,Shape *s_Shape2,glm::vec3 &v3_CollisionPoint);

	bool Sphere_Sphere	(Sphere *s_Shape1	, Sphere	*s_Shape2	, glm::vec3 &v3_CollisionPoint);
	bool Sphere_AABB	(Sphere *s_Shape1	, AxisBox	*s_Shape2	, glm::vec3 &v3_CollisionPoint);
	bool Sphere_Box		(Sphere *s_Shape1	, Box		*s_Shape2	, glm::vec3 &v3_CollisionPoint);
	bool Sphere_Plane	(Sphere *s_Shape1	, Plane		*s_Shape2	, glm::vec3 &v3_CollisionPoint);
	bool Sphere_Ray		(Sphere *s_Shape1	, Ray		*s_Shape2	, glm::vec3 &v3_CollisionPoint);
	bool Sphere_Line	(Sphere *s_Shape1	, Line		*s_Shape2	, glm::vec3 &v3_CollisionPoint);
	bool Sphere_Mesh	(Sphere *s_Shape1	, Mesh		*s_Shape2	, glm::vec3 &v3_CollisionPoint);

	bool AABB_Sphere	(AxisBox *s_Shape1	, Sphere	*s_Shape2	, glm::vec3 &v3_CollisionPoint);
	bool AABB_AABB		(AxisBox *s_Shape1	, AxisBox	*s_Shape2	, glm::vec3 &v3_CollisionPoint);
	bool AABB_Box		(AxisBox *s_Shape1	, Box		*s_Shape2	, glm::vec3 &v3_CollisionPoint);
	bool AABB_Plane		(AxisBox *s_Shape1	, Plane		*s_Shape2	, glm::vec3 &v3_CollisionPoint);
	bool AABB_Ray		(AxisBox *s_Shape1	, Ray		*s_Shape2	, glm::vec3 &v3_CollisionPoint);
	bool AABB_Line		(AxisBox *s_Shape1	, Line		*s_Shape2	, glm::vec3 &v3_CollisionPoint);
	bool AABB_Mesh		(AxisBox *s_Shape1	, Mesh		*s_Shape2	, glm::vec3 &v3_CollisionPoint);

	bool Box_Sphere		(Box	*s_Shape1	, Sphere	*s_Shape2	, glm::vec3 &v3_CollisionPoint);
	bool Box_AABB		(Box	*s_Shape1	, AxisBox	*s_Shape2	, glm::vec3 &v3_CollisionPoint);
	bool Box_Box		(Box	*s_Shape1	, Box		*s_Shape2	, glm::vec3 &v3_CollisionPoint);
	bool Box_Plane		(Box	*s_Shape1	, Plane		*s_Shape2	, glm::vec3 &v3_CollisionPoint);
	bool Box_Ray		(Box	*s_Shape1	, Ray		*s_Shape2	, glm::vec3 &v3_CollisionPoint);
	bool Box_Line		(Box	*s_Shape1	, Line		*s_Shape2	, glm::vec3 &v3_CollisionPoint);
	bool Box_Mesh		(Box	*s_Shape1	, Mesh		*s_Shape2	, glm::vec3 &v3_CollisionPoint);

	bool Plane_Sphere	(Plane	*s_Shape1	, Sphere	*s_Shape2	, glm::vec3 &v3_CollisionPoint);
	bool Plane_AABB		(Plane	*s_Shape1	, AxisBox	*s_Shape2	, glm::vec3 &v3_CollisionPoint);
	bool Plane_Box		(Plane	*s_Shape1	, Box		*s_Shape2	, glm::vec3 &v3_CollisionPoint);
	bool Plane_Plane	(Plane	*s_Shape1	, Plane		*s_Shape2	, glm::vec3 &v3_CollisionPoint);
	bool Plane_Ray		(Plane	*s_Shape1	, Ray		*s_Shape2	, glm::vec3 &v3_CollisionPoint);
	bool Plane_Line		(Plane	*s_Shape1	, Line		*s_Shape2	, glm::vec3 &v3_CollisionPoint);
	bool Plane_Mesh		(Plane	*s_Shape1	, Mesh		*s_Shape2	, glm::vec3 &v3_CollisionPoint);

	bool Ray_Sphere		(Ray	*s_Shape1	, Sphere	*s_Shape2	, glm::vec3 &v3_CollisionPoint);
	bool Ray_AABB		(Ray	*s_Shape1	, AxisBox	*s_Shape2	, glm::vec3 &v3_CollisionPoint);
	bool Ray_Box		(Ray	*s_Shape1	, Box		*s_Shape2	, glm::vec3 &v3_CollisionPoint);
	bool Ray_Plane		(Ray	*s_Shape1	, Plane		*s_Shape2	, glm::vec3 &v3_CollisionPoint);
	bool Ray_Ray		(Ray	*s_Shape1	, Ray		*s_Shape2	, glm::vec3 &v3_CollisionPoint);
	bool Ray_Line		(Ray	*s_Shape1	, Line		*s_Shape2	, glm::vec3 &v3_CollisionPoint);
	bool Ray_Mesh		(Ray	*s_Shape1	, Mesh		*s_Shape2	, glm::vec3 &v3_CollisionPoint);

	bool Line_Sphere	(Line	*s_Shape1	, Sphere	*s_Shape2	, glm::vec3 &v3_CollisionPoint);
	bool Line_AABB		(Line	*s_Shape1	, AxisBox	*s_Shape2	, glm::vec3 &v3_CollisionPoint);
	bool Line_Box		(Line	*s_Shape1	, Box		*s_Shape2	, glm::vec3 &v3_CollisionPoint);
	bool Line_Plane		(Line	*s_Shape1	, Plane		*s_Shape2	, glm::vec3 &v3_CollisionPoint);
	bool Line_Ray		(Line	*s_Shape1	, Ray		*s_Shape2	, glm::vec3 &v3_CollisionPoint);
	bool Line_Line		(Line	*s_Shape1	, Line		*s_Shape2	, glm::vec3 &v3_CollisionPoint);
	bool Line_Mesh		(Line	*s_Shape1	, Mesh		*s_Shape2	, glm::vec3 &v3_CollisionPoint);

	bool Mesh_Sphere	(Mesh	*s_Shape1	, Sphere	*s_Shape2	, glm::vec3 &v3_CollisionPoint);
	bool Mesh_AABB		(Mesh	*s_Shape1	, AxisBox	*s_Shape2	, glm::vec3 &v3_CollisionPoint);
	bool Mesh_Box		(Mesh	*s_Shape1	, Box		*s_Shape2	, glm::vec3 &v3_CollisionPoint);
	bool Mesh_Plane		(Mesh	*s_Shape1	, Plane		*s_Shape2	, glm::vec3 &v3_CollisionPoint);
	bool Mesh_Ray		(Mesh	*s_Shape1	, Ray		*s_Shape2	, glm::vec3 &v3_CollisionPoint);
	bool Mesh_Line		(Mesh	*s_Shape1	, Line		*s_Shape2	, glm::vec3 &v3_CollisionPoint);
	bool Mesh_Mesh		(Mesh	*s_Shape1	, Mesh		*s_Shape2	, glm::vec3 &v3_CollisionPoint);

}
