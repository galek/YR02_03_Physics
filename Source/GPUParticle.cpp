#include "GPUParticle.h"
#include "Utilities.h"

GPUParticle::GPUParticle(unsigned int MAX) {
	//GPU Stuff
	uiMaxParticles = MAX;
	gpParticles = new GPUParticleS[ uiMaxParticles ];

	uivao[0] = 0;		uivao[1] = 0;
	uivbo[0] = 0;		uivbo[1] = 0;
	uiActiveBuffer = 0;	uiOtherBuffer = 0;
	createBuffers();
	createUpdateShader();
	createDrawShader();

	setTextureID(0);
}
GPUParticle::~GPUParticle() {
	delete[] gpParticles;

	glDeleteVertexArrays(2, uivao);
	glDeleteBuffers(2, uivbo);

	// delete the shaders
	glDeleteProgram(uiDrawShader);
	glDeleteProgram(uiUpdateShader);
}

void GPUParticle::createBuffers() {
	// create opengl buffers
	glGenVertexArrays(2, uivao);
	glGenBuffers(2, uivbo);

	glBindVertexArray(uivao[0]);
	glBindBuffer(GL_ARRAY_BUFFER, uivbo[0]);
	glBufferData(GL_ARRAY_BUFFER, uiMaxParticles * sizeof(GPUParticleS), gpParticles, GL_STREAM_DRAW);

	glEnableVertexAttribArray(0); // position
	glEnableVertexAttribArray(1); // velocity
	glEnableVertexAttribArray(2); // lifetime
	glEnableVertexAttribArray(3); // lifespan
	glEnableVertexAttribArray(4); // rotation
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(GPUParticleS), 0);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(GPUParticleS), ((char*)0) + 12);
	glVertexAttribPointer(2, 1, GL_FLOAT, GL_FALSE, sizeof(GPUParticleS), ((char*)0) + 24);
	glVertexAttribPointer(3, 1, GL_FLOAT, GL_FALSE, sizeof(GPUParticleS), ((char*)0) + 28);
	glVertexAttribPointer(4, 1, GL_FLOAT, GL_FALSE, sizeof(GPUParticleS), ((char*)0) + 32);

	glBindVertexArray(uivao[1]);
	glBindBuffer(GL_ARRAY_BUFFER, uivbo[1]);
	glBufferData(GL_ARRAY_BUFFER, uiMaxParticles * sizeof(GPUParticleS), 0, GL_STREAM_DRAW);

	glEnableVertexAttribArray(0); // position
	glEnableVertexAttribArray(1); // velocity
	glEnableVertexAttribArray(2); // lifetime
	glEnableVertexAttribArray(3); // lifespan
	glEnableVertexAttribArray(4); // rotation
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(GPUParticleS), 0);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(GPUParticleS), ((char*)0) + 12);
	glVertexAttribPointer(2, 1, GL_FLOAT, GL_FALSE, sizeof(GPUParticleS), ((char*)0) + 24);
	glVertexAttribPointer(3, 1, GL_FLOAT, GL_FALSE, sizeof(GPUParticleS), ((char*)0) + 28);
	glVertexAttribPointer(4, 1, GL_FLOAT, GL_FALSE, sizeof(GPUParticleS), ((char*)0) + 32);

	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
}
void GPUParticle::createDrawShader() {
	unsigned int vs = Utility::loadShader("shaders/Assignment/GPUParticle.vert", GL_VERTEX_SHADER);
	unsigned int fs = Utility::loadShader("shaders/Assignment/GPUParticle.frag", GL_FRAGMENT_SHADER);
	unsigned int gs = Utility::loadShader("shaders/Assignment/GPUParticle.geom", GL_GEOMETRY_SHADER);
	const char* inputs[] = { "Position", "Velocity", "Lifetime", "Lifespan" ,"Rotation" };
	uiDrawShader = Utility::createProgram(vs, 0, 0, gs, fs, 5, inputs);
	
	// remove unneeded handles
	glDeleteShader(vs);
	glDeleteShader(gs);
	glDeleteShader(fs);
}
void GPUParticle::createUpdateShader() {
	// load a text file into an unsigned char buffer
	unsigned char* source = Utility::fileToBuffer("shaders/Assignment/GPUParticleUpdate.vert");

	// create a shader ourselves
	unsigned int vs = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vs, 1, (const char**)&source, 0);
	glCompileShader(vs);

	delete[] source;

	uiUpdateShader = glCreateProgram();
	glAttachShader(uiUpdateShader, vs);

	// set the inputs (not needed if using layout(location) shader tags)
	glBindAttribLocation(uiUpdateShader, 0, "Position");
	glBindAttribLocation(uiUpdateShader, 1, "Velocity");
	glBindAttribLocation(uiUpdateShader, 2, "Lifetime");
	glBindAttribLocation(uiUpdateShader, 3, "Lifespan");
	glBindAttribLocation(uiUpdateShader, 4, "Rotation");

	const char* varyings[] = { "position", "velocity", "lifetime", "lifespan" ,"rotation" };
	glTransformFeedbackVaryings(uiUpdateShader, 5, varyings, GL_INTERLEAVED_ATTRIBS);
	
	glLinkProgram(uiUpdateShader);
	
	// remove unneeded handles
	glDeleteShader(vs);
}

