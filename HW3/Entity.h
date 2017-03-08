#pragma  once

#include <vector>
#include "SpriteSheet.h"
#include "ThreeAxis.h"
#include "Matrix.h"

enum Type {Character_Object, Shooter_Object};


class Entity{
public:
    
    Entity();
    
    
    void updateSize(float w, float h);
    void updatePosition();
    void updateBox();
    void nextFrame();
    void check_wallCollision();
    void bounceOff_Wall();
    /*  variables   */

    ThreeAxis position;
    ThreeAxis velocity;
    ThreeAxis acceleration;
    ThreeAxis friction;

    Matrix ModelMatrix;
    std::vector<SpriteSheet> sprites;
    int currentFrame;
    
    float top, bottom, left, right;
    float width, height;
    int type;
    uint hp;
    bool isCollided;
    
    
};

