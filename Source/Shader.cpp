#include <iostream>
#include <fstream>	//ifstream
#include <string>
#include <stdarg.h>	// va_list, va_start, va_arg, va_end

#include "Shader.h"

std::string strWord(int index, std::string line);

Shader::Shader(const char* ccs_VertFile,const char* ccs_FragFile,const char* ccs_TessControlFile,const char* ccs_TessEvalFile,const char* ccs_GeomFile,const char* ccs_CompuFile){

	cssVertFile		= (char*)ccs_VertFile;
	cssFragFile		= (char*)ccs_FragFile;
	cssTessContFile = (char*)ccs_TessControlFile;
	cssTessEvalFile = (char*)ccs_TessEvalFile;
	cssGeomFile		= (char*)ccs_GeomFile;
	cssComputeFile	= (char*)ccs_CompuFile;
	
	Create();
}

Shader::~Shader(){
	Remove();
}

void Shader::Create(){
	//Create shader group/program
	id = glCreateProgram();

	if (id == 0){
		printf("Error creating ShaderProgram. \n");
		return;
	}

	//load everything
	if (cssVertFile != nullptr){
		char* csvertShad = LoadShaderFile(cssVertFile);
		if (csvertShad != nullptr){
			Load(id,GL_VERTEX_SHADER,csvertShad);
		}else{id = 0;return;}
	}
	
	if (cssFragFile != nullptr){
		char* csfragShad = LoadShaderFile(cssFragFile);
		if (csfragShad != nullptr){
			Load(id,GL_FRAGMENT_SHADER,csfragShad);
		}else{id = 0;return;}
	}
	
	if (cssTessContFile != nullptr){
		char* csTessControlShad = LoadShaderFile(cssTessContFile);
		if (csTessControlShad != nullptr){
			Load(id,GL_TESS_CONTROL_SHADER,csTessControlShad);
		}else{id = 0;return;}
	}
	
	if (cssTessEvalFile != nullptr){
		char* csTessEvalShad = LoadShaderFile(cssTessEvalFile);
		if (csTessEvalShad != nullptr){
			Load(id,GL_TESS_EVALUATION_SHADER,csTessEvalShad);
		}else{id = 0;return;}
	}
	
	if (cssGeomFile != nullptr){
		char* csGeomShad = LoadShaderFile(cssGeomFile);
		if (csGeomShad != nullptr){
			Load(id,GL_GEOMETRY_SHADER,csGeomShad);
		}else{id = 0;return;}
	}
	
	if (cssComputeFile != nullptr){
		char* csCompuShad = LoadShaderFile(cssComputeFile);
		if (csCompuShad != nullptr){
			Load(id,GL_COMPUTE_SHADER,csCompuShad);
		}else{id = 0;return;}
	}

	//link everything
	GLint iPass = 0;
	glLinkProgram(id);
	glGetProgramiv(id,GL_LINK_STATUS,&iPass);
	if (iPass == 0){
		int infoLogLength = 0;		
		glGetShaderiv(id, GL_INFO_LOG_LENGTH, &infoLogLength);
		char* infoLog = new char[infoLogLength];

		glGetShaderInfoLog(id, infoLogLength, 0, infoLog);
		printf("Error while linking ShaderProgram \n '%s' \n",infoLog);
		id = 0;
		return;
	}
	//validate everything
	glValidateProgram(id);
	glGetProgramiv(id,GL_VALIDATE_STATUS,&iPass);
	if (iPass == 0){
		int infoLogLength = 0;		
		glGetShaderiv(id, GL_INFO_LOG_LENGTH, &infoLogLength);
		char* infoLog = new char[infoLogLength];

		glGetShaderInfoLog(id, infoLogLength, 0, infoLog);
		printf("Error while validating ShaderProgram \n '%s' \n",infoLog);
		id = 0;
		return;
	}

	//	Find all the uniform names and their locations
	if (cssVertFile		!= nullptr)	{FindUniforms(cssVertFile		);}
	if (cssFragFile		!= nullptr)	{FindUniforms(cssFragFile		);}
	if (cssTessContFile != nullptr)	{FindUniforms(cssTessContFile	);}
	if (cssTessEvalFile != nullptr)	{FindUniforms(cssTessEvalFile	);}
	if (cssGeomFile		!= nullptr)	{FindUniforms(cssGeomFile		);}
	if (cssComputeFile	!= nullptr)	{FindUniforms(cssComputeFile	);}
}

