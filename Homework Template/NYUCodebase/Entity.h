#include <SDL.h>
#include <SDL_opengl.h>
#include <SDL_image.h>
#include <vector>

#include "ShaderProgram.h"
#include "Matrix.h"
#include "ThreeAxis.h"
#include "Sprite.h"

class Entity{
public:
    
    Entity();
    
    //load texture
    GLuint LoadTexture(const char *filePath);


    ThreeAxis position;
    ThreeAxis velocity;
    ThreeAxis acceleration;
    ThreeAxis friction;

    std::vector<Sprite> Sprites;

    float angle;
    int textureID;
    Matrix ModelMatrix;
};
