#include "Colliders.hpp"
#include "Shape.hpp"
#include "Actor.hpp"
#include "Scene.hpp"

namespace Osiris{
	ShapeType a1,a2;
	//!--COLLISIONS

	bool Shape_Shape(Shape *s_Shape1,Shape *s_Shape2,glm::vec3 &v3_CollisionPoint){
		switch(s_Shape1->getType()){
			case ShapeType::AABB:{
				switch(s_Shape2->getType()){
					case ShapeType::AABB:{
						return AABB_AABB((AxisBox*)s_Shape1,(AxisBox*)s_Shape2,v3_CollisionPoint);
						break;
					}					
					case ShapeType::SPHERE:{
						return AABB_Sphere((AxisBox*)s_Shape1,(Sphere*)s_Shape2,v3_CollisionPoint);
						break;
					}					
					case ShapeType::PLANE:{
						return AABB_Plane((AxisBox*)s_Shape1,(Plane*)s_Shape2,v3_CollisionPoint);
						break;
					}
				}
			}
			case ShapeType::SPHERE:{
				switch(s_Shape2->getType()){
					case ShapeType::SPHERE:{
						return Sphere_Sphere((Sphere*)s_Shape1,(Sphere*)s_Shape2,v3_CollisionPoint);
						break;
					}
					case ShapeType::AABB:{
						return Sphere_AABB((Sphere*)s_Shape1,(AxisBox*)s_Shape2,v3_CollisionPoint);
						break;
					}
					case ShapeType::PLANE:{
						return Sphere_Plane((Sphere*)s_Shape1,(Plane*)s_Shape2,v3_CollisionPoint);
						break;
					}
				}
				break;
			}	
			case ShapeType::PLANE:{
				switch(s_Shape2->getType()){
					case ShapeType::AABB:{
						return Plane_AABB((Plane*)s_Shape1,(AxisBox*)s_Shape2,v3_CollisionPoint);
						break;
					}					
					case ShapeType::SPHERE:{
						return Plane_Sphere((Plane*)s_Shape1,(Sphere*)s_Shape2,v3_CollisionPoint);
						break;
					}					
					case ShapeType::PLANE:{
						return Plane_Plane((Plane*)s_Shape1,(Plane*)s_Shape2,v3_CollisionPoint);
						break;
					}
				}
			}				   
		}
		return false;
	}

	void Actor_Actor(Actor *a_Actor1,Actor *a_Actor2,glm::vec3 v3_CollisionPoint){
		if (a_Actor1->getType() == ActorType::DYNAMIC){
			printf("%s %s \n",SHAPETYPE(a1),SHAPETYPE(a2));
			if ( v3_CollisionPoint == glm::vec3(0.0f) ) { v3_CollisionPoint = glm::vec3(glm::epsilon<float>()); }
			glm::vec3 vel = ((ActorDynamic*)a_Actor1)->getVelocity();
			glm::vec3 pos(0);
			((ActorDynamic*)a_Actor1)->setTranslate(pos + v3_CollisionPoint);

			pos.x = (v3_CollisionPoint.x == 0.0f) ? vel.x : -vel.x;
			pos.y = (v3_CollisionPoint.y == 0.0f) ? vel.y : -vel.y;
			pos.z = (v3_CollisionPoint.z == 0.0f) ? vel.z : -vel.z;

			glm::vec3 friction = 1.0f - (((ActorDynamic*)a_Actor1)->getFriction() + ((ActorDynamic*)a_Actor2)->getFriction());
			pos *= friction;
			((ActorDynamic*)a_Actor1)->setVelocity(pos);
			printf("v %f %f %f \n",pos.x,pos.y,pos.z);
		}
		if (a_Actor2->getType() == ActorType::DYNAMIC){
			printf("%s %s \n",SHAPETYPE(a2),SHAPETYPE(a1));
			if ( v3_CollisionPoint == glm::vec3(0.0f) ) { v3_CollisionPoint = -glm::vec3(glm::epsilon<float>()); }
			glm::vec3 vel = ((ActorDynamic*)a_Actor2)->getVelocity();
			glm::vec3 pos(0);
			((ActorDynamic*)a_Actor2)->setTranslate(pos + v3_CollisionPoint);

			pos.x = (v3_CollisionPoint.x == 0.0f) ? vel.x : -vel.x;
			pos.y = (v3_CollisionPoint.y == 0.0f) ? vel.y : -vel.y;
			pos.z = (v3_CollisionPoint.z == 0.0f) ? vel.z : -vel.z;

			glm::vec3 friction = 1.0f - (((ActorDynamic*)a_Actor2)->getFriction() + ((ActorDynamic*)a_Actor1)->getFriction());
			pos *= friction;
			((ActorDynamic*)a_Actor2)->setVelocity(pos);
			printf("v %f %f %f \n",pos.x,pos.y,pos.z);
		}
	}

