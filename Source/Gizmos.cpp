#include "Gizmos.h"
#include <GL/glew.h>
#include <glm/ext.hpp>

Gizmos* Gizmos::sm_singleton = nullptr;

Gizmos::Gizmos(unsigned int a_maxLines, unsigned int a_maxTris)
	: m_maxLines(a_maxLines),
	m_maxTris(a_maxTris),
	m_lineCount(0),
	m_triCount(0),
	m_transparentTriCount(0),
	m_lines(new GizmoLine[a_maxLines]),
	m_tris(new GizmoTri[a_maxTris]),
	m_transparentTris(new GizmoTri[a_maxTris])
{
	// create shaders
	const char* vsSource = "#version 150\n \
					 in vec4 Position; \
					 in vec4 Colour; \
					 out vec4 vColour; \
					 uniform mat4 ProjectionView; \
					 void main() { vColour = Colour; gl_Position = ProjectionView * Position; }";

	const char* fsSource = "#version 150\n \
					 in vec4 vColour; \
                     out vec4 FragColor; \
					 void main()	{ FragColor = vColour; }";
    
	int success = GL_FALSE;
    
	m_vertexShader = glCreateShader(GL_VERTEX_SHADER);
	m_fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);

	glShaderSource(m_vertexShader, 1, (const char**)&vsSource, 0);
	glCompileShader(m_vertexShader);
    
	glGetShaderiv(m_vertexShader, GL_COMPILE_STATUS, &success);
	if (success == GL_FALSE)
	{
		int infoLogLength = 0;
		glGetShaderiv(m_vertexShader, GL_INFO_LOG_LENGTH, &infoLogLength);
		char* infoLog = new char[infoLogLength];
        
		glGetShaderInfoLog(m_vertexShader, infoLogLength, 0, infoLog);
		printf("Error: Failed to compile vertex shader!\n");
		printf("%s",infoLog);
		printf("\n");
		delete[] infoLog;
	}

	glShaderSource(m_fragmentShader, 1, (const char**)&fsSource, 0);
	glCompileShader(m_fragmentShader);
    
	glGetShaderiv(m_fragmentShader, GL_COMPILE_STATUS, &success);
	if (success == GL_FALSE)
	{
		int infoLogLength = 0;
		glGetShaderiv(m_fragmentShader, GL_INFO_LOG_LENGTH, &infoLogLength);
		char* infoLog = new char[infoLogLength];
        
		glGetShaderInfoLog(m_fragmentShader, infoLogLength, 0, infoLog);
		printf("Error: Failed to compile fragment shader!\n");
		printf("%s",infoLog);
		printf("\n");
		delete[] infoLog;
	}
    
	m_programID = glCreateProgram();
	glAttachShader(m_programID, m_vertexShader);
	glAttachShader(m_programID, m_fragmentShader);
	glBindAttribLocation(m_programID, 0, "Position");
	glBindAttribLocation(m_programID, 1, "Colour");
	glLinkProgram(m_programID);
    
    glGetProgramiv(m_programID, GL_LINK_STATUS, &success);
	if (success == GL_FALSE)
	{
		int infoLogLength = 0;
		glGetShaderiv(m_programID, GL_INFO_LOG_LENGTH, &infoLogLength);
		char* infoLog = new char[infoLogLength];
        
		glGetShaderInfoLog(m_programID, infoLogLength, 0, infoLog);
		printf("Error: Failed to link shader program!\n");
		printf("%s",infoLog);
		printf("\n");
		delete[] infoLog;
	}
    
    // create VBOs
	glGenBuffers( 1, &m_lineVBO );
	glBindBuffer(GL_ARRAY_BUFFER, m_lineVBO);
	glBufferData(GL_ARRAY_BUFFER, m_maxLines * sizeof(GizmoLine), m_lines, GL_DYNAMIC_DRAW);


	glGenBuffers( 1, &m_triVBO );
	glBindBuffer(GL_ARRAY_BUFFER, m_triVBO);
	glBufferData(GL_ARRAY_BUFFER, m_maxTris * sizeof(GizmoTri), m_tris, GL_DYNAMIC_DRAW);


	glGenBuffers( 1, &m_transparentTriVBO );
	glBindBuffer(GL_ARRAY_BUFFER, m_transparentTriVBO);
	glBufferData(GL_ARRAY_BUFFER, m_maxTris * sizeof(GizmoTri), m_transparentTris, GL_DYNAMIC_DRAW);

	glGenVertexArrays(1, &m_lineVAO);
	glBindVertexArray(m_lineVAO);
	glBindBuffer(GL_ARRAY_BUFFER, m_lineVBO);
	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, sizeof(GizmoVertex), 0);
	glVertexAttribPointer(1, 4, GL_FLOAT, GL_TRUE, sizeof(GizmoVertex), ((char*)0) + 16);

	glGenVertexArrays(1, &m_triVAO);
	glBindVertexArray(m_triVAO);
	glBindBuffer(GL_ARRAY_BUFFER, m_triVBO);
	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, sizeof(GizmoVertex), 0);
	glVertexAttribPointer(1, 4, GL_FLOAT, GL_TRUE, sizeof(GizmoVertex), ((char*)0) + 16);

	glGenVertexArrays(1, &m_transparentTriVAO);
	glBindVertexArray(m_transparentTriVAO);
	glBindBuffer(GL_ARRAY_BUFFER, m_transparentTriVBO);
	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, sizeof(GizmoVertex), 0);
	glVertexAttribPointer(1, 4, GL_FLOAT, GL_TRUE, sizeof(GizmoVertex), ((char*)0) + 16);

	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
}

