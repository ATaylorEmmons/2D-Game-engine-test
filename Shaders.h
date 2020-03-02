#pragma once
#include "GLAD/glad.h"


#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#define PPM_f 64.0f
#define PPM_s "64.0f"


const int RECT_VERTEX_SIZE = 12;
const float RECT_CLIPSPACE[12] = {
		0.0f, 0.0f,
		1.0f, 0.0f,
		1.0f,  1.0f,

		0.0f,  0.0f,
		1.0f,  1.0f,
		0.0f,  1.0f
};

const float RECT_UISPACE[12] = {
		0.0f, 0.0f,
		1.0f, 0.0f,
		1.0f,  1.0f,

		0.0f,  0.0f,
		1.0f,  1.0f,
		0.0f,  1.0f
};

const float RECT_UNITSPACE[12] = {
		-.5f, -.5f,
		.5f, -.5f,
		.5f,  .5f,

		-.5f,  -.5f,
		.5f,  .5f,
		-.5f,  .5f
};

//TODO: Texture manager?
//TODO: allocate the bytes?
struct Texture {
	int pixelsPerTile;
	int width, height, depth;
	unsigned char* colorBytes;

	GLuint texture;
	Texture(char* source) {
		
		int force_channels = 4;
		stbi_set_flip_vertically_on_load(true);
		colorBytes = stbi_load(source, &width, &height, &depth, force_channels);



		if (!colorBytes) {
			//error management
		}

		/*
		Check if texture is power of 2
		Area	|100 ... 0|
				 -00 ... 1
				____________
		Area - 1 0111 ... 1
				 &100 ... 0
				____________
				0000 ... 0
		0 => Was a power of 2
		!0 => Wasn't a power of 2
		*/
		int area = width * height;
		if (!(area&(area - 1))) {

		}


		glGenTextures(1, &texture);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, texture);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, colorBytes);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		//TODO: Learn about these paramaters
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	}

	//TODO: Implement which texture to bind
	void use() {

	}

	~Texture() {

	}
};

//TODO Define along with shader code in a Shader.H
GLuint buildAndLinkShaders(std::string vertCode, std::string fragCode) {
	static int count = 0;
	const char* c_str_vertCode = vertCode.c_str();
	GLuint vs = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vs, 1, &c_str_vertCode, NULL);
	glCompileShader(vs);

	const char* c_str_fragCode = fragCode.c_str();
	GLuint fs = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fs, 1, &c_str_fragCode, NULL);
	glCompileShader(fs);

	int errorStringSize = 256;
	std::vector<char> buffer(errorStringSize);
	glGetShaderiv(vs, GL_INFO_LOG_LENGTH, &errorStringSize);
	glGetShaderInfoLog(vs, errorStringSize, &errorStringSize, &buffer[0]);
	std::cout << count << std::endl;
	debug_printMsg("Vertex Shader error: ");
	debug_printMsg(std::string(buffer.begin(), buffer.end()));

	glGetShaderiv(fs, GL_INFO_LOG_LENGTH, &errorStringSize);
	glGetShaderInfoLog(fs, errorStringSize, &errorStringSize, &buffer[0]);

	debug_printMsg("Fragment Shader error: ");
	debug_printMsg(std::string(buffer.begin(), buffer.end()));

	GLuint shader_program = glCreateProgram();
	glAttachShader(shader_program, vs);
	glAttachShader(shader_program, fs);
	glLinkProgram(shader_program);

	glDeleteShader(vs);
	glDeleteShader(fs);

	count++;
	return shader_program;
}


/*____________________ Sprite Shader______________________________*/
//|
//|
//|_______________________________________________________________*/


//Minimum number of uniform floats per draw call = 1024
std::string vert_Sprite =
"#version 420\n"
"const float PPM = " PPM_s ";"
"in vec2 rect;"
"uniform vec2 atlasRes;"
"uniform vec2 offsets[22];"

"uniform mat3 transforms[22];"


"uniform vec3 view;"
"uniform vec2 resolution;"


"out vec2 textureCoord;"
"void main() {"

//Local space
"	vec3 worldPoint = (vec3(rect, 1.0) * transforms[gl_InstanceID]);"
//World space
//View is buggy due to unit square definition
"	vec3 viewPoint = (view.z + 1.0f)*vec3(worldPoint.x - view.x, worldPoint.y - view.y, 1.0);"

//Clip space
"	vec2 clipped = PPM*vec2(viewPoint.x/resolution.x, viewPoint.y/resolution.y);"
"	vec2 textureNormalized = (rect + vec2(.5, .5));"
//"	textureCoord = vec2(textureNormalized.x*((PPM )/atlasRes.x),textureNormalized.y*(PPM/atlasRes.y)) + vec2(offsets[gl_InstanceID].x*(PPM/atlasRes.x), offsets[gl_InstanceID].y*(PPM/atlasRes.y));"
"	textureCoord = textureNormalized;"
"	gl_Position = vec4(clipped, 0.0, 1.0);"
"}";

std::string frag_Sprite =
"#version 420\n "
"out vec4 frag_color;"
"in vec2 textureCoord;"

//Allowed to sample from around 8 textures at a time
"layout(binding = 0)uniform sampler2D textureColor;"
"void main() {"
"	vec4 texel = texture(textureColor, textureCoord);"
"	if(texel.rgb == vec3(0.0f, 0.0f, 0.0f)) { discard; }"
"	frag_color = texel; "
"}";


#define MAX_STRING_SIZE = 50;
std::string vert_RenderString =
"#version 420\n"

"in vec2 uiRect;"

//Packing vec2's
"uniform vec4 fontSize_resolution;"
"uniform vec4 charId_translate[50];"

//"vec2 rectSize = vec2(64, 64);"
//"vec2 rectPos = vec2(-1280f/2, 100);"

//"vec2 charId = vec2(2, 14);"

"float tileSize = 64;"
"float textureSize = 1024;"
"float textureUnits = tileSize/textureSize;"

//"float aspectRatio = 1280.0/720.0;"
//"vec2 resolution = vec2(1280, 720);"

"out vec2 textureCoord;"


"void main() {"
//Unpacking the vec4 uniforms
"	vec2 fontSize = vec2(fontSize_resolution.xy);"
"	vec2 resolution = vec2(fontSize_resolution.zw);"
"	vec2 charId = vec2(charId_translate[gl_InstanceID].xy);"
"	vec2 translate = vec2(charId_translate[gl_InstanceID].zw);"

"	vec2 unitTexture = uiRect + vec2(.5, .5);"
"	vec2 clipSpace = ((uiRect*fontSize) + translate) / resolution*2;"
"	textureCoord = unitTexture*textureUnits + charId*vec2(textureUnits, textureUnits);"
"	gl_Position = vec4(clipSpace, 0.0, 1.0);"
"}";

std::string frag_RenderString =
"#version 420\n "

"in vec2 textureCoord;"
"out vec4 frag_color;"

"layout(binding = 0)uniform sampler2D textureColor;"
"void main() {"
"	vec4 texel = texture(textureColor, textureCoord);"
"	if(texel.rgb == vec3(0.0f, 0.0f, 0.0f)) { discard; }"
"	frag_color = texel*vec4(1, 0, 0, 1);"
"}";
