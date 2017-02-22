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
    
    //2-D
    void translate(float x, float y);

    Matrix getModelMatrix()const;
    
    
    float get_yPosition()const;
    
    float get_xPosition()const;
    
    void setPosition(float x, float y);
    void setAngle(float angle);
    float getAngle()const;
    void reset();
private:
    
    float x_init, y_init;
    float x, y;
    float angle;
    int textureID;
    Matrix ModelMatrix;
};