Gizmos::~Gizmos()
{
	delete[] m_lines;
	delete[] m_tris;
	delete[] m_transparentTris;
	glDeleteBuffers( 1, &m_lineVBO );
	glDeleteBuffers( 1, &m_triVBO );
	glDeleteBuffers( 1, &m_transparentTriVBO );
	glDeleteVertexArrays( 1, &m_lineVAO );
	glDeleteVertexArrays( 1, &m_triVAO );
	glDeleteVertexArrays( 1, &m_transparentTriVAO );
	glDeleteProgram(m_programID);
	glDeleteShader(m_fragmentShader);
	glDeleteShader(m_vertexShader);
}

void Gizmos::create(unsigned int a_maxLines /* = 16384 */, unsigned int a_maxTris /* = 16384 */)
{
	if (sm_singleton == nullptr)
		sm_singleton = new Gizmos(a_maxLines,a_maxTris);
}

void Gizmos::destroy()
{
	delete sm_singleton;
	sm_singleton = nullptr;
}

void Gizmos::clear()
{
	sm_singleton->m_lineCount = 0;
	sm_singleton->m_triCount = 0;
	sm_singleton->m_transparentTriCount = 0;
}

// Adds 3 unit-length lines (red,green,blue) representing the 3 axis of a transform, 
// at the transform's translation. Optional scale available.
void Gizmos::addTransform(const glm::mat4& a_transform, float a_fScale /* = 1.0f */)
{
	glm::vec4 vXAxis = a_transform[3] + a_transform[0] * a_fScale;
	glm::vec4 vYAxis = a_transform[3] + a_transform[1] * a_fScale;
	glm::vec4 vZAxis = a_transform[3] + a_transform[2] * a_fScale;

	glm::vec4 vRed(1,0,0,1);
	glm::vec4 vGreen(0,1,0,1);
	glm::vec4 vBlue(0,0,1,1);

	addLine(a_transform[3].xyz, vXAxis.xyz, vRed, vRed);
	addLine(a_transform[3].xyz, vYAxis.xyz, vGreen, vGreen);
	addLine(a_transform[3].xyz, vZAxis.xyz, vBlue, vBlue);
}

void Gizmos::addAABB(const glm::vec3& a_center, 
	const glm::vec3& a_rvExtents, 
	const glm::vec4& a_colour, 
	const glm::mat4* a_transform /* = nullptr */)
{
	glm::vec3 vVerts[8];
	glm::vec3 vX(a_rvExtents.x, 0, 0);
	glm::vec3 vY(0, a_rvExtents.y, 0);
	glm::vec3 vZ(0, 0, a_rvExtents.z);

	if (a_transform != nullptr)
	{
		vX = (*a_transform * glm::vec4(vX,0)).xyz;
		vY = (*a_transform * glm::vec4(vY,0)).xyz;
		vZ = (*a_transform * glm::vec4(vZ,0)).xyz;
	}

	// top verts
	vVerts[0] = a_center - vX - vZ - vY;
	vVerts[1] = a_center - vX + vZ - vY;
	vVerts[2] = a_center + vX + vZ - vY;
	vVerts[3] = a_center + vX - vZ - vY;

	// bottom verts
	vVerts[4] = a_center - vX - vZ + vY;
	vVerts[5] = a_center - vX + vZ + vY;
	vVerts[6] = a_center + vX + vZ + vY;
	vVerts[7] = a_center + vX - vZ + vY;

	addLine(vVerts[0], vVerts[1], a_colour, a_colour);
	addLine(vVerts[1], vVerts[2], a_colour, a_colour);
	addLine(vVerts[2], vVerts[3], a_colour, a_colour);
	addLine(vVerts[3], vVerts[0], a_colour, a_colour);

	addLine(vVerts[4], vVerts[5], a_colour, a_colour);
	addLine(vVerts[5], vVerts[6], a_colour, a_colour);
	addLine(vVerts[6], vVerts[7], a_colour, a_colour);
	addLine(vVerts[7], vVerts[4], a_colour, a_colour);

	addLine(vVerts[0], vVerts[4], a_colour, a_colour);
	addLine(vVerts[1], vVerts[5], a_colour, a_colour);
	addLine(vVerts[2], vVerts[6], a_colour, a_colour);
	addLine(vVerts[3], vVerts[7], a_colour, a_colour);
}

