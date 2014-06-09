#pragma once

#include <glm\glm.hpp>

class Actor;

enum ShapeType { BOX_AABB, PLANE, SPHERE };

class Shape {
public:
	glm::vec3 _Offset;
	glm::vec4 _Colour;
	Actor *_Parent;

	ShapeType _Type;

public:
	Shape(glm::vec3 Offset, glm::vec4 Colour) { _Parent = nullptr; _Offset = Offset; _Colour = Colour; }
	virtual ~Shape() { }

	virtual void Draw(glm::vec3 Position) = 0;
};