	bool Sphere_Sphere	(Sphere *s_Shape1	, Sphere	*s_Shape2	, glm::vec3 &v3_CollisionPoint){
		a1 = s_Shape1->getType();
		a2 = s_Shape2->getType();
		float combinedRad = s_Shape1->getRadius() + s_Shape2->getRadius();
		if ((combinedRad > s_Shape1->getTranslate().x - s_Shape2->getTranslate().x) && 
			(combinedRad > s_Shape1->getTranslate().y - s_Shape2->getTranslate().y) && 
			(combinedRad > s_Shape1->getTranslate().z - s_Shape2->getTranslate().z)
			){
			v3_CollisionPoint = s_Shape1->getModel()[3].xyz - s_Shape2->getModel()[3].xyz;
			return true;
		}
		v3_CollisionPoint = glm::vec3(0);
		return false;
	}
	bool Sphere_AABB	(Sphere *s_Shape1	, AxisBox	*s_Shape2	, glm::vec3 &v3_CollisionPoint){
		a1 = s_Shape1->getType();
		a2 = s_Shape2->getType();
		glm::vec3 dist = s_Shape1->getModel()[3].xyz - s_Shape2->getModel()[3].xyz;
		glm::vec3 boxPoint(0);
		
		if (dist.x < -s_Shape2->getExtents().x * 0.5f)
			boxPoint.x = -s_Shape2->getExtents().x * 0.5f;
		else if (dist.x > s_Shape2->getExtents().x * 0.5f)
			boxPoint.x = s_Shape2->getExtents().x * 0.5f;
		else
			boxPoint.x = dist.x;

		if (dist.y < -s_Shape2->getExtents().y * 0.5f)
			boxPoint.y = -s_Shape2->getExtents().y * 0.5f;
		else if (dist.y > s_Shape2->getExtents().y * 0.5f)
			boxPoint.y = s_Shape2->getExtents().y * 0.5f;
		else
			boxPoint.y = dist.y;

		if (dist.z < -s_Shape2->getExtents().z * 0.5f)
			boxPoint.z = -s_Shape2->getExtents().z * 0.5f;
		else if (dist.z > s_Shape2->getExtents().z * 0.5f)
			boxPoint.z = s_Shape2->getExtents().z * 0.5f;
		else
			boxPoint.z = dist.z;

		dist = dist - boxPoint;

		if (dist.x * dist.x + dist.y * dist.y + dist.z * dist.z < s_Shape1->getRadius() * s_Shape1->getRadius()){
			v3_CollisionPoint = s_Shape1->getModel()[3].xyz - s_Shape2->getModel()[3].xyz;
			return true;
		}else{
			v3_CollisionPoint = glm::vec3(0);
			return false;
		}
	}
	bool Sphere_Box		(Sphere *s_Shape1	, Box		*s_Shape2	, glm::vec3 &v3_CollisionPoint){
		return false;
	}
	bool Sphere_Plane	(Sphere *s_Shape1	, Plane		*s_Shape2	, glm::vec3 &v3_CollisionPoint){
		a1 = s_Shape1->getType();
		a2 = s_Shape2->getType();
		float dist = glm::dot<float>(s_Shape2->getModel()[3].xyz - s_Shape2->getUp(),s_Shape1->getModel()[3].xyz ) - s_Shape2->getModel()[3].w;

		glm::vec3 s1Pos = s_Shape1->getModel()[3].xyz;
		glm::vec3 s2Pos = s_Shape2->getModel()[3].xyz;
		v3_CollisionPoint.x = (s_Shape2->getUp().x != 0.0f) ? s2Pos.x + (s_Shape1->getRadius() * 0.5f) : 0.0f ;
		v3_CollisionPoint.y = (s_Shape2->getUp().y != 0.0f) ? s2Pos.y + (s_Shape1->getRadius() * 0.5f) : 0.0f ;
		v3_CollisionPoint.z = (s_Shape2->getUp().z != 0.0f) ? s2Pos.z + (s_Shape1->getRadius() * 0.5f) : 0.0f ;

		if (dist < -s_Shape1->getRadius()){
			//behind
			return true;
		}else if (dist > s_Shape1->getRadius()){
			//above
			v3_CollisionPoint = glm::vec3(0);
			return false;
		}
		return true;
	}
	bool Sphere_Ray		(Sphere *s_Shape1	, Ray		*s_Shape2	, glm::vec3 &v3_CollisionPoint){
		return false;
	}
	bool Sphere_Line	(Sphere *s_Shape1	, Line		*s_Shape2	, glm::vec3 &v3_CollisionPoint){
		return false;
	}
	bool Sphere_Mesh	(Sphere *s_Shape1	, Mesh		*s_Shape2	, glm::vec3 &v3_CollisionPoint){
		return false;
	}