void Gizmos::addAABBFilled(const glm::vec3& a_center, 
	const glm::vec3& a_rvExtents, 
	const glm::vec4& a_fillColour, 
	const glm::mat4* a_transform /* = nullptr */)
{
	glm::vec3 vVerts[8];
	glm::vec3 vX(a_rvExtents.x, 0, 0);
	glm::vec3 vY(0, a_rvExtents.y, 0);
	glm::vec3 vZ(0, 0, a_rvExtents.z);

	if (a_transform != nullptr)
	{
		vX = (*a_transform * glm::vec4(vX,0)).xyz;
		vY = (*a_transform * glm::vec4(vY,0)).xyz;
		vZ = (*a_transform * glm::vec4(vZ,0)).xyz;
	}

	// top verts
	vVerts[0] = a_center - vX - vZ - vY;
	vVerts[1] = a_center - vX + vZ - vY;
	vVerts[2] = a_center + vX + vZ - vY;
	vVerts[3] = a_center + vX - vZ - vY;

	// bottom verts
	vVerts[4] = a_center - vX - vZ + vY;
	vVerts[5] = a_center - vX + vZ + vY;
	vVerts[6] = a_center + vX + vZ + vY;
	vVerts[7] = a_center + vX - vZ + vY;

	glm::vec4 vWhite(1,1,1,1);

	addLine(vVerts[0], vVerts[1], vWhite, vWhite);
	addLine(vVerts[1], vVerts[2], vWhite, vWhite);
	addLine(vVerts[2], vVerts[3], vWhite, vWhite);
	addLine(vVerts[3], vVerts[0], vWhite, vWhite);

	addLine(vVerts[4], vVerts[5], vWhite, vWhite);
	addLine(vVerts[5], vVerts[6], vWhite, vWhite);
	addLine(vVerts[6], vVerts[7], vWhite, vWhite);
	addLine(vVerts[7], vVerts[4], vWhite, vWhite);

	addLine(vVerts[0], vVerts[4], vWhite, vWhite);
	addLine(vVerts[1], vVerts[5], vWhite, vWhite);
	addLine(vVerts[2], vVerts[6], vWhite, vWhite);
	addLine(vVerts[3], vVerts[7], vWhite, vWhite);

	// top
	addTri(vVerts[2], vVerts[1], vVerts[0], a_fillColour);
	addTri(vVerts[3], vVerts[2], vVerts[0], a_fillColour);

	// bottom
	addTri(vVerts[5], vVerts[6], vVerts[4], a_fillColour);
	addTri(vVerts[6], vVerts[7], vVerts[4], a_fillColour);

	// front
	addTri(vVerts[4], vVerts[3], vVerts[0], a_fillColour);
	addTri(vVerts[7], vVerts[3], vVerts[4], a_fillColour);

	// back
	addTri(vVerts[1], vVerts[2], vVerts[5], a_fillColour);
	addTri(vVerts[2], vVerts[6], vVerts[5], a_fillColour);

	// left
	addTri(vVerts[0], vVerts[1], vVerts[4], a_fillColour);
	addTri(vVerts[1], vVerts[5], vVerts[4], a_fillColour);

	// right
	addTri(vVerts[2], vVerts[3], vVerts[7], a_fillColour);
	addTri(vVerts[6], vVerts[2], vVerts[7], a_fillColour);
}