void Shader::ReloadShader(){
	Remove();
	Create();
}

void Shader::Remove(){
	if (id != 0){
		lUniforms.clear();
		glDeleteProgram(id);
	}
}

void Shader::Bind(){
	if (id != 0){
		glUseProgram(id);
	}
}

void Shader::Load(GLuint glui_ShaderProgram,GLenum gle_ShaderType,const char* ccs_File){

	GLint iPass = GL_FALSE;

	GLuint ShaderObj = glCreateShader(gle_ShaderType);

	glShaderSource(ShaderObj,1,&ccs_File,0);
	glCompileShader(ShaderObj);

    glGetShaderiv(ShaderObj, GL_COMPILE_STATUS, &iPass);
	if (iPass == GL_FALSE){
		int infoLogLength = 0;		
		glGetShaderiv(ShaderObj, GL_INFO_LOG_LENGTH, &infoLogLength);
		char* infoLog = new char[infoLogLength];

		glGetShaderInfoLog(ShaderObj, infoLogLength, 0, infoLog);
        printf("Error compiling shader type %d: '%s'\n", gle_ShaderType, infoLog);
		return;
	}else{
		char* type;
		switch (gle_ShaderType) {
		case GL_VERTEX_SHADER:			type = "Vertex"; break;
		case GL_FRAGMENT_SHADER:		type = "Fragment"; break;
		case GL_TESS_CONTROL_SHADER:	type = "Control"; break;
		case GL_TESS_EVALUATION_SHADER: type = "Evaluation"; break;
		case GL_GEOMETRY_SHADER:		type = "Geometry"; break;
		case GL_COMPUTE_SHADER:			type = "Compute"; break;
		}
		printf("%s Shader File Loaded.\n",type);
	}

	glAttachShader(glui_ShaderProgram,ShaderObj);
}

char* Shader::LoadShaderFile(const char* ccs_FilePath){
	std::ifstream is(ccs_FilePath,std::ifstream::binary);
	if (!is){
        printf("Error Opening shader file: %s \n",ccs_FilePath);
		return nullptr;
	}
	is.seekg(0,is.end);
	int length = (int)is.tellg();
	is.seekg(0,is.beg);
	char* cReturn = new char[length + 1];
	is.read(cReturn,length);
	cReturn[length] = '\0';
	//printf("Loaded shader %s \n%s\n",ccs_FilePath,cReturn);
	return (cReturn);
}

void Shader::FindUniforms(const char* ccs_FilePath) {
	if (id == 0)
		return;
	//!!WARNING!! WILL ADD ONTO CURRENT PAIR, PAIR IS NOT CLEARED
	std::ifstream ifs;
	ifs.open(ccs_FilePath);
	char* search = "uniform";											//	what to search for (could be changed to attrib)
	std::string line;
	std::cout<< ccs_FilePath << "\n";
	while (!ifs.eof()) {
		std::getline(ifs,line);
		int offset = line.find(search, 0);								//	how far from the start of the line uniform occurs
		if (offset != std::string::npos) {
			std::string name = strWord(3, line.substr(offset));			//	third word from uniform is the name of the uniform
			int isarray = name.find('[');
			int isarray2 = name.find(']');
			if (isarray > 0){
				name.erase(isarray,isarray2);
			}
			char* cName = new char[name.length()+1];					//	Uniform Name as Char*
			std::strcpy(cName,name.c_str());
			char* str = new char[line.length()+1];						//	Uniform Type as Char*
			std::strcpy(str, strWord(2, line.substr(offset)).c_str());
			char* arr = ((isarray > 0) ? "True" : "False");				//	Uniform Arrayed as Char*

			//	!!NEED TO CHECK IF ITEM IS ALREADY IN LIST!!
			unsigned int loc = glGetUniformLocation(id,cName);
			lUniforms.push_back( std::pair<int,char*>(loc,cName) );
			printf("%i : %s : %s : %s \n", loc, cName, str, arr); 
		}
	}
	std::cout << "\n";
	ifs.close();
}

