#include <SDL.h>
#include <SDL_opengl.h>
#include <SDL_image.h>
#include "ShaderProgram.h"
#include "Matrix.h"

class Entity{
public:
    
    Entity();
    
    //load texture
    GLuint LoadTexture(const char *filePath);

    void resetPosition();
    

    float x_init, y_init;
    float x, y;
    float angle;
    int textureID;
    Matrix ModelMatrix;
};
