#ifdef _WINDOWS
#include <GL/glew.h>
#endif
#include <SDL.h>
#include <SDL_opengl.h>
#include <SDL_image.h>
#include <vector>
#include "ShaderProgram.h"
#include "Matrix.h"
#include "Entity.h"
#include "SpriteSheet.h"

#ifdef _WINDOWS
#define RESOURCE_FOLDER ""
#else
#define RESOURCE_FOLDER "NYUCodebase.app/Contents/Resources/"
#endif

/* core game definitions */
#define FPS 15.0f
#define MAX_TIMESTEPS 6
SDL_Window* displayWindow;
bool done = false;
enum GameState { Main_Menu, Game_Running };

/* background  */
Entity background;

/* title */
std::vector<Entity*> title;
uint title_index;
int title_ASCII[16] = {68, 114, 97, 103, 111, 110, 32, 73, 110, 118, 97, 100, 101, 114, 115, 32};




/* player dragon + enemy dragon information */
Entity dragon;
std::vector<Entity*> enemies;
int enemy_index;
#define DRAGON_SHEET_COUNT 4
#define ENEMY_LAYOUT_X 7
#define ENEMY_LAYOUT_Y 3



/* fireball information */
#define FIREBALL_SHEET_COUNT 8
#define MAX_FIREBALLS 20
#define EXPLOSION_SHEET_X 8
#define EXPLOSION_SHEET_Y 4
std::vector<Entity*> fireballs;
std::vector<Entity*> enemy_fireballs;
std::vector<Entity*> explosions;
uint fireball_index;
uint enemy_fireball_index;
uint explosion_index;


int state = Main_Menu;

//setup


void setup(){
    SDL_Init(SDL_INIT_VIDEO);
    displayWindow = SDL_CreateWindow("Dragon Invader", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 640, 360, SDL_WINDOW_OPENGL);
    SDL_GLContext context = SDL_GL_CreateContext(displayWindow);
    SDL_GL_MakeCurrent(displayWindow, context);


    glViewport(0, 0, 640, 360);
    
}




float lerp(float v0, float v1, float t) {
    return (1.0-t)*v0 + t*v1;
}

/*  initializing game object functions                    */

void load_background(){
    /* Load background texture */
    
    SpriteSheet background_sprite;
    background_sprite.LoadTexture(RESOURCE_FOLDER"background.png");
    background.sprites.push_back(background_sprite.getSprite(0, 1, 1));
    background.updateSize(12.50, 7.25);
    
}
void load_title(){
    
    SpriteSheet title_sprite;
    // Dragon Invaders in ASCII = 068 114 097 103 111 110 0 073 110 118 097 100 101 114 115
    title_sprite.LoadTexture(RESOURCE_FOLDER"font.png");
    float xPos = -3.5f;
    for(uint i = 0; i < 16; i++){
        Entity* letter = new Entity();
        letter->position.x = xPos;
        letter->sprites.push_back(title_sprite.getSprite(title_ASCII[i], 16, 16 ));
        title.push_back(letter);
        xPos += 0.5f;
    }
    

}
void explosion_Init(){
    
    SpriteSheet explosion_SpriteSheet;
    explosion_SpriteSheet.LoadTexture(RESOURCE_FOLDER"explosion.png");
    for(size_t i = 0; i < MAX_FIREBALLS; i++){
        Entity *explosion = new Entity();
        for(uint i = 8; i < 24; i++ ){
            explosion->type = Shooter_Object;
            explosion->sprites.push_back(explosion_SpriteSheet.getSprite(i, EXPLOSION_SHEET_X, EXPLOSION_SHEET_Y));
        }
        explosion->updateSize(0.5, 0.5);
        explosion->position.y = 0;
        explosions.push_back(explosion);
    }
}

