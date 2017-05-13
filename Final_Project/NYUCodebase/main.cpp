// Final Project


#ifdef _WINDOWS
#include <GL/glew.h>
#endif
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#include <SDL_mixer.h>
#include <SDL.h>
#include <SDL_opengl.h>
#include <SDL_image.h>
#include <vector>
#include <fstream>
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
#define FPS 60.0f
#define MAX_TIMESTEPS 6
#define SIZE 1.5
float lastFrameTicks;

Mix_Music *music = Mix_LoadMUS(RESOURCE_FOLDER"GameAssets/Music/music.mp3");
Mix_Chunk *sound_throw;

/* Matrices */
Matrix ModelMatrix, ProjectionMatrix, ViewMatrix;

/*Platform*/
#define TILE_SIZE 0.5f

int map1_width, map1_height;
int map2_width, map2_height;
int map3_width, map3_height;

unsigned char** level1_data;
unsigned char** level2_data;
unsigned char** level3_data;

bool isSetup1;
bool isSetup2;
bool isSetup3;

/*Spritesheets*/
GLuint sheet_platform, sheet_ninja, sheet_zombie_m, sheet_zombie_f;
GLuint font;
GLuint sprite_background;
SpriteSheet background;

/*Entities*/
std::vector<Entity*> zombies1, zombies2, kunais;
Entity zombie_male;
Entity zombie_female;

Entity kunai;
Entity ninja;
uint kunai_counter = 0;
float kunai_delay;
float countdown = 30;

/* game states */
bool done = false;

enum states{
    main_menu,
    level_one,
    level_two,
    level_three,
    pause_menu,
    quit
};


/* menu tween*/
float text_collapsed = 8.0f;
float title_pos = 8.5f;
float start_pos = -4.0f;
int gameState = main_menu;


SDL_Window* displayWindow;
void setup(){
    SDL_Init(SDL_INIT_VIDEO);
    displayWindow = SDL_CreateWindow("Girls can be Ninjas Too", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,1280, 720, SDL_WINDOW_OPENGL);
    SDL_GLContext context = SDL_GL_CreateContext(displayWindow);
    SDL_GL_MakeCurrent(displayWindow, context);
    glViewport(0, 0, 1280, 720);
    Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 4096);
    sound_throw = Mix_LoadWAV(RESOURCE_FOLDER"GameAssets/Music/throw.wav");
}

/*Load texture from a directory*/
GLuint loadTexture(const char *filePath) {
    int w, h, comp;
    unsigned char* image = stbi_load(filePath, &w, &h, &comp, STBI_rgb_alpha);
    
    if (image == NULL) {
        std::cout << "Unable to load image. Make sure the path is correct\n";
        assert(false);
    }
    GLuint retTexture;
    glGenTextures(1, &retTexture);
    glBindTexture(GL_TEXTURE_2D, retTexture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, w, h, 0, GL_RGBA, GL_UNSIGNED_BYTE, image);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    
    stbi_image_free(image);
    return retTexture;
}
/* Draw text in ASCII order */
void DrawText(ShaderProgram *program, int fontTexture, std::string text, float size, float spacing) {
    float texture_size = 1.0 / 16.0f;
    std::vector<float> vertexData;
    std::vector<float> texCoordData;
    for (int i = 0; i < text.size(); i++) {
        int spriteIndex = (int)text[i];
        float texture_x = (float)(spriteIndex % 16) / 16.0f;
        float texture_y = (float)(spriteIndex / 16) / 16.0f;
        vertexData.insert(vertexData.end(), {
            ((size + spacing) * i) + (-0.5f * size), 0.5f * size,
            ((size + spacing) * i) + (-0.5f * size), -0.5f * size,
            ((size + spacing) * i) + (0.5f * size), 0.5f * size,
            ((size + spacing) * i) + (0.5f * size), -0.5f * size,
            ((size + spacing) * i) + (0.5f * size), 0.5f * size,
            ((size + spacing) * i) + (-0.5f * size), -0.5f * size,
        });
        texCoordData.insert(texCoordData.end(), {
            texture_x, texture_y,
            texture_x, texture_y + texture_size,
            texture_x + texture_size, texture_y,
            texture_x + texture_size, texture_y + texture_size,
            texture_x + texture_size, texture_y,
            texture_x, texture_y + texture_size,
        });
    }
    glBindTexture(GL_TEXTURE_2D, fontTexture);
    program->setModelMatrix(ModelMatrix);
    glVertexAttribPointer(program->positionAttribute, 2, GL_FLOAT, false, 0, vertexData.data());
    glEnableVertexAttribArray(program->positionAttribute);
    glVertexAttribPointer(program->texCoordAttribute, 2, GL_FLOAT, false, 0, texCoordData.data());
    glEnableVertexAttribArray(program->texCoordAttribute);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glDrawArrays(GL_TRIANGLES, 0, text.size() * 6.0);
    glDisableVertexAttribArray(program->positionAttribute);
    glDisableVertexAttribArray(program->texCoordAttribute);
}

void setupKunai(){
    kunai.position.x = kunai.init_position.x = -20;
    kunai.acceleration.y = 0.0f;

    for (uint i = 0; i < 30; i++){
        kunais.push_back(new Entity(kunai));
    }
}

