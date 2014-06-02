#pragma once

#include <unordered_map>
#include <glm\ext.hpp>
#include <Gizmos.h>
#include <vector>
#include "Base.hpp"

namespace Osiris{

	//!--SHAPES

	enum ShapeType { AABB, SPHERE, BOX, PLANE, RAY, LINE, MESH };
	#define SHAPETYPE(type) ((type == ShapeType::AABB) ? "AABB" : (type == ShapeType::SPHERE) ? "SPHERE" : (type == ShapeType::BOX) ? "BOX" : (type == ShapeType::PLANE) ? "PLANE" : (type == ShapeType::RAY) ? "RAY" : (type == ShapeType::LINE) ? "LINE" : "MESH")

	class Shape : public _Base {
	public:
		Shape();
		virtual ~Shape();
		const glm::vec4&	getColour();
		void				setColour(const glm::vec4& v4_Colour);
		const ShapeType		getType();
		virtual void draw() = 0;
	protected:
		glm::vec4 v4Colour;
		ShapeType sType;
	};

	class Sphere : public Shape {
	public:
		Sphere();
		virtual ~Sphere();
		const float			getRadius();
		void				setRadius(const float& f_Radius);
		const ShapeType		getType();
		virtual void draw();
	protected:
		float fRadius;
	};

	class AxisBox : public Shape {
	public:
		AxisBox();
		virtual ~AxisBox();
		const glm::vec3&	getExtents();
		void				setExtents(const glm::vec3& v3_Extents);
		const ShapeType		getType();
		virtual void draw();

	protected:
		glm::vec3 v3Extents;
	};

	class Box : public Shape {
	public:
		Box();
		virtual ~Box();
		const glm::vec3&	getExtents();
		void				setExtents(const glm::vec3& v3_Extents);
		const ShapeType		getType();
		virtual void draw();
	protected:
		glm::vec3 v3Extents;
	};

	class Plane : public Shape {
	public:
		Plane();
		virtual ~Plane();		
		const glm::vec3&	getUp();
		void				setUp(const glm::vec3& v3_Up);
		const ShapeType		getType();
		virtual void draw();
	protected:
		glm::vec3 v3Normal;
	};

	class Mesh : public Shape {
	public:
		Mesh();
		virtual ~Mesh();
		virtual void draw();
	protected:
		std::vector<glm::vec3> vVerts;
	};
		
	class Ray : public Shape {
	public:
		Ray();
		virtual ~Ray();		
		const glm::vec3&	getDirection();
		void				setDirection(const glm::vec3& v3_Direction);
		virtual void draw();
	protected:
		glm::vec3 v3Direction;
	};

	class Line : public Shape {
	public:
		Line();
		virtual ~Line();		
		const glm::vec3&	getDirection();
		void				setDirection(const glm::vec3& v3_Direction);

		const float&		getDistance();
		void				setDistance(const float& fDistance);
		virtual void draw();
	protected:
		float fDistance;
		glm::vec3 v3Direction;
	};
}

