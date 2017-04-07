/* WASD for movement */
/* Pressing two of the WASD keys will rotate the plane */



#ifdef _WINDOWS
#include <GL/glew.h>
#endif
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#include <SDL.h>
#include <SDL_opengl.h>
#include <SDL_image.h>
#include <vector>
#include <algorithm>


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
#define LEFT_BOUND -5.33f
#define RIGHT_BOUND 5.33f
#define LOWER_BOUND -3.55f
#define UPPER_BOUND 3.55f


GLuint sheetID;
Entity plane, rock1, rock2, rock3, rock4, rock5, rock6;
float lastFrameTicks;
bool done = false;
Matrix ModelMatrix, ProjectionMatrix, ViewMatrix;

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



/*Load texture from a directory; also provided by Professor Safrin*/
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
    
    stbi_image_free(image);
    return retTexture;
}


SDL_Window* displayWindow;

void setup(){
    
    SDL_Init(SDL_INIT_VIDEO);
    displayWindow = SDL_CreateWindow("SAT Collision Demo", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 640, 360, SDL_WINDOW_OPENGL);
    SDL_GLContext context = SDL_GL_CreateContext(displayWindow);
    SDL_GL_MakeCurrent(displayWindow, context);
    glViewport(0, 0, 640, 360);
}


void process_input(SDL_Event* event){
    const Uint8 *key1 = SDL_GetKeyboardState(NULL);
    const Uint8 *key2 = SDL_GetKeyboardState(NULL);
    while (SDL_PollEvent(event)) {
        if (event->type == SDL_QUIT || event->type == SDL_WINDOWEVENT_CLOSE) {
            done = true;
        }
        else if(event->type == SDL_KEYUP){
            if (event->key.keysym.scancode == SDL_SCANCODE_W||
                event->key.keysym.scancode == SDL_SCANCODE_A||
                event->key.keysym.scancode == SDL_SCANCODE_S||
                event->key.keysym.scancode == SDL_SCANCODE_D){
                plane.acceleration.x = plane.acceleration.y = plane.angle = 0;
            }
        }
        if(key1[SDL_SCANCODE_D]){
            plane.acceleration.x = 6.0f;
        }
        else if(key1[SDL_SCANCODE_A]){
            plane.acceleration.x = -6.0f;
        }
        else if(key1[SDL_SCANCODE_W]){
            plane.acceleration.y = 6.0f;
        }
        else if(key1[SDL_SCANCODE_S]){
            plane.acceleration.y = -6.0f;
        }
        if((key1[SDL_SCANCODE_A] && key2[SDL_SCANCODE_W]) || (key1[SDL_SCANCODE_A] && key2[SDL_SCANCODE_S])){
            plane.angle = 0.02f;
        }
        else if((key1[SDL_SCANCODE_W] && key2[SDL_SCANCODE_D]) || (key1[SDL_SCANCODE_D] && key2[SDL_SCANCODE_S])){
            plane.angle = -0.02f;
        }
    }
}


void checkCollision(Entity& entity1, Entity& entity2){
    
    std::vector<Vector> entity1_toWorld, entity2_toWorld;
    entity1.toWorldSpace(entity1_toWorld);
    entity2.toWorldSpace(entity2_toWorld);
    
    
    Vector penetration;
    
    if(checkSATCollision(entity1_toWorld, entity2_toWorld, penetration)){
//        entity1.matrix.Translate(0.5f*penetration.x, 0.5*penetration.y, 0);
//        entity2.matrix.Translate(-0.5f*penetration.x, -0.5*penetration.y, 0);
        
        entity1.matrix.m[3][0] += 0.5f*penetration.x;
        entity1.matrix.m[3][1] += 0.5f*penetration.y;
        
        entity2.matrix.m[3][0] -= 0.5f*penetration.x;
        entity2.matrix.m[3][1] -= 0.5f*penetration.y;
    }

}

void update(float elapsed){

    
    checkCollision(plane,rock1);
    checkCollision(plane,rock2);
    checkCollision(plane, rock3);
    checkCollision(plane,rock4);
    checkCollision(plane,rock5);
    checkCollision(plane, rock6);
    
    checkCollision(rock1, rock2);
    checkCollision(rock1, rock3);
    checkCollision(rock1, rock4);
    checkCollision(rock1, rock5);
    checkCollision(rock1, rock6);
    
    checkCollision(rock2, rock3);
    checkCollision(rock2, rock4);
    checkCollision(rock2, rock5);
    checkCollision(rock2, rock6);
    
    checkCollision(rock3, rock4);
    checkCollision(rock3, rock5);
    checkCollision(rock3, rock6);
    
    checkCollision(rock4, rock5);
    checkCollision(rock4, rock6);
    

    checkCollision(rock5, rock6);

    rock1.update(elapsed);
    rock2.update(elapsed);
    rock3.update(elapsed);
    rock4.update(elapsed);
    rock5.update(elapsed);
    rock6.update(elapsed);
    plane.update(elapsed);
    
    
}