	bool AABB_Sphere	(AxisBox *s_Shape1	, Sphere	*s_Shape2	, glm::vec3 &v3_CollisionPoint){return Sphere_AABB(s_Shape2,s_Shape1,v3_CollisionPoint);}
	bool AABB_AABB		(AxisBox *s_Shape1	, AxisBox	*s_Shape2	, glm::vec3 &v3_CollisionPoint){
		a1 = s_Shape1->getType();
		a2 = s_Shape2->getType();
		v3_CollisionPoint = glm::vec3(0);

		glm::vec3 s1Pos = s_Shape1->getModel()[3].xyz;
		glm::vec3 s2Pos = s_Shape2->getModel()[3].xyz;

		float TempExtent1 = s_Shape1->getExtents().x;
		float TempExtent2 = s_Shape2->getExtents().x;

		if ((s1Pos.x + (TempExtent1) < s2Pos.x - (TempExtent2) ) || 
			(s2Pos.x + (TempExtent2) < s1Pos.x - (TempExtent1) )) {return false;}

		TempExtent1 = s_Shape1->getExtents().y;
		TempExtent2 = s_Shape2->getExtents().y;

		if ((s1Pos.y + (TempExtent1) < s2Pos.y - (TempExtent2) ) || 
			(s2Pos.y + (TempExtent2) < s1Pos.y - (TempExtent1) )) {return false;}

		TempExtent1  = s_Shape1->getExtents().z;
		TempExtent2  = s_Shape2->getExtents().z;

		if ((s1Pos.z + (TempExtent1) < s2Pos.z - (TempExtent2) ) ||
			(s2Pos.z + (TempExtent2) < s1Pos.z - (TempExtent1) )) {return false;}

		v3_CollisionPoint = s_Shape1->getModel()[3].xyz - s_Shape2->getModel()[3].xyz;
		return true;		
	}
	bool AABB_Box		(AxisBox *s_Shape1	, Box		*s_Shape2	, glm::vec3 &v3_CollisionPoint){
		// Call box box
		return false;
	}
	bool AABB_Plane		(AxisBox *s_Shape1	, Plane		*s_Shape2	, glm::vec3 &v3_CollisionPoint){
		
		a1 = s_Shape1->getType();
		a2 = s_Shape2->getType();
		
		glm::vec3 s1Pos = s_Shape1->getModel()[3].xyz;
		glm::vec3 s2Pos = s_Shape2->getModel()[3].xyz;
		glm::vec3 s1Extents = s_Shape1->getExtents();

		//v3_CollisionPoint = s_Shape2->getModel()[3].xyz;
		v3_CollisionPoint.x = (s_Shape2->getUp().x != 0.0f) ? s2Pos.x + (s1Extents.x * 0.5f * s_Shape2->getUp().x ) : 0.0f;
		v3_CollisionPoint.y = (s_Shape2->getUp().y != 0.0f) ? s2Pos.y + (s1Extents.y * 0.5f * s_Shape2->getUp().y ) : 0.0f;
		v3_CollisionPoint.z = (s_Shape2->getUp().z != 0.0f) ? s2Pos.z + (s1Extents.z * 0.5f * s_Shape2->getUp().z ) : 0.0f;

		if (s_Shape2->getUp().x != 0.0f){
			if (s_Shape2->getUp().x < 0.0f){
				if (s1Pos.x - s1Extents.x >= s2Pos.x || s1Pos.x + s1Extents.x >= s2Pos.x){
					return true;
				}
			}else{
				if (s1Pos.x - s1Extents.x <= s2Pos.x || s1Pos.x + s1Extents.x <= s2Pos.x){
					return true;
				}
			}
		}else if (s_Shape2->getUp().y != 0.0f){
			if (s_Shape2->getUp().y < 0.0f){
				if (s1Pos.y - s1Extents.y >= s2Pos.y || s1Pos.y + s1Extents.y >= s2Pos.y){
					return true;
				}
			}else{
				if (s1Pos.y - s1Extents.y <= s2Pos.y || s1Pos.y + s1Extents.y <= s2Pos.y){
					return true;
				}
			}
		}else if (s_Shape2->getUp().z != 0.0f){
			if (s_Shape2->getUp().z < 0.0f){
				if (s1Pos.z - s1Extents.z >= s2Pos.z || s1Pos.z + s1Extents.z >= s2Pos.z){
					return true;
				}
			}else{
				if (s1Pos.z - s1Extents.z <= s2Pos.z || s1Pos.z + s1Extents.z <= s2Pos.z){
					return true;
				}
			}
		}

		v3_CollisionPoint = glm::vec3(0);
		return false;
	}
	bool AABB_Ray		(AxisBox *s_Shape1	, Ray		*s_Shape2	, glm::vec3 &v3_CollisionPoint){
		return false;
	}
	bool AABB_Line		(AxisBox *s_Shape1	, Line		*s_Shape2	, glm::vec3 &v3_CollisionPoint){
		return false;
	}
	bool AABB_Mesh		(AxisBox *s_Shape1	, Mesh		*s_Shape2	, glm::vec3 &v3_CollisionPoint){
		return false;
	}

