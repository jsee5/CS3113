/* A: move left D: move right Space: jump.
 collect coins, and it will randomly appear @ a different location. game is endless.
 
 By the way. The cat can walk on air.
 */



#ifdef _WINDOWS
#include <GL/glew.h>
#endif
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#include <SDL.h>
#include <SDL_opengl.h>
#include <SDL_image.h>
#include <vector>


#include "ShaderProgram.h"
#include "Matrix.h"
#include "Entity.h"
#include "SpriteSheet.h"

using namespace std;



#ifdef _WINDOWS
#define RESOURCE_FOLDER ""
#else
#define RESOURCE_FOLDER "NYUCodebase.app/Contents/Resources/"
#endif

/* core game definitions */
#define FPS 60.0f
#define MAX_TIMESTEPS 6

Matrix ModelMatrix;
Matrix ViewMatrix;
Matrix ProjectionMatrix;

float lastFrameTicks;
bool done = false;

/*
GLuint textureID_of_entity;
Entity an_entity;
#define spritecount_x 
#define spritecount_y
*/

/*Platformer
int map_width, map_height;

unsigned char** level_data;
*/


SDL_Window* displayWindow;
void setup(){
    SDL_Init(SDL_INIT_VIDEO);
    displayWindow = SDL_CreateWindow("Name of Game", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 640, 360, SDL_WINDOW_OPENGL);
    SDL_GLContext context = SDL_GL_CreateContext(displayWindow);
    SDL_GL_MakeCurrent(displayWindow, context);
    glViewport(0, 0, 640, 360);
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

void process_input(SDL_Event* event){
    while (SDL_PollEvent(event)) {
        if (event->type == SDL_QUIT || event->type == SDL_WINDOWEVENT_CLOSE) {
            done = true;
        }
        else if (event->type == SDL_KEYDOWN){
            //key press
            if (event->key.keysym.scancode == SDL_SCANCODE_ESCAPE){
                //something happens
            } 
        }
    }
}



void update(ShaderProgram* program, float& animationElapsed){
  
}

/* Level Rendering
void render_level(ShaderProgram* program){
    glBindTexture(GL_TEXTURE_2D, Tile);
    
    for (int y = 0; y < map_height; y++) {
        for (int x = 0; x < map_width; x++) {
            if (level_data[y][x] != 0) {
                
                float u = (float)(((int)level_data[y][x]) % TILE_X) / (float)TILE_X;
                float v = (float)(((int)level_data[y][x]) / TILE_X) / (float)TILE_Y;
                
                float spriteWidth = 1.0f / (float)TILE_X;
                float spriteHeight = 1.0f / (float)TILE_Y;
                
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
 */

/*read layer data from flare txt
bool readLayerData(std::ifstream &stream) {
    string line;
    while (getline(stream, line)) {
        if (line == "") {
            break;
        }
        istringstream sStream(line);
        string key, value;
        getline(sStream, key, '=');
        getline(sStream, value);
        if (key == "data") {
            for (int y = 0; y < map_height; y++) {
                getline(stream, line);
                istringstream lineStream(line);
                string tile;
                for (int x = 0; x < map_width; x++) {
                    getline(lineStream, tile, ',');
                    unsigned char val = (unsigned char)atoi(tile.c_str());
                    if (val > 1) {
                        // be careful, the tiles in this format are indexed from 1 not 0
                        level_data[y][x] = val - 1;
                    }
                    
                    else {
                        level_data[y][x] = 0;
                    }
                }
            }
        }
    }
    return true;
}
*/

/* place the entity data from flare txt file
void placeEntity(string type, float x, float y) {
    if (type == "some_entity") {
        some_entity.isStatic = false;
        some_entity.position.x = x;
        some_entity.position.y = y-TILE_SIZE/2;
        some_entity.width = TILE_SIZE;
        some_entity.height = TILE_SIZE;
        some_entity.acceleration.y = -15;
    }
    else if(type == "Collectible"){
        
        coin.isStatic = true;
        coin.position.x = x;
        coin.position.y = y;
        coin.width = TILE_SIZE;
        coin.height = TILE_SIZE;
    }
}
*/

/* read entity data from flare.txt file
bool readEntityData(std::ifstream &stream) {
    string line;
    string type;
    while (getline(stream, line)) {
        if (line == "") {
            break;
        }
        istringstream sStream(line);
        string key, value;
        getline(sStream, key, '=');
        getline(sStream, value);
        
        if (key == "type") {
            type = value;
        } else if (key == "losome_entityion") {
            istringstream lineStream(value);
            string xPosition, yPosition;
            getline(lineStream, xPosition, ',');
            getline(lineStream, yPosition, ',');
            
            float placeX = atoi(xPosition.c_str())*TILE_SIZE;
            float placeY = atoi(yPosition.c_str())*-TILE_SIZE;
            
            placeEntity(type, placeX, placeY);
        }
    }
    return true;
}
*/

/* read header from flare.txt
bool readHeader(ifstream& stream) {
    string line;
    map_width = -1;
    map_height = -1;
    while (getline(stream, line)) {
        if (line == "") {
            break;
        }
        istringstream sStream(line);
        string key, value;
        getline(sStream, key, '=');
        getline(sStream, value);
        if (key == "width") {
            map_width = atoi(value.c_str());
        }
        else if (key == "height") {
            map_height = atoi(value.c_str());
        }
    }
    
    if (map_width == -1 || map_height == -1) {
        return false;
    } else { // allocate our map data
        level_data = new unsigned char*[map_height];
        for(int i = 0; i < map_height; ++i) {
            level_data[i] = new unsigned char[map_width];
        }
        return true;
    }
}
*/

/*
void parse_flare(){
    
    ifstream infile(RESOURCE_FOLDER"tile_platform.txt");
    string line;
    while (getline(infile, line)) {
        if (line == "[header]") {
            if (!readHeader(infile)) {
                return;
            }
        }
        else if (line == "[layer]") {
            readLayerData(infile);
        }
        else if (line == "[Entity]") {
            readEntityData(infile);
        }
    }
    
    some_entity = loadTexture(RESOURCE_FOLDER"some_entity.png");


    std::vector<SpriteSheet*> cat_idle;
    std::vector<SpriteSheet*> cat_walking;
    
    for(int i = 35; i < 44; ++i ){
        cat_idle.push_back(new SpriteSheet(Cat, i, CAT_X, CAT_Y, TILE_SIZE));
    }
    cat.actions[IDLE] = cat_idle;
}
*/

/* collision
void colliding_bot(Entity& entity){
    int gridX;
    int gridY;
    
    
    worldToTileCoordinates(entity.position.x, entity.position.y - entity.height/2, &gridX, &gridY);
    if(level_data[gridY][gridX] != 0){
        float entityEndpoint = entity.position.y - entity.height/2;
        float tileEndpoint = (gridY * -TILE_SIZE);
        float penetration = tileEndpoint - entityEndpoint;
        entity.position.y += (penetration+0.0001);
        entity.collided_bot = true;
        
    } else {
        entity.collided_bot = false;
    }
}

void colliding_left(Entity& entity){
    
    int gridX;
    int gridY;
    worldToTileCoordinates(entity.position.x, entity.position.y - entity.height/2, &gridX, &gridY);
    
    worldToTileCoordinates(entity.position.x - entity.width/2, entity.position.y,&gridX, &gridY);
    if(level_data[gridY][gridX] != 0){
        float entityEndpoint = entity.position.x - entity.width/2;
        float tileEndpoint = (gridX * TILE_SIZE)+TILE_SIZE;
        float penetration = tileEndpoint - entityEndpoint;
        entity.position.x += (penetration+0.0001);
        entity.collided_left = true;
    } else {
        entity.collided_left = false;
    }
}

void colliding_right(Entity& entity){
    int gridX;
    int gridY;
    
    worldToTileCoordinates(entity.position.x + entity.width/2, entity.position.y,&gridX, &gridY);
    if(level_data[gridY][gridX] != 0){
        float entityEndpoint = entity.position.x + entity.width/2;
        float tileEndpoint = (gridX * TILE_SIZE);
        float penetration = tileEndpoint - entityEndpoint;
        entity.position.x += (penetration-0.0001);
        entity.collided_right = true;
    } else {
        entity.collided_right = false;
    }
}

*/

void render(ShaderProgram* program){
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    
    /*render level
    ModelMatrix.identity();
    program->setModelMatrix(ModelMatrix);
    render_level(program);
    */
    
    /*render an entity
    ModelMatrix.identity();
    ModelMatrix.Translate(coin.position.x + TILE_SIZE/2, coin.position.y + TILE_SIZE/2, 0);
    program->setModelMatrix(ModelMatrix);
    coin.render(program);
    SDL_GL_SwapWindow(displayWindow);
    
    glClear(GL_COLOR_BUFFER_BIT);
     */
}
void update(float elapsed){
    
    /* update with tile collision detection
    cat.collided_bot = cat.collided_top = cat.collided_left = cat.collided_right = false;
    
    colliding_bot(cat);
    colliding_right(cat);
    colliding_left(cat);
    
    cat.isColliding_with(coin);
    
    coin.update(elapsed);
    cat.update(elapsed);
    
    ViewMatrix.identity();
    ViewMatrix.Translate(-cat.position.x*2*TILE_SIZE, -cat.position.y*TILE_SIZE, 0.0f);
    */

}
int main(int argc, char *argv[]){
    
    setup();
    ShaderProgram program(RESOURCE_FOLDER"vertex_textured.glsl", RESOURCE_FOLDER"fragment_textured.glsl");
    glUseProgram(program.programID);
    
    ProjectionMatrix.setOrthoProjection(-5.0f, 5.00f, -3.00f, 3.00f, -1.0f, 1.0f);
    
    
#ifdef _WINDOWS
    glewInit();
#endif
    
   // parse_flare()
    
    SDL_Event event;
    
    while (!done) {
        
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
            update(fixedElapsed);
        }
        update(1/FPS);
        
        render(&program);
        
        
    }
    SDL_Quit();
    return 0;
}
