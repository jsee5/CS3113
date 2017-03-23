#pragma once


#include "ShaderProgram.h"

class Sprite{
    public:

    Sprite();

    void LoadTexture(const char *filePath);
    void Draw(ShaderProgram *program);


    /* variables */
    
    GLuint textureID;
    float u, v, width, height;

};