void Gizmos::addCylinderFilled(const glm::vec3& a_center, float a_radius, float a_fHalfLength,
	unsigned int a_segments, const glm::vec4& a_fillColour, const glm::mat4* a_transform /* = nullptr */)
{
	glm::vec4 vWhite(1,1,1,1);

	float fSegmentSize = (2 * glm::pi<float>()) / a_segments;

	for ( unsigned int i = 0 ; i < a_segments ; ++i )
	{
		glm::vec3 v0top(0,a_fHalfLength,0);
		glm::vec3 v1top( sinf( i * fSegmentSize ) * a_radius, a_fHalfLength, cosf( i * fSegmentSize ) * a_radius);
		glm::vec3 v2top( sinf( (i+1) * fSegmentSize ) * a_radius, a_fHalfLength, cosf( (i+1) * fSegmentSize ) * a_radius);
		glm::vec3 v0bottom(0,-a_fHalfLength,0);
		glm::vec3 v1bottom( sinf( i * fSegmentSize ) * a_radius, -a_fHalfLength, cosf( i * fSegmentSize ) * a_radius);
		glm::vec3 v2bottom( sinf( (i+1) * fSegmentSize ) * a_radius, -a_fHalfLength, cosf( (i+1) * fSegmentSize ) * a_radius);

		if (a_transform != nullptr)
		{
			v0top = (*a_transform * glm::vec4(v0top,0)).xyz;
			v1top = (*a_transform * glm::vec4(v1top,0)).xyz;
			v2top = (*a_transform * glm::vec4(v2top,0)).xyz;
			v0bottom = (*a_transform * glm::vec4(v0bottom,0)).xyz;
			v1bottom = (*a_transform * glm::vec4(v1bottom,0)).xyz;
			v2bottom = (*a_transform * glm::vec4(v2bottom,0)).xyz;
		}

		// triangles
		addTri( a_center + v0top, a_center + v1top, a_center + v2top, a_fillColour);
		addTri( a_center + v0bottom, a_center + v2bottom, a_center + v1bottom, a_fillColour);
		addTri( a_center + v2top, a_center + v1top, a_center + v1bottom, a_fillColour);
		addTri( a_center + v1bottom, a_center + v2bottom, a_center + v2top, a_fillColour);

		// lines
		addLine(a_center + v1top, a_center + v2top, vWhite, vWhite);
		addLine(a_center + v1top, a_center + v1bottom, vWhite, vWhite);
		addLine(a_center + v1bottom, a_center + v2bottom, vWhite, vWhite);
	}
}

void Gizmos::addRing(const glm::vec3& a_center, float a_innerRadius, float a_outerRadius,
	unsigned int a_segments, const glm::vec4& a_fillColour, const glm::mat4* a_transform /* = nullptr */)
{
	glm::vec4 vSolid = a_fillColour;
	vSolid.w = 1;

	float fSegmentSize = (2 * glm::pi<float>()) / a_segments;

	for ( unsigned int i = 0 ; i < a_segments ; ++i )
	{
		glm::vec3 v1outer( sinf( i * fSegmentSize ) * a_outerRadius, 0, cosf( i * fSegmentSize ) * a_outerRadius );
		glm::vec3 v2outer( sinf( (i+1) * fSegmentSize ) * a_outerRadius, 0, cosf( (i+1) * fSegmentSize ) * a_outerRadius );
		glm::vec3 v1inner( sinf( i * fSegmentSize ) * a_innerRadius, 0, cosf( i * fSegmentSize ) * a_innerRadius );
		glm::vec3 v2inner( sinf( (i+1) * fSegmentSize ) * a_innerRadius, 0, cosf( (i+1) * fSegmentSize ) * a_innerRadius );

		if (a_transform != nullptr)
		{
			v1outer = (*a_transform * glm::vec4(v1outer,0)).xyz;
			v2outer = (*a_transform * glm::vec4(v2outer,0)).xyz;
			v1inner = (*a_transform * glm::vec4(v1inner,0)).xyz;
			v2inner = (*a_transform * glm::vec4(v2inner,0)).xyz;
		}

		if (a_fillColour.w != 0)
		{
			addTri(a_center + v2outer, a_center + v1outer, a_center + v1inner, a_fillColour);
			addTri(a_center + v1inner, a_center + v2inner, a_center + v2outer, a_fillColour);

			addTri(a_center + v1inner, a_center + v1outer, a_center + v2outer, a_fillColour);
			addTri(a_center + v2outer, a_center + v2inner, a_center + v1inner, a_fillColour);
		}
		else
		{
			// line
			addLine(a_center + v1inner + a_center, a_center + v2inner + a_center, vSolid, vSolid);
			addLine(a_center + v1outer + a_center, a_center + v2outer + a_center, vSolid, vSolid);
		}
	}
}