void init_frames(){
    
    /*load textures*/
    sheet_platform = loadTexture(RESOURCE_FOLDER"GameAssets/graveyard_tile.png");
    sheet_ninja = loadTexture(RESOURCE_FOLDER"GameAssets/Ninja_Girl/ninja_girl.png");
    sheet_zombie_f = loadTexture(RESOURCE_FOLDER"GameAssets/Zombies/zombie_f.png");
    sheet_zombie_m = loadTexture(RESOURCE_FOLDER"GameAssets/Zombies/zombie_m.png");
    font = loadTexture(RESOURCE_FOLDER"GameAssets/Etc/font.png");
    
    
    sprite_background = loadTexture(RESOURCE_FOLDER"GameAssets/BG.png");
    
    background = SpriteSheet(sprite_background, 0, 0, 1000, 572, 1000, 572, 0.5);
    
    
    /* Ninja animation states */
    ninja.animated_states["dead"] = std::vector<SpriteSheet>{
        SpriteSheet(sheet_ninja, 256, 138, 128, 133, 1024, 1757, SIZE),
        SpriteSheet(sheet_ninja, 384, 138, 128, 133, 1024, 1757, SIZE),
        SpriteSheet(sheet_ninja, 512, 138, 128, 133, 1024, 1757, SIZE),
        SpriteSheet(sheet_ninja, 640, 138, 128, 133, 1024, 1757, SIZE),
        SpriteSheet(sheet_ninja, 768, 138, 128, 133, 1024, 1757, SIZE),
        SpriteSheet(sheet_ninja, 896, 138, 128, 133, 1024, 1757, SIZE),
        SpriteSheet(sheet_ninja, 0, 276, 128, 133, 1024, 1757, SIZE),
        SpriteSheet(sheet_ninja, 128, 276, 128, 133, 1024, 1757, SIZE),
        SpriteSheet(sheet_ninja, 256, 276, 128, 133, 1024, 1757, SIZE),
        SpriteSheet(sheet_ninja, 384, 276, 128, 133, 1024, 1757, SIZE)
    };
    
    ninja.animated_states["hurt"] = std::vector<SpriteSheet>{
        SpriteSheet(sheet_ninja, 256, 138, 128, 133, 1024, 1757, SIZE),
        SpriteSheet(sheet_ninja, 256, 138, 128, 133, 1024, 1757, SIZE)
    };
    
    ninja.animated_states["jump"] = std::vector<SpriteSheet>{
        SpriteSheet(sheet_ninja, 768, 409, 128, 174, 1024, 1757, SIZE),
        SpriteSheet(sheet_ninja, 896, 409, 128, 174, 1024, 1757, SIZE),
        SpriteSheet(sheet_ninja, 0, 583, 128, 174, 1024, 1757, SIZE),
        SpriteSheet(sheet_ninja, 128, 583, 128, 174, 1024, 1757, SIZE),
        SpriteSheet(sheet_ninja, 256, 583, 128, 174, 1024, 1757, SIZE),
        SpriteSheet(sheet_ninja, 384, 583, 128, 174, 1024, 1757, SIZE),
        SpriteSheet(sheet_ninja, 512, 583, 128, 174, 1024, 1757, SIZE),
        SpriteSheet(sheet_ninja, 640, 583, 128, 174, 1024, 1757, SIZE),
        SpriteSheet(sheet_ninja, 768, 583, 128, 174, 1024, 1757, SIZE),
        SpriteSheet(sheet_ninja, 896, 583, 128, 174, 1024, 1757, SIZE)
    };
    
    
    ninja.animated_states["jump_throw"] = std::vector<SpriteSheet>{
        SpriteSheet(sheet_ninja, 256, 908, 128, 150, 1024, 1757, SIZE),
        SpriteSheet(sheet_ninja, 384, 908, 128, 150, 1024, 1757, SIZE),
        SpriteSheet(sheet_ninja, 512, 908, 128, 150, 1024, 1757, SIZE),
        SpriteSheet(sheet_ninja, 640, 908, 128, 150, 1024, 1757, SIZE),
        SpriteSheet(sheet_ninja, 768, 908, 128, 150, 1024, 1757, SIZE),
        SpriteSheet(sheet_ninja, 896, 908, 128, 150, 1024, 1757, SIZE),
        SpriteSheet(sheet_ninja, 0, 1059, 128, 150, 1024, 1757, SIZE),
        SpriteSheet(sheet_ninja, 128, 1059, 128, 150, 1024, 1757, SIZE),
        SpriteSheet(sheet_ninja, 256, 1059, 128, 150, 1024, 1757, SIZE),
        SpriteSheet(sheet_ninja, 384, 1059, 128, 150, 1024, 1757, SIZE),
    };
    
    ninja.animated_states["walk"] = std::vector<SpriteSheet>{
        SpriteSheet(sheet_ninja, 592, 1059, 128, 177, 1024, 1757, SIZE),
        SpriteSheet(sheet_ninja, 720, 1059, 128, 177, 1024, 1757, SIZE),
        SpriteSheet(sheet_ninja, 848, 1059, 128, 177, 1024, 1757, SIZE),
        SpriteSheet(sheet_ninja, 0, 1236, 128, 177, 1024, 1757, SIZE),
        SpriteSheet(sheet_ninja, 128, 1236, 128, 177, 1024, 1757, SIZE),
        SpriteSheet(sheet_ninja, 256, 1236, 128, 177, 1024, 1757, SIZE),
        SpriteSheet(sheet_ninja, 384, 1236, 128, 177, 1024, 1757, SIZE),
        SpriteSheet(sheet_ninja, 512, 1236, 128, 177, 1024, 1757, SIZE),
        SpriteSheet(sheet_ninja, 640, 1236, 128, 177, 1024, 1757, SIZE),
        SpriteSheet(sheet_ninja, 768, 1236, 128, 177, 1024, 1757, SIZE)
    };
    
    
    ninja.animated_states["throw"] = std::vector<SpriteSheet>{
        SpriteSheet(sheet_ninja, 896, 1236, 128, 172, 1024, 1757, SIZE),
        SpriteSheet(sheet_ninja, 0, 1413, 128, 172, 1024, 1757, SIZE),
        SpriteSheet(sheet_ninja, 128, 1413, 128, 172, 1024, 1757, SIZE),
        SpriteSheet(sheet_ninja, 256, 1413, 128, 172, 1024, 1757, SIZE),
        SpriteSheet(sheet_ninja, 384, 1413, 128, 172, 1024, 1757, SIZE),
        SpriteSheet(sheet_ninja, 512, 1413, 128, 172, 1024, 1757, SIZE),
        SpriteSheet(sheet_ninja, 640, 1413, 128, 172, 1024, 1757, SIZE),
        SpriteSheet(sheet_ninja, 768, 1413, 128, 172, 1024, 1757, SIZE),
        SpriteSheet(sheet_ninja, 896, 1413, 128, 172, 1024, 1757, SIZE),
        SpriteSheet(sheet_ninja, 0, 1585, 128, 172, 1024, 1757, SIZE)
    };
    
    ninja.animated_states["idle"] = std::vector<SpriteSheet>{
        SpriteSheet(sheet_ninja, 896, 1236, 128, 172, 1024, 1757, SIZE),
        SpriteSheet(sheet_ninja, 896, 1236, 128, 172, 1024, 1757, SIZE)
    };
    
    kunai.animated_states["throw"] = std::vector<SpriteSheet>{
        SpriteSheet(sheet_ninja, 512, 1059, 80, 16, 1024, 1757, 0.15),
        SpriteSheet(sheet_ninja, 512, 1059, 80, 16, 1024, 1757, 0.15)
    };
    
    
    /*female zombie animation states */
    
    zombie_female.animated_states["attack"] = std::vector<SpriteSheet>{
        SpriteSheet(sheet_zombie_f, 0, 0, 128, 142, 1024, 543, SIZE),
        SpriteSheet(sheet_zombie_f, 128, 0, 128, 142, 1024, 543, SIZE),
        SpriteSheet(sheet_zombie_f, 256, 0, 128, 142, 1024, 543, SIZE),
        SpriteSheet(sheet_zombie_f, 384, 0, 128, 142, 1024, 543, SIZE),
        SpriteSheet(sheet_zombie_f, 512, 0, 128, 142, 1024, 543, SIZE),
        SpriteSheet(sheet_zombie_f, 640, 0, 128, 142, 1024, 543, SIZE),
        SpriteSheet(sheet_zombie_f, 768, 0, 128, 142, 1024, 543, SIZE),
        SpriteSheet(sheet_zombie_f, 896, 0, 128, 142, 1024, 543, SIZE),
    };
    
    
    zombie_female.animated_states["dead"] = std::vector<SpriteSheet>{
        SpriteSheet(sheet_zombie_f, 0, 142, 128, 117, 1024, 543, SIZE),
        SpriteSheet(sheet_zombie_f, 128, 142, 128, 117, 1024, 543, SIZE),
        SpriteSheet(sheet_zombie_f, 256, 142, 128, 117, 1024, 543, SIZE),
        SpriteSheet(sheet_zombie_f, 384, 142, 128, 117, 1024, 543, SIZE),
        SpriteSheet(sheet_zombie_f, 512, 142, 128, 117, 1024, 543, SIZE),
        SpriteSheet(sheet_zombie_f, 640, 142, 128, 117, 1024, 543, SIZE),
        SpriteSheet(sheet_zombie_f, 768, 142, 128, 117, 1024, 543, SIZE),
        SpriteSheet(sheet_zombie_f, 896, 142, 128, 117, 1024, 543, SIZE),
        SpriteSheet(sheet_zombie_f, 0, 259, 128, 117, 1024, 543, SIZE),
        SpriteSheet(sheet_zombie_f, 128, 259, 128, 117, 1024, 543, SIZE),
        SpriteSheet(sheet_zombie_f, 256, 259, 128, 117, 1024, 543, SIZE),
        SpriteSheet(sheet_zombie_f, 384, 259, 128, 117, 1024, 543, SIZE)
    };
    
    
    zombie_female.animated_states["walk"] = std::vector<SpriteSheet>{
        SpriteSheet(sheet_zombie_f, 512, 259, 128, 142, 1024, 543, SIZE),
        SpriteSheet(sheet_zombie_f, 640, 259, 128, 142, 1024, 543, SIZE),
        SpriteSheet(sheet_zombie_f, 768, 259, 128, 142, 1024, 543, SIZE),
        SpriteSheet(sheet_zombie_f, 896, 259, 128, 142, 1024, 543, SIZE),
        SpriteSheet(sheet_zombie_f, 0, 401, 128, 142, 1024, 543, SIZE),
        SpriteSheet(sheet_zombie_f, 128, 401, 128, 142, 1024, 543, SIZE),
        SpriteSheet(sheet_zombie_f, 256, 401, 128, 142, 1024, 543, SIZE),
        SpriteSheet(sheet_zombie_f, 384, 401, 128, 142, 1024, 543, SIZE),
        SpriteSheet(sheet_zombie_f, 512, 401, 128, 142, 1024, 543, SIZE),
        SpriteSheet(sheet_zombie_f, 640, 401, 128, 142, 1024, 543, SIZE)
    };
    
    
    /* male zombie animation states*/
    zombie_male.animated_states["attack"] = std::vector<SpriteSheet>{
        SpriteSheet(sheet_zombie_m, 0, 0, 128, 154, 1024, 569, SIZE),
        SpriteSheet(sheet_zombie_m, 128, 0, 128, 154, 1024, 569, SIZE),
        SpriteSheet(sheet_zombie_m, 256, 0, 128, 154, 1024, 569, SIZE),
        SpriteSheet(sheet_zombie_m, 384, 0, 128, 154, 1024, 569, SIZE),
        SpriteSheet(sheet_zombie_m, 512, 0, 128, 154, 1024, 569, SIZE),
        SpriteSheet(sheet_zombie_m, 768, 0, 128, 154, 1024, 569, SIZE),
        SpriteSheet(sheet_zombie_m, 896, 0, 128, 154, 1024, 569, SIZE)
    };
    
    zombie_male.animated_states["dead"] = std::vector<SpriteSheet>{
        SpriteSheet(sheet_zombie_m, 0, 154, 128, 107, 1024, 569, SIZE),
        SpriteSheet(sheet_zombie_m, 128, 154, 128, 107, 1024, 569, SIZE),
        SpriteSheet(sheet_zombie_m, 256, 154, 128, 107, 1024, 569, SIZE),
        SpriteSheet(sheet_zombie_m, 384, 154, 128, 107, 1024, 569, SIZE),
        SpriteSheet(sheet_zombie_m, 512, 154, 128, 107, 1024, 569, SIZE),
        SpriteSheet(sheet_zombie_m, 640, 154, 128, 107, 1024, 569, SIZE),
        SpriteSheet(sheet_zombie_m, 768, 154, 128, 107, 1024, 569, SIZE),
        SpriteSheet(sheet_zombie_m, 896, 154, 128, 107, 1024, 569, SIZE),
        SpriteSheet(sheet_zombie_m, 0, 261, 128, 107, 1024, 569, SIZE),
        SpriteSheet(sheet_zombie_m, 128, 261, 128, 107, 1024, 569, SIZE),
        SpriteSheet(sheet_zombie_m, 256, 261, 128, 107, 1024, 569, SIZE),
        SpriteSheet(sheet_zombie_m, 384, 261, 128, 107, 1024, 569, SIZE),
    };
    
    
    zombie_male.animated_states["walk"] = std::vector<SpriteSheet>{
        SpriteSheet(sheet_zombie_m, 512, 261, 128, 154, 1024, 569, SIZE),
        SpriteSheet(sheet_zombie_m, 640, 261, 128, 154, 1024, 569, SIZE),
        SpriteSheet(sheet_zombie_m, 768, 261, 128, 154, 1024, 569, SIZE),
        SpriteSheet(sheet_zombie_m, 896, 261, 128, 154, 1024, 569, SIZE),
        SpriteSheet(sheet_zombie_m, 0, 415, 128, 154, 1024, 569, SIZE),
        SpriteSheet(sheet_zombie_m, 128, 415, 128, 154, 1024, 569, SIZE),
        SpriteSheet(sheet_zombie_m, 256, 415, 128, 154, 1024, 569, SIZE),
        SpriteSheet(sheet_zombie_m, 384, 415, 128, 154, 1024, 569, SIZE),
        SpriteSheet(sheet_zombie_m, 512, 415, 128, 154, 1024, 569, SIZE),
        SpriteSheet(sheet_zombie_m, 640, 415, 128, 154, 1024, 569, SIZE)
    };
    

    zombie_male.frame_counts["dead"] = 12;
    zombie_male.current_frame["dead"] = 0;
    
    zombie_male.frame_counts["walk"] = 10;
    zombie_male.current_frame["walk"] = 0;
    
    zombie_male.frame_counts["attack"] = 8;
    zombie_male.current_frame["attack"] = 0;
    
    zombie_female.frame_counts["walk"] = 10;
    zombie_female.current_frame["walk"] = 0;
    
    zombie_female.frame_counts["dead"] = 12;
    zombie_female.current_frame["dead"] = 0;
    
    zombie_female.frame_counts["attack"] = 8;
    zombie_female.current_frame["attack"] = 0;
    
    ninja.frame_counts["dead"] = 10;
    ninja.current_frame["dead"] = 0;
    
    ninja.frame_counts["jump"] = 10;
    ninja.current_frame["jump"] = 0;
    
    ninja.frame_counts["jump_throw"] = 10;
    ninja.current_frame["jump_throw"] = 0;
    
    ninja.frame_counts["throw"] = 10;
    ninja.current_frame["throw"] = 0;
    
    ninja.frame_counts["walk"] = 10;
    ninja.current_frame["walk"] = 0;
    
    ninja.frame_counts["idle"] = 2;
    ninja.current_frame["idle"] = 0;
    
    kunai.frame_counts["throw"] = 2;
    kunai.current_frame["throw"] = 0;
    
    ninja.isJumping = ninja.isAttacking = false;
    setupKunai();
}


