#include "Spatial.hpp"

#include <Gizmos.h>
#include "Actor.hpp"
#include "Colliders.hpp"

//!--SPATIAL
namespace Osiris {

	Bucket::Bucket() {
	
	}

	Bucket::~Bucket(){
		clear();
	}

	bool	Bucket::isEmpty(){return (vvpElements.size() == 0) ? true : false;}

	const BucketList& Bucket::getElements(){
		return vvpElements;
	}

	void	Bucket::addElement(Actor* t_element){
		vvpElements.push_back(t_element);
	}

	void	Bucket::clear(){
		vvpElements.clear();
	}

	void	Bucket::update(float f_timeStep){
		// Double for loop, bad, but not sure how to fix with the second being a smart double for
		for (BucketList::iterator child1 = vvpElements.begin(); child1 != vvpElements.end(); child1++) {
			(*child1)->update(f_timeStep);
		}
		glm::vec3 v3CollPoint(0);
		for (BucketList::iterator child1 = vvpElements.begin(); child1 != vvpElements.end(); child1++) {
			for (BucketList::iterator child2 = child1 + 1; child2 != vvpElements.end(); child2++) {
				if ((*child1)->getActive() || (*child2)->getActive()){
					ShapeVector shapes1 = (*child1)->getShapes();
					ShapeVector shapes2 = (*child2)->getShapes();
					if ( AABB_AABB((AxisBox*)shapes1[0], (AxisBox*)shapes2[0], v3CollPoint ) ) {
						(*child1)->setActive(true);
						(*child2)->setActive(true);
						//Actor_Actor((*child1),(*child2),glm::vec3((*child1)->getModel()[3].xyz - (*child2)->getModel()[3].xyz));
						for (ShapeVector::iterator shape1 = shapes1.begin() + (((*child1)->getType() == ActorType::DYNAMIC) ? 2 : 1); shape1 != shapes1.end(); shape1++) {
							for (ShapeVector::iterator shape2 = shapes2.begin() + (((*child2)->getType() == ActorType::DYNAMIC) ? 2 : 1); shape2 != shapes2.end(); shape2++) {
								if ( Shape_Shape(*shape1, *shape2, v3CollPoint ) ) {
									Actor_Actor((*child1), (*child2), v3CollPoint);
								}
							}
						}
					}
				}
			}
		}
	}

	Spatial::Spatial(glm::ivec3 v3_Strides){
		v3Strides = v3_Strides;
	}

	Spatial::~Spatial(){
		for (auto child : umBuckets){
			delete child.second;
		}
		umBuckets.clear();
	}

	void	Spatial::clear(){
		for (auto child : umBuckets){
			delete child.second;
		}
		umBuckets.clear();
	}

	void	Spatial::clearElements(){
		for (auto child : umBuckets){
			child.second->clear();
		}
	}

	void	Spatial::clearEmpty(){
		for (BucketMap::iterator child = umBuckets.begin(); child != umBuckets.end();){
			if (child->second->isEmpty()){
				delete child->second;
				child = umBuckets.erase(child);
			}else{
				child++;
			}
		}
	}

	GridList Spatial::getBuckets(){
		GridList grid;
		for (auto child : umBuckets){
			grid.push_back(child.second);
		}
		return grid;
	}

	// Returns an existing bucket, if it doesnt exist, creates one
	Bucket* Spatial::getBucket(glm::vec3 v3_BucketLocation){
		v3_BucketLocation = (glm::ivec3)(v3_BucketLocation) / v3Strides;
		size_t hash = getHash(v3_BucketLocation);
		if (umBuckets.find(hash) == umBuckets.end()){
			Bucket *b = new Bucket();
			umBuckets[hash] = b;
		}
		return umBuckets[hash];
	}

	size_t Spatial::getHash(glm::vec3 v3_BucketLocation){
		return size_t(v3_BucketLocation.x * 1000000 + v3_BucketLocation.y * 10000 + v3_BucketLocation.z);
	}

	const glm::ivec3&	Spatial::getStride(){
		return v3Strides;
	}

	void				Spatial::setStride(const glm::ivec3& v3_Stride){
		v3Strides = v3_Stride;
	}
}

