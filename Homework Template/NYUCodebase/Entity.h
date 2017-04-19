#pragma once

#ifndef Entity_h
#define Entity_h

#include <SDL.h>
#include <SDL_opengl.h>
#include <SDL_image.h>
#include "ShaderProgram.h"
#include "Vector.h"
#include "SpriteSheet.h"
#include <vector>
#include <map>


class Entity{
public:
    
    
    Entity();
    
    void update(float);
    void nextFrame();
    void init_Vectors();
    void render(ShaderProgram*&);
    void toWorldSpace(std::vector<Vector>&);
    
    
    Vector position;
    Vector velocity;
    Vector acceleration;
    Vector resistance;
    
    Vector left_lower, left_upper, right_lower, right_upper;
    
    float angle;
    unsigned frame_index;
    float frame_timer;
    
    SpriteSheet sprite;
    Matrix matrix;
    
};

#endif /* Entity_h */




