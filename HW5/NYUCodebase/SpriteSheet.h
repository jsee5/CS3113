#pragma once


#include "ShaderProgram.h"

class SpriteSheet {
public:
    SpriteSheet() {}
    SpriteSheet(unsigned int textureID, int index, int sprite_count_x, int sprite_count_y, float size);
    
    void draw(ShaderProgram*& program);
    
    float size;
    GLuint textureID;
    float u;
    float v;
    float width;
    float height;
    int index;
    
};
