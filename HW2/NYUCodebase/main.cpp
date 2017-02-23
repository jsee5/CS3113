#ifdef _WINDOWS
#include <GL/glew.h>
#endif
#include <SDL.h>
#include <SDL_opengl.h>
#include <SDL_image.h>
#include "ShaderProgram.h"
#include "Matrix.h"
#include "Entity.h"
#include <math.h>

#ifdef _WINDOWS
#define RESOURCE_FOLDER ""
#else
#define RESOURCE_FOLDER "NYUCodebase.app/Contents/Resources/"
#endif

#define PI 3.1415926

SDL_Window* displayWindow;


//setup
void setup(){
    SDL_Init(SDL_INIT_VIDEO);
    displayWindow = SDL_CreateWindow("Pong", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 640, 360, SDL_WINDOW_OPENGL);
    SDL_GLContext context = SDL_GL_CreateContext(displayWindow);
    SDL_GL_MakeCurrent(displayWindow, context);

    glViewport(0, 0, 640, 360);

}

bool isRestrict_yPosition(Entity entity, float y_Units){

    bool isRestrict = true;
    float newPos = entity.get_yPosition() + y_Units;
    if (fabs(newPos) < 1.5){
        isRestrict = false;
    }
//    std::cout << paddle.get_yPosition() << " " << fabs(paddle.get_yPosition()) << std::endl;

    return isRestrict;
}

bool isRestrict_xPosition(Entity entity, float x_Units){

    bool isRestrict = true;
    float newPos = entity.get_yPosition() + x_Units;
    if (fabs(newPos) < 1.5){
        isRestrict = false;
    }
    //    std::cout << paddle.get_yPosition() << " " << fabs(paddle.get_xPosition()) << std::endl;

    return isRestrict;
}

bool isOffWall(Entity ball){
    bool bounce = false;
    if (fabs(ball.get_yPosition()) >= 2.1){
        bounce = true;
    }
    return bounce;
}
bool isOffLeftPaddle(Entity ball, Entity leftPaddle){
    bool bounce = false;
    //left paddle check
    if(ball.get_xPosition() < -4.0){
        if(ball.get_yPosition() + 0.2 >= fabs(leftPaddle.get_yPosition())){
            bounce = true;
        }
    }
    return bounce;
}

bool isOver(Entity ball, Entity leftPaddle, Entity rightPaddle){
    bool isOver = false;

    if (ball.get_xPosition() < leftPaddle.get_xPosition()){
        isOver = true;
    }
    else if(ball.get_xPosition() > rightPaddle.get_xPosition()){
        isOver = true;
    }
    return isOver;
}

bool isOffRightPaddle(Entity ball, Entity rightPaddle){
    bool bounce = false;
    //right paddle check
    if(ball.get_xPosition() > 4.0){

        if(ball.get_yPosition() + 0.2 >= fabs(rightPaddle.get_yPosition())){
            bounce = true;
        }
    }
    return bounce;
}


