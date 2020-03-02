#pragma once

#include "EngineMath.h"




/* TODO
______________________
	-Add vertex buffer for sprite rendering bounds

*/
struct Sprite {

	Transform2D transform;
	float width;
	float height;

	Vector2 textureOffset;

	int16_t id;

	Sprite() {}

	Sprite(Vector2 position, float width, float height, int16_t id) {
		transform = { 0 };
		transform.matrix = Mat3::Identity();
		transform.translation = position;
		transform.scaleX = 1.0f;
		transform.scaleY = 1.0f;
		this->width = width;
		this->height = height;
		this->id = id;

	}

	float* pack() {
		transform.matrix = Mat3::Identity();

		Mat3 scale = {
			this->transform.scaleX * this->width, 0.0f, 0.0f,
			0.0f, this->transform.scaleY * this->height, 0.0f,
			0.0f, 0.0f,  1.0f
		};

		Mat3 skew = {
			1.0f, transform.skewX, 0.0f,
			transform.skewY, 1.0f, 0.0f,
			0.0f, 0.0f, 1.0f
		};

		Mat3 orientation = {
			cosf(transform.orientation), -sinf(transform.orientation), 0.0,
			sinf(transform.orientation), cosf(transform.orientation), 0.0,
			0.0f, 0.0f,  1.0f
		};

		Mat3 translate = {
			1.0f, 0.0f, transform.translation.x,
			0.0f, 1.0f, transform.translation.y,
			0.0f, 0.0f, 1.0f
		};


		transform.matrix = translate * orientation * skew * scale;
		return &this->transform.matrix[0];
	}


	Vector2 Position() {
		return transform.translation;
	}

	void Resize(float x, float y) {
		this->width += x;
		this->height += y;
	}

	void setSize(float x, float y) {
		this->width = x;
		this->height = y;
	}

	void setSkew(float x, float y) {
		transform.skewX = x;
		transform.skewY = y;
	}

	void Rotate(float theta) {
		transform.orientation += theta;
	}

	void setRotate(float theta) {
		transform.orientation = theta;
	}

	void Translate(Vector2 v) {
		transform.translation += v;
	}

	void setPosition(Vector2 v) {
		transform.translation = v;
	}

	void setTextureOffset(int u, int v) {
		this->textureOffset = { (float)u,(float)v };
	}

	float* getTextureOffset() {
		return &this->textureOffset[0];
	}
};

struct  SpriteBatch {
	//TODO: Find this upper limit based off of the min number of uniform floats gl allows
	static const int16_t MAX_SPRITES = 50;

	static const uint32_t VERTEX_COMPONENT_COUNT = 12;
	float vertices[VERTEX_COMPONENT_COUNT]{
								-.5f, -.5f,
									.5f, -.5f,
									.5f,  .5f,

								-.5f,  -.5f,
									.5f,  .5f,
								-.5f,  .5f
	};

	Sprite sprites[MAX_SPRITES];
	int16_t count_Sprites;

	uint32_t vertexBuffer;
	uint32_t vertexObject;

	uint32_t* shaderToUse;
	Texture* atlasToUse;

	bool updateShader = false;

	int u_transforms;
	int u_atlasResolution;
	int u_textureOffsets;


	float matrices[MAX_SPRITES * 9] = { 0 };
	float offsets[MAX_SPRITES * 2] = { 0 };

	SpriteBatch(uint32_t* program, Texture* atlas) {
		count_Sprites = 0;

		glGenBuffers(1, &vertexBuffer);
		glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);
		glBufferData(GL_ARRAY_BUFFER, sizeof(float)*VERTEX_COMPONENT_COUNT, vertices, GL_STATIC_DRAW);

		uint32_t vao = 0;
		glGenVertexArrays(1, &vertexObject);
		glBindVertexArray(vertexObject);
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, NULL);

		shaderToUse = program;

		u_textureOffsets = glGetUniformLocation(*program, "offsets");
		u_atlasResolution = glGetUniformLocation(*program, "atlasRes");
	
		u_transforms = glGetUniformLocation(*program, "transforms");


		

		atlasToUse = atlas;
	}

	//Note: Returns a positive integer(the id of the sprite) 0 <= return <= (max_sprites -1)
	//	or -1 if there was an error(likely too many sprites.

	//ToDo: Use paging, and have more than one draw call.

	Sprite* CreateSprite(Vector2 position, float width, float height) {

		if (count_Sprites < MAX_SPRITES) {
			this->sprites[count_Sprites] = Sprite(position, width, height, count_Sprites);
			count_Sprites++;
			return &this->sprites[count_Sprites - 1];
		}
		else {
			return NULL;
		}
	}

	Sprite* getSprite(int16_t id) {
		if (0 >= id && id < count_Sprites) {
			return &sprites[id];
		}
		else {
			return NULL;
		}
	}

	//TODO: Define another draw call that takes in a different shader program

	void Draw() {

		glUseProgram(*shaderToUse);
		glBindVertexArray(vertexObject);
		
		float* source;
		float* dest = matrices;



		//TODO?: Remove the array move and have the sprite
		//		update directly into the matrices and offsets array?


		for (int32_t drawIndex = 0; drawIndex < this->count_Sprites; drawIndex++) {

			source = sprites[drawIndex].pack();
			for (int32_t i = 0; i < 9; i++) {
				*dest++ = *source++;
			}
		}

		dest = offsets;
		for (int32_t drawIndex = 0; drawIndex < this->count_Sprites; drawIndex++) {

			source = sprites[drawIndex].getTextureOffset();
			for (int32_t i = 0; i < 2; i++) {
				*dest++ = *source++;
			}
		}
		
		glBindTexture(GL_TEXTURE_2D, this->atlasToUse->texture);
		glUniform2f(u_atlasResolution, (float)atlasToUse->width, (float)atlasToUse->height);
		glUniform2fv(u_textureOffsets, count_Sprites, offsets);
		glUniformMatrix3fv(u_transforms, count_Sprites, false, matrices);
		glDrawArraysInstanced(GL_TRIANGLES, 0, 6, count_Sprites);
	}

	~SpriteBatch() {
	}

};
