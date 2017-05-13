#include "SpriteSheet.h"

SpriteSheet::SpriteSheet(){}

SpriteSheet::SpriteSheet(GLuint textureID, float x, float y,float width, float height, float sheet_width, float sheet_height, float size): textureID(textureID), u(x/sheet_width), v(y/sheet_height), width(width/sheet_width), height(height/sheet_height), aspect(width/height), size(size){}


void SpriteSheet::draw(ShaderProgram*& program) {
    
    
    glBindTexture(GL_TEXTURE_2D, textureID);
    GLfloat texCoords[] = {
        u,                  v + height,
        u  +width,   v,
        u,                  v,
        u  +width,   v,
        u,                  v + height,
        u  +width,   v + height
    };

    
    float vertices[] = {
        -0.5f * size*aspect,    -0.5f * size,
        0.5f * size*aspect,     0.5f * size,
        -0.5f * size*aspect,    0.5f * size,
        0.5f * size*aspect,     0.5f * size,
        -0.5f * size*aspect,    -0.5f * size,
        0.5f * size*aspect,     -0.5f * size
    };
    
    glVertexAttribPointer(program->positionAttribute, 2, GL_FLOAT, false, 0, vertices);
    glEnableVertexAttribArray(program->positionAttribute);
    glVertexAttribPointer(program->texCoordAttribute, 2, GL_FLOAT, false, 0, texCoords);
    glEnableVertexAttribArray(program->texCoordAttribute);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glDrawArrays(GL_TRIANGLES, 0, 6);
    glDisableVertexAttribArray(program->positionAttribute);
    glDisableVertexAttribArray(program->texCoordAttribute);
    
    
}

void SpriteSheet::uniform_draw(ShaderProgram *program) {
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
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glDrawArrays(GL_TRIANGLES, 0, 6);
    glDisableVertexAttribArray(program->positionAttribute);
    glDisableVertexAttribArray(program->texCoordAttribute);
}