void Gizmos::addDisk(const glm::vec3& a_center, float a_radius,
	unsigned int a_segments, const glm::vec4& a_fillColour, const glm::mat4* a_transform /* = nullptr */)
{
	glm::vec4 vSolid = a_fillColour;
	vSolid.w = 1;

	float fSegmentSize = (2 * glm::pi<float>()) / a_segments;

	for ( unsigned int i = 0 ; i < a_segments ; ++i )
	{
		glm::vec3 v1outer( sinf( i * fSegmentSize ) * a_radius, 0, cosf( i * fSegmentSize ) * a_radius );
		glm::vec3 v2outer( sinf( (i+1) * fSegmentSize ) * a_radius, 0, cosf( (i+1) * fSegmentSize ) * a_radius );

		if (a_transform != nullptr)
		{
			v1outer = (*a_transform * glm::vec4(v1outer,0)).xyz;
			v2outer = (*a_transform * glm::vec4(v2outer,0)).xyz;
		}

		if (a_fillColour.w != 0)
		{
			addTri(a_center, v1outer, a_center + v2outer, a_fillColour);
			addTri(a_center + v2outer, a_center + v1outer, a_center, a_fillColour);
		}
		else
		{
			// line
			addLine(a_center + v1outer, a_center + v2outer, vSolid, vSolid);
		}
	}
}

void Gizmos::addArc(const glm::vec3& a_center, float a_rotation,
	float a_radius, float a_arcHalfAngle,
	unsigned int a_segments, const glm::vec4& a_fillColour, const glm::mat4* a_transform /* = nullptr */)
{
	glm::vec4 vSolid = a_fillColour;
	vSolid.w = 1;

	float fSegmentSize = (2 * a_arcHalfAngle) / a_segments;

	for ( unsigned int i = 0 ; i < a_segments ; ++i )
	{
		glm::vec3 v1outer( sinf( i * fSegmentSize - a_arcHalfAngle + a_rotation ) * a_radius, 0, cosf( i * fSegmentSize - a_arcHalfAngle + a_rotation ) * a_radius);
		glm::vec3 v2outer( sinf( (i+1) * fSegmentSize - a_arcHalfAngle + a_rotation ) * a_radius, 0, cosf( (i+1) * fSegmentSize - a_arcHalfAngle + a_rotation ) * a_radius);

		if (a_transform != nullptr)
		{
			v1outer = (*a_transform * glm::vec4(v1outer,0)).xyz;
			v2outer = (*a_transform * glm::vec4(v2outer,0)).xyz;
		}

		if (a_fillColour.w != 0)
		{
			addTri(a_center, a_center + v1outer, a_center + v2outer, a_fillColour);
			addTri(a_center + v2outer, a_center + v1outer, a_center, a_fillColour);
		}
		else
		{
			// line
			addLine(a_center + v1outer, a_center + v2outer, vSolid, vSolid);
		}
	}

	// edge lines
	if (a_fillColour.w == 0)
	{
		glm::vec3 v1outer( sinf( -a_arcHalfAngle + a_rotation ) * a_radius, 0, cosf( -a_arcHalfAngle + a_rotation ) * a_radius );
		glm::vec3 v2outer( sinf( a_arcHalfAngle + a_rotation ) * a_radius, 0, cosf( a_arcHalfAngle + a_rotation ) * a_radius );

		if (a_transform != nullptr)
		{
			v1outer = (*a_transform * glm::vec4(v1outer,0)).xyz;
			v2outer = (*a_transform * glm::vec4(v2outer,0)).xyz;
		}

		addLine(a_center, a_center + v1outer, vSolid, vSolid);
		addLine(a_center, a_center + v2outer, vSolid, vSolid);
	}
}

