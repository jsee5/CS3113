#ifdef _WINDOWS
#include <GL/glew.h>
#endif
#include <SDL.h>
#include <SDL_opengl.h>
#include <SDL_image.h>
#include "ShaderProgram.h"
#include "Matrix.h"

#ifdef _WINDOWS
#define RESOURCE_FOLDER ""
#else
#define RESOURCE_FOLDER "NYUCodebase.app/Contents/Resources/"
#endif

#define PI2RAD 3.1415926 / 180.0

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

SDL_Window* displayWindow;

//loading textures function
GLuint LoadTexture(const char *filePath) {
    int w,h,comp;
    unsigned char* image = stbi_load(filePath, &w, &h, &comp, STBI_rgb_alpha);
    if(image == NULL) {
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



void spin(Matrix& modelMatrix, float& elapsed, float angle){

    angle*= elapsed;
    modelMatrix.Rotate(angle * (3.1415926 / 180.0));
}


int main(int argc, char *argv[])
{
    //setup
    SDL_Init(SDL_INIT_VIDEO);
    displayWindow = SDL_CreateWindow("My Game", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 640, 360, SDL_WINDOW_OPENGL);
    SDL_GLContext context = SDL_GL_CreateContext(displayWindow);
    SDL_GL_MakeCurrent(displayWindow, context);

    glViewport(0, 0, 640, 360);
    
    ShaderProgram program(RESOURCE_FOLDER"vertex_textured.glsl", RESOURCE_FOLDER"fragment_textured.glsl");
    
   
    
    Matrix blackHole2_projectMatrix;
    Matrix blackHole2_modelMatrix;
    Matrix blackHole2_viewMatrix;
    
    Matrix darkrai_projectMatrix;
    Matrix darkrai_modelMatrix;
    Matrix darkrai_viewMatrix;
  
    Matrix blackHole1_projectMatrix;
    Matrix blackHole1_modelMatrix;
    Matrix blackHole1_viewMatrix;

    blackHole2_projectMatrix.setOrthoProjection(-5.5, 5.5, -2.0f, 2.0f, -1.0f, 1.0f);
    darkrai_projectMatrix.setOrthoProjection(-4.2, 4.2, -2.55f, 2.55f, -1.0f, 1.0f);
    blackHole1_projectMatrix.setOrthoProjection(-5.5, 5.5, -2.0f, 2.0f, -1.0f, 1.0f);
    
    //time init
    float lastFrameTicks = 0.0f;
    
    glUseProgram(program.programID);
    
    


    blackHole2_modelMatrix.Translate(4.25, 0.0, 0.0);
    blackHole1_modelMatrix.Translate(-0.4, 1.0, 0.0);

#ifdef _WINDOWS
    glewInit();
#endif
    
    GLuint darkrai_Texture = LoadTexture(RESOURCE_FOLDER"darkrai.png");
    GLuint blackHole_Texture = LoadTexture(RESOURCE_FOLDER"blackhole_2.jpg");
    GLuint blackHole_1_Texture = LoadTexture(RESOURCE_FOLDER"blackHole_1.jpg");
    
    SDL_Event event;
    bool done = false;
    while (!done) {
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT || event.type == SDL_WINDOWEVENT_CLOSE) {
                done = true;
            }
        }

        
        //black background
        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);
        
        
        //time-based
        float ticks = (float)SDL_GetTicks()/1000.0f;
        float elapsed = ticks - lastFrameTicks;
        lastFrameTicks = ticks;
        
   
        spin(blackHole2_modelMatrix, elapsed, 22.5f);
        spin(blackHole1_modelMatrix, elapsed, -35.0f);
        program.setModelMatrix(blackHole2_modelMatrix);
        program.setProjectionMatrix(blackHole2_projectMatrix);
        program.setViewMatrix(blackHole2_viewMatrix);
   
        //drawing black hole
        glBindTexture(GL_TEXTURE_2D, blackHole_Texture);
        
        float blackHole2_Vertices[] =   {-1.7, -1.7,
                                        1.7, -1.7,
                                        1.7, 1.7,
                                        -1.7, -1.7,
                                        1.7, 1.7,
                                        -1.7, 1.7};
        
        glVertexAttribPointer(program.positionAttribute, 2, GL_FLOAT, false, 0, blackHole2_Vertices);
        
        glEnableVertexAttribArray(program.positionAttribute);
        
        float blackHole2_TexCoords[] = { 0.0, 1.0, 1.0,1.0, 1.0, 0.0, 0.0, 1.0, 1.0, 0.0, 0.0, 0.0};
        glVertexAttribPointer(program.texCoordAttribute, 2, GL_FLOAT, false, 0, blackHole2_TexCoords);
        glEnableVertexAttribArray(program.texCoordAttribute);
        
        glDrawArrays(GL_TRIANGLES, 0, 6);
        
        glDisableVertexAttribArray(program.positionAttribute);
        glDisableVertexAttribArray(program.texCoordAttribute);

     
        
    
        
        program.setModelMatrix(darkrai_modelMatrix);
        program.setProjectionMatrix(darkrai_projectMatrix);
        program.setViewMatrix(darkrai_viewMatrix);
        
        //drawing darkrai
        glBindTexture(GL_TEXTURE_2D, darkrai_Texture);
        float darkrai_Vertices[] =   {  -3.9, -2.0,
                                        -2.0,-2.0,
                                        -2.0, 0.0,
                                        -3.9, -2.0,
                                        -2.0, 0.0,
                                        -3.9, 0.0};
        
        glVertexAttribPointer(program.positionAttribute, 2, GL_FLOAT, false, 0, darkrai_Vertices);
        
        glEnableVertexAttribArray(program.positionAttribute);
        
        float darkrai_TexCoords[] = {   0.0, 1.0, 1.0, 1.0, 1.0, 0.0, 0.0, 1.0, 1.0, 0.0, 0.0, 0.0};
        
        glVertexAttribPointer(program.texCoordAttribute, 2, GL_FLOAT, false, 0, darkrai_TexCoords);
        glEnableVertexAttribArray(program.texCoordAttribute);
        
        glDrawArrays(GL_TRIANGLES, 0, 6);
        
        glDisableVertexAttribArray(program.positionAttribute);
        glDisableVertexAttribArray(program.texCoordAttribute);
        
    
        program.setModelMatrix(blackHole1_modelMatrix);
        program.setProjectionMatrix(blackHole1_projectMatrix);
        program.setViewMatrix(blackHole1_viewMatrix);
        
        //drawing blackHole_1
        glBindTexture(GL_TEXTURE_2D, blackHole_1_Texture);
        float blackHole1_Vertices[] =   {   -1.25, -1.25,
                                            1.25,-1.25,
                                            1.25,1.25,
                                            -1.25, -1.25,
                                            1.25, 1.25,
                                            -1.25, 1.25};
        
        glVertexAttribPointer(program.positionAttribute, 2, GL_FLOAT, false, 0, blackHole1_Vertices);
        
        glEnableVertexAttribArray(program.positionAttribute);
        
        float blackHole1_TexCoords[] = {   0.0, 1.0, 1.0,1.0, 1.0, 0.0,0.0, 1.0, 1.0, 0.0, 0.0, 0.0};
        
        glVertexAttribPointer(program.texCoordAttribute, 2, GL_FLOAT, false, 0, blackHole1_TexCoords);
        glEnableVertexAttribArray(program.texCoordAttribute);
        
        glDrawArrays(GL_TRIANGLES, 0, 6);
        
        glDisableVertexAttribArray(program.positionAttribute);
        glDisableVertexAttribArray(program.texCoordAttribute);
        
        
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    
        SDL_GL_SwapWindow(displayWindow);
    }

    SDL_Quit();
    return 0;
}