void dragon_Init(){
    /* Load Dragon Sprite Sheet */
    SpriteSheet dragon_SpriteSheet;
    dragon_SpriteSheet.LoadTexture(RESOURCE_FOLDER"dragon_sprite.png");
    
    /* get last 4 row of sprite from dragon spritesheet and store in entity*/
    for(uint i = 12; i < 16; i++ ){
        dragon.sprites.push_back(dragon_SpriteSheet.getSprite(i, DRAGON_SHEET_COUNT, DRAGON_SHEET_COUNT));
    }
    dragon.type = Character_Object;
    dragon.position.x = 0.0f;
    dragon.position.y = -2.8f;
    dragon.updateSize(1.0, 1.4);
    dragon.updateBox();
    dragon.friction.x = 2.0f;
    dragon.hp = 10;
}

void enemies_Init(){
    /* Load enemy dragons */
    SpriteSheet enemy_SpriteSheet;
    enemy_SpriteSheet.LoadTexture(RESOURCE_FOLDER"enemy_dragon_sprite.png");
    
    // different frame allows the enemy dragons to flap non-uniformly
    uint diff_frame = 0;
    float y_Position = 0.75f;
    for(size_t i = 0; i < ENEMY_LAYOUT_Y; i++){
        float x_Position = -4.0f;
        y_Position += 0.75;
        for(size_t j = 0; j < ENEMY_LAYOUT_X; j++){
            x_Position += 1.0;
            Entity *enemy = new Entity();
            for(uint i = 0; i < 4; i++ ){
                enemy->type = Character_Object;
                enemy->position.x = x_Position;
                enemy->position.y = y_Position;
                enemy->updateSize(0.8, 0.8);
                enemy->velocity.x = 0.1;
                enemy->velocity.y = -0.02;
                enemy->sprites.push_back(enemy_SpriteSheet.getSprite( ((i+diff_frame)%3), DRAGON_SHEET_COUNT, DRAGON_SHEET_COUNT));
                diff_frame++;
                enemy->hp = 3;
            }
            enemies.push_back(enemy);
        }
    }

    
}
void fireball_Init(){
    /* Load Fireball Sprite Sheet */
    SpriteSheet fireball_SpriteSheet;
    SpriteSheet enemy_fireball_SpriteSheet;
    
    fireball_SpriteSheet.LoadTexture(RESOURCE_FOLDER"fireball_sprite.png");
    
    
    
    for(size_t i = 0; i < MAX_FIREBALLS; i++){
        Entity *fireball = new Entity();;
        for(uint i = 16; i < 24; i++ ){
            fireball->type = Shooter_Object;
            fireball->sprites.push_back(fireball_SpriteSheet.getSprite(i, FIREBALL_SHEET_COUNT, FIREBALL_SHEET_COUNT));
        }
        fireball->updateSize(0.7, 0.5);
        fireball->position.x = 10.0;
        fireball->position.y = 10.0;
        fireballs.push_back(fireball);
    }
    for(size_t i = 0; i < MAX_FIREBALLS; i++){
        Entity *enemy_fireball = new Entity();

        for(uint i = 48; i < 56; i++ ){
            
            enemy_fireball->type = Shooter_Object;
            enemy_fireball->sprites.push_back(fireball_SpriteSheet.getSprite(i, FIREBALL_SHEET_COUNT, FIREBALL_SHEET_COUNT));
        }
        enemy_fireball->updateSize(0.7, 0.5);
        enemy_fireball->position.x = 10.0;
        enemy_fireball->position.y = 10.0;
        enemy_fireballs.push_back(enemy_fireball);
    }
}

void explode(float x, float y){
    explosions[explosion_index]->position.x = x;
    explosions[explosion_index]->position.y = y;
    explosion_index++;
    
    if(explosion_index + 1 > MAX_FIREBALLS){
        explosion_index = 0;
    }
    
}
void resetExplosions(){
    for(Entity *explosion: explosions){
        explosion->position.x = 10;
        explosion->position.y = 10;
    }
}

void shoot_fireBall(){
    fireballs[fireball_index]->position.x = dragon.position.x;
    fireballs[fireball_index]->position.y = dragon.position.y + 0.2;
    fireball_index++;
    
    //let's not waste fireballs and re-use them!
    if(fireball_index + 1 > MAX_FIREBALLS){
        fireball_index = 0;
    }
    
}