void Gizmos::addArcRing(const glm::vec3& a_center, float a_rotation, 
	float a_innerRadius, float a_outerRadius, float a_arcHalfAngle,
	unsigned int a_segments, const glm::vec4& a_fillColour, const glm::mat4* a_transform /* = nullptr */)
{
	glm::vec4 vSolid = a_fillColour;
	vSolid.w = 1;

	float fSegmentSize = (2 * a_arcHalfAngle) / a_segments;

	for ( unsigned int i = 0 ; i < a_segments ; ++i )
	{
		glm::vec3 v1outer( sinf( i * fSegmentSize - a_arcHalfAngle + a_rotation ) * a_outerRadius, 0, cosf( i * fSegmentSize - a_arcHalfAngle + a_rotation ) * a_outerRadius );
		glm::vec3 v2outer( sinf( (i+1) * fSegmentSize - a_arcHalfAngle + a_rotation ) * a_outerRadius, 0, cosf( (i+1) * fSegmentSize - a_arcHalfAngle + a_rotation ) * a_outerRadius );
		glm::vec3 v1inner( sinf( i * fSegmentSize - a_arcHalfAngle + a_rotation  ) * a_innerRadius, 0, cosf( i * fSegmentSize - a_arcHalfAngle + a_rotation  ) * a_innerRadius );
		glm::vec3 v2inner( sinf( (i+1) * fSegmentSize - a_arcHalfAngle + a_rotation  ) * a_innerRadius, 0, cosf( (i+1) * fSegmentSize - a_arcHalfAngle + a_rotation  ) * a_innerRadius );

		if (a_transform != nullptr)
		{
			v1outer = (*a_transform * glm::vec4(v1outer,0)).xyz;
			v2outer = (*a_transform * glm::vec4(v2outer,0)).xyz;
			v1inner = (*a_transform * glm::vec4(v1inner,0)).xyz;
			v2inner = (*a_transform * glm::vec4(v2inner,0)).xyz;
		}

		if (a_fillColour.w != 0)
		{
			addTri(a_center + v2outer, a_center + v1outer, a_center + v1inner, a_fillColour);
			addTri(a_center + v1inner, a_center + v2inner, a_center + v2outer, a_fillColour);

			addTri(a_center + v1inner, a_center + v1outer, a_center + v2outer, a_fillColour);
			addTri(a_center + v2outer, a_center + v2inner, a_center + v1inner, a_fillColour);
		}
		else
		{
			// line
			addLine(a_center + v1inner, a_center + v2inner, vSolid, vSolid);
			addLine(a_center + v1outer, a_center + v2outer, vSolid, vSolid);
		}
	}

	// edge lines
	if (a_fillColour.w == 0)
	{
		glm::vec3 v1outer( sinf( -a_arcHalfAngle + a_rotation ) * a_outerRadius, 0, cosf( -a_arcHalfAngle + a_rotation ) * a_outerRadius );
		glm::vec3 v2outer( sinf( a_arcHalfAngle + a_rotation ) * a_outerRadius, 0, cosf( a_arcHalfAngle + a_rotation ) * a_outerRadius );
		glm::vec3 v1inner( sinf( -a_arcHalfAngle + a_rotation  ) * a_innerRadius, 0, cosf( -a_arcHalfAngle + a_rotation  ) * a_innerRadius );
		glm::vec3 v2inner( sinf( a_arcHalfAngle + a_rotation  ) * a_innerRadius, 0, cosf( a_arcHalfAngle + a_rotation  ) * a_innerRadius );

		if (a_transform != nullptr)
		{
			v1outer = (*a_transform * glm::vec4(v1outer,0)).xyz;
			v2outer = (*a_transform * glm::vec4(v2outer,0)).xyz;
			v1inner = (*a_transform * glm::vec4(v1inner,0)).xyz;
			v2inner = (*a_transform * glm::vec4(v2inner,0)).xyz;
		}

		addLine(a_center + v1inner, a_center + v1outer, vSolid, vSolid);
		addLine(a_center + v2inner, a_center + v2outer, vSolid, vSolid);
	}
}

void Gizmos::addSphere(const glm::vec3& a_center, int a_rows, int a_columns, float a_radius, const glm::vec4& a_fillColour, 
								const glm::mat4* a_transform /*= nullptr*/, float a_longMin /*= 0.f*/, float a_longMax /*= 360*/, 
								float a_latMin /*= -90*/, float a_latMax /*= 90*/)
{
	float inverseRadius = 1/a_radius;
	//Invert these first as the multiply is slightly quicker
	float invColumns = 1.0f/float(a_columns);
	float invRows = 1.0f/float(a_rows);

	float DEG2RAD = glm::pi<float>() / 180;
	
	//Lets put everything in radians first
	float latitiudinalRange = (a_latMax - a_latMin) * DEG2RAD;
	float longitudinalRange = (a_longMax - a_longMin) * DEG2RAD;
	// for each row of the mesh
	glm::vec3* v4Array = new glm::vec3[a_rows*a_columns + a_columns];

	for (int row = 0; row <= a_rows; ++row)
	{
		// y ordinates this may be a little confusing but here we are navigating around the xAxis in GL
		float ratioAroundXAxis = float(row) * invRows;
		float radiansAboutXAxis  = ratioAroundXAxis * latitiudinalRange + (a_latMin * DEG2RAD);
		float y  =  a_radius * sin(radiansAboutXAxis);
		float z  =  a_radius * cos(radiansAboutXAxis);
		
		for ( int col = 0; col <= a_columns; ++col )
		{
			float ratioAroundYAxis   = float(col) * invColumns;
			float theta = ratioAroundYAxis * longitudinalRange + (a_longMin * DEG2RAD);
			glm::vec3 v4Point( -z * sinf(theta), y, -z * cosf(theta) );
			glm::vec3 v4Normal( inverseRadius * v4Point.x, inverseRadius * v4Point.y, inverseRadius * v4Point.z);

			if (a_transform != nullptr)
			{
				v4Point = (*a_transform * glm::vec4(v4Point,0)).xyz;
				v4Normal = (*a_transform * glm::vec4(v4Normal,0)).xyz;
			}

			int index = row * a_columns + (col % a_columns);
			v4Array[index] = v4Point;
		}
	}
	
	for (int face = 0; face < (a_rows)*(a_columns); ++face )
	{
		int iNextFace = face + 1;		
		
		if( iNextFace % a_columns == 0 )
		{
			iNextFace = iNextFace - (a_columns);
		}

		addLine(a_center + v4Array[face], a_center + v4Array[face+a_columns], glm::vec4(1.f,1.f,1.f,1.f), glm::vec4(1.f,1.f,1.f,1.f));
		
		if( face % a_columns == 0 && longitudinalRange < (glm::pi<float>() * 2))
		{
				continue;
		}
		addLine(a_center + v4Array[iNextFace+a_columns], a_center + v4Array[face+a_columns], glm::vec4(1.f,1.f,1.f,1.f), glm::vec4(1.f,1.f,1.f,1.f));

		addTri( a_center + v4Array[iNextFace+a_columns], a_center + v4Array[face], a_center + v4Array[iNextFace], a_fillColour);
		addTri( a_center + v4Array[iNextFace+a_columns], a_center + v4Array[face+a_columns], a_center + v4Array[face], a_fillColour);		
	}

	delete[] v4Array;	
}

