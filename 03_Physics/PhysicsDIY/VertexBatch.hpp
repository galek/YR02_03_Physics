#pragma once

#include <glm/ext.hpp>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <map>

namespace Osiris{

	//3 3 4 1 = 11
	struct POINT{

		glm::vec3 Position;
		glm::vec3 Information;
		glm::vec4 Colour;
		int Bitset;

		enum Offsets{
			PositionOffset		= 0,
			InformationOffset	= PositionOffset	+ sizeof(glm::vec3),
			ColourOffset		= InformationOffset	+ sizeof(glm::vec3),
			BitsetOffset		= ColourOffset		+ sizeof(glm::vec4),
		};
	};

	// Base GizmoObject, Inherit from this to create your own
	class GizmoObject{
	public:
		POINT Vertex;
		
		void move(){
			Vertex.Position = glm::vec3(rand()% 100 - 50,0,rand()% 100 - 50);
		}
	};

	typedef std::map<unsigned int,GizmoObject*> GizmoMap;

	// VertexBatch
	// Basic Single draw call batch of elements.
	class VertexBatch{
	public:
		// Instantiate the Gizmos.
		// Must setup all VertexAtribArrays
		VertexBatch(){

			_NextObjectID = _VertexCount = _sGizmoShader = 0;

			// Create opengl buffers
			glGenVertexArrays(1, &_VAO);
			glBindVertexArray(_VAO);

			glGenBuffers(1, &_VBO);
			glBindBuffer(GL_ARRAY_BUFFER, _VBO);
			glBufferData(GL_ARRAY_BUFFER, 1048576 * sizeof(POINT), nullptr, GL_DYNAMIC_DRAW);

			glEnableVertexAttribArray(0);
			glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(POINT), (void*) + POINT::PositionOffset);
			glEnableVertexAttribArray(1);
			glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(POINT), (void*) + POINT::InformationOffset);
			glEnableVertexAttribArray(2);
			glVertexAttribPointer(2, 4, GL_FLOAT, GL_FALSE, sizeof(POINT), (void*) + POINT::ColourOffset);
			glEnableVertexAttribArray(3);
			glVertexAttribIPointer(3, 1, GL_INT , sizeof(POINT), (void*) + POINT::BitsetOffset);

			glBindBuffer(GL_ARRAY_BUFFER, 0);
			glBindVertexArray(0);
		}

		const GizmoMap* getMap(){
			return &_GizmoObjects;
		}

		unsigned int getCount(){
			return _VertexCount;
		}

		// Let the Gizmos know what shader to use.
		void SetShader(unsigned int s_GizmoShader){
			_sGizmoShader = s_GizmoShader;
		}

		// Destroy the Gizmos.
		// !WARNING! does NOT clean up the given shader, be sure to delete it.
		// !WARNING! DOES clean up all given GizmoObjects.
		~VertexBatch(){
			glDeleteVertexArrays(1,&_VAO);
			glDeleteBuffers(1, &_VBO);
			for (auto gizmo : _GizmoObjects){
				delete gizmo.second;
			}
			_GizmoObjects.clear();
		}

		// Create the given Gizmo.
		// go_ObjectToAdd a pointer to a GizmoObject.
		// return is the ID for this GizmoObject.
		unsigned int Add(GizmoObject *go_ObjectToAdd){
			_GizmoObjects[_NextObjectID] = go_ObjectToAdd;
			_VertexCount	+= 1;
			_NextObjectID++;
			return _NextObjectID - 1;
		}

		// Delete the given Gizmo.
		// ui_GizmoID = The ID of the gizmo to remove.
		// !WARNING! This calls a delete on the GizmoObject.
		// !WARNING! This batch will draw weird until Update() is called.
		void Remove(unsigned int ui_GizmoID){
			if (_GizmoObjects.find(ui_GizmoID) != _GizmoObjects.end()){
				_VertexCount	-= 1;
				delete _GizmoObjects[ui_GizmoID];
				_GizmoObjects.erase(ui_GizmoID);
			}
			if (_GizmoObjects.size() == 0){_NextObjectID = 0;}
		}

		// Get a Gizmo from the batch.
		// ui_GizmoID = The ID of the gizmo to return.
		// GizmoType  = The Type of the gizmo to return;
		// !WARNING! any changes will need to be updated.
		template <typename GizmoType>
		GizmoType* Get(unsigned int ui_GizmoID){
			if (_GizmoObjects.find(ui_GizmoID) != _GizmoObjects.end()){
				return (GizmoType*)_GizmoObjects[ui_GizmoID];
			}
		}

		// Update the Gizmos.
		void Update(){

			POINT *_Verticies = new POINT[_VertexCount];

			_VertexCount = 0;

			for (std::map<unsigned int,GizmoObject*>::iterator it = _GizmoObjects.begin(); it != _GizmoObjects.end(); it++){
				_Verticies[_VertexCount++] = it->second->Vertex;
			}

			glBindBuffer(GL_ARRAY_BUFFER, _VBO);
			glBufferSubData(GL_ARRAY_BUFFER, 0, _VertexCount * sizeof(POINT), _Verticies);
			glBindBuffer(GL_ARRAY_BUFFER, 0);

			delete[] _Verticies;
		}

		// Draw the Gizmos with the given Shader.
		void Draw(){
			if (_sGizmoShader != 0 && _VertexCount >= 0 && _VAO != 0){
				glBindVertexArray( _VAO );
				glBindBuffer(GL_ARRAY_BUFFER, _VBO);
				glDrawArrays(GL_POINTS, 0, _VertexCount);
				glBindBuffer(GL_ARRAY_BUFFER, 0);
				glBindVertexArray(0);
			}
		}

	protected:
		unsigned int	_VAO,
						_VBO;

		unsigned int	_VertexCount,
						_sGizmoShader;

		unsigned int	 _NextObjectID;
		GizmoMap _GizmoObjects;   
	};

	namespace Gizmo{

		int setBits(short s_ID, short s_SmoothNormals, short s_Facing){
			return (s_ID + (s_SmoothNormals << 9) + (s_Facing << 10));
		}

		class Point : public GizmoObject{
		public:
			Point(glm::vec3 Location,glm::vec4 Colour){
				Vertex.Position = glm::vec3(Location); 
				Vertex.Information = glm::vec3(0); 
				Vertex.Bitset = setBits(0,1,0);
				Vertex.Colour = Colour;
			}
		};

		class Box : public GizmoObject{
		public:
			Box(glm::vec3 Location,glm::vec3 Extents, glm::vec4 Colour){
				Vertex.Position = glm::vec3(Location); 
				Vertex.Information = Extents; 
				Vertex.Bitset = setBits(1,1,0);
				Vertex.Colour = Colour;
			}
		};

		class Plane : public GizmoObject{
		public:
			Plane(glm::vec3 Location,glm::vec3 Extents, glm::vec4 Colour){
				Vertex.Position = glm::vec3(Location); 
				Vertex.Information = Extents; 
				Vertex.Bitset = setBits(2,1,0);
				Vertex.Colour = Colour;
			}
		};

		class Sphere : public GizmoObject{
		public:
			Sphere(glm::vec3 Location,float Radius, int Rows, int Columns, glm::vec4 Colour){
				Vertex.Position = glm::vec3(Location); 
				Vertex.Information = glm::vec3(Radius,(float)Rows,(float)Columns); 
				Vertex.Bitset = setBits(3,1,0);
				Vertex.Colour = Colour;
			}
		};

	}
}