void render(ShaderProgram* program){
    glClear(GL_COLOR_BUFFER_BIT);
    
    rock1.render(program);
    rock2.render(program);
    rock3.render(program);
    rock4.render(program);
    rock5.render(program);
    rock6.render(program);
    plane.render(program);
    
    SDL_GL_SwapWindow(displayWindow);

    
}

void preload(){
    sheetID = loadTexture(RESOURCE_FOLDER"sheet.png");
    
    
    plane.sprite = SpriteSheet(sheetID, 112, 791, 112, 75, 1024, 1024, 0.5);
    rock1.sprite = SpriteSheet(sheetID, 282, 452, 45, 40, 1024, 1024, 0.5);
    rock2.sprite = SpriteSheet(sheetID, 282, 452, 45, 40, 1024, 1024, 0.5);
    rock3.sprite = SpriteSheet(sheetID, 282, 452, 45, 40, 1024, 1024, 0.5);
    rock4.sprite = SpriteSheet(sheetID, 282, 452, 45, 40, 1024, 1024, 0.5);
    rock5.sprite = SpriteSheet(sheetID, 282, 452, 45, 40, 1024, 1024, 0.5);
    rock6.sprite = SpriteSheet(sheetID, 282, 452, 45, 40, 1024, 1024, 0.5);
    
    plane.init_Vectors();
    rock1.init_Vectors();
    rock2.init_Vectors();
    rock3.init_Vectors();
    rock4.init_Vectors();
    rock5.init_Vectors();
    rock6.init_Vectors();
    
    
    plane.matrix.Translate(-3.0f, 0.0f, 0.0f);
    
    rock1.matrix.Translate(3.0f, 1.0f, 0.0f);
    rock2.matrix.Translate(-3.0f, -2.0f, 0.0f);
    rock3.matrix.Translate(-2.0f, -1.0f, 0.0f);
    rock4.matrix.Translate(1.0f, 1.0f, 0.0f);
    rock5.matrix.Translate(-3.0f, -4.0f, 0.0f);
    rock6.matrix.Translate(-1.0f, -3.0f, 0.0f);
    
    rock1.matrix.Rotate(0.5f);
    rock2.matrix.Rotate(-0.3f);
    rock3.matrix.Rotate(0.3f);
    rock4.matrix.Rotate(0.5f);
    rock5.matrix.Rotate(-0.3f);
    rock6.matrix.Rotate(0.3f);
    
    rock1.matrix.Scale(2.0f, 2.0f, 0.0f);
    rock2.matrix.Scale(1.75f, 1.75f, 0.0f);
    rock3.matrix.Scale(0.75f, 0.75f, 0.0f);
    rock4.matrix.Scale(1.65f, 1.23f, 0.0f);
    rock5.matrix.Scale(2.3f, 2.3f, 0.0f);
    rock6.matrix.Scale(0.75f, 0.45f, 0.0f);
    
    rock1.acceleration.x = 2.2f;
    rock1.acceleration.y = 2.2f;
    
    rock2.acceleration.x = -2.2f;
    rock2.acceleration.y = 2.2f;
    
    rock3.acceleration.x = -2.2f;
    rock3.acceleration.y = 2.2f;
    
    rock4.acceleration.x = 2.2f;
    rock4.acceleration.y = 2.2f;
    
    rock5.acceleration.x = -2.2f;
    rock5.acceleration.y = 2.2f;
    
    rock6.acceleration.x = -2.2f;
    rock6.acceleration.y = 2.2f;
    

    
   
    

 
}

int main(int argc, char *argv[]){
    
    setup();
    
    ShaderProgram program(RESOURCE_FOLDER"vertex_textured.glsl", RESOURCE_FOLDER"fragment_textured.glsl");
    glUseProgram(program.programID);
    
#ifdef _WINDOWS
    glewInit();
#endif
    
    SDL_Event event;
    program.setModelMatrix(ModelMatrix);
    preload();

    ProjectionMatrix.setOrthoProjection(LEFT_BOUND, RIGHT_BOUND, LOWER_BOUND, UPPER_BOUND, -1.0f, 1.0f);
    
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
        
//        std::cout<< plane.matrix.m[3][0] << " " << plane.matrix.m[3][1] << std::endl;
        render(&program);
        
    }
    SDL_Quit();
    return 0;
}