void Shader::SetUniform(char* cs_Name,char* cs_Type,const int count,...){
	if (id == 0)
		return;
	va_list vaItems;
	va_start(vaItems,count);
	//find our uniform
	int iLoc = -1;
	for (unsigned int i = 0; i < lUniforms.size(); i++){
		if (std::strcmp(lUniforms.at(i).second,cs_Name) == 0){
			iLoc = lUniforms.at(i).first;
			break;
		}
	}
	if (iLoc == -1)
		return;

	//Make sure we enable ourself
	Bind();
	//Singles
	//Float
	if (cs_Type == "1f"){
		float data = (float)va_arg(vaItems, double);		
		glUniform1f(iLoc,data);
		return;
	}if (cs_Type == "2f"){
		float data1 = (float)va_arg(vaItems, double);		float data2 = (float)va_arg(vaItems, double);		
		glUniform2f(iLoc,data1,data2);
		return;
	}if (cs_Type == "3f"){
		float data1 = (float)va_arg(vaItems, double);		float data2 = (float)va_arg(vaItems, double);		float data3 = (float)va_arg(vaItems, double);
		glUniform3f(iLoc,data1,data2,data3);
		return;
	}if (cs_Type == "4f"){
		float data1 = (float)va_arg(vaItems, double);		float data2 = (float)va_arg(vaItems, double);		float data3 = (float)va_arg(vaItems, double);		float data4 = (float)va_arg(vaItems, double);
		glUniform4f(iLoc,data1,data2,data3,data4);
		return;
	}
	//Int
	if (cs_Type == "1i"){
		int data = va_arg(vaItems, int);		
		glUniform1i(iLoc,data);
		return;
	}if (cs_Type == "2i"){
		int data1 = va_arg(vaItems, int);		int data2 = va_arg(vaItems, int);		
		glUniform2i(iLoc,data1,data2);
		return;
	}if (cs_Type == "3i"){
		int data1 = va_arg(vaItems, int);		int data2 = va_arg(vaItems, int);		int data3 = va_arg(vaItems, int);
		glUniform3i(iLoc,data1,data2,data3);
		return;
	}if (cs_Type == "4i"){
		int data1 = va_arg(vaItems, int);		int data2 = va_arg(vaItems, int);		int data3 = va_arg(vaItems, int);		int data4 = va_arg(vaItems, int);
		glUniform4i(iLoc,data1,data2,data3,data4);
		return;
	}
	//Unsigned Int
	if (cs_Type == "1ui"){
		unsigned int data = va_arg(vaItems, unsigned int);		
		glUniform1ui(iLoc,data);
		return;
	}if (cs_Type == "2ui"){
		unsigned int data1 = va_arg(vaItems, unsigned int);		unsigned int data2 = va_arg(vaItems, unsigned int);		
		glUniform2ui(iLoc,data1,data2);
		return;
	}if (cs_Type == "3ui"){
		unsigned int data1 = va_arg(vaItems, unsigned int);		unsigned int data2 = va_arg(vaItems, unsigned int);		unsigned int data3 = va_arg(vaItems, unsigned int);
		glUniform3ui(iLoc,data1,data2,data3);
		return;
	}if (cs_Type == "4ui"){
		unsigned int data1 = va_arg(vaItems, unsigned int);		unsigned int data2 = va_arg(vaItems, unsigned int);		unsigned int data3 = va_arg(vaItems, unsigned int);		unsigned int data4 = va_arg(vaItems, unsigned int);
		glUniform4ui(iLoc,data1,data2,data3,data4);
		return;
	}
	//Arrays
	//float Array
	if (cs_Type == "1fv"){
		float* data = new float[count];
		for(int i = 0;i < count;++i){
			float *vdata = va_arg(vaItems, float*);	
			data[i] = vdata[0];
		}	
		glUniform1fv(iLoc,count,data);
		return;
	}if (cs_Type == "2fv"){
		float* data = new float[count * 2];
		for(int i = 0;i < count;++i){
			float *vdata = va_arg(vaItems, float*);	
			data[i * 2] = vdata[0];
			data[i * 2 + 1] = vdata[1];
		}		
		glUniform2fv(iLoc,count,data);
		return;
	}if (cs_Type == "3fv"){
		float* data = new float[count * 3];
		for(int i = 0;i < count;++i){
			float *vdata = va_arg(vaItems, float*);	
			data[i * 3] = vdata[0];
			data[i * 3 + 1] = vdata[1];
			data[i * 3 + 2] = vdata[2];
		}		
		glUniform3fv(iLoc,count,data);
		return;
	}if (cs_Type == "4fv"){
		float* data = new float[count * 4];
		for(int i = 0;i < count;++i){
			float *vdata = va_arg(vaItems, float*);	
			data[i * 4] = vdata[0];
			data[i * 4 + 1] = vdata[1];
			data[i * 4 + 2] = vdata[2];
			data[i * 4 + 3] = vdata[3];
		}
		glUniform4fv(iLoc,count,data);
		return;
	}
	//Int Array
	if (cs_Type == "1iv"){
		int *data = va_arg(vaItems, int*);		glUniform1iv(iLoc,count,data);
		return;
	}if (cs_Type == "2iv"){
		int *data = va_arg(vaItems, int*);		glUniform2iv(iLoc,count,data);
		return;
	}if (cs_Type == "3iv"){
		int *data = va_arg(vaItems, int*);		glUniform3iv(iLoc,count,data);
		return;
	}if (cs_Type == "4iv"){
		int *data = va_arg(vaItems, int*);		glUniform4iv(iLoc,count,data);
		return;
	}
	//Unsigned Int Array
	if (cs_Type == "1uiv"){
		unsigned int *data = va_arg(vaItems, unsigned int*);		glUniform1uiv(iLoc,count,data);		
		return;
	}if (cs_Type == "2uiv"){
		unsigned int *data = va_arg(vaItems, unsigned int*);		glUniform2uiv(iLoc,count,data);
		return;
	}if (cs_Type == "3uiv"){
		unsigned int *data = va_arg(vaItems, unsigned int*);		glUniform3uiv(iLoc,count,data);
		return;
	}if (cs_Type == "4uiv"){
		unsigned int *data = va_arg(vaItems, unsigned int*);		glUniform4uiv(iLoc,count,data);
		return;
	}
	//Matrix
	//Same Dimension Matricies
	if (cs_Type == "m2fv"){
		bool norm = va_arg(vaItems,bool);		float *m2fv = va_arg(vaItems,float*);		glUniformMatrix2fv(iLoc,count,norm,m2fv);
		return;
	}if (cs_Type == "m3fv"){
		bool norm = va_arg(vaItems,bool);		float *m3fv = va_arg(vaItems,float*);		glUniformMatrix3fv(iLoc,count,norm,m3fv);
		return;
	}if (cs_Type == "m4fv"){
		bool norm = va_arg(vaItems,bool);		float* m4fv = va_arg(vaItems,float*);		glUniformMatrix4fv(iLoc,count,norm,m4fv);
		return;
	}
	//Odd Dimension Matricies
	if (cs_Type == "m23fv"){
		bool norm = va_arg(vaItems,bool);		float *m23fv = va_arg(vaItems,float*);		glUniformMatrix2x3fv(iLoc,count,norm,m23fv);
		return;
	}if (cs_Type == "m32fv"){
		bool norm = va_arg(vaItems,bool);		float *m32fv = va_arg(vaItems,float*);		glUniformMatrix3x2fv(iLoc,count,norm,m32fv);
		return;
	}if (cs_Type == "m34fv"){
		bool norm = va_arg(vaItems,bool);		float *m34fv = va_arg(vaItems,float*);		glUniformMatrix3x4fv(iLoc,count,norm,m34fv);
		return;
	}if (cs_Type == "m43fv"){
		bool norm = va_arg(vaItems,bool);		float *m43fv = va_arg(vaItems,float*);		glUniformMatrix4x3fv(iLoc,count,norm,m43fv);
		return;
	}if (cs_Type == "m24fv"){
		bool norm = va_arg(vaItems,bool);		float *m24fv = va_arg(vaItems,float*);		glUniformMatrix2x4fv(iLoc,count,norm,m24fv);
		return;
	}if (cs_Type == "m42fv"){
		bool norm = va_arg(vaItems,bool);		float *m42fv = va_arg(vaItems,float*);		glUniformMatrix4x2fv(iLoc,count,norm,m42fv);
		return;
	}
}

