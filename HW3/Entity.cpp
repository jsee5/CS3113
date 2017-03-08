#include "Entity.h"
#include "ShaderProgram.h"
#include <math.h>


#define Wall_Position 5.0f

Entity::Entity(){
    /* 2D disregard Z-axis */
    position.z = velocity.z = acceleration.z = 0;
    
    /* default dimensions for the box */
    width = 0.5;
    height = 0.5;
    
    updateBox();
}



void Entity::updateBox(){
    top = position.y + height/2;
    bottom = position.y - height/2;
    right = position.x + width/2;
    left = position.x - width/2;
    
}

void Entity::updateSize(float w, float h){
    ModelMatrix.setScale(w, h, 1.0);
    width *= w;
    height *= h;
    updateBox();
}

void Entity::bounceOff_Wall(){
    if(velocity.x >= 0 ){
        position.x -= 0.1;
    }
    else if(velocity.x < 0){
        position.x += 0.1;

    }
    velocity.x *= -1;
    isCollided = false;

}

void Entity::check_wallCollision(){
    if(fabs(left) > Wall_Position || fabs(right) > Wall_Position){
        isCollided = true;
    }
    else{
        isCollided = false;
    }
}


void Entity::updatePosition(){
    updateBox();
    switch (type){
        case Character_Object:
            check_wallCollision();
            ModelMatrix.setPosition(position.x, position.y, position.z);
            updateBox();
            break;
            
        case Shooter_Object:
        
            ModelMatrix.setPosition(position.x, position.y, position.z);
            updateBox();

            break;
    }

}


void Entity::nextFrame(){
    if(currentFrame >= sprites.size()-1){
        currentFrame = 0;
    }
    currentFrame++;
}