void enemy_shoot(Entity* enemy){
    
    if(enemy->hp != 0){
        if(rand() % 1500 == 0){
            enemy_fireballs[enemy_fireball_index]->position.x = enemy->position.x;
            enemy_fireballs[enemy_fireball_index]->position.y = enemy->position.y + 0.2;
        }
        
        enemy_fireball_index ++;
        if(enemy_fireball_index + 1 > MAX_FIREBALLS){
            enemy_fireball_index = 0;
        }
    }
}


/* process Inputs */
void process_input(SDL_Event* event){
    

    while (SDL_PollEvent(event)) {
        if (event->type == SDL_QUIT || event->type == SDL_WINDOWEVENT_CLOSE) {
            done = true;
        }
        else if (event->type == SDL_KEYDOWN){
            if (event->key.keysym.scancode == SDL_SCANCODE_ESCAPE){
                state = Main_Menu;
            }
            else if(event->key.keysym.scancode == SDL_SCANCODE_RETURN){
                state = Game_Running;
            }
            else if(event->key.keysym.scancode == SDL_SCANCODE_SPACE){
                shoot_fireBall();
            }
        }
        
    }
    const Uint8 *player= SDL_GetKeyboardState(NULL);

    if (player[SDL_SCANCODE_A]){
        dragon.acceleration.x = -3.0;
    }
    else if(player[SDL_SCANCODE_D]){
        dragon.acceleration.x = 3.0;
    }
    else{
        dragon.acceleration.x = 0.0f;
    }
}

/* Update */
void update_mainMenu(float& animationElapsed){
    
    for(Entity* letter: title){
        letter->updatePosition();
    }
  
}

void group_Bounce(float elapsed){
    for(Entity* enemy : enemies){
        if(enemy->hp != 0){
            enemy->bounceOff_Wall();
            enemy->position.x += enemy->velocity.x * elapsed;
            enemy->updatePosition();
        }
    }
}

void update_gameRunning(ShaderProgram* program, float& animationElapsed){

    //player dragon
    dragon.velocity.x = lerp(dragon.velocity.x, 0.0f, animationElapsed* dragon.friction.x);
    dragon.velocity.x += dragon.acceleration.x * animationElapsed;
    dragon.position.x += dragon.velocity.x * animationElapsed;
    dragon.updatePosition();
    std::cout << dragon.hp << std::endl;
    
    if(dragon.hp == 0){
        state = Main_Menu;
    }
    
    for(Entity* enemy: enemies){
        if(enemy->hp != 0 ){
            if(enemy->isCollided == true){
                group_Bounce(animationElapsed);
            }
            enemy->position.y += enemy->velocity.y * animationElapsed;
            enemy->position.x += enemy->velocity.x * animationElapsed;
            enemy_shoot(enemy);
            
        }
        else{
            enemy->position.x = 10;
            enemy->position.y = 10;
            enemy->updatePosition();
        }
        enemy->updatePosition();
       
    }

    //fireball
    for(Entity *fireball: fireballs){
        fireball->velocity.y = 2.0;
        fireball->position.y += fireball->velocity.y * animationElapsed;
        fireball->updatePosition();
        
        for(Entity*enemy : enemies){
            if(fireball->position.x > enemy->left && fireball->position.x < enemy->right){
                if(fireball->position.y < enemy->bottom){
                    explode(enemy->position.x, enemy->position.y);
                    fireball->position.x = 10;
                    fireball->position.y = 10;
                    enemy->hp -= 1;
                }

            }
        }
    }


    for(Entity *fireball: enemy_fireballs){
        fireball->velocity.y = -1.0;
        fireball->position.y += fireball->velocity.y * animationElapsed;
        fireball->updatePosition();
        
        /*fireball and dragon collision */
        if(fireball->position.x > dragon.left && fireball->position.x < dragon.right){
            if(fireball->position.y < dragon.top){
                explode(dragon.position.x, dragon.position.y);
                fireball->position.x = 10;
                fireball->position.y = 10;
                dragon.hp--;
            }
        }
    }

    if(animationElapsed/1.2 > 1/FPS){
        for(Entity *explosion: explosions){
            explosion->updatePosition();
        }
    }

    
    if(animationElapsed > 1/FPS){
        animationElapsed = 0;
        dragon.nextFrame();
        for (Entity *enemy: enemies){
            if(enemy->hp > 0){
            enemy->nextFrame();
            }
        }
        for (Entity *fireball: fireballs){
            if(fireball->position.x != 10){
            fireball->nextFrame();
            }
        }
        for (Entity *fireball: enemy_fireballs){
            if(fireball->position.x != 10){
            fireball->nextFrame();
            }
        }
        for (Entity *explosion: explosions){
            explosion->nextFrame();
        }
        
    }

    
}