int main(int argc, char *argv[])
{

    setup();

    ShaderProgram program(RESOURCE_FOLDER"vertex_textured.glsl", RESOURCE_FOLDER"fragment_textured.glsl");
    glUseProgram(program.programID);

    //time init
    float lastFrameTicks = 0.0f;



    /* setting up entities */
    Entity leftPaddle;
    GLuint leftPaddle_Texture = leftPaddle.LoadTexture(RESOURCE_FOLDER"paddleBlu.png");
    Entity rightPaddle;

    GLuint rightPaddle_Texture = rightPaddle.LoadTexture(RESOURCE_FOLDER"paddleRed.png");
    Entity ball;
    GLuint ball_Texture = ball.LoadTexture(RESOURCE_FOLDER"pokeball.png");

    Entity line;
    GLuint line_Texture = line.LoadTexture(RESOURCE_FOLDER"line.png");

    Matrix ProjectionMatrix;
    Matrix ViewMatrix;

    ProjectionMatrix.setOrthoProjection(-5.0, 5.0, -2.0f, 2.0f, -1.0f, 1.0f);

    leftPaddle.setPosition(-4.2, 0.0);
    rightPaddle.setPosition(4.2, 0.0);
    ball.setPosition(0.0, 0.0);


#ifdef _WINDOWS
    glewInit();
#endif



    SDL_Event event;

    bool done = false;
    bool begin = false;
    bool restart = false;
    float speed = 1.7;
    ball.setAngle(PI/4);

    while (!done) {

        /* time tracking */
        float ticks = (float)SDL_GetTicks()/1000.0f;
        float elapsed = ticks - lastFrameTicks;
        lastFrameTicks = ticks;
        const Uint8 *player_1 = SDL_GetKeyboardState(NULL);
        const Uint8 *player_2 = SDL_GetKeyboardState(NULL);
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT || event.type == SDL_WINDOWEVENT_CLOSE) {
                done = true;
            }
            else if(event.type == SDL_KEYDOWN){
                if (event.key.keysym.scancode == SDL_SCANCODE_SPACE){
                    begin = true;
                }
                else if(event.key.keysym.scancode == SDL_SCANCODE_ESCAPE){
                    restart = true;
                }
            }
        }

