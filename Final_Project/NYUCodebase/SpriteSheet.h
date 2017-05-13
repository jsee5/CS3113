#pragma once


#include "ShaderProgram.h"

class SpriteSheet {
public:
    SpriteSheet();
    SpriteSheet(GLuint textureID, float x, float y, float width, float height, float sheet_width, float sheet_height, float size);
 
    void draw(ShaderProgram*& program);
    void uniform_draw(ShaderProgram* program);
    
    
    float size;
    GLuint textureID;
    float u, v;
    float width, height;
    float aspect;
    
};