void process_input(SDL_Event* event){
    const Uint8 *dir = SDL_GetKeyboardState(NULL);
    const Uint8 *attack = SDL_GetKeyboardState(NULL);
    const Uint8 *jump = SDL_GetKeyboardState(NULL);
    while (SDL_PollEvent(event)) {
        if (event->type == SDL_QUIT || event->type == SDL_WINDOWEVENT_CLOSE) {
            gameState = quit;
        }
        else if (event->type == SDL_KEYDOWN){
            if (event->key.keysym.scancode == SDL_SCANCODE_P){
                gameState = pause_menu;
            }
            else if (event->key.keysym.scancode == SDL_SCANCODE_RETURN && gameState == main_menu){
                gameState = level_one;
            }
            else if( event->key.keysym.scancode == SDL_SCANCODE_ESCAPE)
                gameState = main_menu;
        }
        
        /*directional input*/
        if(dir[SDL_SCANCODE_A])
            ninja.acceleration.x = -25.0f;
        else if (dir[SDL_SCANCODE_D])
            ninja.acceleration.x = 25.0f;
        else
            ninja.acceleration.x = 0.0f;
        
        /*---------------------------*/
        
        /*attack*/
        if(attack[SDL_SCANCODE_K]){
            ninja.isAttacking = true;
            Mix_PlayChannelTimed(-1, sound_throw, -1, 250);
        }
        else
            ninja.isAttacking = false;
        
        /*jump*/
        if(jump[SDL_SCANCODE_SPACE] && ninja.isJumping == false)
        {
            ninja.velocity.y = 10.0f;
            ninja.isJumping = true;
        }
    }
}



/* Draw level 1 */
void draw_level1(ShaderProgram* program, uint sprite_x, uint sprite_y){
    glBindTexture(GL_TEXTURE_2D, sheet_platform);
    
    for (int y = 0; y < map1_height; y++) {
        for (int x = 0; x < map1_width; x++) {
            if (level1_data[y][x] != 0) {
                
                float u = (float)(((int)level1_data[y][x]) % sprite_x) / (float)sprite_x;
                float v = (float)(((int)level1_data[y][x]) / sprite_x) / (float)sprite_y;
                
                float spriteWidth = 1.0f / (float)sprite_x;
                float spriteHeight = 1.0f / (float)sprite_y;
                
                float vertices[] = {
                    TILE_SIZE * x, -TILE_SIZE * y,
                    TILE_SIZE * x, (-TILE_SIZE * y) - TILE_SIZE,
                    (TILE_SIZE * x) + TILE_SIZE, (-TILE_SIZE * y) - TILE_SIZE,
                    
                    TILE_SIZE * x, -TILE_SIZE * y,
                    (TILE_SIZE * x) + TILE_SIZE, (-TILE_SIZE * y) - TILE_SIZE,
                    (TILE_SIZE * x) + TILE_SIZE, -TILE_SIZE * y
                };
                
                GLfloat texCoords[] = {
                    u, v,
                    u, v + (spriteHeight),
                    u + spriteWidth, v + (spriteHeight),
                    
                    u, v,
                    u + spriteWidth, v + (spriteHeight),
                    u + spriteWidth, v
                };
                
                program->setModelMatrix(ModelMatrix);
                program->setProjectionMatrix(ProjectionMatrix);
                program->setViewMatrix(ViewMatrix);
                
                glVertexAttribPointer(program->positionAttribute, 2, GL_FLOAT, false, 0, vertices);
                glEnableVertexAttribArray(program->positionAttribute);
                
                glVertexAttribPointer(program->texCoordAttribute, 2, GL_FLOAT, false, 0, texCoords);
                glEnableVertexAttribArray(program->texCoordAttribute);
                
                glDrawArrays(GL_TRIANGLES, 0, 6);
                glDisableVertexAttribArray(program->positionAttribute);
                glDisableVertexAttribArray(program->texCoordAttribute);
                
            }
        }
    }
}

