#ifndef Entity_h
#define Entity_h

#include <SDL.h>
#include <SDL_opengl.h>
#include <SDL_image.h>
#include "ShaderProgram.h"
#include "Axis.h"
#include "SpriteSheet.h"
#include <vector>
#include <map>

//enum ACTION { IDLE, WALKING, JUMPING};

class Entity{
public:
    
    
    Entity();
    
    
    void update(float);
    void nextFrame();
    
    void render(ShaderProgram*&);
    bool isColliding_with(Entity&);
    
    bool isStatic;
    bool collided_left;
    bool collided_right;
    bool collided_top;
    bool collided_bot;
    
    Axis position;
    Axis velocity;
    Axis acceleration;
    Axis resistor;
    
    float width, height;
    unsigned frame_index;
    float frame_timer;
    
//    ACTION action_type;
    Matrix ModelMatrix;
    
//    std::map<ACTION, std::vector<SpriteSheet*>> actions;
};

#endif /* Entity_h */


    
    
