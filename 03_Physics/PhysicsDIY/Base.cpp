#include "Base.hpp"
#include <glm\ext.hpp>

namespace Osiris{
	//!-- BASE
	_Base::_Base(){
		m4Model = glm::mat4(1);
		v3Translate = glm::vec3(0);
		v3Rotation = glm::vec3(0);
		fMass = 0.0f;
	}
	_Base::~_Base(){}

	const glm::vec3&	_Base::getTranslate		()									{return v3Translate;}
	void				_Base::setTranslate		(const glm::vec3& v3_Translate)		{v3Translate = v3_Translate;  bUpdated = true;}
	void				_Base::addTranslate		(const glm::vec3& v3_Translate)		{v3Translate += v3_Translate; bUpdated = true;}

	const glm::vec3&	_Base::getRotation		()									{return v3Rotation;}
	void				_Base::setRotation		(const glm::vec3& v3_Rotation)		{v3Rotation = v3_Rotation;  bUpdated = true;}
	void				_Base::addRotation		(const glm::vec3& v3_Rotation)		{v3Rotation += v3_Rotation; bUpdated = true;}

	const float&		_Base::getMass			()									{return fMass;}
	void				_Base::setMass			(const float& f_Mass)				{fMass = f_Mass;}

	const glm::vec3&	_Base::getFriction		()									{return v3Friction;}
	void				_Base::setFriction		(const glm::vec3& v3_Friction)		{v3Friction = glm::abs(v3_Friction);}
	void				_Base::addFriction		(const glm::vec3& v3_Friction)		{v3Friction += glm::abs(v3_Friction);}

	const glm::mat4&	_Base::getModel			()									{return m4Model;}

	void				_Base::updateModel		(const glm::mat4* m4_Parent)		{
		glm::mat4 m4R(1);

		m4R = glm::rotate(m4R,v3Rotation.x,glm::vec3(1,0,0));
		m4R = glm::rotate(m4R,v3Rotation.y,glm::vec3(0,1,0));
		m4R = glm::rotate(m4R,v3Rotation.z,glm::vec3(0,0,1));
			
		if (m4_Parent != nullptr){
			m4Model = (*m4_Parent) * m4R;
		}else{
			m4Model = m4R;
		}
		m4Model[3] += glm::vec4(v3Translate,1);
		bUpdated = false;
	}
}

