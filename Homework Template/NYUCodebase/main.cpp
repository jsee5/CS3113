#ifdef _WINDOWS
#include <GL/glew.h>
#endif
#include <SDL.h>
#include <SDL_opengl.h>
#include <SDL_image.h>
#include "ShaderProgram.h"
#include "Matrix.h"
#include "Entity.h"

#ifdef _WINDOWS
#define RESOURCE_FOLDER ""
#else
#define RESOURCE_FOLDER "NYUCodebase.app/Contents/Resources/"
#endif

SDL_Window* displayWindow;

//setup
void setup(){
    SDL_Init(SDL_INIT_VIDEO);
    displayWindow = SDL_CreateWindow("Game Programming Template", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 640, 360, SDL_WINDOW_OPENGL);
    SDL_GLContext context = SDL_GL_CreateContext(displayWindow);
    SDL_GL_MakeCurrent(displayWindow, context);
    
    glViewport(0, 0, 640, 360);
    
    
}

int main(int argc, char *argv[]){
    
    setup();
    
    ShaderProgram program(RESOURCE_FOLDER"vertex_textured.glsl", RESOURCE_FOLDER"fragment_textured.glsl");
    glUseProgram(program.programID);
    
    
//    Entity entity;
//    GLuint entity_Texture = leftPaddle.LoadTexture(RESOURCE_FOLDER"paddleBlu.png");
    
    Matrix ProjectionMatrix;
    Matrix ViewMatrix;
    
    ProjectionMatrix.setOrthoProjection(-5.0, 5.0, -2.0f, 2.0f, -1.0f, 1.0f);

    /*time init*/
    float lastFrameTicks = 0.0f;
    
#ifdef _WINDOWS
    glewInit();
#endif
    
    /* time tracking */
    float ticks = (float)SDL_GetTicks()/1000.0f;
    float elapsed = ticks - lastFrameTicks;
    lastFrameTicks = ticks;
    
    SDL_Event event;
    bool done = false;
    while (!done) {
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT || event.type == SDL_WINDOWEVENT_CLOSE) {
                done = true;
            }
        }
        glClear(GL_COLOR_BUFFER_BIT);
        
        /* left paddle */
//        program.setModelMatrix(ModelMatrix);
//        program.setViewMatrix(ViewMatrix);
//        program.setProjectionMatrix(ProjectionMatrix);
        
//        glBindTexture(GL_TEXTURE_2D, TextureName);
        
        float TexCoords[] = { 0.0, 1.0, 1.0,1.0, 1.0, 0.0, 0.0, 1.0, 1.0, 0.0, 0.0, 0.0};
        
        float Vertices[] =   {   -0.1, -0.5,
            0.1, -0.5,
            0.1, 0.5,
            -0.1, -0.5,
            0.1, 0.5,
            -0.1, 0.5};
        
        
        glVertexAttribPointer(program.positionAttribute, 2, GL_FLOAT, false, 0, Vertices );
        glEnableVertexAttribArray(program.positionAttribute);
        
        glVertexAttribPointer(program.texCoordAttribute, 2, GL_FLOAT, false, 0, TexCoords);
        glEnableVertexAttribArray(program.texCoordAttribute);
        glDrawArrays(GL_TRIANGLES, 0, 6);
        glDisableVertexAttribArray(program.positionAttribute);
        glDisableVertexAttribArray(program.texCoordAttribute);
        
        
        
        
        
        SDL_GL_SwapWindow(displayWindow);
    }
    
    SDL_Quit();
    return 0;
}
