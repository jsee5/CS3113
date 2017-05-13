#pragma once

#ifndef Vector_h
#define Vector_h
#include <cmath>

class Vector{
public:
    Vector();
    
    Vector(float x, float y, float z);
    Vector(float x, float y);
    Vector(Vector const &);
    Vector& operator=(const Vector other);
    float dot(Vector&)const;
    void normalize();
    void reset();
    float length()const;
    float x, y, z;
};



#endif /* Vector_h */