//        std::cout<< ball.get_xPosition() << " " << ball.get_yPosition() << std::endl;

     /* Player 1 movement */

        if(player_1[SDL_SCANCODE_W]){
            if (isRestrict_yPosition(leftPaddle, 0.1) == false){
                leftPaddle.translate(0.0, 0.1);
            }
        }
        else if(player_1[SDL_SCANCODE_S]){
            if (isRestrict_yPosition(leftPaddle, -0.1) == false){
                leftPaddle.translate(0.0, -0.1);
            }
        }

        /* player 2 movement */
        if(player_2[SDL_SCANCODE_UP]){
            if (isRestrict_yPosition(rightPaddle, 0.1) == false){
                rightPaddle.translate(0.0, 0.1);
            }
        }
        else if(player_2[SDL_SCANCODE_DOWN]){
            if (isRestrict_yPosition(rightPaddle, -0.1) == false){
                rightPaddle.translate(0.0, -0.1);
            }
        }

        glClearColor(0.2f,0.1f, 0.2f, 0.2f);
        glClear(GL_COLOR_BUFFER_BIT);



        if (restart == true){
            leftPaddle.reset();
            rightPaddle.reset();
            ball.reset();
            restart = false;
            begin = false;

        }

        if(begin == true){
            if(isOver(ball, leftPaddle, rightPaddle) == true){
                restart = true;
                begin = false;
            }
            else if(isOffLeftPaddle(ball, leftPaddle)==true){
                ball.setAngle(PI/2 + ball.getAngle());
                ball.translate(speed*cos(ball.getAngle())*elapsed, speed*sin(ball.getAngle())*elapsed);

            }
            else if(isOffRightPaddle(ball, rightPaddle)==true){
                ball.setAngle(PI/2 + ball.getAngle());
                ball.translate(speed*cos(ball.getAngle())*elapsed, speed*sin(ball.getAngle())*elapsed);
            }
            else if(isOffWall(ball) == true){
                if(ball.get_xPosition() > 0){
                    ball.setAngle(PI/2 + ball.getAngle());
                    ball.translate(speed*cos(ball.getAngle())*elapsed, speed*sin(ball.getAngle())*elapsed);
                }
                else{
                    ball.setAngle(PI/2 + ball.getAngle());
                    ball.translate(speed*cos(ball.getAngle())*elapsed, speed*sin(ball.getAngle())*elapsed);

                }

            }

            else
                ball.translate(speed*cos(ball.getAngle())*elapsed, speed*sin(ball.getAngle())*elapsed);
            }






        /* left paddle */
        program.setModelMatrix(leftPaddle.getModelMatrix());
        program.setViewMatrix(ViewMatrix);
        program.setProjectionMatrix(ProjectionMatrix);

        glBindTexture(GL_TEXTURE_2D, leftPaddle_Texture);

        float TexCoords[] = { 0.0, 1.0, 1.0,1.0, 1.0, 0.0, 0.0, 1.0, 1.0, 0.0, 0.0, 0.0};

        float leftPaddle_Vertices[] =   {   -0.1, -0.5,
                                        0.1, -0.5,
                                        0.1, 0.5,
                                        -0.1, -0.5,
                                        0.1, 0.5,
                                        -0.1, 0.5};


        glVertexAttribPointer(program.positionAttribute, 2, GL_FLOAT, false, 0, leftPaddle_Vertices );
        glEnableVertexAttribArray(program.positionAttribute);

        glVertexAttribPointer(program.texCoordAttribute, 2, GL_FLOAT, false, 0, TexCoords);
        glEnableVertexAttribArray(program.texCoordAttribute);
        glDrawArrays(GL_TRIANGLES, 0, 6);
        glDisableVertexAttribArray(program.positionAttribute);
        glDisableVertexAttribArray(program.texCoordAttribute);


        /* right paddle */
        program.setModelMatrix(rightPaddle.getModelMatrix());
        program.setViewMatrix(ViewMatrix);
        program.setProjectionMatrix(ProjectionMatrix);

        glBindTexture(GL_TEXTURE_2D, rightPaddle_Texture);

        float rightPaddle_Vertices[] =   {   -0.1, -0.5,
                                        0.1, -0.5,
                                        0.1, 0.5,
                                        -0.1, -0.5,
                                        0.1, 0.5,
                                        -0.1, 0.5};


        glVertexAttribPointer(program.positionAttribute, 2, GL_FLOAT, false, 0, rightPaddle_Vertices );
        glEnableVertexAttribArray(program.positionAttribute);

        glVertexAttribPointer(program.texCoordAttribute, 2, GL_FLOAT, false, 0, TexCoords);
        glEnableVertexAttribArray(program.texCoordAttribute);
        glDrawArrays(GL_TRIANGLES, 0, 6);
        glDisableVertexAttribArray(program.positionAttribute);
        glDisableVertexAttribArray(program.texCoordAttribute);



        /* ball */
        program.setModelMatrix(ball.getModelMatrix());
        program.setViewMatrix(ViewMatrix);
        program.setProjectionMatrix(ProjectionMatrix);

        glBindTexture(GL_TEXTURE_2D, ball_Texture);

        float ball_Vertices[] =   {  -0.15, -0.15,
                                    0.15, -0.15,
                                    0.15, 0.15,
                                    -0.15, -0.15,
                                    0.15, 0.15,
                                    -0.15, 0.15};


        glVertexAttribPointer(program.positionAttribute, 2, GL_FLOAT, false, 0, ball_Vertices );
        glEnableVertexAttribArray(program.positionAttribute);

        glVertexAttribPointer(program.texCoordAttribute, 2, GL_FLOAT, false, 0, TexCoords);
        glEnableVertexAttribArray(program.texCoordAttribute);
        glDrawArrays(GL_TRIANGLES, 0, 6);
        glDisableVertexAttribArray(program.positionAttribute);
        glDisableVertexAttribArray(program.texCoordAttribute);


        program.setModelMatrix(line.getModelMatrix());
        program.setViewMatrix(ViewMatrix);
        program.setProjectionMatrix(ProjectionMatrix);

        glBindTexture(GL_TEXTURE_2D, line_Texture);

        float line_Vertices[] =   {   -0.1, -0.5,
            0.1, -0.05,
            0.1, 0.05,
            -0.1, -0.05,
            0.1, 0.50,
            -0.1, 0.05};

        glVertexAttribPointer(program.positionAttribute, 2, GL_FLOAT, false, 0, line_Vertices );
        glEnableVertexAttribArray(program.positionAttribute);

        glVertexAttribPointer(program.texCoordAttribute, 2, GL_FLOAT, false, 0, TexCoords);
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
