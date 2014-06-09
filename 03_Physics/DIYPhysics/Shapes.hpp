#pragma once
#include "Shape.hpp"
#include "Gizmos.h"
#include <glm/ext.hpp>
#include <limits>

class Plane : public Shape {
public:
	glm::vec3 _Forward, _Up, _Right;

public:
	Plane(glm::vec3 Normal, glm::vec3 Offset, glm::vec4 Colour) : Shape(Offset, Colour) { 
		_Up = glm::normalize(Normal); 
		_Right = glm::cross(_Up, glm::vec3(0,0,1));
		_Forward = glm::cross(_Up, -_Right);

		_Type = ShapeType::PLANE; 
	}
	virtual ~Plane() { }

	virtual void Draw(glm::vec3 Position) {
		glm::vec3 P1	= Position + ( _Forward - _Right) * 10000.0f;
		glm::vec3 P2	= Position + ( _Forward + _Right) * 10000.0f;
		glm::vec3 P3	= Position + (-_Forward - _Right) * 10000.0f;
		glm::vec3 P4	= Position + (-_Forward + _Right) * 10000.0f;

		Gizmos::addTri(P1, P2, P3, _Colour);
		Gizmos::addTri(P3, P2, P4, _Colour);
	}
};

class AABB : public Shape {
public:
	glm::vec3 _Extents;

public:
	AABB(glm::vec3 Extents, glm::vec3 Offset, glm::vec4 Colour) : Shape(Offset, Colour) { _Extents = Extents; _Type = ShapeType::BOX_AABB; }
	virtual ~AABB() { }

	virtual void Draw(glm::vec3 Position) {
		Gizmos::addAABBFilled(Position + _Offset, _Extents, _Colour);
	}
};

class Sphere : public Shape {
public:
	float _Radius;

public:
	Sphere(float Radius, glm::vec3 Offset, glm::vec4 Colour) : Shape(Offset, Colour) { _Radius = Radius; _Type = ShapeType::SPHERE; }
	virtual ~Sphere() { }

	virtual void Draw(glm::vec3 Position) {
		Gizmos::addSphere(Position + _Offset, 5, 5, _Radius, _Colour);
	}
};