void GPUParticle::draw(const glm::mat4& a_cameraTransform, const glm::mat4& a_projection) {
	// update the particles using a vertex shader and transform feedback
	shaderUpdate();
	// draw the particles using the Geometry Shader to billboard them
	shaderDraw(a_cameraTransform,a_projection);
}
void GPUParticle::shaderUpdate(float time, float delta){
	glUseProgram(uiUpdateShader);

	// bind time information
	unsigned int location = glGetUniformLocation(uiUpdateShader,"time");
	if (time == 0.0f){time = Utility::getTotalTime();}
	glUniform1f(location, time);

	location = glGetUniformLocation(uiUpdateShader,"deltaTime");
	if (delta == 0.0f){delta = Utility::getDeltaTime();}
	glUniform1f(location, delta);

	// disable rasterisation
	glEnable(GL_RASTERIZER_DISCARD);

	// bind the buffer we will update
	glBindVertexArray(uivao[uiActiveBuffer]);

	// work out the "other" buffer
	uiOtherBuffer = (uiActiveBuffer + 1) % 2;

	// bind the buffer we will update into as points and begin transform feedback
	glBindBufferBase(GL_TRANSFORM_FEEDBACK_BUFFER, 0, uivbo[ uiOtherBuffer ]);
	glBeginTransformFeedback(GL_POINTS);

	glDrawArrays(GL_POINTS, 0, uiMaxParticles);

	// disable transform feedback and enable rasterisation again
	glEndTransformFeedback();
	glDisable(GL_RASTERIZER_DISCARD);
	glBindBufferBase(GL_TRANSFORM_FEEDBACK_BUFFER, 0, 0);
}
void GPUParticle::shaderDraw(const glm::mat4& a_cameraTransform, const glm::mat4& a_projection){
	
	glUseProgram(uiDrawShader);

	unsigned int location = glGetUniformLocation(uiDrawShader,"Projection");
	glUniformMatrix4fv(location, 1, false, glm::value_ptr(a_projection));

	location = glGetUniformLocation(uiDrawShader,"cameraTransform");
	glUniformMatrix4fv(location, 1,false, glm::value_ptr(a_cameraTransform));

	location = glGetUniformLocation(uiDrawShader,"useTextures");
	if (uiTexID != 0){
		glUniform1i(location, 1);
		GLint uDiffuseTexture = glGetUniformLocation(uiDrawShader, "DiffuseTexture");
		glActiveTexture( GL_TEXTURE1 );
		glBindTexture( GL_TEXTURE_2D, uiTexID );
		glUniform1i( uDiffuseTexture, 1 );
		glActiveTexture( 0 );
	}else{
	glUniform1i(location,0);
	}

	// draw particles in the "other" buffer
	glBindVertexArray(uivao[uiOtherBuffer]);
	glDrawArrays(GL_POINTS, 0, uiMaxParticles);

	// swap for next frame
	uiActiveBuffer = uiOtherBuffer;
	//back to default
}