/* Draw level 2 */
void draw_level2(ShaderProgram* program, uint sprite_x, uint sprite_y){
    glBindTexture(GL_TEXTURE_2D, sheet_platform);
    
    for (int y = 0; y < map2_height; y++) {
        for (int x = 0; x < map2_width; x++) {
            if (level2_data[y][x] != 0) {
                
                float u = (float)(((int)level2_data[y][x]) % sprite_x) / (float)sprite_x;
                float v = (float)(((int)level2_data[y][x]) / sprite_x) / (float)sprite_y;
                
                float spriteWidth = 1.0f / (float)sprite_x;
                float spriteHeight = 1.0f / (float)sprite_y;
                
                float vertices[] = {
                    TILE_SIZE * x, -TILE_SIZE * y,
                    TILE_SIZE * x, (-TILE_SIZE * y) - TILE_SIZE,
                    (TILE_SIZE * x) + TILE_SIZE, (-TILE_SIZE * y) - TILE_SIZE,
                    
                    TILE_SIZE * x, -TILE_SIZE * y,
                    (TILE_SIZE * x) + TILE_SIZE, (-TILE_SIZE * y) - TILE_SIZE,
                    (TILE_SIZE * x) + TILE_SIZE, -TILE_SIZE * y
                };
                
                GLfloat texCoords[] = {
                    u, v,
                    u, v + (spriteHeight),
                    u + spriteWidth, v + (spriteHeight),
                    
                    u, v,
                    u + spriteWidth, v + (spriteHeight),
                    u + spriteWidth, v
                };
                
                program->setModelMatrix(ModelMatrix);
                program->setProjectionMatrix(ProjectionMatrix);
                program->setViewMatrix(ViewMatrix);
                
                glVertexAttribPointer(program->positionAttribute, 2, GL_FLOAT, false, 0, vertices);
                glEnableVertexAttribArray(program->positionAttribute);
                
                glVertexAttribPointer(program->texCoordAttribute, 2, GL_FLOAT, false, 0, texCoords);
                glEnableVertexAttribArray(program->texCoordAttribute);
                
                glDrawArrays(GL_TRIANGLES, 0, 6);
                glDisableVertexAttribArray(program->positionAttribute);
                glDisableVertexAttribArray(program->texCoordAttribute);
                
            }
        }
    }
}
void draw_level3(ShaderProgram* program, uint sprite_x, uint sprite_y){
    glBindTexture(GL_TEXTURE_2D, sheet_platform);
    
    for (int y = 0; y < map3_height; y++) {
        for (int x = 0; x < map3_width; x++) {
            if (level3_data[y][x] != 0) {
                
                float u = (float)(((int)level3_data[y][x]) % sprite_x) / (float)sprite_x;
                float v = (float)(((int)level3_data[y][x]) / sprite_x) / (float)sprite_y;
                
                float spriteWidth = 1.0f / (float)sprite_x;
                float spriteHeight = 1.0f / (float)sprite_y;
                
                float vertices[] = {
                    TILE_SIZE * x, -TILE_SIZE * y,
                    TILE_SIZE * x, (-TILE_SIZE * y) - TILE_SIZE,
                    (TILE_SIZE * x) + TILE_SIZE, (-TILE_SIZE * y) - TILE_SIZE,
                    
                    TILE_SIZE * x, -TILE_SIZE * y,
                    (TILE_SIZE * x) + TILE_SIZE, (-TILE_SIZE * y) - TILE_SIZE,
                    (TILE_SIZE * x) + TILE_SIZE, -TILE_SIZE * y
                };
                
                GLfloat texCoords[] = {
                    u, v,
                    u, v + (spriteHeight),
                    u + spriteWidth, v + (spriteHeight),
                    
                    u, v,
                    u + spriteWidth, v + (spriteHeight),
                    u + spriteWidth, v
                };
                
                program->setModelMatrix(ModelMatrix);
                program->setProjectionMatrix(ProjectionMatrix);
                program->setViewMatrix(ViewMatrix);
                
                glVertexAttribPointer(program->positionAttribute, 2, GL_FLOAT, false, 0, vertices);
                glEnableVertexAttribArray(program->positionAttribute);
                
                glVertexAttribPointer(program->texCoordAttribute, 2, GL_FLOAT, false, 0, texCoords);
                glEnableVertexAttribArray(program->texCoordAttribute);
                
                glDrawArrays(GL_TRIANGLES, 0, 6);
                glDisableVertexAttribArray(program->positionAttribute);
                glDisableVertexAttribArray(program->texCoordAttribute);
                
            }
        }
    }
}
/* read level 1 layer data*/
bool readLayer1Data(std::ifstream &stream) {
    std::string line;
    while (getline(stream, line)) {
        if (line == "") {
            break;
        }
        std::istringstream sStream(line);
        std::string key, value;
        getline(sStream, key, '=');
        getline(sStream, value);
        if (key == "data") {
            for (int y = 0; y < map1_height; y++) {
                getline(stream, line);
                std::istringstream lineStream(line);
                std::string tile;
                for (int x = 0; x < map1_width; x++) {
                    getline(lineStream, tile, ',');
                    unsigned char val = (unsigned char)atoi(tile.c_str());
                    if (val > 1) {
                        // be careful, the tiles in this format are indexed from 1 not 0
                        level1_data[y][x] = val - 1;
                    }
                    
                    else {
                        level1_data[y][x] = 0;
                    }
                }
            }
        }
    }
    return true;
}

/* read level 2 layer data*/
bool readLayer2Data(std::ifstream &stream) {
    std::string line;
    while (getline(stream, line)) {
        if (line == "") {
            break;
        }
        std::istringstream sStream(line);
        std::string key, value;
        getline(sStream, key, '=');
        getline(sStream, value);
        if (key == "data") {
            for (int y = 0; y < map2_height; y++) {
                getline(stream, line);
                std::istringstream lineStream(line);
                std::string tile;
                for (int x = 0; x < map2_width; x++) {
                    getline(lineStream, tile, ',');
                    unsigned char val = (unsigned char)atoi(tile.c_str());
                    if (val > 1) {
                        // be careful, the tiles in this format are indexed from 1 not 0
                        level2_data[y][x] = val - 1;
                    }
                    
                    else {
                        level2_data[y][x] = 0;
                    }
                }
            }
        }
    }
    return true;
}

/* read level 3 layer data*/
bool readLayer3Data(std::ifstream &stream) {
    std::string line;
    while (getline(stream, line)) {
        if (line == "") {
            break;
        }
        std::istringstream sStream(line);
        std::string key, value;
        getline(sStream, key, '=');
        getline(sStream, value);
        if (key == "data") {
            for (int y = 0; y < map3_height; y++) {
                getline(stream, line);
                std::istringstream lineStream(line);
                std::string tile;
                for (int x = 0; x < map3_width; x++) {
                    getline(lineStream, tile, ',');
                    unsigned char val = (unsigned char)atoi(tile.c_str());
                    if (val > 1) {
                        // be careful, the tiles in this format are indexed from 1 not 0
                        level3_data[y][x] = val - 1;
                    }
                    
                    else {
                        level3_data[y][x] = 0;
                    }
                }
            }
        }
    }
    return true;
}

