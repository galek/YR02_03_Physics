#include "FBXModel.h"
#include "soil.h"
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <Utilities.h>

FBXModel::FBXModel(const char* a_filename, FBXFile::UNIT_SCALE a_scale, bool a_loadTextures, bool a_loadAnimations){
	m_ModelMatrix = glm::mat4(1);
	HasAnimation = false;
	m_fbx = new FBXFile();
	m_fbx->load( a_filename,a_scale,a_loadTextures,a_loadAnimations );

	// how manu meshes and materials are stored within the fbx file
	unsigned int meshCount = m_fbx->getMeshCount();
	unsigned int matCount = m_fbx->getMaterialCount();

	// loop through each mesh
	for(unsigned int i=0; i<meshCount; ++i) {
		// get the current mesh
		FBXMeshNode *pMesh = m_fbx->getMeshByIndex(i);

		// genorate our OGL_FBXRenderData for storing the meshes VBO, IBO and VAO
		// and assign it to the meshes m_userData pointer so that we can retrive 
		// it again within the render function
		OGL_FBXRenderData *ro = new OGL_FBXRenderData();
		pMesh->m_userData = ro;

		// OPENGL: genorate the VBO, IBO and VAO
		glGenBuffers(1, &ro->VBO);
		glGenBuffers(1, &ro->IBO);
		glGenVertexArrays(1, &ro->VAO);

		// OPENGL: Bind  VAO, and then bind the VBO and IBO to the VAO
		glBindVertexArray(ro->VAO);
		glBindBuffer(GL_ARRAY_BUFFER, ro->VBO);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ro->IBO);

		// Send the vertex data to the VBO
		glBufferData(GL_ARRAY_BUFFER, pMesh->m_vertices.size() * sizeof(FBXVertex), pMesh->m_vertices.data(), GL_STATIC_DRAW);
		
		// send the index data to the IBO
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, pMesh->m_indices.size() * sizeof(unsigned int), pMesh->m_indices.data(), GL_STATIC_DRAW);

		//Here we push everything! just need to setup in the shaders

		// enable the attribute locations that will be used on our shaders
		glEnableVertexAttribArray(0); //Pos
		glEnableVertexAttribArray(1); //Col
		glEnableVertexAttribArray(2); //Norm
		glEnableVertexAttribArray(3); //Tan
		glEnableVertexAttribArray(4); //BiNorm
		glEnableVertexAttribArray(5); //Ind
		glEnableVertexAttribArray(6); //Weight
		glEnableVertexAttribArray(7); //Tex1
		glEnableVertexAttribArray(8); //Tex2
		
		// tell our shaders where the information within our buffers lie
		glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, sizeof(FBXVertex), ((char *)0) + FBXVertex::PositionOffset);
		glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, sizeof(FBXVertex), ((char *)0) + FBXVertex::ColourOffset);
		glVertexAttribPointer(2, 4, GL_FLOAT, GL_FALSE, sizeof(FBXVertex), ((char *)0) + FBXVertex::NormalOffset);
		glVertexAttribPointer(3, 4, GL_FLOAT, GL_FALSE, sizeof(FBXVertex), ((char *)0) + FBXVertex::TangentOffset);
		glVertexAttribPointer(4, 4, GL_FLOAT, GL_FALSE, sizeof(FBXVertex), ((char *)0) + FBXVertex::BiNormalOffset);
		glVertexAttribPointer(5, 4, GL_FLOAT, GL_FALSE, sizeof(FBXVertex), ((char *)0) + FBXVertex::IndicesOffset);
		glVertexAttribPointer(6, 4, GL_FLOAT, GL_FALSE, sizeof(FBXVertex), ((char *)0) + FBXVertex::WeightsOffset);
		glVertexAttribPointer(7, 2, GL_FLOAT, GL_FALSE, sizeof(FBXVertex), ((char *)0) + FBXVertex::TexCoord1Offset);
		glVertexAttribPointer(8, 2, GL_FLOAT, GL_FALSE, sizeof(FBXVertex), ((char *)0) + FBXVertex::TexCoord2Offset);

		// finally, where done describing our mesh to the shader
		// we can describe the next mesh
		glBindVertexArray(0);
	}

	// Loop through each material and load the texture
	for(unsigned int i = 0; i<matCount; ++i) {
		// get the current material being loaded
		FBXMaterial *pMaterial = m_fbx->getMaterialByIndex(i);

		// each material can optionally have many different textures that can be used to describe how the object should be rendered.
		// Look at the FBXMaterial structure.
		// the following textures can be assigned to a material within the 3D modeling programs
		// we can optionally support loading these in...
		// - DiffuseTexture,
		// - AmbientTexture,
		// - GlowTexture,
		// - SpecularTexture,
		// - GlossTexture,
		// - NormalTexture,
		// - AlphaTexture,
		// - DisplacementTexture

		for(unsigned int j = 0; j<FBXMaterial::TextureTypes_Count; ++j)	{
			// find the path to the texture to be loaded
			std::string path = m_fbx->getPath();
			// append the filename of the texture
			path += pMaterial->textureFilenames[j];
			// load the texture using SOIL
			pMaterial->textureIDs[j] = SOIL_load_OGL_texture(path.c_str(), 4, 0, SOIL_FLAG_TEXTURE_REPEATS | SOIL_FLAG_INVERT_Y);
			// lets just print what is loaded to the console...
			if (pMaterial->textureIDs[j] != 0){
				printf("Loading texture : %s - ID: %i\n", path.c_str(), pMaterial->textureIDs[j]);
			}
		}
	}
}

