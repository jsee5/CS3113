#include "Entity.h"



float lerp(float v0, float v1, float t) {
    return (1.0 - t)*v0 + t*v1;
}

Entity::Entity(){
    velocity.x = velocity.y = 0;
    resistance.x = 1.5f;
    resistance.y = 1.5f;

}
void Entity::init_Vectors(){
    left_lower = Vector(-0.5f*sprite.size*sprite.aspect, -0.5f*sprite.size);
    right_upper = Vector(0.5f*sprite.size*sprite.aspect, 0.5f*sprite.size);
    left_upper = Vector(-0.5f*sprite.size*sprite.aspect, 0.5f*sprite.size);
    right_lower = Vector(0.5f*sprite.size*sprite.aspect, -0.5f*sprite.size);

    
}
void Entity::update(float elapsed) {
    

    velocity.x = lerp(velocity.x, 0.0f, elapsed * resistance.x);
    velocity.y = lerp(velocity.y, 0.0f, elapsed * resistance.y);
    
    velocity.x += acceleration.x * elapsed;
    velocity.y += acceleration.y * elapsed;
    
    position.x = velocity.x * elapsed;
    position.y = velocity.y * elapsed;
    
    matrix.Translate(position.x, position.y, 0.0f);
    matrix.Rotate(angle);

    if(matrix.m[3][0] + sprite.width > 5.33){
        
        acceleration.x *= -1;
        velocity.x *= -0.5;
        acceleration.y *= -1;
        matrix.m[3][0] -= (sprite.width);

    }
    if(matrix.m[3][0] - sprite.width < -5.33){
        acceleration.x *= -1;
        velocity.x *= -0.5;
        acceleration.y *= -1;
        matrix.m[3][0] += (sprite.width);
    }
    
    if(matrix.m[3][1] + sprite.height > 3.55){
        acceleration.x *= -1;
        acceleration.y *= -1;
        velocity.y *= -0.5;
        matrix.m[3][1] -= (sprite.height);

    }
    if(matrix.m[3][1] - sprite.height < -3.55){
        
        acceleration.x *= -1;
        acceleration.y *= -1;
        velocity.y *= -0.5;
        matrix.m[3][1] += (sprite.height);

    }

}

void Entity::toWorldSpace(std::vector<Vector>& world){
    
    world.resize(4);
    
    world[0].x = left_lower.x * matrix.m[0][0] + left_lower.y *matrix.m[1][0] + matrix.m[3][0];
    world[0].y = left_lower.x * matrix.m[0][1] + left_lower.y *matrix.m[1][1] + matrix.m[3][1];
    
    world[1].x = right_upper.x * matrix.m[0][0] + right_upper.y *matrix.m[1][0] + matrix.m[3][0];
    world[1].y = right_upper.x * matrix.m[0][1] + right_upper.y *matrix.m[1][1] + matrix.m[3][1];
    
    world[2].x = left_upper.x * matrix.m[0][0] + left_upper.y *matrix.m[1][0] + matrix.m[3][0];
    world[2].y = left_upper.x * matrix.m[0][1] + left_upper.y *matrix.m[1][1] + matrix.m[3][1];
    
    world[3].x = right_lower.x * matrix.m[0][0] + right_lower.y *matrix.m[1][0] + matrix.m[3][0];
    world[3].y = right_lower.x * matrix.m[0][1] + right_lower.y *matrix.m[1][1] + matrix.m[3][1];
}

void Entity::render(ShaderProgram*& program){

    program->setModelMatrix(matrix);
    sprite.draw(program);
}