/*place the entity data from flare txt file*/
void placeEntity1(std::string type, float x, float y) {
    if (type == "zombie_f"){

        zombie_female.position.x = zombie_female.init_position.x = x;
        zombie_female.position.y = zombie_female.init_position.y = y;
        zombie_female.HP = zombie_female.init_HP = 4;
        zombie_female.acceleration.x = 10.0f;
        zombies1.push_back(new Entity(zombie_female));
    }
    else if(type == "zombie_m"){

        zombie_male.position.x = zombie_male.init_position.x = x;
        zombie_male.position.y = zombie_male.init_position.y = y;
        zombie_male.HP = zombie_male.init_HP = 4;
        zombie_male.acceleration.x = -10.0f;
        zombies1.push_back(new Entity(zombie_male));
    }
    else if(type == "ninja"){
        ninja.HP = ninja.init_HP = 500;
        ninja.position.x = ninja.init_position.x = x;
        ninja.position.y = ninja.init_position.y = y;
    }
}
/*place the entity data from flare txt file*/
void placeEntity2(std::string type, float x, float y) {
    if (type == "zombie_f"){
        
        zombie_female.position.x = zombie_female.init_position.x = x;
        zombie_female.position.y = zombie_female.init_position.y = y;
        zombie_female.HP = zombie_female.init_HP = 4;
        zombie_female.acceleration.x = 10.0f;
        zombies2.push_back(new Entity(zombie_female));
    }
    else if(type == "zombie_m"){
        
        zombie_male.position.x = zombie_male.init_position.x = x;
        zombie_male.position.y = zombie_male.init_position.y = y;
        zombie_male.HP = zombie_male.init_HP = 4;
        zombie_male.acceleration.x = -10.0f;
        zombies2.push_back(new Entity(zombie_male));

    }
    else if(type == "ninja"){
        ninja.HP = ninja.init_HP = 500;
        ninja.position.x = ninja.init_position.x = x;
        ninja.position.y = ninja.init_position.y = y;
    }
}
/*place the entity data from flare txt file*/
void placeEntity3(std::string type, float x, float y) {
    if (type == "zombie_f1"){
        
        zombie_female.position.x = zombie_female.init_position.x = x;
        zombie_female.position.y = zombie_female.init_position.y = y;
        zombie_female.HP = zombie_female.init_HP = 1500;
        zombie_female.acceleration.x = 10.0f;


    }
    else if(type == "ninja"){
        ninja.HP = ninja.init_HP = 500;
        ninja.position.x = ninja.init_position.x = x;
        ninja.position.y = ninja.init_position.y = y;
    }
}
/* read entity data from flare.txt file */
bool readEntityData1(std::ifstream &stream) {
    std::string line;
    std::string type;
    while (getline(stream, line)) {
        if (line == "") {
            break;
        }
        std::istringstream sStream(line);
        std::string key, value;
        getline(sStream, key, '=');
        getline(sStream, value);
        
        if (key == "type") {
            type = value;
        } else if (key == "location") {
            std::istringstream lineStream(value);
            std::string xPosition, yPosition;
            getline(lineStream, xPosition, ',');
            getline(lineStream, yPosition, ',');
            
            float placeX = atoi(xPosition.c_str())*TILE_SIZE;
            float placeY = atoi(yPosition.c_str())*-TILE_SIZE;
            placeEntity1(type, placeX, placeY);
        }
    }
    return true;
}
/* read entity data from flare.txt file */
bool readEntityData2(std::ifstream &stream) {
    std::string line;
    std::string type;
    while (getline(stream, line)) {
        if (line == "") {
            break;
        }
        std::istringstream sStream(line);
        std::string key, value;
        getline(sStream, key, '=');
        getline(sStream, value);
        
        if (key == "type") {
            type = value;
        } else if (key == "location") {
            std::istringstream lineStream(value);
            std::string xPosition, yPosition;
            getline(lineStream, xPosition, ',');
            getline(lineStream, yPosition, ',');
            
            float placeX = atoi(xPosition.c_str())*TILE_SIZE;
            float placeY = atoi(yPosition.c_str())*-TILE_SIZE;
            placeEntity2(type, placeX, placeY);
        }
    }
    return true;
}
//read entity data from flare.txt file
bool readEntityData3(std::ifstream &stream) {
    std::string line;
    std::string type;
    while (getline(stream, line)) {
        if (line == "") {
            break;
        }
        std::istringstream sStream(line);
        std::string key, value;
        getline(sStream, key, '=');
        getline(sStream, value);
        
        if (key == "type") {
            type = value;
        } else if (key == "location") {
            std::istringstream lineStream(value);
            std::string xPosition, yPosition;
            getline(lineStream, xPosition, ',');
            getline(lineStream, yPosition, ',');
            
            float placeX = atoi(xPosition.c_str())*TILE_SIZE;
            float placeY = atoi(yPosition.c_str())*-TILE_SIZE;
            placeEntity3(type, placeX, placeY);
        }
    }
    return true;
}

bool readHeader(std::ifstream& stream) {
    std::string line;
    
    if(gameState == level_one){
        map1_width = -1;
        map1_height = -1;
        while (getline(stream, line)) {
            if (line == "") {
                break;
            }
            std::istringstream sStream(line);
            std::string key, value;
            getline(sStream, key, '=');
            getline(sStream, value);
            if (key == "width") {
                map1_width = atoi(value.c_str());
            }
            else if (key == "height") {
                map1_height = atoi(value.c_str());
            }
        }
        
        if (map1_width == -1 || map1_height == -1) {
            return false;
        }
        else { // allocate our map data
            level1_data = new unsigned char*[map1_height];
            for(int i = 0; i < map1_height; ++i) {
                level1_data[i] = new unsigned char[map1_width];
            }
        }
    }
    else if (gameState == level_two){
        map2_width = -1;
        map2_height = -1;
        while (getline(stream, line)) {
            if (line == "") {
                break;
            }
            std::istringstream sStream(line);
            std::string key, value;
            getline(sStream, key, '=');
            getline(sStream, value);
            if (key == "width") {
                map2_width = atoi(value.c_str());
                
            }
            else if (key == "height") {
                map2_height = atoi(value.c_str());
            }
        }
        
        if (map2_width == -1 || map2_height == -1) {
            return false;
        }
        else { // allocate our map data
            level2_data = new unsigned char*[map2_height];
            for(int i = 0; i < map2_height; ++i) {
                level2_data[i] = new unsigned char[map2_width];
            }
        }
    }
    else if( gameState == level_three){
        map3_width = -1;
        map3_height = -1;
        while (getline(stream, line)) {
            if (line == "") {
                break;
            }
            std::istringstream sStream(line);
            std::string key, value;
            getline(sStream, key, '=');
            getline(sStream, value);
            if (key == "width") {
                map3_width = atoi(value.c_str());
                
            }
            else if (key == "height") {
                map3_height = atoi(value.c_str());
            }
        }
        
        if (map3_width == -1 || map3_height == -1) {
            return false;
        }
        else { // allocate our map data
            level3_data = new unsigned char*[map3_height];
            for(int i = 0; i < map3_height; ++i) {
                level3_data[i] = new unsigned char[map3_width];
            }
            return true;
        }
    }
    return true;
}



/*setup level 1*/
void setup_level1(){
    
    std::ifstream infile(RESOURCE_FOLDER"GameAssets/Levels/platform_level_1.txt");
    std::string line;
    
    if(!infile){
        std::cout << "Unable to load platform flaretxt. Make sure the path is correct\n";
        assert(false);
    }
    
    while (getline(infile, line)) {
        if (line == "[header]") {
            if (!readHeader(infile)) {
                return;
            }
        }
        else if (line == "[layer]") {
            readLayer1Data(infile);
        }
        else if (line == "[Entity]" && !isSetup1) {
            readEntityData1(infile);
        }
    }
}
/*setup level 2*/
void setup_level2(){
    
    std::ifstream infile(RESOURCE_FOLDER"GameAssets/Levels/platform_level_2.txt");
    std::string line;
    
    if(!infile){
        std::cout << "Unable to load platform flaretxt. Make sure the path is correct\n";
        assert(false);
    }
    
    while (getline(infile, line)) {
        if (line == "[header]") {
            if (!readHeader(infile)) {
                return;
            }
        }
        else if (line == "[layer]") {
            readLayer2Data(infile);
        }
        else if (line == "[Entity]" && !isSetup2) {
            readEntityData2(infile);
        }
    }
}

/*setup level 3*/
void setup_level3(){
    
    std::ifstream infile(RESOURCE_FOLDER"GameAssets/Levels/platform_level_3.txt");
    std::string line;
    
    if(!infile){
        std::cout << "Unable to load platform flaretxt. Make sure the path is correct\n";
        assert(false);
    }
    
    while (getline(infile, line)) {
        if (line == "[header]") {
            if (!readHeader(infile)) {
                return;
            }
        }
        else if (line == "[layer]") {
            readLayer3Data(infile);
        }
        else if (line == "[Entity]" && !isSetup3) {
            readEntityData3(infile);
        }
    }
}


void worldToTileCoordinates(float worldX, float worldY, int *gridX, int *gridY) {
    *gridX = (int)(worldX / TILE_SIZE);
    *gridY = (int)(-worldY / TILE_SIZE);
}