void Gizmos::addHermiteSpline(const glm::vec3& a_start, const glm::vec3& a_end,
	const glm::vec3& a_tangentStart, const glm::vec3& a_tangentEnd, unsigned int a_segments, const glm::vec4& a_colour)
{
	a_segments = a_segments > 1 ? a_segments : 1;

	glm::vec3 prev = a_start;

	for ( unsigned int i = 1 ; i <= a_segments ; ++i )
	{
		float s = i / (float)a_segments;

		float s2 = s * s;
		float s3 = s2 * s;
		float h1 = (2.0f * s3) - (3.0f * s2) + 1.0f;
		float h2 = (-2.0f * s3) + (3.0f * s2);
		float h3 =  s3- (2.0f * s2) + s;
		float h4 = s3 - s2;
		glm::vec3 p = (a_start * h1) + (a_end * h2) + (a_tangentStart * h3) + (a_tangentEnd * h4);

		addLine(prev,p,a_colour,a_colour);
		prev = p;
	}
}

void Gizmos::addLine(const glm::vec3& a_rv0,  const glm::vec3& a_rv1, const glm::vec4& a_colour)
{
	addLine(a_rv0,a_rv1,a_colour,a_colour);
}

void Gizmos::addLine(const glm::vec3& a_rv0, const glm::vec3& a_rv1, const glm::vec4& a_colour0, const glm::vec4& a_colour1)
{
	if (sm_singleton != nullptr &&
		sm_singleton->m_lineCount < sm_singleton->m_maxLines)
	{
		sm_singleton->m_lines[ sm_singleton->m_lineCount ].v0.position = glm::vec4(a_rv0,1);
		sm_singleton->m_lines[ sm_singleton->m_lineCount ].v0.colour = a_colour0;
		sm_singleton->m_lines[ sm_singleton->m_lineCount ].v1.position = glm::vec4(a_rv1,1);
		sm_singleton->m_lines[ sm_singleton->m_lineCount ].v1.colour = a_colour1;

		sm_singleton->m_lineCount++;
	}
}

void Gizmos::addTri(const glm::vec3& a_rv0, const glm::vec3& a_rv1, const glm::vec3& a_rv2, const glm::vec4& a_colour)
{
	if (sm_singleton != nullptr)
	{
		if (a_colour.w == 1)
		{
			if (sm_singleton->m_triCount < sm_singleton->m_maxTris)
			{
				sm_singleton->m_tris[ sm_singleton->m_triCount ].v0.position = glm::vec4(a_rv0,1);
				sm_singleton->m_tris[ sm_singleton->m_triCount ].v1.position = glm::vec4(a_rv1,1);
				sm_singleton->m_tris[ sm_singleton->m_triCount ].v2.position = glm::vec4(a_rv2,1);
				sm_singleton->m_tris[ sm_singleton->m_triCount ].v0.colour = a_colour;
				sm_singleton->m_tris[ sm_singleton->m_triCount ].v1.colour = a_colour;
				sm_singleton->m_tris[ sm_singleton->m_triCount ].v2.colour = a_colour;

				sm_singleton->m_triCount++;
			}
		}
		else
		{
			if (sm_singleton->m_transparentTriCount < sm_singleton->m_maxTris)
			{
				sm_singleton->m_transparentTris[ sm_singleton->m_transparentTriCount ].v0.position = glm::vec4(a_rv0,1);
				sm_singleton->m_transparentTris[ sm_singleton->m_transparentTriCount ].v1.position = glm::vec4(a_rv1,1);
				sm_singleton->m_transparentTris[ sm_singleton->m_transparentTriCount ].v2.position = glm::vec4(a_rv2,1);
				sm_singleton->m_transparentTris[ sm_singleton->m_transparentTriCount ].v0.colour = a_colour;
				sm_singleton->m_transparentTris[ sm_singleton->m_transparentTriCount ].v1.colour = a_colour;
				sm_singleton->m_transparentTris[ sm_singleton->m_transparentTriCount ].v2.colour = a_colour;

				sm_singleton->m_transparentTriCount++;
			}
		}
	}
}

