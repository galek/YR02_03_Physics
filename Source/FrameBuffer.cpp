#include "FrameBuffer.h"

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/ext.hpp>
#include <Utilities.h>

FrameBuffer::FrameBuffer(int i_count,unsigned int ui_Width,unsigned int ui_Height) {

	iMaxTextures = 0;
	glGetIntegerv(GL_MAX_COLOR_ATTACHMENTS, &iMaxTextures);
	printf("The number of FBO color attachments this graphics card supports is: %i\n", iMaxTextures);

	uiWidth = ui_Width;
	uiHeight = ui_Height;

	glGenFramebuffers(1, &m_fbo);
	glBindFramebuffer(GL_FRAMEBUFFER, m_fbo); 

	if (i_count < iMaxTextures){
		iCount = i_count;
	}else{
		iCount = iMaxTextures;
	}

	Textures = new unsigned int[iCount + 1];
	GLuint *drawBuffers = new GLuint[iCount];

	for (int i = 0; i < iCount; i++){
		glGenTextures(1, &Textures[i]); 
		glBindTexture(GL_TEXTURE_2D, Textures[i]); 
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);  
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);  
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);  
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR); 	 
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, uiWidth, uiHeight, 0, GL_RGBA, GL_FLOAT, NULL); 
		glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + i, Textures[i], 0); 
		drawBuffers[i] = GL_COLOR_ATTACHMENT0 + i;
	}

	glGenTextures(1,&Textures[iCount]);
	glBindTexture(GL_TEXTURE_2D,Textures[iCount]);	
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexImage2D(GL_TEXTURE_2D,0,GL_DEPTH_COMPONENT32F,uiWidth, uiHeight,0,GL_DEPTH_COMPONENT,GL_FLOAT,NULL);
	glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, Textures[iCount], 0);

	glDrawBuffers(i_count, drawBuffers);
	delete drawBuffers;

	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {  
		printf("Error creating frame buffer!\n"); 
	}

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

FrameBuffer::~FrameBuffer(void){
	glDeleteTextures(iCount,Textures);
	glDeleteFramebuffers(1, &m_fbo);
	delete Textures;
}

void FrameBuffer::Bind(){
	glBindFramebuffer(GL_FRAMEBUFFER, m_fbo);
	glViewport(0, 0, uiWidth, uiHeight);	
	glClearColor( 0.0f, 0.0f, 0.0f, 0.0f);
}

void FrameBuffer::Unbind(){
	glBindFramebuffer(GL_FRAMEBUFFER, 0); 
	glViewport(0, 0, Utility::Width, Utility::Height);
}

unsigned int FrameBuffer::GetTexture(unsigned int ui_ID){
	return Textures[ui_ID];
}

unsigned int FrameBuffer::GetDepthTexture(){
	return Textures[iCount];
}