/* zombie ai */
void zombie_ai(){
    
    if(gameState == level_one){
        for(Entity* zombie: zombies1){
            int gridX = 0;
            int gridY = 0;
            float sensor_x = 0.0f;
            float sensor_y = 0.0f;
            if(zombie->position.x > zombie->init_position.x)
                sensor_x = zombie->position.x + TILE_SIZE;
            else
                sensor_x = zombie->position.x - TILE_SIZE;
            sensor_y = zombie->position.y-TILE_SIZE/2;
            worldToTileCoordinates(sensor_x, sensor_y, &gridX, &gridY);
            
            if ((gridX > 0 && gridX < map1_width) && (gridY > 0 && gridY < map1_height)){
                if(level1_data[gridY][gridX] == 0){

                    
                    if(zombie->velocity.x > 0)
                        zombie->position.x -= 0.2;
                    else
                        zombie->position.x += 0.2;
                    zombie->acceleration.x *= -1;

                }
            }
        }
    }
    else if(gameState == level_two){
        for(Entity* zombie: zombies2){
            int gridX = 0;
            int gridY = 0;
            float sensor_x = 0.0f;
            float sensor_y = 0.0f;
            if(zombie->position.x > zombie->init_position.x)
                sensor_x = zombie->position.x + TILE_SIZE;
            else
                sensor_x = zombie->position.x - TILE_SIZE;
            sensor_y = zombie->position.y-TILE_SIZE/2;
            worldToTileCoordinates(sensor_x, sensor_y, &gridX, &gridY);
            
            if ((gridX > 0 && gridX < map2_width) && (gridY > 0 && gridY < map2_height)){
                if(level2_data[gridY][gridX] == 0){
                    
                    
                    if(zombie->velocity.x > 0)
                        zombie->position.x -= 0.2;
                    else
                        zombie->position.x += 0.2;
                    zombie->acceleration.x *= -1;
                    
                }
            }
        }
    }
    else if(gameState == level_three){

        if(zombie_female.position.x -0.3 < 4.5){
            zombie_female.position.x += 0.5;
            zombie_female.acceleration.x  *= -1;
        }
        else if(zombie_female.position.x + 0.3 > 23.5){
            zombie_female.position.x -= 0.5;
            zombie_female.acceleration.x  *= -1;
        }
        
        

    }
    
}


/* bottom */
bool isOnPlatform(Entity& entity){
    int gridX;
    int gridY;

    worldToTileCoordinates(entity.position.x, entity.position.y, &gridX, &gridY);
    
    
    if(gameState == level_one){
        if ((gridX > 0 && gridX < map1_width) && (gridY > 0 && gridY < map1_height)){
            if(level1_data[gridY][gridX] == 5 ||
               level1_data[gridY][gridX] == 6 ||
               level1_data[gridY][gridX] == 7 ||
               level1_data[gridY][gridX] == 18||
               level1_data[gridY][gridX] == 19||
               level1_data[gridY][gridX] == 20
               ){
                float entity_edge = entity.position.y - entity.height/4;
                float tile_edge = (gridY * -TILE_SIZE);
                float penetration = fabs(tile_edge - entity_edge) + 0.0000001;
                entity.position.y += penetration;
                entity.collided_bot = true;
            }
            else{
                entity.collided_bot = false;
            }
        }

    }
    else if(gameState == level_two){
        if ((gridX > 0 && gridX < map2_width) && (gridY > 0 && gridY < map2_height)){
            if(level2_data[gridY][gridX] == 5 ||
               level2_data[gridY][gridX] == 6 ||
               level2_data[gridY][gridX] == 7 ||
               level2_data[gridY][gridX] == 18||
               level2_data[gridY][gridX] == 19||
               level2_data[gridY][gridX] == 20
               ){
                float entity_edge = entity.position.y - entity.height/4;
                float tile_edge = (gridY * -TILE_SIZE);
                float penetration = fabs(tile_edge - entity_edge) + 0.0000001;
                entity.position.y += penetration;
                entity.collided_bot = true;
            }
            else{
                entity.collided_bot = false;
            }
        }

    }
    else if(gameState == level_three){
        if ((gridX > 0 && gridX < map3_width) && (gridY > 0 && gridY < map3_height)){
            if(level3_data[gridY][gridX] == 5 ||
               level3_data[gridY][gridX] == 6 ||
               level3_data[gridY][gridX] == 7 ||
               level3_data[gridY][gridX] == 18||
               level3_data[gridY][gridX] == 19||
               level3_data[gridY][gridX] == 20
               ){
                float entity_edge = entity.position.y - entity.height/4;
                float tile_edge = (gridY * -TILE_SIZE);
                float penetration = fabs(tile_edge - entity_edge) + 0.0000001;
                entity.position.y += penetration;
                entity.collided_bot = true;
            }
            else{
                entity.collided_bot = false;
            }
        }
    }
    return true;
}
/* left */
bool collided_left(Entity& entity, int map_height, int map_width, unsigned char** level_data){

    int gridX;
    int gridY;
    worldToTileCoordinates(entity.position.x, entity.position.y, &gridX, &gridY);
    
    
    if ((gridX > 0 && gridX < map_width) && (gridY > 0 && gridY < map_height)){

        if(level_data[gridY][gridX] == 6 ||
           level_data[gridY][gridX] == 9 ||
           level_data[gridY][gridX] == 13 ||
           level_data[gridY][gridX] == 16
           ){
            float entity_edge = entity.position.x - entity.width/2;
            float tile_edge = (gridX * TILE_SIZE);
            float penetration = fabs(tile_edge - entity_edge) + 0.00001;
            entity.position.x += penetration;
            entity.collided_left = true;
            
        }
        else{
            entity.collided_left = false;
        }
        
    }
    return true;
}

bool kunai_hit(Entity* zombie){
    bool isHit = false;
    for(Entity* kunai: kunais){
        if(fabs(kunai->position.y - zombie->position.y) < SIZE/2){
            if(fabs(kunai->position.x-zombie->position.x) < SIZE/2){
                kunai->position.x = -20;
                isHit = true;
            }
        }
    }
    return isHit;
}
bool is_nearby(Entity* zombie){
    bool isNear = false;
    if(fabs(zombie->position.x-ninja.position.x) < SIZE)
        isNear = true;
    return isNear;
}