	bool Box_Sphere		(Box	*s_Shape1	, Sphere	*s_Shape2	, glm::vec3 &v3_CollisionPoint){return Sphere_Box(s_Shape2,s_Shape1,v3_CollisionPoint);}
	bool Box_AABB		(Box	*s_Shape1	, AxisBox	*s_Shape2	, glm::vec3 &v3_CollisionPoint){return AABB_Box(s_Shape2,s_Shape1,v3_CollisionPoint);}
	bool Box_Box		(Box	*s_Shape1	, Box		*s_Shape2	, glm::vec3 &v3_CollisionPoint){
		//rotate all -B

		//	 SELF AABB == false; false

		// rotate all -A

		//	AABB SELF == false; false
		return false;
	}
	bool Box_Plane		(Box	*s_Shape1	, Plane		*s_Shape2	, glm::vec3 &v3_CollisionPoint){
		return false;
	}
	bool Box_Ray		(Box	*s_Shape1	, Ray		*s_Shape2	, glm::vec3 &v3_CollisionPoint){
		return false;
	}
	bool Box_Line		(Box	*s_Shape1	, Line		*s_Shape2	, glm::vec3 &v3_CollisionPoint){
		return false;
	}
	bool Box_Mesh		(Box	*s_Shape1	, Mesh		*s_Shape2	, glm::vec3 &v3_CollisionPoint){
		return false;
	}

	bool Plane_Sphere	(Plane	*s_Shape1	, Sphere	*s_Shape2	, glm::vec3 &v3_CollisionPoint){return Sphere_Plane(s_Shape2,s_Shape1,v3_CollisionPoint);}
	bool Plane_AABB		(Plane	*s_Shape1	, AxisBox	*s_Shape2	, glm::vec3 &v3_CollisionPoint){return AABB_Plane(s_Shape2,s_Shape1,v3_CollisionPoint);}
	bool Plane_Box		(Plane	*s_Shape1	, Box		*s_Shape2	, glm::vec3 &v3_CollisionPoint){return Box_Plane(s_Shape2,s_Shape1,v3_CollisionPoint);}
	bool Plane_Plane	(Plane	*s_Shape1	, Plane		*s_Shape2	, glm::vec3 &v3_CollisionPoint){
		return false;
	}
	bool Plane_Ray		(Plane	*s_Shape1	, Ray		*s_Shape2	, glm::vec3 &v3_CollisionPoint){
		return false;
	}
	bool Plane_Line		(Plane	*s_Shape1	, Line		*s_Shape2	, glm::vec3 &v3_CollisionPoint){
		return false;
	}
	bool Plane_Mesh		(Plane	*s_Shape1	, Mesh		*s_Shape2	, glm::vec3 &v3_CollisionPoint){
		return false;
	}

