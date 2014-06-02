#include <unordered_map>
#include <glm\glm.hpp>
#include <Gizmos.h>
#include <vector>
#include "Shape.hpp"
#include "Actor.hpp"
#include "Scene.hpp"

namespace Osiris{

	Actor::Actor() {
		bActive = true;
		parentScene = nullptr;
		bRemoveMe = false;
		// Add our AABB
		AxisBox *b = new AxisBox();
		b->setExtents(glm::vec3(1.0f));
		addShape(b);
		fMass = 0.01f;
	}

	Actor::~Actor(){
		for (auto shape : vShapes){
			delete shape;
		}
		vShapes.clear();
	}

	const bool&			Actor::getActive	()									{return bActive;}
	void				Actor::setActive	(bool b_Active)						{bActive = b_Active;}
	const bool&			Actor::toRemove		()									{return bRemoveMe;}

	Scene*				Actor::getScene		()									{return parentScene;}
	void				Actor::setScene		(Scene* s_Scene)					{parentScene = s_Scene;}

	ShapeVector&		Actor::getShapes    ()									{return vShapes;}

	const ActorType		Actor::getType		()									{return aType;}

	void Actor::addShape(Shape* a_Shape){
		vShapes.push_back(a_Shape);
		a_Shape->updateModel(&m4Model);
		calculateAABB();
	}

	void Actor::update(float f_timeStepInterval){
		updateModel(nullptr);
		for (auto shape : vShapes){
			shape->updateModel(&m4Model);
		}
	}

	void Actor::draw(){
		for (auto shape : vShapes){
			shape->draw();
		}
	}

	void Actor::calculateAABB(){
		if (vShapes[0]->getType() == ShapeType::AABB){
			glm::vec3 ext(0), trans(0);
			fMass = 0.0f;
			float fOffset = 0.01f;
			for (ShapeVector::iterator it = vShapes.begin() + ((aType == ActorType::DYNAMIC) ? 2 : 1); it != vShapes.end(); it++){
				fMass += (*it)->getMass();
				// Figure out extents
				switch((*it)->getType()){
					case ShapeType::AABB:
						ext += ((AxisBox*)(*it))->getExtents();
						break;
					case ShapeType::BOX:
						ext += ((Box*)(*it))->getExtents();
						break;
					case ShapeType::MESH:

						break;
					case ShapeType::PLANE:
						if (parentScene != nullptr){
							ext.xz += parentScene->getClipBounds().xz * 2.0f;
							ext.y += 0.1f;
						}else{
							ext += glm::vec3(100,0.1f,100);
						}
						break;
					case ShapeType::RAY:
						//Rays can be anywhere, so set center to 0 and extents to clip bounds
						((AxisBox*)vShapes[0])->setTranslate(-v3Translate);
						((AxisBox*)vShapes[0])->setExtents(parentScene->getClipBounds());
						break;
					case ShapeType::SPHERE:
						ext += ((Sphere*)(*it))->getRadius();
						break;
				}
				// Figure out center of mass
				trans += trans - (*it)->getTranslate();
			}
			if (aType == ActorType::STATIC){
				fMass = 10000;
			}
			((AxisBox*)vShapes[0])->setTranslate(trans);
			((AxisBox*)vShapes[0])->setExtents(ext + fOffset);
		}
	}

	ActorStatic::ActorStatic(){
		fMass = 10000;
		aType = ActorType::STATIC;
		Actor::update(0);
		bActive = true;
	}

	ActorStatic::~ActorStatic(){}

	void ActorStatic::spatial(){
		glm::vec3 boundLocal = parentScene->getClipBounds();
		glm::ivec3 gridStride = parentScene->getSpatial()->getStride();
		if (boundLocal.x - abs(v3Translate.x) > 0 && boundLocal.y - abs(v3Translate.y) > 0 && boundLocal.z - abs(v3Translate.z) > 0){
			glm::vec3 ext = ((AxisBox*)vShapes[0])->getExtents();
			for (int x = -ext.x; x < ext.x; x += gridStride.x){
				for (int y = -ext.y; y < ext.y; y += gridStride.y){
					for (int z = -ext.z; z < ext.z; z += gridStride.z){
						parentScene->getSpatial()->getBucket(m4Model[3].xyz + glm::vec3(x,y,z))->addElement(this);
					}
				}
			}
		}else{
			char* cs = ACTORTYPE(aType);
			bRemoveMe = true;
		}
	}