bool isHit_by(Entity* zombie){
    bool isHit = false;
    if(fabs(zombie->position.x-ninja.position.x) < 0.25)
        isHit = true;
    return isHit;
}
void update_main(float elapsed){
    if(text_collapsed > 0){
        text_collapsed -= elapsed*10;
    }
    else{
        text_collapsed = 0;
    }
    
    if(title_pos > 2.5){
        title_pos -= elapsed*7;
    }
    else{
        title_pos = 2.5;
    }
    
    if(start_pos < -1.5f){
        start_pos += elapsed*2.5;
    }
    else{
        start_pos = -1.5;
    }
}
/* updating level 1*/
void update_level1(float elapsed){
    
    isOnPlatform(ninja);
    collided_left(ninja, map1_height, map1_width, level1_data);
    if(ninja.collided_bot == true){
        ninja.isJumping = false;
    }

    ninja.delay += elapsed;
    ninja.update(elapsed);
    zombie_ai();
    for (Entity* zombie : zombies1){
        isOnPlatform(*zombie);
        if(is_nearby(zombie)){
            zombie->isAttacking = true;
            if(isHit_by(zombie)){
                zombie->velocity.x = 0;
                ninja.HP -= 1;
            }
        }
        else if(zombie->HP == 0)
            zombie->position.x = -20;
        else if(kunai_hit(zombie)){
            zombie->HP -= 1;
        }
        zombie->update(elapsed);
    }
    kunai_delay += elapsed;
    if(ninja.isAttacking && kunai_delay > 0.25){
        if (kunai_counter + 1 == 30)
            kunai_counter = 0;
        
        kunais[kunai_counter]->position.y = ninja.position.y - 0.1;
        if(ninja.velocity.x < 0){
            kunais[kunai_counter]->position.x = kunais[kunai_counter]->init_position.x = ninja.position.x - 0.6;
            kunais[kunai_counter]->acceleration.x = -50.0f;
        }
        else{
            kunais[kunai_counter]->position.x = kunais[kunai_counter]->init_position.x =ninja.position.x + 0.6;
            kunais[kunai_counter]->acceleration.x = 50.0f;
        }
        kunais[kunai_counter]->update(elapsed);
        kunai_counter += 1;
        kunai_delay = 0;
        
    }
    for (Entity* kunai: kunais){
        if(fabs(kunai->init_position.x - kunai->position.x) > 10.0f)
            kunai->position.x = kunai->init_position.x = -20;
        kunai->update(elapsed);
    }
    if(ninja.position.x > 47){
        gameState = level_two;
    }
}
/* updating level 2*/
void update_level2(float elapsed){
    
    isOnPlatform(ninja);
    if(ninja.collided_bot == true){
        ninja.isJumping = false;
    }
    //bounding//
    ninja.delay += elapsed;
    if(ninja.position.x < 5.5){
        ninja.velocity.x = 0;
        ninja.position.x += 0.2;
    }
    if(ninja.position.x > 24.5){
        ninja.velocity.x = 0;
        ninja.position.x -= 0.2;
    }
    ninja.update(elapsed);
    
    zombie_ai();
    for (Entity* zombie : zombies2){
        isOnPlatform(*zombie);

        if(is_nearby(zombie)){
            zombie->isAttacking = true;
        }
        if(isHit_by(zombie)){
            zombie->velocity.x = 0;
            ninja.HP -= 1;
        }

        if(zombie->HP == 0 || fabs(zombie->position.y-zombie->init_position.y)>0.2){
            zombie->reset();
        }
        else if(kunai_hit(zombie)){
            zombie->HP -= 1;
        }
        zombie->update(elapsed);
        
    }

    kunai_delay += elapsed;
    if(ninja.isAttacking && kunai_delay > 0.25){
        if (kunai_counter + 1 == 30)
            kunai_counter = 0;
        
        kunais[kunai_counter]->position.y = ninja.position.y - 0.1;
        if(ninja.velocity.x < 0){
            kunais[kunai_counter]->position.x = kunais[kunai_counter]->init_position.x = ninja.position.x - 0.6;
            kunais[kunai_counter]->acceleration.x = -50.0f;
        }
        else{
            kunais[kunai_counter]->position.x = kunais[kunai_counter]->init_position.x =ninja.position.x + 0.6;
            kunais[kunai_counter]->acceleration.x = 50.0f;
        }
        kunais[kunai_counter]->update(elapsed);
        kunai_counter += 1;
        kunai_delay = 0;
        
    }
    for (Entity* kunai: kunais){
        if(fabs(kunai->init_position.x - kunai->position.x) > 10.0f)
            kunai->position.x = kunai->init_position.x = -200;
        kunai->update(elapsed);
    }
    
    countdown -= elapsed;
    if(countdown < 0){
        gameState = level_three;
    }

}
/* updating level 3*/
void update_level3(float elapsed){
    
    isOnPlatform(ninja);
    if(ninja.collided_bot == true){
        ninja.isJumping = false;
    }
    //bounding//
    ninja.delay += elapsed;
    if(ninja.position.x < 5.5){
        ninja.velocity.x = 0;
        ninja.position.x += 0.2;
    }
    if(ninja.position.x > 24.5){
        ninja.velocity.x = 0;
        ninja.position.x -= 0.2;
    }
    ninja.update(elapsed);
    
    zombie_ai();

    isOnPlatform(zombie_female);
    
    if(is_nearby(&zombie_female)){
        zombie_female.isAttacking = true;
    }
    if(isHit_by(&zombie_female)){
        zombie_female.velocity.x = 0;
        ninja.HP -= 1;
    }
    
    if(zombie_female.HP == 0 || fabs(zombie_female.position.y-zombie_female.init_position.y)>0.2){
        zombie_female.reset();
    }
    else if(kunai_hit(&zombie_female)){
        zombie_female.HP -= rand() % 20;
    }
    zombie_female.update(elapsed);
        
    
    
    kunai_delay += elapsed;
    if(ninja.isAttacking && kunai_delay > 0.25){
        if (kunai_counter + 1 == 30)
            kunai_counter = 0;
        
        kunais[kunai_counter]->position.y = ninja.position.y - 0.1;
        if(ninja.velocity.x < 0){
            kunais[kunai_counter]->position.x = kunais[kunai_counter]->init_position.x = ninja.position.x - 0.6;
            kunais[kunai_counter]->acceleration.x = -50.0f;
        }
        else{
            kunais[kunai_counter]->position.x = kunais[kunai_counter]->init_position.x =ninja.position.x + 0.6;
            kunais[kunai_counter]->acceleration.x = 50.0f;
        }
        kunais[kunai_counter]->update(elapsed);
        kunai_counter += 1;
        kunai_delay = 0;
        
    }
    for (Entity* kunai: kunais){
        if(fabs(kunai->init_position.x - kunai->position.x) > 10.0f)
            kunai->position.x = kunai->init_position.x = -200;
        kunai->update(elapsed);
    }
    countdown -= elapsed;
//        std::cout << zombie_female.position.x << std::endl;
}

void reset(){
    ninja.reset();
    for(Entity* zombie :zombies1){
        zombie->reset();
    }

    for(Entity* zombie :zombies2){
        zombie->reset();
    }
    
    zombie_female.reset();

}

/*rendering main menu*/
void render_main(ShaderProgram* program){
    
    ModelMatrix.identity();
    ModelMatrix.Scale(30.0, 30.0, 1.0);
    program->setModelMatrix(ModelMatrix);
    background.draw(program);
    
    ModelMatrix.identity();
    ModelMatrix.Translate(title_pos, 4.0f, 0.0f);
    ModelMatrix.Scale(1.25f, 1.25f, 1.0f);
    program->setModelMatrix(ModelMatrix);
    DrawText(program, font, "Ninja Girl", 0.5, text_collapsed);
    
    ModelMatrix.identity();
    ModelMatrix.Translate(-4.0f, start_pos, 0);
    program->setModelMatrix(ModelMatrix);
    DrawText(program, font, "Press Return to Start", 0.5, 0);
    
    ViewMatrix.identity();
    program->setViewMatrix(ViewMatrix);
    SDL_GL_SwapWindow(displayWindow);
    glClear(GL_COLOR_BUFFER_BIT);
 
}


/*rendering level 1*/
void render_level1(ShaderProgram* program){
    ModelMatrix.identity();
    ModelMatrix.Translate(ninja.position.x, ninja.position.y, 0.0f);
    ModelMatrix.Scale(25.0, 25.0, 1.0);
    program->setModelMatrix(ModelMatrix);
    background.draw(program);
    
    ModelMatrix.identity();
    ModelMatrix.Translate(-5.0f, 0.0f, 0.0f);
    program->setModelMatrix(ModelMatrix);
    
    draw_level1(program, 7, 3);
    for (Entity* zombie : zombies1){
        if(zombie->HP == 0){
            zombie->render(program, "dead", 1);
        }
        
        else if(zombie->isAttacking == true){
            zombie->render(program, "attack",1);
            zombie->isAttacking = false;
        }
        else{
            zombie->render(program, "walk",1);
            zombie->isAttacking = false;
        }
        
    }
    for(uint i = 0; i < 30; i++){
        kunais[i]->render(program, "throw", 1);
    }
    if(ninja.isJumping){
        
        if (ninja.isAttacking)
            ninja.render(program, "jump_throw", 1);
        else
            ninja.render(program, "jump", 1);
    }
    else if(ninja.isAttacking)
        ninja.render(program, "throw", 1);
    else if(fabs(ninja.velocity.x) >= 0.5)
        ninja.render(program, "walk", 1);
    else if (ninja.velocity.x < 0.5)
        ninja.render(program, "idle", 1);
    
    if(ninja.HP < 0 || ninja.position.y < -10){
        gameState = main_menu;
        reset();
    }
    ModelMatrix.identity();
    ModelMatrix.Translate(ninja.position.x-5.1f, ninja.position.y+1.5, 0.0f);
    ModelMatrix.Scale(1.25f, 1.25f, 1.0f);
    program->setModelMatrix(ModelMatrix);
    std::string hp = std::to_string((int)ninja.HP);
    DrawText(program, font, hp , 0.25, 0.05);

    
    ViewMatrix.identity();
    ViewMatrix.Translate(-ninja.position.x, -ninja.position.y, 0.0f);
    program->setViewMatrix(ViewMatrix);
    SDL_GL_SwapWindow(displayWindow);
    glClear(GL_COLOR_BUFFER_BIT);

}
/*rendering level 2*/
void render_level2(ShaderProgram* program){
    ModelMatrix.identity();
    ModelMatrix.Translate(15, -4, 0.0f);
    ModelMatrix.Scale(25.0, 25.0, 1.0);
    program->setModelMatrix(ModelMatrix);
    background.draw(program);
    
    ModelMatrix.identity();
    program->setModelMatrix(ModelMatrix);
    
    draw_level2(program, 7, 3);
    for (Entity* zombie : zombies2){
        if(zombie->HP == 0){
            zombie->render(program, "dead",2);
        }
        else if(zombie->isAttacking == true){
            zombie->render(program, "attack",2);
            zombie->isAttacking = false;
        }
        else{
            zombie->render(program, "walk",2);
            zombie->isAttacking = false;
        }
    }
    for(uint i = 0; i < 30; i++){
        kunais[i]->render(program, "throw",2);
    }
    if(ninja.isHit == true){
        ninja.render(program, "hurt", 2);
    }
    else if(ninja.isJumping){
        
        if (ninja.isAttacking)
            ninja.render(program, "jump_throw",2);
        else
            ninja.render(program, "jump",2);
    }
    else if(ninja.isAttacking)
        ninja.render(program, "throw",2);
    
    else if(fabs(ninja.velocity.x) >= 0.5)
        ninja.render(program, "walk",2);
    
    else if (ninja.velocity.x < 0.5)
        ninja.render(program, "idle",2);
    

    if(ninja.HP < 0){
        gameState = main_menu;
        reset();
    }
    ModelMatrix.identity();
    ModelMatrix.Translate(ninja.position.x, ninja.position.y+1.5, 0.0f);
    ModelMatrix.Scale(1.25f, 1.25f, 1.0f);
    program->setModelMatrix(ModelMatrix);
    std::string hp = std::to_string((int)ninja.HP);
    DrawText(program, font, hp , 0.25, 0.05);

    ModelMatrix.identity();
    ModelMatrix.Translate(13.5f, -2.0f, 0.0f);
    ModelMatrix.Scale(1.25f, 1.25f, 1.0f);
    program->setModelMatrix(ModelMatrix);
    std::string cd = std::to_string((int)countdown);
    DrawText(program, font, cd , 2.5, 0.1);
    
    ViewMatrix.identity();
    ViewMatrix.Translate(-15, 4, 0.0f);
    program->setViewMatrix(ViewMatrix);
    SDL_GL_SwapWindow(displayWindow);
    glClear(GL_COLOR_BUFFER_BIT);
}