FBXModel::~FBXModel(void){
	// how manu meshes and materials are stored within the fbx file
	unsigned int meshCount = m_fbx->getMeshCount();
	unsigned int matCount = m_fbx->getMaterialCount();
	// remove meshes
	for(unsigned int i=0; i<meshCount; i++){
		// Get the current mesh and retrive the render data we assigned to m_userData
		FBXMeshNode* pMesh = m_fbx->getMeshByIndex(i);
		OGL_FBXRenderData *ro = (OGL_FBXRenderData *)pMesh->m_userData;
		// delete the buffers and free memory from the graphics card
		glDeleteBuffers(1, &ro->VBO);
		glDeleteBuffers(1, &ro->IBO);
		glDeleteVertexArrays(1, &ro->VAO);
		// this is memory we created earlier in the InitFBXSceneResources function
		// make sure to destroy it
		delete ro;
	}
	// loop through each of the materials
	for(unsigned int i=0; i<matCount; i++){
		// get the current material
		FBXMaterial *pMaterial = m_fbx->getMaterialByIndex(i);
		for(int j=0; j<FBXMaterial::TextureTypes_Count; j++){
			// delete the texture if it was loaded
			if( pMaterial->textureIDs[j] != 0 ) { glDeleteTextures(1, &pMaterial->textureIDs[j]); }
		}
	}
	m_fbx->unload();
}

void FBXModel::Update() {

	for(unsigned int i=0; i<m_fbx->getMeshCount(); ++i) {
		// get the current mesh
		FBXMeshNode *mesh = m_fbx->getMeshByIndex(i);
		// if you move an object around within your scene
		// children nodes are not updated until this function is called.
		mesh->updateGlobalTransform();
	}

}

void FBXModel::Model(glm::mat4 NewModel){
	m_ModelMatrix = NewModel;
}

void FBXModel::Colour(glm::vec4 NewColour){
	for(unsigned int i=0; i<m_fbx->getMeshCount(); ++i) {
		FBXMeshNode *pMesh = m_fbx->getMeshByIndex(i);
		for(unsigned int j = 0; j < pMesh->m_vertices.size(); ++j) {
			pMesh->m_vertices[j].colour = NewColour;
		}
		OGL_FBXRenderData *ro = (OGL_FBXRenderData *) pMesh->m_userData;
		glBindVertexArray(ro->VAO);
		glBufferSubData(GL_ARRAY_BUFFER, 0, pMesh->m_vertices.size() * sizeof(FBXVertex), pMesh->m_vertices.data());
		glBindVertexArray(0);
	}
}

void FBXModel::Draw(Shader* a_Shader,glm::mat4 &a_View) {
	// get the location of uniforms on the shader
	a_Shader->Bind();

	if (HasAnimation){
		//Animation
		// grab the skeleton and animation we want to use
		FBXSkeleton* skeleton = m_fbx->getSkeletonByIndex(0);
		FBXAnimation* animation = m_fbx->getAnimationByIndex(0);

		// evaluate the animation to update bones
		skeleton->updateBones();
		skeleton->evaluate( animation, Utility::getTotalTime() );
		a_Shader->SetUniform("bones","m4fv",skeleton->m_boneCount,false,(float*)skeleton->m_bones);
	}

	// for each mesh in the model...
	for(unsigned int i=0; i<m_fbx->getMeshCount(); ++i) {
		// get the current mesh
		FBXMeshNode *mesh = m_fbx->getMeshByIndex(i);

		// get the render data attached to the m_userData pointer for this mesh
		OGL_FBXRenderData *ro = (OGL_FBXRenderData *) mesh->m_userData;
		// Bind the texture to one of the ActiveTextures
		// if your shader supported multiple textures, you would bind each texture to a new Active Texture ID here
		a_Shader->SetTexture("DiffuseTexture",1,mesh->m_material->textureIDs[ FBXMaterial::DiffuseTexture] );
		a_Shader->SetTexture("SpecularTexture",2,mesh->m_material->textureIDs[ FBXMaterial::SpecularTexture] );
		a_Shader->SetTexture("NormalTexture",3,mesh->m_material->textureIDs[ FBXMaterial::NormalTexture] );
		//// send the Model, View and Projection Matrices to the shader
		glm::mat4 m4Model = mesh->m_globalTransform * m_ModelMatrix;
		a_Shader->SetUniform("Model","m4fv",1,false,glm::value_ptr(m4Model));
		//glm::mat3 NormalMatx = glm::inverseTranspose(glm::mat3(a_View * mesh->m_globalTransform));
		//a_Shader->SetUniform("NormalMatrix","m3fv",1,false,glm::value_ptr(NormalMatx));
		// bind our vertex array objects
		// remember in the initialise function, we bound the VAO and IBO to the VAO
		// so when we bind the VAO, openGL knows what what vertices,
		// indices and vertex attributes to send to the shader
		glBindVertexArray(ro->VAO);
		glDrawElements(GL_TRIANGLES, mesh->m_indices.size(), GL_UNSIGNED_INT, 0);
	}
	// finally, we have finished rendering the meshes
	// disable this shader
	glUseProgram(0);
}