	void ActorStatic::update(float f_timeStepInterval){
		if (bActive){
			vShapes[0]->setColour(glm::vec4(0,1,0,0.5f));
		}else{
			vShapes[0]->setColour(glm::vec4(1,0,0,0.5f));
		}
		if (bUpdated){
			bActive = true;
			Actor::update(f_timeStepInterval);
		}
	}

	ActorDynamic::ActorDynamic(){
		aType = ActorType::DYNAMIC;
		v3Velocity = glm::vec3(0);
		v3Friction = glm::vec3(0.0f);
		v3Damping = glm::vec3(0.1f);
		// Add our Line from oldpos to new pos
		Line *l = new Line();
		addShape(l);
		Actor::update(0);
		bActive = true;
		calculateAABB();
	}

	ActorDynamic::~ActorDynamic(){
	
	}

	const glm::vec3&	ActorDynamic::getDamping		()									{return v3Damping;}
	void				ActorDynamic::setDamping		(const glm::vec3& v3_Damping)		{v3Damping = v3_Damping;}

	const glm::vec3&	ActorDynamic::getVelocity		()									{return v3Velocity;}
	void				ActorDynamic::setVelocity		(const glm::vec3& v3_Velocity)		{v3Velocity = v3_Velocity;}
	void				ActorDynamic::addForce			(const glm::vec3& v3_Velocity)		{v3Velocity += v3_Velocity / fMass;}

	void ActorDynamic::spatial(){
		glm::vec3 boundLocal = parentScene->getClipBounds();
		glm::ivec3 gridStride = parentScene->getSpatial()->getStride();
		if (boundLocal.x - abs(v3Translate.x) > 0 && boundLocal.y - abs(v3Translate.y) > 0 && boundLocal.z - abs(v3Translate.z) > 0){
			glm::vec3 ext = ((AxisBox*)vShapes[0])->getExtents();
			for (int x = -ext.x; x < ext.x; x += gridStride.x){
				for (int y = -ext.y; y < ext.y; y += gridStride.y){
					for (int z = -ext.z; z < ext.z; z += gridStride.z){
						parentScene->getSpatial()->getBucket(m4Model[3].xyz + glm::vec3(x,y,z))->addElement(this);
					}
				}
			}	
		}else{
			bRemoveMe = true;
		}
	}

	void ActorDynamic::update(float f_timeStepInterval){		
		if (bUpdated){
			bActive = true;
			Actor::update(f_timeStepInterval);
		}
		if (bActive){
			vShapes[0]->setColour(glm::vec4(0,1,0,0.5f));
			vShapes[1]->setColour(glm::vec4(0,1,0,0.5f));
			float g = 0.0f;
			if (parentScene != nullptr){
				g = parentScene->getGravity();
				if (g != 0.0f){
					addForce(glm::vec3(0,g * fMass * f_timeStepInterval,0));
					bActive = true;
				}
			}
			if (abs(v3Velocity.x) < 0.1f && abs(v3Velocity.y) == 0.0f && abs(v3Velocity.z) < 0.1f ){
				v3Velocity = glm::vec3(0.0f);
				bActive = false;
			} else {
				glm::vec3 OldPos = v3Translate;
				v3Velocity *= glm::vec3(1.0f);
				addTranslate(v3Velocity * f_timeStepInterval);
				((Line*)vShapes[1])->setDirection(OldPos - v3Translate);
				((Line*)vShapes[1])->setDistance(glm::distance(OldPos,v3Translate));
			}
		}else{
			vShapes[0]->setColour(glm::vec4(1,0,0,0.5f));
			vShapes[1]->setColour(glm::vec4(1,0,0,0.5f));
		}
	}
}

