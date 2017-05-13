#include "Entity.h"



float lerp(float v0, float v1, float t) {
    return (1.0 - t)*v0 + t*v1;
}

Entity::Entity(){
    resistance.x = 5.0f;
    acceleration.y = -20.0f;
}

void Entity::update(float elapsed) {
    
    if (collided_bot){
        velocity.y = 0;
    }
    
    if(collided_left || collided_right)
        velocity.x = 0.0;
    
    velocity.x = lerp(velocity.x, 0.0f, elapsed * resistance.x);
    
    velocity.x += acceleration.x * elapsed;
    velocity.y += acceleration.y * elapsed;
    
    position.x += velocity.x * elapsed;
    position.y += velocity.y * elapsed;
    delay += elapsed;
    if(delay > 0.5){
        isNext = true;
    }
}


void Entity::render(ShaderProgram*& program, std::string state, uint which){
    
    width =  animated_states[state][current_frame[state]].width;
    height =  animated_states[state][current_frame[state]].height/8;
    
    matrix.identity();
    if(which == 1)
        matrix.Translate(position.x-5.1f, position.y+0.75, 0.0f);
    else if(which == 2)
        matrix.Translate(position.x, position.y + 0.75, 0.0f);
    else if(which == 3){
        matrix.Translate(position.x, position.y + 1.5, 0.0f);
        matrix.Scale(2, 2, 1.0);
    }
    if(velocity.x < 0)
        matrix.Scale(-1.0f, 1.0f, 1.0f);
    program->setModelMatrix(matrix);
    
    if(current_frame[state] + 1 == frame_counts[state]){
        current_frame[state] = 0;
    }
    if(isNext){
        current_frame[state] += 1;
        isNext = false;
    }
    animated_states[state][current_frame[state]].draw(program);
}


void Entity::reset(){
    position = init_position;
    velocity.reset();
    collided_bot = collided_left = collided_right = false;
    HP = init_HP;
}


