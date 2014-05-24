#version 330
// Gizmo Geometry Shader

layout (points) in;
layout (triangle_strip,max_vertices = 1024) out;

in vec3 vPosition[];
in vec3 vInformation[];
in vec4 vColour[];
in int  vID[];
in int  vSmooth[];
in int  vFacing[];

out vec4 gColour;

uniform mat4 ViewProjection;

// GLOBAL CONSTANTS
const float _PI = 3.14159;

void addTri(vec3 p0,vec3 p1,vec3 p2){
	gl_Position = ViewProjection * vec4(p0,1.0f);
	EmitVertex();
	gl_Position = ViewProjection * vec4(p1,1.0f);
	EmitVertex();
	gl_Position = ViewProjection * vec4(p2,1.0f);
	EmitVertex();

	EndPrimitive();
}
// GLOBAL CONSTANTS

// CUBE CONSTANTS
const vec4 CUBEvertices[8] = vec4[8](
	vec4( -1.0f, -1.0f,  1.0f, 1.0f),	//A 0
	vec4(  1.0f, -1.0f,  1.0f, 1.0f),	//B 1
	vec4( -1.0f,  1.0f,  1.0f, 1.0f),	//C 2
	vec4(  1.0f,  1.0f,  1.0f, 1.0f),	//D 3
	vec4( -1.0f, -1.0f,	-1.0f, 1.0f),	//F 4
	vec4(  1.0f, -1.0f,	-1.0f, 1.0f),	//G 5
	vec4( -1.0f,  1.0f,	-1.0f, 1.0f),	//E 6
	vec4(  1.0f,  1.0f,	-1.0f, 1.0f)	//H 7
	);
const int CUBEindices[24] = int[24](0, 1, 2, 3, 1, 5, 3, 7, 5, 4, 7, 6, 4, 0, 6, 2, 2, 3, 6, 7, 4, 5, 0, 1);
const vec3 CUBEtexcoords[8] = vec3[8](vec3(0,1,0), vec3(1,1,0), vec3(0,0,0), vec3(1,0,0), vec3(1,1,0), vec3(1,1,0), vec3(1,1,0), vec3(1,1,0));
void DrawCube(){
	for (int i = 0;i < 24;i++){
		vec4 Pos = vec4(vPosition[0],1.0f);
		vec4 Extents = vec4(vInformation[0],0.0f);
		gl_Position = ViewProjection * (Pos + (Extents * CUBEvertices[CUBEindices[i]]));
		EmitVertex();
		if (int(mod(i,4)) == 3) { EndPrimitive(); }
	}
}
// CUBE CONSTANTS

// PLANE CONSTANTS
const vec4 PLANEvertices[4] = vec4[4](
	vec4( -1.0f, 0.0f, -1.0f, 1.0f),	//A 0
	vec4( -1.0f, 0.0f,  1.0f, 1.0f),	//B 1
	vec4(  1.0f, 0.0f,  1.0f, 1.0f),	//C 2
	vec4(  1.0f, 0.0f, -1.0f, 1.0f) 	//D 3
	);
const int PLANEindices[6] = int[6](0, 1, 2, 0, 2, 3);
const vec2 PLANEtexcoords[4] = vec2[4](vec2(0,1), vec2(1,1), vec2(0,0), vec2(1,0));
void DrawPlane(){
	for (int i = 0;i < 6;i++){
		vec4 Pos = vec4(vPosition[0],1.0f);
		vec4 Up = vec4(vInformation[0],0.0f);
		gl_Position = ViewProjection * (Pos + (Up * PLANEvertices[PLANEindices[i]]));
		EmitVertex();
		if (int(mod(i,3)) == 2) { EndPrimitive(); }
	}
}
// PLANE CONSTANTS

// POINT CONSTANTS
// Not actually a point, really small box
void DrawPoint(){
	for (int i = 0;i < 24;i++){
		vec4 Pos = vec4(vPosition[0],1.0f);
		vec4 Extents = vec4(0.01f,0.01f,0.01f,0.0f);
		gl_Position = ViewProjection * (Pos + (Extents * CUBEvertices[CUBEindices[i]]));
		EmitVertex();
		if (int(mod(i,4)) == 3) { EndPrimitive(); }
	}
}
// POINT CONSTANTS

// SPHERE CONSTANTS
// Draw a sphere with the given Radius, Rows and Columns
void DrawSphere(){
	float latMin = -90;
	float latMax =  90;

	float longMin = 0;
	float longMax = 360;

	float invRad = 1.0f/vInformation[0].x;
	int invRow = int(1.0f/vInformation[0].y);
	int invCol = int(1.0f/vInformation[0].z);

	float Rad = vInformation[0].x;
	int Row = int(vInformation[0].y);
	int Col = int(vInformation[0].z);

	float deg2rad = _PI / 180.0f;

	float latRange = (latMax - latMin) * deg2rad;
	float longRange = (longMax - longMin) * deg2rad;
	vec3 v4Array[1024];

	for (int row = 0; row <= Row; ++row ) {

		float ratioAroundXAxis = float(row) * invRow;
		float radiansAboutXAxis  = ratioAroundXAxis * latRange + (latMin * deg2rad);
		float y = Rad * sin(radiansAboutXAxis);
		float z = Rad * cos(radiansAboutXAxis);
		
		for (int col = 0; col <= Col; ++col ) {

			float ratioAroundYAxis   = float(col) * invCol;
			float theta = ratioAroundYAxis * longRange + (longMin * deg2rad);
			vec3 v4Point = vec3( -z * sin(theta), y, -z * cos(theta) );
			vec3 v4Normal = vec3( Rad * v4Point.x, Rad * v4Point.y, Rad * v4Point.z);

			int index = row * Col + (col % Col);
			v4Array[index] = v4Point;
		}
	}
	vec3 PointPos = vPosition[0];
	int count = int(Row * Col);
	for (int face = 0; face < count; ++face ) {
		
		int iNextFace = face + 1;
		if( iNextFace % Col == 0 ) {
			iNextFace = iNextFace - (Col);
		}

		if( face % Col != 0 && longRange >= (_PI * 2)) {
			addTri( PointPos + v4Array[iNextFace+Col], PointPos + v4Array[face], PointPos + v4Array[iNextFace]);
			addTri( PointPos + v4Array[iNextFace+Col], PointPos + v4Array[face+Col], PointPos + v4Array[face]);
		}
	}
}
// SPHERE CONSTANTS

// CYLINDER CONSTANTS
// Draw a sphere with the given Radius, Rows and Columns
void DrawCylinder(){
	vec4 PointPos = vec4(vPosition[0],1.0f);
	//for (float i = 0; i < Row; i += 1.0f){
		vec4 Pos = PointPos * 1.0f;
		gl_Position = ViewProjection * Pos;
		EmitVertex();
	//}
}
// CYLINDER CONSTANTS

void main(){
	gColour = vColour[0];
	switch(vID[0]){
		case 0:
			DrawPoint();
			break;
		case 1:
			DrawCube();
			break;
		case 2:
			DrawPlane();
			break;
		case 3:
			DrawSphere();
			break;
	}
}

