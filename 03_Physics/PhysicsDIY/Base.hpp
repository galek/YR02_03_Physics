#pragma once

#include <glm\glm.hpp>

namespace Osiris{
	class _Base{
	public:
		_Base();
		~_Base();

		const glm::vec3&	getTranslate();
		void				setTranslate(const glm::vec3& v3_Translate);
		void				addTranslate(const glm::vec3& v3_Translate);

		const glm::vec3&	getRotation();
		void				setRotation(const glm::vec3& v3_Rotation);
		void				addRotation(const glm::vec3& v3_Rotation);

		const float&		getMass();
		void				setMass(const float& f_Mass);

		const glm::vec3&	getFriction();
		void				setFriction(const glm::vec3& v3_Friction);
		void				addFriction(const glm::vec3& v3_Friction);

		const glm::mat4&	getModel();

		void				updateModel(const glm::mat4* m4_Parent);
	protected:
		bool bUpdated;
		glm::vec3 v3Friction;
		glm::vec3 v3Translate;
		glm::vec3 v3Rotation;
		glm::mat4 m4Model;
		float fMass;
	};
}

