#pragma once

#include <unordered_map>
#include <glm\glm.hpp>
#include <vector>

//!--SPATIAL
namespace Osiris{
	class Actor;

	typedef std::vector<Actor*> BucketList;

	class Bucket {
	public:
		Bucket();
		~Bucket();
		bool	isEmpty();
		const BucketList& getElements();
		void	addElement(Actor* t_element);
		void	clear();
		void	update(float f_timeStep);
	protected:
		BucketList vvpElements;
	};

	typedef std::vector<Bucket*> GridList;
	typedef std::unordered_map<size_t,Bucket*> BucketMap;

	class Spatial {
	public:
		Spatial(glm::ivec3 v3_Strides);
		~Spatial();
		void	clear();
		void	clearEmpty();
		void	clearElements();
		GridList getBuckets();
		Bucket* getBucket(glm::vec3 v3_BucketLocation);
		size_t getHash(glm::vec3 v3_BucketLocation);

		const glm::ivec3&	getStride();
		void				setStride(const glm::ivec3& v3_Stride);
	protected:
		glm::ivec3 v3Strides;
		std::unordered_map<size_t,Bucket*> umBuckets;
	};
}

