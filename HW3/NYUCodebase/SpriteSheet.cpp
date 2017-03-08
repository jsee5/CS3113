#include "SpriteSheet.h"
#include <SDL.h>
#include <SDL_opengl.h>
#include <SDL_image.h>


#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"


SpriteSheet::SpriteSheet(){}

SpriteSheet SpriteSheet::getSprite(int index, int x_count, int y_count){
    
    SpriteSheet sprite;
    sprite.textureID = textureID;
    sprite.u = (float)(((int)index) % x_count) / (float) x_count;
    sprite.v = (float)(((int)index) / x_count) / (float) y_count;
    sprite.width = 1.0/(float)x_count;
    sprite.height = 1.0/(float)y_count;

    return sprite;
}


void SpriteSheet::LoadTexture(const char* filePath){
    
    int w,h,comp;
    unsigned char* image = stbi_load(filePath, &w, &h, &comp, STBI_rgb_alpha);
    if(image == NULL) {
        std::cout << "Unable to load image. Make sure the path is correct\n";
        assert(false);
    }

    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_2D, textureID);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, w, h, 0, GL_RGBA, GL_UNSIGNED_BYTE, image);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    stbi_image_free(image);
}

void SpriteSheet::Draw(ShaderProgram *program) {
    glBindTexture(GL_TEXTURE_2D, textureID);
    GLfloat TexCoords[] = {
        u, v+height,
        u+width, v,
        u, v,
        u+width, v,
        u, v+height,
        u+width, v+height
    };

    float vertices[] = {-0.5f, -0.5f,
                        0.5f, 0.5f,
                        -0.5f, 0.5f,
                        0.5f, 0.5f,
                        -0.5f,-0.5f,
                        0.5f, -0.5f};

    glVertexAttribPointer(program->positionAttribute, 2, GL_FLOAT, false, 0, vertices );
    glEnableVertexAttribArray(program->positionAttribute);
    glVertexAttribPointer(program->texCoordAttribute, 2, GL_FLOAT, false, 0, TexCoords);
    glEnableVertexAttribArray(program->texCoordAttribute);
    glDrawArrays(GL_TRIANGLES, 0, 6);
    glDisableVertexAttribArray(program->positionAttribute);
    glDisableVertexAttribArray(program->texCoordAttribute);
}


