#ifdef _WINDOWS
#include <GL/glew.h>
#endif
#include <SDL.h>
#include <SDL_opengl.h>
#include <SDL_image.h>
#include "ShaderProgram.h"
#include "Matrix.h"
#include "Entity.h"
#include "Sprite.h"
#ifdef _WINDOWS
#define RESOURCE_FOLDER ""
#else
#define RESOURCE_FOLDER "NYUCodebase.app/Contents/Resources/"
#endif

/* core game definitions */
#define FPS 15.0f
#define MAX_TIMESTEPS 6

bool done = false;

SDL_Window* displayWindow;

void setup(){
    SDL_Init(SDL_INIT_VIDEO);
    displayWindow = SDL_CreateWindow("Dragon Invader", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 640, 360, SDL_WINDOW_OPENGL);
    SDL_GLContext context = SDL_GL_CreateContext(displayWindow);
    SDL_GL_MakeCurrent(displayWindow, context);
    glViewport(0, 0, 640, 360);
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
void render(ShaderProgram* program){
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    SDL_GL_SwapWindow(displayWindow);
}

int main(int argc, char *argv[]){
    
    setup();
    ShaderProgram program(RESOURCE_FOLDER"vertex_textured.glsl", RESOURCE_FOLDER"fragment_textured.glsl");
    glUseProgram(program.programID);

    Matrix ProjectionMatrix;
    ProjectionMatrix.setOrthoProjection(-5.33f, 5.33f, -3.55f, 3.55f, -1.0f, 1.0f);
    Matrix ViewMatrix;

    /*time init*/
    float lastFrameTicks = 0.0f;
    float animationElapsed = 0.0f;
    
#ifdef _WINDOWS
    glewInit();
#endif
    
    SDL_Event event;
    while (!done) {
        process_input(&event);
        
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