	bool Ray_Sphere		(Ray	*s_Shape1	, Sphere	*s_Shape2	, glm::vec3 &v3_CollisionPoint){return Sphere_Ray(s_Shape2,s_Shape1,v3_CollisionPoint);}
	bool Ray_AABB		(Ray	*s_Shape1	, AxisBox	*s_Shape2	, glm::vec3 &v3_CollisionPoint){return AABB_Ray(s_Shape2,s_Shape1,v3_CollisionPoint);}
	bool Ray_Box		(Ray	*s_Shape1	, Box		*s_Shape2	, glm::vec3 &v3_CollisionPoint){return Box_Ray(s_Shape2,s_Shape1,v3_CollisionPoint);}
	bool Ray_Plane		(Ray	*s_Shape1	, Plane		*s_Shape2	, glm::vec3 &v3_CollisionPoint){return Plane_Ray(s_Shape2,s_Shape1,v3_CollisionPoint);}
	bool Ray_Ray		(Ray	*s_Shape1	, Ray		*s_Shape2	, glm::vec3 &v3_CollisionPoint){
		return false;
	}
	bool Ray_Line		(Ray	*s_Shape1	, Line		*s_Shape2	, glm::vec3 &v3_CollisionPoint){
		return false;
	}
	bool Ray_Mesh		(Ray	*s_Shape1	, Mesh		*s_Shape2	, glm::vec3 &v3_CollisionPoint){
		return false;
	}

	bool Line_Sphere	(Line	*s_Shape1	, Sphere	*s_Shape2	, glm::vec3 &v3_CollisionPoint){return Sphere_Line(s_Shape2,s_Shape1,v3_CollisionPoint);}
	bool Line_AABB		(Line	*s_Shape1	, AxisBox	*s_Shape2	, glm::vec3 &v3_CollisionPoint){return AABB_Line(s_Shape2,s_Shape1,v3_CollisionPoint);}
	bool Line_Box		(Line	*s_Shape1	, Box		*s_Shape2	, glm::vec3 &v3_CollisionPoint){return Box_Line(s_Shape2,s_Shape1,v3_CollisionPoint);}
	bool Line_Plane		(Line	*s_Shape1	, Plane		*s_Shape2	, glm::vec3 &v3_CollisionPoint){return Plane_Line(s_Shape2,s_Shape1,v3_CollisionPoint);}
	bool Line_Ray		(Line	*s_Shape1	, Ray		*s_Shape2	, glm::vec3 &v3_CollisionPoint){return Ray_Line(s_Shape2,s_Shape1,v3_CollisionPoint);}
	bool Line_Line		(Line	*s_Shape1	, Line		*s_Shape2	, glm::vec3 &v3_CollisionPoint){
		return false;
	}
	bool Line_Mesh		(Line	*s_Shape1	, Mesh		*s_Shape2	, glm::vec3 &v3_CollisionPoint){
		return false;
	}

	bool Mesh_Sphere	(Mesh	*s_Shape1	, Sphere	*s_Shape2	, glm::vec3 &v3_CollisionPoint){return Sphere_Mesh(s_Shape2,s_Shape1,v3_CollisionPoint);}
	bool Mesh_AABB		(Mesh	*s_Shape1	, AxisBox	*s_Shape2	, glm::vec3 &v3_CollisionPoint){return AABB_Mesh(s_Shape2,s_Shape1,v3_CollisionPoint);}
	bool Mesh_Box		(Mesh	*s_Shape1	, Box		*s_Shape2	, glm::vec3 &v3_CollisionPoint){return Box_Mesh(s_Shape2,s_Shape1,v3_CollisionPoint);}
	bool Mesh_Plane		(Mesh	*s_Shape1	, Plane		*s_Shape2	, glm::vec3 &v3_CollisionPoint){return Plane_Mesh(s_Shape2,s_Shape1,v3_CollisionPoint);}
	bool Mesh_Ray		(Mesh	*s_Shape1	, Ray		*s_Shape2	, glm::vec3 &v3_CollisionPoint){return Ray_Mesh(s_Shape2,s_Shape1,v3_CollisionPoint);}
	bool Mesh_Line		(Mesh	*s_Shape1	, Line		*s_Shape2	, glm::vec3 &v3_CollisionPoint){return Line_Mesh(s_Shape2,s_Shape1,v3_CollisionPoint);}
	bool Mesh_Mesh		(Mesh	*s_Shape1	, Mesh		*s_Shape2	, glm::vec3 &v3_CollisionPoint){
		return false;
	}
}