void Shader::SetAttribs(int i_Count,...){
	if (id == 0)
		return;
	//Make sure we enable ourself
	Bind();
	va_list vaItems;
	va_start(vaItems,i_Count);
	for (int i = 0; i < i_Count; i++){
		int loc = va_arg(vaItems,int);
		char* name = va_arg(vaItems,char*);
		//printf("Set attrib at location : %i with name : %s \n",loc,name);
		glBindAttribLocation(id,loc,name);
		glLinkProgram(id);
	}
}

void Shader::SetFragOuts(int i_Count,...){
	if (id == 0)
		return;
	//Make sure we enable ourself
	Bind();
	va_list vaItems;
	va_start(vaItems,i_Count);
	for (int i = 0; i < i_Count; i++){
		int loc = va_arg(vaItems,int);
		char* name = va_arg(vaItems,char*);
		//printf("Bound frag at location : %i with name : %s \n",loc,name);
		glBindFragDataLocation(id,loc,name);
		glLinkProgram(id);
	}
}	

void Shader::SetTexture(char* cs_Name,unsigned int ShaderTexID,unsigned int ModelTexID){
	if (id == 0)
		return;
	//Make sure we enable ourself
	Bind();
	GLint uDiffuseTexture = glGetUniformLocation(id, cs_Name);
	glActiveTexture( 0x84C0 + ShaderTexID );
	glBindTexture( GL_TEXTURE_2D, ModelTexID );
	glUniform1i( uDiffuseTexture, ShaderTexID );
	glActiveTexture( 0 );
}

