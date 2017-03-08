#pragma once

#include "ShaderProgram.h"

class SpriteSheet{
public:
    
    /* constructor */
    SpriteSheet();
    
    /* load texture */
    void LoadTexture(const char *filePath);
    
    SpriteSheet getSprite(int index, int x_count, int y_count);
    
    void Draw(ShaderProgram *program);
    
    /*variables*/
    
    GLuint textureID;
    float u;
    float v;
    float width;
    float height;
    

};

