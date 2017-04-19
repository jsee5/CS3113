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
    const Uint8 *key = SDL_GetKeyboardState(NULL);
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
        if(key[SDL_SCANCODE_D]){
            //entity.acceleration.x = 6.0f;
        }


    }
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
}
*/


//sat collision

/*Required to implement penetrationSort and testSATSeparationForEdge */
bool testSATSeparationForEdge(float edgeX, float edgeY, const std::vector<Vector> &points1, const std::vector<Vector> &points2, Vector &penetration) {
    float normal_x = -edgeY;
    float normal_y = edgeX;
    float len = sqrtf(normal_x*normal_x + normal_y*normal_y);
    normal_x /= len;
    normal_y /= len;
    
    std::vector<float> e1_project;
    std::vector<float> e2_project;
    
    for (int i = 0; i < points1.size(); i++) {
        e1_project.push_back(points1[i].x * normal_x + points1[i].y * normal_y);
    }
    for (int i = 0; i < points2.size(); i++) {
        e2_project.push_back(points2[i].x * normal_x + points2[i].y * normal_y);
    }
    
    std::sort(e1_project.begin(), e1_project.end());
    std::sort(e2_project.begin(), e2_project.end());
    
    float e1_min = e1_project[0];
    float e1_max = e1_project[e1_project.size() - 1];
    float e2_min = e2_project[0];
    float e2_max = e2_project[e2_project.size() - 1];
    
    float e1_width = fabs(e1_max - e1_min);
    float e2_width = fabs(e2_max - e2_min);
    float e1Center = e1_min + (e1_width / 2.0);
    float e2Center = e2_min + (e2_width / 2.0);
    float dist = fabs(e1Center - e2Center);
    float p = dist - ((e1_width + e2_width) / 2.0);
    
    if (p >= 0) {
        return false;
    }
    
    float pen_Min1 = e1_max - e2_min;
    float pen_Min2 = e2_max - e1_min;
    
    float pen_Min = pen_Min1;
    if (pen_Min2 < pen_Min) {
        pen_Min = pen_Min2;
    }
    
    penetration.x = normal_x * pen_Min;
    penetration.y = normal_y * pen_Min;
    
    return true;
}


//sort based on ascending order
bool penetrationSort(const Vector& p1, const Vector& p2){
    return (p1.length() < p2.length());
}


/*Function provided by Professor Safrin*/

bool checkSATCollision(const std::vector<Vector> &e1Points, const std::vector<Vector> &e2Points, Vector &penetration) {
    std::vector<Vector> penetrations;
    for(int i=0; i < e1Points.size(); i++) {
        float edgeX, edgeY;
        
        if(i == e1Points.size()-1) {
            edgeX = e1Points[0].x - e1Points[i].x;
            edgeY = e1Points[0].y - e1Points[i].y;
        } else {
            edgeX = e1Points[i+1].x - e1Points[i].x;
            edgeY = e1Points[i+1].y - e1Points[i].y;
        }
        
        Vector penetration;
        bool result = testSATSeparationForEdge(edgeX, edgeY, e1Points, e2Points, penetration);
        if(!result) {
            return false;
        }
        penetrations.push_back(penetration);
    }
    for(int i=0; i < e2Points.size(); i++) {
        float edgeX, edgeY;
        
        if(i == e2Points.size()-1) {
            edgeX = e2Points[0].x - e2Points[i].x;
            edgeY = e2Points[0].y - e2Points[i].y;
        } else {
            edgeX = e2Points[i+1].x - e2Points[i].x;
            edgeY = e2Points[i+1].y - e2Points[i].y;
        }
        Vector penetration;
        bool result = testSATSeparationForEdge(edgeX, edgeY, e1Points, e2Points, penetration);
        
        if(!result) {
            return false;
        }
        penetrations.push_back(penetration);
    }
    
    std::sort(penetrations.begin(), penetrations.end(), penetrationSort);
    penetration = penetrations[0];
    
    Vector e1Center;
    for(int i=0; i < e1Points.size(); i++) {
        e1Center.x += e1Points[i].x;
        e1Center.y += e1Points[i].y;
    }
    e1Center.x /= (float)e1Points.size();
    e1Center.y /= (float)e1Points.size();
    
    Vector e2Center;
    for(int i=0; i < e2Points.size(); i++) {
        e2Center.x += e2Points[i].x;
        e2Center.y += e2Points[i].y;
    }
    e2Center.x /= (float)e2Points.size();
    e2Center.y /= (float)e2Points.size();
    
    Vector ba;
    ba.x = e1Center.x - e2Center.x;
    ba.y = e1Center.y - e2Center.y;
    
    if( (penetration.x * ba.x) + (penetration.y * ba.y) < 0.0f) {
        penetration.x *= -1.0f;
        penetration.y *= -1.0f;
    }
    return true;
}


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


void update(float elapsed){
    
    
    //check for colliions and update
    
    //entity.update()
    
    
}

void render(ShaderProgram* program){
    glClear(GL_COLOR_BUFFER_BIT);
    
    //entity.render()
    
    SDL_GL_SwapWindow(displayWindow);
    
    
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
        program.setViewMatrix(ViewMatrix);
        program.setProjectionMatrix(ProjectionMatrix);
        program.setModelMatrix(ModelMatrix);
        
        if(fixedElapsed > 1/FPS * MAX_TIMESTEPS){
            fixedElapsed = 1/FPS * MAX_TIMESTEPS;
        }
        while(fixedElapsed >= 1/FPS){
            fixedElapsed -= 1/FPS;
            update(1/FPS);
        }
        update(fixedElapsed);
        render(&program);
        
    }
    SDL_Quit();
    return 0;
}
