#include "Vector.h"
#include <cmath>

Vector::Vector():x(0), y(0), z(0){}

Vector::Vector(float x, float y, float z): x(x), y(y), z(z) {}

Vector::Vector(float x, float y): x(x), y(y) {}

Vector::Vector(Vector const &vector){
    x = vector.x;
    y = vector.y;
    z = vector.z;
}


void Vector::normalize(){
    if(length() == 0) return;
    
    x = x/length();
    y = y/length();
    z = z/length();
}


//pythagorean theorem
float Vector::length()const{
    return std::sqrt((x*x + y*y));
}

Vector& Vector::operator=(const Vector other){
    x = other.x;
    y = other.y;
    z = other.z;
    return *this;
}

float Vector::dot(Vector& other)const{
    return x * other.x + y * other.y;
}

void Vector::reset(){
    x = 0.0f;
    y = 0.0f;
    z = 0.0f;
}