void update(ShaderProgram* program, float& animationElapsed){
    background.updatePosition();
    switch(state){
        case Main_Menu:
            update_mainMenu(animationElapsed);
            break;
        case Game_Running:
            update_gameRunning(program, animationElapsed);
            break;
    }
    resetExplosions();
}

/* Render */


void render_mainMenu(ShaderProgram* program){
 
    for(Entity *letter: title){
        program->setModelMatrix(letter->ModelMatrix);
        letter->sprites[0].Draw(program);
    }
    
}
void render_gameRunning(ShaderProgram* program){

    program->setModelMatrix(dragon.ModelMatrix);
    dragon.sprites[dragon.currentFrame].Draw(program);
    
    for(Entity *enemy: enemies){
        program->setModelMatrix(enemy->ModelMatrix);
        enemy->sprites[enemy->currentFrame].Draw(program);
    }
    for(Entity *fireball: fireballs){
        program->setModelMatrix(fireball->ModelMatrix);
        fireball->sprites[fireball->currentFrame].Draw(program);
    }
    for(Entity *fireball: enemy_fireballs){
        program->setModelMatrix(fireball->ModelMatrix);
        fireball->sprites[fireball->currentFrame].Draw(program);
    }
    for(Entity *explosion: explosions){
        program->setModelMatrix(explosion->ModelMatrix);
        explosion->sprites[explosion->currentFrame].Draw(program);
    }

  
}

void render(ShaderProgram* program){
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    
    program->setModelMatrix(background.ModelMatrix);
    background.sprites[0].Draw(program);
    
    switch (state) {
        case Main_Menu:
            render_mainMenu(program);
            break;
        case Game_Running:
            render_gameRunning(program);
            break;
    }
    SDL_GL_SwapWindow(displayWindow);
}


int main(int argc, char *argv[]){
    

    setup();
    ShaderProgram program(RESOURCE_FOLDER"vertex_textured.glsl", RESOURCE_FOLDER"fragment_textured.glsl");
    glUseProgram(program.programID);

    Matrix ProjectionMatrix;
    ProjectionMatrix.setOrthoProjection(-5.33f, 5.33f, -3.55f, 3.55f, -1.0f, 1.0f);
    Matrix ViewMatrix;
    
    load_background();
    load_title();
    
    /* init entities for the game */
    dragon_Init();
    enemies_Init();
    fireball_Init();
    explosion_Init();
    /*time init*/
    float lastFrameTicks = 0.0f;
    float animationElapsed = 0.0f;
    
  
#ifdef _WINDOWS
    glewInit();
#endif
    

    
    SDL_Event event;
    while(!done){
        process_input(&event);
        program.setViewMatrix(ViewMatrix);
        program.setProjectionMatrix(ProjectionMatrix);
    
        if(state == Main_Menu){
            lastFrameTicks = 0.0f;
            animationElapsed = 0.0f;
        }
        
        /* time tracking */
        float ticks = (float)SDL_GetTicks()/1000.0f;
        float elapsed = ticks - lastFrameTicks;
        lastFrameTicks = ticks;
        
        float fixedElapsed = elapsed;
        if(fixedElapsed > 1/FPS * MAX_TIMESTEPS) {
            fixedElapsed = 1/FPS * MAX_TIMESTEPS;
        }
        animationElapsed+=elapsed;
        
        while (fixedElapsed >= 1/FPS ) {
            fixedElapsed -= 1/FPS;
            update(&program, animationElapsed);
        }
        update(&program, animationElapsed);
        
        render(&program);
        
    }
    
    SDL_Quit();
    return 0;
}
