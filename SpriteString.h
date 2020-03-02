#pragma once

#include <string>

#include "EngineMath.h"
#include "Shaders.h"




/* TODO
______________________

	-Resizing the string to a smaller size causes
		wierdness to occur. 
	-Move from ascii?

	-Note) Only one fontsize per batch of strings
		-Pros: 
			*Waaay reduces number of uniform uploads
			*Allows more strings to be batched
			*Some fonts only look good at one size
		-Cons:
			*Will need more than one batch for each font
		
	-Note) Changing the string text to a larger character count
			clips it.
	

*/
struct SpriteString {

	Vector2 position;
	float characterSpacing;

	std::string text;

	int16_t id;

	SpriteString() {}

	SpriteString(Vector2 position, std::string text, float charSpacing, int16_t id) {
		this->position = position;
		this->characterSpacing = charSpacing;
		this->text = text;
	}

	int Length() {
		return this->text.length();
	}

};

struct  SpriteStringBatch {
	static const uint16_t MAX_CHARACTERS = 50;
	const float* vertices;

	std::vector<SpriteString> spriteStrings;
	uint16_t count_Chars;

	uint32_t vertexBuffer;
	uint32_t vertexObject;

	uint32_t* shaderToUse;
	Texture* atlasToUse;

	float fontWidth;
	float fontHeight;

	bool updateShader = false;

	int u_fontSize_resolution;
	int ua_charId_translate;


	float charId_translate[MAX_CHARACTERS * 4] = { 0 };
	float fontSize_resolution[MAX_CHARACTERS * 4] = { 0 };

	SpriteStringBatch(uint32_t* program, Texture* atlas, float fontWidth, float fontHeight) {
		count_Chars = 0;

		this->fontWidth = fontWidth;
		this->fontHeight = fontHeight;
		vertices = RECT_UNITSPACE;

		u_fontSize_resolution = glGetUniformLocation(*program, "fontSize_resolution");
		ua_charId_translate = glGetUniformLocation(*program, "charId_translate");
		
		glGenBuffers(1, &vertexBuffer);
		glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);
		glBufferData(GL_ARRAY_BUFFER, sizeof(float)*RECT_VERTEX_SIZE, vertices, GL_STATIC_DRAW);

		uint32_t vao = 0;
		glGenVertexArrays(1, &vertexObject);
		glBindVertexArray(vertexObject);
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, NULL);

		shaderToUse = program;

		atlasToUse = atlas;
	}

	SpriteString* CreateSpriteString(Vector2 position, std::string text, float charSpacing) {

		if (count_Chars < MAX_CHARACTERS - (uint16_t)text.length()) {
			SpriteString s = SpriteString(position, text, charSpacing, count_Chars);
			spriteStrings.push_back(s);
			
			count_Chars += (uint16_t)text.length();

			return &spriteStrings.back();
		}
		else {
			//TODO: Use paging 
			return NULL;
		}

		
	}



	Vector2 parseOffset(char c) {
		int tableSize = 16;
		int characterOffset = 32;

		int xOffset = (((int)c) - characterOffset)%tableSize;
		int yOffset = (((int)c) - characterOffset)/tableSize + 1;

		return Vector2 {(float)xOffset,	tableSize - (float)yOffset};
	}


	void Draw(float clipResWidth, float clipResHeight) {

		glUseProgram(*shaderToUse);
		glBindVertexArray(vertexObject);
		

		/* Mapping a character into the 2d coords on the texture*/
		uint16_t currentStringIndex = 0;
		SpriteString currentSprite = spriteStrings[currentStringIndex];

		for(int currentChar = 0; currentChar < this->count_Chars*4; currentChar += 4) {
			Vector2 val = parseOffset(currentSprite.text[currentChar/4]);
			Vector2 pos = currentSprite.position + Vector2{currentChar/4 * currentSprite.characterSpacing, 0.0f};
			charId_translate[currentChar] = val.x;
			charId_translate[currentChar + 1] = val.y;
			charId_translate[currentChar + 2] = pos.x;
			charId_translate[currentChar + 3] = pos.y; 

			if((currentChar / 4) == currentSprite.Length()) {
				currentStringIndex++;
				currentSprite = spriteStrings[currentStringIndex];
			}
		}


		glBindTexture(GL_TEXTURE_2D, this->atlasToUse->texture);
		
		glUniform4f(u_fontSize_resolution, fontWidth, fontHeight, clipResWidth, clipResHeight);
		glUniform4fv(ua_charId_translate, count_Chars, charId_translate);
		//TODO: Allow non 1024px sized bitmap fonts
		//glUniform2f(u_atlasResolution, (float)atlasToUse->width, (float)atlasToUse->height);

		glDrawArraysInstanced(GL_TRIANGLES, 0, 6, count_Chars);
		
	}

	~SpriteStringBatch() {
	}

};