/*rendering level 3*/
void render_level3(ShaderProgram* program){
    ModelMatrix.identity();
    ModelMatrix.Translate(15, -4, 0.0f);
    ModelMatrix.Scale(25.0, 25.0, 1.0);
    program->setModelMatrix(ModelMatrix);
    background.draw(program);
    
    ModelMatrix.identity();
    program->setModelMatrix(ModelMatrix);
    
    draw_level3(program, 7, 3);
    if(zombie_female.HP == 0){
        zombie_female.render(program, "dead",3);
    }
    if(zombie_female.isAttacking == true){
        zombie_female.render(program, "attack",3);
        zombie_female.isAttacking = false;
    }
    else{
        zombie_female.render(program, "walk",3);
        zombie_female.isAttacking = false;
    }
    
    for(uint i = 0; i < 30; i++){
        kunais[i]->render(program, "throw",2);
    }
    if(ninja.isHit == true){
        ninja.render(program, "hurt", 2);
    }
    else if(ninja.isJumping){
        
        if (ninja.isAttacking)
            ninja.render(program, "jump_throw",2);
        else
            ninja.render(program, "jump",2);
    }
    else if(ninja.isAttacking)
        ninja.render(program, "throw",2);
    
    else if(fabs(ninja.velocity.x) >= 0.5)
        ninja.render(program, "walk",2);
    
    else if (ninja.velocity.x < 0.5)
        ninja.render(program, "idle",2);
    
    
    if(ninja.HP < 0){
        gameState = main_menu;
        reset();
    }
    ModelMatrix.identity();
    ModelMatrix.Translate(ninja.position.x, ninja.position.y+1.5, 0.0f);
    ModelMatrix.Scale(1.25f, 1.25f, 1.0f);
    program->setModelMatrix(ModelMatrix);
    std::string hp = std::to_string((int)ninja.HP);
    DrawText(program, font, hp , 0.25, 0.05);
    
    ModelMatrix.identity();
    ModelMatrix.Translate(zombie_female.position.x, zombie_female.position.y+2.5, 0.0f);
    ModelMatrix.Scale(1.25f, 1.25, 1.0f);
    program->setModelMatrix(ModelMatrix);
    std::string hp2 = std::to_string((int)zombie_female.HP);
    DrawText(program, font, hp2 , 0.25, 0.05);
    
    ViewMatrix.identity();
    ViewMatrix.Translate(-15, 4, 0.0f);
    program->setViewMatrix(ViewMatrix);
    SDL_GL_SwapWindow(displayWindow);
    glClear(GL_COLOR_BUFFER_BIT);
}

/* run menu */
void menu(ShaderProgram* program){
    SDL_Event event;
    while (gameState == main_menu) {
        process_input(&event);
        
        float ticks = (float) SDL_GetTicks()/1000;
        float elapsed = ticks - lastFrameTicks;
        lastFrameTicks = ticks;
        float fixedElapsed = elapsed;
        
        if(fixedElapsed > 1/FPS * MAX_TIMESTEPS){
            fixedElapsed = 1/FPS * MAX_TIMESTEPS;
        }
        while(fixedElapsed >= 1/FPS){
            fixedElapsed -= 1/FPS;
            update_main(1/FPS);
        }
        update_main(fixedElapsed);
        render_main(program);
    }

}
/*run level 1*/
void level1(ShaderProgram* program){

    setup_level1();
    isSetup1 = true;
    SDL_Event event;
    while (gameState == level_one) {
        process_input(&event);
        
        float ticks = (float) SDL_GetTicks()/1000;
        float elapsed = ticks - lastFrameTicks;
        lastFrameTicks = ticks;
        float fixedElapsed = elapsed;
        
        if(fixedElapsed > 1/FPS * MAX_TIMESTEPS){
            fixedElapsed = 1/FPS * MAX_TIMESTEPS;
        }
        while(fixedElapsed >= 1/FPS){
            fixedElapsed -= 1/FPS;
            update_level1(1/FPS);
        }
        update_level1(fixedElapsed);
        
        render_level1(program);
    }
    
}
/*run level 2*/
void level2(ShaderProgram* program){

    setup_level2();
    isSetup2 = true;
    SDL_Event event;
    while (gameState == level_two) {
        process_input(&event);
        
        float ticks = (float) SDL_GetTicks()/1000;
        float elapsed = ticks - lastFrameTicks;
        lastFrameTicks = ticks;
        float fixedElapsed = elapsed;
        
        if(fixedElapsed > 1/FPS * MAX_TIMESTEPS){
            fixedElapsed = 1/FPS * MAX_TIMESTEPS;
        }
        while(fixedElapsed >= 1/FPS){
            fixedElapsed -= 1/FPS;
            update_level2(1/FPS);
        }
        update_level2(fixedElapsed);
        
        render_level2(program);

    }
}
/*run level 3*/
void level3(ShaderProgram* program){
    setup_level3();
    isSetup3 = true;
    SDL_Event event;
    while (gameState == level_three) {
        process_input(&event);
        
        float ticks = (float) SDL_GetTicks()/1000;
        float elapsed = ticks - lastFrameTicks;
        lastFrameTicks = ticks;
        float fixedElapsed = elapsed;
        
        if(fixedElapsed > 1/FPS * MAX_TIMESTEPS){
            fixedElapsed = 1/FPS * MAX_TIMESTEPS;
        }
        while(fixedElapsed >= 1/FPS){
            fixedElapsed -= 1/FPS;
            update_level3(1/FPS);
        }
        update_level3(fixedElapsed);
        
        render_level3(program);
    }
}


int main(int argc, char *argv[]){
    
    setup();
    
    ShaderProgram program(RESOURCE_FOLDER"vertex_textured.glsl", RESOURCE_FOLDER"fragment_textured.glsl");
    glUseProgram(program.programID);
#ifdef _WINDOWS
    glewInit();
#endif
    
    init_frames();
    
    program.setViewMatrix(ViewMatrix);
    program.setModelMatrix(ModelMatrix);
    
    while (!done) {
        Mix_PlayMusic(music, -1);
        switch (gameState){
            case main_menu:
                ProjectionMatrix.setOrthoProjection(-10.0f, 10.0f, -4.0f, 6.0f, -1.0f, 1.0f);
                program.setProjectionMatrix(ProjectionMatrix);
                menu(&program);
                break;
            case level_one:
                ProjectionMatrix.setOrthoProjection(-10.0f, 10.0f, -4.0f, 6.0f, -1.0f, 1.0f);
                program.setProjectionMatrix(ProjectionMatrix);
                level1(&program);
                break;
            case level_two:
                ProjectionMatrix.setOrthoProjection(-10.0f, 10.0f, -5.0f, 5.0f, -1.0f, 1.0f);
                program.setProjectionMatrix(ProjectionMatrix);
                level2(&program);
                break;
            case level_three:
                ProjectionMatrix.setOrthoProjection(-10.0f, 10.0f, -5.0f, 5.0f, -1.0f, 1.0f);
                level3(&program);
                break;
            case quit:
                done = true;
        }
    }
    Mix_FreeChunk(sound_throw);
    Mix_FreeMusic(music);
    SDL_Quit();
    return 0;
}
