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
#include <unordered_map>


class Entity{
public:
    
    
    Entity();
    void update(float);
    void render(ShaderProgram*&, std::string, uint);
    void render_freeze(ShaderProgram*&, std::string, uint);
    void reset();
    
    Vector position;
    Vector velocity;
    Vector acceleration;
    Vector resistance;
    
    Vector init_position;

    
    float width, height;
    float angle;
    float delay;
    int HP, init_HP;
    
    bool isHit;
    bool isAttacking;
    bool isJumping;
    bool isNext;
    bool collided_bot;
    bool collided_left;
    bool collided_right;
    
    Matrix matrix;
    
    std::unordered_map<std::string, std::vector<SpriteSheet>> animated_states;
    std::unordered_map<std::string, unsigned> frame_counts;
    std::unordered_map<std::string, unsigned> current_frame;

    
};

#endif /* Entity_h */