void GPUParticle::setPosition(const glm::vec3 a_Vec){
	v3Position = a_Vec;
	glUseProgram(uiUpdateShader);
	unsigned int location = glGetUniformLocation(uiUpdateShader,"emitterPosition");
	glUniform3fv(location, 1, glm::value_ptr(v3Position));
}
void GPUParticle::setRange(const glm::vec3 a_Vec){
	v3Range = a_Vec;
	glUseProgram(uiUpdateShader);
	unsigned int location = glGetUniformLocation(uiUpdateShader,"emitterRange");
	glUniform3fv(location, 1, glm::value_ptr(v3Range));
}
void GPUParticle::setVelocity(const glm::vec3 a_Vec){
	v3Velocity = a_Vec;
	glUseProgram(uiUpdateShader);
	unsigned int location = glGetUniformLocation(uiUpdateShader,"StartVelocity");
	glUniform3fv(location, 1, glm::value_ptr(v3Velocity));
}
void GPUParticle::setVelocityJitterMax(const glm::vec3 a_Vec){
	v3VelocityJitter = a_Vec;
	glUseProgram(uiUpdateShader);
	unsigned int location = glGetUniformLocation(uiUpdateShader,"VelocityJitter");
	glUniform3fv(location, 1, glm::value_ptr(v3VelocityJitter));
}
void GPUParticle::setAcceleration(const glm::vec3 a_Vec){
	v3Acceleration = a_Vec;
	glUseProgram(uiUpdateShader);
	unsigned int location = glGetUniformLocation(uiUpdateShader,"acceleration");
	glUniform3fv(location,1, glm::value_ptr(v3Acceleration));
}
void GPUParticle::setSizeStart(const glm::vec2 a_Vec){
	v2SizeStart = a_Vec;
	glUseProgram(uiDrawShader);
	unsigned int location = glGetUniformLocation(uiDrawShader,"sizeStart");
	glUniform2fv(location,1, glm::value_ptr(v2SizeStart));
}
void GPUParticle::setSizeEnd(const glm::vec2 a_Vec){
	v2SizeEnd = a_Vec;
	glUseProgram(uiDrawShader);
	unsigned int location = glGetUniformLocation(uiDrawShader,"sizeEnd");
	glUniform2fv(location,1, glm::value_ptr(v2SizeEnd));
}
void GPUParticle::setColourStart(const glm::vec4 a_Vec){
	v4ColourStart = a_Vec;
	glUseProgram(uiDrawShader);
	unsigned int location = glGetUniformLocation(uiDrawShader,"colourStart");
	glUniform4fv(location, 1, glm::value_ptr(v4ColourStart));
}
void GPUParticle::setColourEnd(const glm::vec4 a_Vec){
	v4ColourEnd	= a_Vec;
	glUseProgram(uiDrawShader);
	unsigned int location = glGetUniformLocation(uiDrawShader,"colourEnd");
	glUniform4fv(location, 1, glm::value_ptr(v4ColourEnd));
}
void GPUParticle::setLifeSpanStart(const float a_Float){
	fLifeSpanMin = a_Float;
	glUseProgram(uiUpdateShader);
	unsigned int location = glGetUniformLocation(uiUpdateShader,"lifeMin");
	glUniform1f(location, fLifeSpanMin);
}
void GPUParticle::setLifeSpanEnd(const float a_Float){
	fLifeSpanMax = a_Float;
	glUseProgram(uiUpdateShader);
	unsigned int location = glGetUniformLocation(uiUpdateShader,"lifeMax");	
	glUniform1f(location, fLifeSpanMax);
}
void GPUParticle::setRotationStepMax(const float a_Degree){
	fRotationStepMax = glm::radians(a_Degree);
	glUseProgram(uiUpdateShader);
	unsigned int location = glGetUniformLocation(uiUpdateShader,"rotationStepMax");
	glUniform1f(location, fRotationStepMax);
}
void GPUParticle::setTextureID(const unsigned int a_TextureID){
	uiTexID = a_TextureID;
}

void GPUParticle::PreWarm(float f_Time){
	float fTime = 0.0f;
	float fDeltaTime = 0.01f;
	while (fTime < f_Time){
		shaderUpdate(fTime,fDeltaTime);
		fTime+=fDeltaTime;
	}
}

