#include "Entity.h"



float lerp(float v0, float v1, float t) {
    return (1.0 - t)*v0 + t*v1;
}

Entity::Entity(){
    velocity.x = velocity.y = 0;
    resistor.x = 0.2f;
    resistor.y = 9.8;
}

void Entity::update(float elapsed) {
    
    if(collided_right){
        if (velocity.x > 0){
            velocity.x = 0;
        }
    }
    if(collided_left){
        if (velocity.x < 0){
            velocity.x = 0;
        }
    }
    
    
    if(!isStatic){
        velocity.x = lerp(velocity.x, 0.0f, elapsed * resistor.x);
        velocity.y = lerp(velocity.y, 0.0f, elapsed * resistor.y);
        
        velocity.x += acceleration.x * elapsed;
        velocity.y += acceleration.y * elapsed;
        
        position.x += velocity.x * elapsed;
        position.y += velocity.y * elapsed;
    }
    else{
        acceleration.x = 0;
        acceleration.y = 0;
        velocity.x = 0;
        velocity.y = 0;
    }
    if(frame_timer > 10*elapsed){
        nextFrame();
        frame_timer = 0;
    }
    frame_timer += elapsed;
    std::cout << position.x << " " << position.y << std::endl;
}

//void Entity::render(ShaderProgram*& program){
//    
//    if(velocity.x != 0){
//        actions[some_action][frame_index]->draw(program);
//    }
//    else{
//        actions[some_action][frame_index]->draw(program);
//        
//    }
//}

//void Entity::nextFrame(){
//    if(frame_index + 1 == 9){
//        frame_index = 0;
//    }
//    frame_index += 1;
//}

bool Entity::isColliding_with(Entity& entity){
    
    if(position.x > entity.position.x - entity.width/2 &&
       position.x < entity.position.x + entity.width/2 &&
       position.y-height/2 <= entity.position.y + entity.height/2 &&
       entity.position.y+height/2 > entity.position.y + entity.height/2){
        
        entity.collided_bot = false;
        
        
    }
    else if(position.x-width < entity.position.x - entity.width/2 &&
            position.x+width/2 >= entity.position.x - entity.width/2 &&
            ((position.y+height/2 >= entity.position.y - entity.height/2 &&
              position.y-height/2 <= entity.position.y - entity.height/2 )
             ||
             (position.y-height/2 <= entity.position.y + entity.height/2 &&
              position.y-height/2 >= entity.position.y - entity.height/2))){
                 
                 collided_bot = false;
                 collided_right = true;
                 entity.position.x = rand()%7;
             }
    
    else if(position.x+width > entity.position.x + entity.width/2 &&
            position.x-width/2 <= entity.position.x + entity.width/2 &&
            ((position.y+height/2 >= entity.position.y - entity.height/2 && position.y-height/2 <= entity.position.y - entity.height/2 )
             ||
             (position.y-height/2 <= entity.position.y + entity.height/2 && position.y-height/2 >= entity.position.y - entity.height/2))){
                
                collided_bot = false;
                collided_left = true;
                entity.position.x = rand()%8;
            }
    
    
    return true;
}


