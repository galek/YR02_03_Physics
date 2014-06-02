#pragma once

#include <unordered_map>
#include <glm\ext.hpp>
#include <Gizmos.h>
#include <vector>
#include "Shape.hpp"

namespace Osiris{

	//!-- SHAPE

	Shape::Shape(){
		v4Colour = glm::vec4(1,1,1,0.5f);
	}

	Shape::~Shape(){
	
	}

	const glm::vec4&	Shape::getColour		()									{return v4Colour;}
	void				Shape::setColour		(const glm::vec4& v4_Colour)		{v4Colour = v4_Colour;}

	const ShapeType		Shape::getType			()									{return sType;}

	//!-- SPHERE

	Sphere::Sphere(){
		sType = ShapeType::SPHERE;
	}

	Sphere::~Sphere(){
	
	}

	const float			Sphere::getRadius			()									{return fRadius;}
	void				Sphere::setRadius			(const float& f_Radius)				{fRadius = f_Radius;}

	const ShapeType		Sphere::getType				()									{return sType;}

	void Sphere::draw(){
		Gizmos::addSphere(m4Model[3].xyz,5,5,fRadius,v4Colour,&m4Model);
	}

	//!-- AABB

	AxisBox::AxisBox(){
		sType = ShapeType::AABB;
	}

	AxisBox::~AxisBox(){
	
	}

	const glm::vec3&	AxisBox::getExtents			()									{return v3Extents;}
	void				AxisBox::setExtents			(const glm::vec3& v3_Extents)		{v3Extents = v3_Extents;}

	const ShapeType		AxisBox::getType			()									{return sType;}

	void AxisBox::draw(){
		Gizmos::addAABBFilled(m4Model[3].xyz,v3Extents,v4Colour);
	}

	//!-- BOX

	Box::Box(){
		sType = ShapeType::BOX;
	}

	Box::~Box(){
	
	}

	const glm::vec3&	Box::getExtents			()									{return v3Extents;}
	void				Box::setExtents			(const glm::vec3& v3_Extents)		{v3Extents = v3_Extents;}

	const ShapeType		Box::getType				()									{return sType;}

	void Box::draw(){
		Gizmos::addAABBFilled(m4Model[3].xyz,v3Extents,v4Colour,&m4Model);
	} 

	//!-- PLANE

	Plane::Plane(){
		sType = ShapeType::PLANE;
	}

	Plane::~Plane(){
	
	}
		
	const glm::vec3&	Plane::getUp				()									{return v3Normal;}
	void				Plane::setUp				(const glm::vec3& v3_Up)			{v3Normal = v3_Up;}

	const ShapeType		Plane::getType				()									{return sType;}

	void Plane::draw(){
		glm::vec3 p1(-100,0,-100);
		glm::vec3 p2(-100,0, 100);
		glm::vec3 p3( 100,0, 100);
		glm::vec3 p4( 100,0,-100);

		//Rotate above points 

		Gizmos::addTri(p1,p2,p3,v4Colour);
		Gizmos::addTri(p1,p3,p4,v4Colour);
	}

	//!-- MESH

	Mesh::Mesh(){
		sType = ShapeType::MESH;
	}

	Mesh::~Mesh(){
	
	}

	void Mesh::draw(){
		for (unsigned int i = 0; i < vVerts.size(); i += 3){
			Gizmos::addTri(vVerts[i + 0],vVerts[i + 1],vVerts[i + 2],v4Colour);
		}
	}

	//!-- RAY
	Ray::Ray(){
		sType = ShapeType::RAY;
		v3Direction = glm::vec3(0);
	}
	Ray::~Ray(){
	
	}
	const glm::vec3& Ray::getDirection(){
		return v3Direction;
	}
	void Ray::setDirection(const glm::vec3& v3_Direction){
		v3Direction = glm::normalize(v3_Direction);
	}
	void Ray::draw(){
		Gizmos::addLine(m4Model[3].xyz,m4Model[3].xyz + (v3Direction * 1000000.0f),v4Colour,v4Colour);
	}

	Line::Line(){
		sType = ShapeType::LINE;
		v3Direction = glm::vec3(0);
		fDistance = 1.0f;
	}
	Line::~Line(){

	}
	const glm::vec3& Line::getDirection(){
		return v3Direction;
	}
	void Line::setDirection(const glm::vec3& v3_Direction){
		if (v3_Direction != glm::vec3(0.0f)){
		v3Direction = glm::normalize(v3_Direction);
		}else{
			v3Direction = v3_Direction;
		}
	}

	const float& Line::getDistance(){
		return fDistance;
	}
	void Line::setDistance(const float& f_Distance){
		fDistance = f_Distance;
	}
	void Line::draw(){
		Gizmos::addLine(m4Model[3].xyz,m4Model[3].xyz + (v3Direction * fDistance),v4Colour,v4Colour);
	}
}