// builds a simple 2 triangle quad with a position, colour and texture coordinates
void Gizmos::add3DPlane(float a_size, const glm::vec4& a_colour /* = glm::vec4(1,1,1,1) */ )
{
	unsigned int a_vao, a_vbo, a_ibo;
	float halfSize = a_size * 0.5f;

	// create mesh
	BasicVertex vertices[4];
	vertices[0].position = glm::vec4(-halfSize,0,-halfSize,1);
	vertices[0].colour = a_colour;
	vertices[0].texCoord = glm::vec2(0,0);
	vertices[1].position = glm::vec4(halfSize,0,-halfSize,1);
	vertices[1].colour = a_colour;
	vertices[1].texCoord = glm::vec2(1,0);
	vertices[2].position = glm::vec4(halfSize,0,halfSize,1);
	vertices[2].colour = a_colour;
	vertices[2].texCoord = glm::vec2(1,1);
	vertices[3].position = glm::vec4(-halfSize,0,halfSize,1);
	vertices[3].colour = a_colour;
	vertices[3].texCoord = glm::vec2(0,1);

	unsigned int indices[6] = {
		3,1,0,
		3,2,1 
	};

	// create and bind buffers to a vertex array object
	glGenBuffers(1, &a_vbo);
	glGenBuffers(1, &a_ibo);
	glGenVertexArrays(1, &a_vao);

	glBindVertexArray(a_vao);
	glBindBuffer(GL_ARRAY_BUFFER, a_vbo);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, a_ibo);

	glBufferData(GL_ARRAY_BUFFER, 4* sizeof(BasicVertex), vertices, GL_STATIC_DRAW);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, 6 * sizeof(unsigned int), indices, GL_STATIC_DRAW);

	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, sizeof(BasicVertex), 0);
	glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, sizeof(BasicVertex), ((char*)0) + 16);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(BasicVertex), ((char*)0) + 32);

	// unbind vertex array
	glBindVertexArray(a_vao);
	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr);
	glBindVertexArray(0);
}

void Gizmos::draw(const glm::mat4& a_projectionView)
{
	if ( sm_singleton != nullptr &&
		(sm_singleton->m_lineCount > 0 || sm_singleton->m_triCount > 0 || sm_singleton->m_transparentTriCount > 0))
	{
		glUseProgram(sm_singleton->m_programID);
		
		unsigned int projectionViewUniform = glGetUniformLocation(sm_singleton->m_programID,"ProjectionView");
		glUniformMatrix4fv(projectionViewUniform, 1, false, glm::value_ptr(a_projectionView));

		if (sm_singleton->m_lineCount > 0)
		{
			glBindBuffer(GL_ARRAY_BUFFER, sm_singleton->m_lineVBO);
			glBufferSubData(GL_ARRAY_BUFFER, 0, sm_singleton->m_lineCount * sizeof(GizmoLine), sm_singleton->m_lines);

			glBindVertexArray(sm_singleton->m_lineVAO);
			glDrawArrays(GL_LINES, 0, sm_singleton->m_lineCount * 2);
		}

		if (sm_singleton->m_triCount > 0)
		{
			glBindBuffer(GL_ARRAY_BUFFER, sm_singleton->m_triVBO);
			glBufferSubData(GL_ARRAY_BUFFER, 0, sm_singleton->m_triCount * sizeof(GizmoTri), sm_singleton->m_tris);

			glBindVertexArray(sm_singleton->m_triVAO);
			glDrawArrays(GL_TRIANGLES, 0, sm_singleton->m_triCount * 3);
		}
		
		if (sm_singleton->m_transparentTriCount > 0)
		{
			glDepthMask(false);
			glBindBuffer(GL_ARRAY_BUFFER, sm_singleton->m_transparentTriVBO);
			glBufferSubData(GL_ARRAY_BUFFER, 0, sm_singleton->m_transparentTriCount * sizeof(GizmoTri), sm_singleton->m_transparentTris);

			glBindVertexArray(sm_singleton->m_transparentTriVAO);
			glDrawArrays(GL_TRIANGLES, 0, sm_singleton->m_transparentTriCount * 3);
			glDepthMask(true);
		}
	}
}