void Shader::SetTextureCube(char* cs_Name,unsigned int ShaderTexID,unsigned int ModelTexID){
	if (id == 0)
		return;
	//Make sure we enable ourself
	Bind();
	GLint uDiffuseTexture = glGetUniformLocation(id, cs_Name);
	glActiveTexture( 0x84C0 + ShaderTexID );
	glBindTexture( GL_TEXTURE_CUBE_MAP, ModelTexID );
	glUniform1i( uDiffuseTexture, ShaderTexID );
	glActiveTexture( 0 );
}

//==================FUNCTIONS==================//
std::string strWord(int index, std::string line) {
	int count = 0;									//	How many words have been gone through
	std::string word;
	for (unsigned int i = 0; i < line.length(); ++i) {
		//	If the character is a word ender (space, comma, colon, semicolon), then we are done reading a word
		if (line[i]== ' ' || line[i]== ';' || line[i]== ',' || line[i]== '.' || line[i]== ':') {		
			if (word.length() > 0) {				//	Make sure it has gathered a word (stops cases of double space or ' : ' increasing the word count)
				count++;
				if (count == index)
					//word.push_back('\n');
					return word;
				word = "";							//	Wasn't the number word we were looking for
			}
		}
		else
			word += line[i];						//	Add on the characters to the word as it walks through
	}
	return word;
}
