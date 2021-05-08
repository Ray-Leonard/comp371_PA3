//
//  functions.h
//  OpenGL
//
//  Created by Jiarui Li on 3/24/21.
//  Copyright © 2021 Jiarui Li. All rights reserved.
//

#ifndef functions_h
#define functions_h

#define GLEW_STATIC
#include <GL/glew.h>
#include <iostream>
#include "shader.h"
// GLFW3
#include <GLFW/glfw3.h>
// GLEW
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#include <random>
using namespace glm;

const GLint WIDTH = 1024, HEIGHT = 768;
const int GRID_SCALE = 128;
const float gridUnit = 0.2f;
const int AXIS_LENGTH = 7;
// offsets for three types of drawings
const int GRID_OFFSET = 0;
const int GRID_COUNT = 2;
const int AXIS_OFFSET = 2;
const int AXIS_COUNT = 6;
const int TRIANGLE_OFFSET = 8;
const int TRIANGLE_COUNT = 36;
const int GRID_CUBE_OFFSET = 44;
const int GRID_CUBE_COUNT = 36;

// create a model selector to select and modify translation, scaling, and rotation data
struct modelSelector
{
    int currentModel;
    // rotation angles for 5 sets of model
    float modelRotationAngle[5];
    // scaling factors for 5 sets of model
    float modelScalingFactor[5];
    // translation positions for 5 sets of model
    glm::vec3 modelTranslationPos[5];
    // shear degrees for 5 sets of model
    float shearDegreeX[5];
    float shearDegreeZ[5];
    
    void randomPosition()
    {
        for(int i = 0; i < 5; ++i)
        {
            // generate random numbers
            float angle = rand() % 180;
            float x = rand() % 90 - 45;
            float z = rand() % 45 + 5;
            
            modelRotationAngle[i] = angle;
            modelTranslationPos[i] = glm::vec3(x * gridUnit, 0.0f, z * gridUnit);
        }
    }
    
    // restore to default value
    void resetModels()
    {
        float p = (GRID_SCALE/2)*gridUnit;  // radius of imaginary circle

        currentModel = 1;

        // for the 1st model
        modelRotationAngle[0] = -25.0f;
        modelScalingFactor[0] = 1.0f;
        modelTranslationPos[0] = vec3(10*gridUnit - p, 0.0f, -20*gridUnit);
        shearDegreeX[0] = 90.0f;
        shearDegreeZ[0] = 90.0f;
        
        // for the 2nd model
        modelRotationAngle[1] = 0.0f;
        modelScalingFactor[1] = 1.0f;
        modelTranslationPos[1] = vec3(0.0f);
        shearDegreeX[1] = 90.0f;
        shearDegreeZ[1] = 90.0f;
        
        // for 3rd model
        modelRotationAngle[2] = 25.0f;
        modelScalingFactor[2] = 1.0f;
        modelTranslationPos[2] = vec3(p-10*gridUnit,0.0f,-20*gridUnit);
        shearDegreeX[2] = 90.0f;
        shearDegreeZ[2] = 90.0f;
        
        // 4th model
        modelRotationAngle[3] = 45.0f;
        modelScalingFactor[3] = 1.0f;
        modelTranslationPos[3] = vec3(25*gridUnit - p, 0.0f, 20*gridUnit);
        shearDegreeX[3] = 90.0f;
        shearDegreeZ[3] = 90.0f;
        
        // 5th
        modelRotationAngle[4] = -45.0f;
        modelScalingFactor[4] = 1.0f;
        modelTranslationPos[4] = vec3(p-25*gridUnit,0.0f,15*gridUnit);
        shearDegreeX[4] = 90.0f;
        shearDegreeZ[4] = 90.0f;
    }
    
    // mutator functions
    void setModelRotationAngle(float angle)
    {
        modelRotationAngle[currentModel-1] = angle;
    }
    
    void setModelScalingFactor(float factor)
    {
        modelScalingFactor[currentModel-1] = factor>0?factor:0;
    }
    
    void setModelTranslationPos(glm::vec3 pos)
    {
        modelTranslationPos[currentModel-1] = pos;
    }
    void setShearDegreeX(float degree)
    {
        shearDegreeX[currentModel - 1] = degree;
    }

    void setShearDegreeZ(float degree)
    {
        shearDegreeZ[currentModel - 1] = degree;
    }
    
    // accessor functions
    float getModelRotationAngle(int index)
    {
        return modelRotationAngle[index - 1];
    }
    
    float getModelScalingFactor(int index)
    {
        return modelScalingFactor[index - 1];
    }
    
    glm::vec3 getModelTranslationPos(int index)
    {
        return modelTranslationPos[index - 1];
    }
    
    float getShearDegreeX(int index)
    {
        return shearDegreeX[index - 1];
    }

    float getShearDegreeZ(int index)
    {
        return shearDegreeZ[index - 1];
    }
};

struct TexturedColoredVertex
{
    TexturedColoredVertex(vec3 _position, vec3 _color, vec2 _uv , vec3 _normal)
    : position(_position), color(_color), uv(_uv), normal(_normal){}

    vec3 position;
    vec3 color;
    vec2 uv;
    vec3 normal;
};

GLuint createVertexBufferObject()
{
    TexturedColoredVertex texturedVertexArray[] = {  // position,                            color
        // unit line for grid
        TexturedColoredVertex(vec3(-1.0f, 0.0f, 0.0f), vec3(1.0f, 1.0f, 0.0f), vec2(0.0f, 0.0f), vec3(0.0f,0.0f,0.0f)),
        TexturedColoredVertex(vec3(1.0f, 0.0f, 0.0f), vec3(1.0f, 1.0f, 0.0f), vec2(0.0f, 0.0f), vec3(0.0f,0.0f,0.0f)),
        
        // x axis - red
        TexturedColoredVertex(vec3(0.0f, 0.0f, 0.0f), vec3(1.0f, 0.0f, 0.0f), vec2(0.0f, 0.0f), vec3(0.0f, 0.0f, 0.0f)),
        TexturedColoredVertex(vec3(AXIS_LENGTH*gridUnit, 0.0f, 0.0f), vec3(1.0f, 0.0f, 0.0f), vec2(0.0f, 0.0f), vec3(0.0f,0.0f,0.0f)),
        
        // y axis - green
        TexturedColoredVertex(vec3(0.0f, 0.0f, 0.0f), vec3(0.0f, 1.0f, 0.0f), vec2(0.0f, 0.0f), vec3(0.0f, 0.0f, 0.0f)),
        TexturedColoredVertex(vec3(0.0f, AXIS_LENGTH*gridUnit, 0.0f), vec3(0.0f, 1.0f, 0.0f), vec2(0.0f, 0.0f), vec3(0.0f,0.0f,0.0f)),
        
        // z axis - Blue
        TexturedColoredVertex(vec3(0.0f, 0.0f, 0.0f), vec3(0.0f, 0.0f, 1.0f), vec2(0.0f, 0.0f), vec3(0.0f, 0.0f, 0.0f)),
        TexturedColoredVertex(vec3(0.0f, 0.0f, AXIS_LENGTH*gridUnit), vec3(0.0f, 0.0f, 1.0f), vec2(0.0f, 0.0f), vec3(0.0f,0.0f,0.0f)),
        
        // 12 triangles - for regular mesh===========================================
        TexturedColoredVertex(vec3(-0.5f,-0.5f,-0.5f), vec3(1.0f, 0.0f, 0.0f), vec2(0.0f, 0.0f),vec3(-1.0f,0.0f,0.0f)), //left - red
        TexturedColoredVertex(vec3(-0.5f,-0.5f, 0.5f), vec3(1.0f, 0.0f, 0.0f), vec2(0.0f, 1.0f),vec3(-1.0f,0.0f,0.0f)),
        TexturedColoredVertex(vec3(-0.5f, 0.5f, 0.5f), vec3(1.0f, 0.0f, 0.0f), vec2(1.0f, 1.0f),vec3(-1.0f,0.0f,0.0f)),

        TexturedColoredVertex(vec3(-0.5f,-0.5f,-0.5f), vec3(1.0f, 0.0f, 0.0f), vec2(0.0f, 0.0f),vec3(-1.0f,0.0f,0.0f)),
        TexturedColoredVertex(vec3(-0.5f, 0.5f, 0.5f), vec3(1.0f, 0.0f, 0.0f), vec2(1.0f, 1.0f),vec3(-1.0f,0.0f,0.0f)),
        TexturedColoredVertex(vec3(-0.5f, 0.5f,-0.5f), vec3(1.0f, 0.0f, 0.0f), vec2(1.0f, 0.0f),vec3(-1.0f,0.0f,0.0f)),

        TexturedColoredVertex(vec3( 0.5f, 0.5f,-0.5f), vec3(0.0f, 0.0f, 1.0f), vec2(1.0f, 1.0f),vec3(0.0f,0.0f,-1.0f)), // far - blue
        TexturedColoredVertex(vec3(-0.5f,-0.5f,-0.5f), vec3(0.0f, 0.0f, 1.0f), vec2(0.0f, 0.0f),vec3(0.0f,0.0f,-1.0f)),
        TexturedColoredVertex(vec3(-0.5f, 0.5f,-0.5f), vec3(0.0f, 0.0f, 1.0f), vec2(0.0f, 1.0f),vec3(0.0f,0.0f,-1.0f)),

        TexturedColoredVertex(vec3( 0.5f, 0.5f,-0.5f), vec3(0.0f, 0.0f, 1.0f), vec2(1.0f, 1.0f),vec3(0.0f,0.0f,-1.0f)),
        TexturedColoredVertex(vec3( 0.5f,-0.5f,-0.5f), vec3(0.0f, 0.0f, 1.0f), vec2(1.0f, 0.0f),vec3(0.0f,0.0f,-1.0f)),
        TexturedColoredVertex(vec3(-0.5f,-0.5f,-0.5f), vec3(0.0f, 0.0f, 1.0f), vec2(0.0f, 0.0f),vec3(0.0f,0.0f,-1.0f)),

        TexturedColoredVertex(vec3( 0.5f,-0.5f, 0.5f), vec3(0.0f, 1.0f, 1.0f), vec2(1.0f, 1.0f),vec3(0.0f,-1.0f,0.0f)), // bottom - turquoise
        TexturedColoredVertex(vec3(-0.5f,-0.5f,-0.5f), vec3(0.0f, 1.0f, 1.0f), vec2(0.0f, 0.0f),vec3(0.0f,-1.0f,0.0f)),
        TexturedColoredVertex(vec3( 0.5f,-0.5f,-0.5f), vec3(0.0f, 1.0f, 1.0f), vec2(1.0f, 0.0f),vec3(0.0f,-1.0f,0.0f)),

        TexturedColoredVertex(vec3( 0.5f,-0.5f, 0.5f), vec3(0.0f, 1.0f, 1.0f), vec2(1.0f, 1.0f),vec3(0.0f,-1.0f,0.0f)),
        TexturedColoredVertex(vec3(-0.5f,-0.5f, 0.5f), vec3(0.0f, 1.0f, 1.0f), vec2(0.0f, 1.0f),vec3(0.0f,-1.0f,0.0f)),
        TexturedColoredVertex(vec3(-0.5f,-0.5f,-0.5f), vec3(0.0f, 1.0f, 1.0f), vec2(0.0f, 0.0f),vec3(0.0f,-1.0f,0.0f)),

        TexturedColoredVertex(vec3(-0.5f, 0.5f, 0.5f), vec3(0.0f, 1.0f, 0.0f), vec2(0.0f, 1.0f),vec3(0.0f,0.0f,1.0f)), // near - green
        TexturedColoredVertex(vec3(-0.5f,-0.5f, 0.5f), vec3(0.0f, 1.0f, 0.0f), vec2(0.0f, 0.0f),vec3(0.0f,0.0f,1.0f)),
        TexturedColoredVertex(vec3( 0.5f,-0.5f, 0.5f), vec3(0.0f, 1.0f, 0.0f), vec2(1.0f, 0.0f),vec3(0.0f,0.0f,1.0f)),

        TexturedColoredVertex(vec3( 0.5f, 0.5f, 0.5f), vec3(0.0f, 1.0f, 0.0f), vec2(1.0f, 1.0f),vec3(0.0f,0.0f,1.0f)),
        TexturedColoredVertex(vec3(-0.5f, 0.5f, 0.5f), vec3(0.0f, 1.0f, 0.0f), vec2(0.0f, 1.0f),vec3(0.0f,0.0f,1.0f)),
        TexturedColoredVertex(vec3( 0.5f,-0.5f, 0.5f), vec3(0.0f, 1.0f, 0.0f), vec2(1.0f, 0.0f),vec3(0.0f,0.0f,1.0f)),

        TexturedColoredVertex(vec3( 0.5f, 0.5f, 0.5f), vec3(1.0f, 0.0f, 1.0f), vec2(1.0f, 1.0f),vec3(1.0f,0.0f,0.0f)), // right - purple
        TexturedColoredVertex(vec3( 0.5f,-0.5f,-0.5f), vec3(1.0f, 0.0f, 1.0f), vec2(0.0f, 0.0f),vec3(1.0f,0.0f,0.0f)),
        TexturedColoredVertex(vec3( 0.5f, 0.5f,-0.5f), vec3(1.0f, 0.0f, 1.0f), vec2(1.0f, 0.0f),vec3(1.0f,0.0f,0.0f)),

        TexturedColoredVertex(vec3( 0.5f,-0.5f,-0.5f), vec3(1.0f, 0.0f, 1.0f), vec2(0.0f, 0.0f),vec3(1.0f,0.0f,0.0f)),
        TexturedColoredVertex(vec3( 0.5f, 0.5f, 0.5f), vec3(1.0f, 0.0f, 1.0f), vec2(1.0f, 1.0f),vec3(1.0f,0.0f,0.0f)),
        TexturedColoredVertex(vec3( 0.5f,-0.5f, 0.5f), vec3(1.0f, 0.0f, 1.0f), vec2(0.0f, 1.0f),vec3(1.0f,0.0f,0.0f)),

        TexturedColoredVertex(vec3( 0.5f, 0.5f, 0.5f), vec3(1.0f, 1.0f, 0.0f), vec2(1.0f, 1.0f),vec3(0.0f,1.0f,0.0f)), // top - yellow
        TexturedColoredVertex(vec3( 0.5f, 0.5f,-0.5f), vec3(1.0f, 1.0f, 0.0f), vec2(1.0f, 0.0f),vec3(0.0f,1.0f,0.0f)),
        TexturedColoredVertex(vec3(-0.5f, 0.5f,-0.5f), vec3(1.0f, 1.0f, 0.0f), vec2(0.0f, 0.0f),vec3(0.0f,1.0f,0.0f)),

        TexturedColoredVertex(vec3( 0.5f, 0.5f, 0.5f), vec3(1.0f, 1.0f, 0.0f), vec2(1.0f, 1.0f),vec3(0.0f,1.0f,0.0f)),
        TexturedColoredVertex(vec3(-0.5f, 0.5f,-0.5f), vec3(1.0f, 1.0f, 0.0f), vec2(0.0f, 0.0f),vec3(0.0f,1.0f,0.0f)),
        TexturedColoredVertex(vec3(-0.5f, 0.5f, 0.5f), vec3(1.0f, 1.0f, 0.0f), vec2(0.0f, 1.0f),vec3(0.0f,1.0f,0.0f)),
        
        // 12 triangles for tile mesh==========================================
        TexturedColoredVertex(vec3(-0.5f,-0.5f,-0.5f), vec3(1.0f, 0.0f, 0.0f), vec2(0.0f, 0.0f),vec3(0.0f,-1.0f,0.0f)), //left - red
        TexturedColoredVertex(vec3(-0.5f,-0.5f, 0.5f), vec3(1.0f, 0.0f, 0.0f), vec2(0.0f, 16.0f),vec3(0.0f,-1.0f,0.0f)),
        TexturedColoredVertex(vec3(-0.5f, 0.5f, 0.5f), vec3(1.0f, 0.0f, 0.0f), vec2(16.0f, 16.0f),vec3(0.0f,1.0f,0.0f)),

        TexturedColoredVertex(vec3(-0.5f,-0.5f,-0.5f), vec3(1.0f, 0.0f, 0.0f), vec2(0.0f, 0.0f),vec3(0.0f,-1.0f,0.0f)),
        TexturedColoredVertex(vec3(-0.5f, 0.5f, 0.5f), vec3(1.0f, 0.0f, 0.0f), vec2(16.0f, 16.0f),vec3(0.0f,-1.0f,0.0f)),
        TexturedColoredVertex(vec3(-0.5f, 0.5f,-0.5f), vec3(1.0f, 0.0f, 0.0f), vec2(16.0f, 0.0f),vec3(0.0f,-1.0f,0.0f)),

        TexturedColoredVertex(vec3( 0.5f, 0.5f,-0.5f), vec3(0.0f, 0.0f, 1.0f), vec2(16.0f, 16.0f),vec3(0.0f,0.0f,-1.0f)), // far - blue
        TexturedColoredVertex(vec3(-0.5f,-0.5f,-0.5f), vec3(0.0f, 0.0f, 1.0f), vec2(0.0f, 0.0f),vec3(0.0f,0.0f,-1.0f)),
        TexturedColoredVertex(vec3(-0.5f, 0.5f,-0.5f), vec3(0.0f, 0.0f, 1.0f), vec2(0.0f, 16.0f),vec3(0.0f,0.0f,-1.0f)),

        TexturedColoredVertex(vec3( 0.5f, 0.5f,-0.5f), vec3(0.0f, 0.0f, 1.0f), vec2(16.0f, 16.0f),vec3(0.0f,0.0f,-1.0f)),
        TexturedColoredVertex(vec3( 0.5f,-0.5f,-0.5f), vec3(0.0f, 0.0f, 1.0f), vec2(16.0f, 0.0f),vec3(0.0f,0.0f,-1.0f)),
        TexturedColoredVertex(vec3(-0.5f,-0.5f,-0.5f), vec3(0.0f, 0.0f, 1.0f), vec2(0.0f, 0.0f),vec3(0.0f,0.0f,-1.0f)),

        TexturedColoredVertex(vec3( 0.5f,-0.5f, 0.5f), vec3(0.0f, 1.0f, 1.0f), vec2(16.0f, 16.0f),vec3(0.0f,-1.0f,0.0f)), // bottom - turquoise
        TexturedColoredVertex(vec3(-0.5f,-0.5f,-0.5f), vec3(0.0f, 1.0f, 1.0f), vec2(0.0f, 0.0f),vec3(0.0f,-1.0f,0.0f)),
        TexturedColoredVertex(vec3( 0.5f,-0.5f,-0.5f), vec3(0.0f, 1.0f, 1.0f), vec2(16.0f, 0.0f),vec3(0.0f,-1.0f,0.0f)),

        TexturedColoredVertex(vec3( 0.5f,-0.5f, 0.5f), vec3(0.0f, 1.0f, 1.0f), vec2(16.0f, 16.0f),vec3(0.0f,-1.0f,0.0f)),
        TexturedColoredVertex(vec3(-0.5f,-0.5f, 0.5f), vec3(0.0f, 1.0f, 1.0f), vec2(0.0f, 16.0f),vec3(0.0f,-1.0f,0.0f)),
        TexturedColoredVertex(vec3(-0.5f,-0.5f,-0.5f), vec3(0.0f, 1.0f, 1.0f), vec2(0.0f, 0.0f),vec3(0.0f,-1.0f,0.0f)),

        TexturedColoredVertex(vec3(-0.5f, 0.5f, 0.5f), vec3(0.0f, 1.0f, 0.0f), vec2(0.0f, 16.0f),vec3(0.0f,0.0f,1.0f)), // near - green
        TexturedColoredVertex(vec3(-0.5f,-0.5f, 0.5f), vec3(0.0f, 1.0f, 0.0f), vec2(0.0f, 0.0f),vec3(0.0f,0.0f,1.0f)),
        TexturedColoredVertex(vec3( 0.5f,-0.5f, 0.5f), vec3(0.0f, 1.0f, 0.0f), vec2(16.0f, 0.0f),vec3(0.0f,0.0f,1.0f)),

        TexturedColoredVertex(vec3( 0.5f, 0.5f, 0.5f), vec3(0.0f, 1.0f, 0.0f), vec2(16.0f, 16.0f),vec3(0.0f,0.0f,1.0f)),
        TexturedColoredVertex(vec3(-0.5f, 0.5f, 0.5f), vec3(0.0f, 1.0f, 0.0f), vec2(0.0f, 16.0f),vec3(0.0f,0.0f,1.0f)),
        TexturedColoredVertex(vec3( 0.5f,-0.5f, 0.5f), vec3(0.0f, 1.0f, 0.0f), vec2(16.0f, 0.0f),vec3(0.0f,0.0f,1.0f)),

        TexturedColoredVertex(vec3( 0.5f, 0.5f, 0.5f), vec3(1.0f, 0.0f, 1.0f), vec2(16.0f, 16.0f),vec3(1.0f,0.0f,0.0f)), // right - purple
        TexturedColoredVertex(vec3( 0.5f,-0.5f,-0.5f), vec3(1.0f, 0.0f, 1.0f), vec2(0.0f, 0.0f),vec3(1.0f,0.0f,0.0f)),
        TexturedColoredVertex(vec3( 0.5f, 0.5f,-0.5f), vec3(1.0f, 0.0f, 1.0f), vec2(16.0f, 0.0f),vec3(1.0f, 0.0f, 0.0f)),

        TexturedColoredVertex(vec3( 0.5f,-0.5f,-0.5f), vec3(1.0f, 0.0f, 1.0f), vec2(0.0f, 0.0f),vec3(1.0f, 0.0f, 0.0f)),
        TexturedColoredVertex(vec3( 0.5f, 0.5f, 0.5f), vec3(1.0f, 0.0f, 1.0f), vec2(16.0f, 16.0f),vec3(1.0f, 0.0f, 0.0f)),
        TexturedColoredVertex(vec3( 0.5f,-0.5f, 0.5f), vec3(1.0f, 0.0f, 1.0f), vec2(0.0f, 16.0f),vec3(1.0f, 0.0f, 0.0f)),

        TexturedColoredVertex(vec3( 0.5f, 0.5f, 0.5f), vec3(1.0f, 1.0f, 0.0f), vec2(16.0f, 16.0f),vec3(0.0f, 1.0f, 0.0f)), // top - yellow
        TexturedColoredVertex(vec3( 0.5f, 0.5f,-0.5f), vec3(1.0f, 1.0f, 0.0f), vec2(16.0f, 0.0f), vec3(0.0f, 1.0f, 0.0f)),
        TexturedColoredVertex(vec3(-0.5f, 0.5f,-0.5f), vec3(1.0f, 1.0f, 0.0f), vec2(0.0f, 0.0f), vec3(0.0f, 1.0f, 0.0f)),

        TexturedColoredVertex(vec3( 0.5f, 0.5f, 0.5f), vec3(1.0f, 1.0f, 0.0f), vec2(16.0f, 16.0f), vec3(0.0f, 1.0f, 0.0f)),
        TexturedColoredVertex(vec3(-0.5f, 0.5f,-0.5f), vec3(1.0f, 1.0f, 0.0f), vec2(0.0f, 0.0f), vec3(0.0f, 1.0f, 0.0f)),
        TexturedColoredVertex(vec3(-0.5f, 0.5f, 0.5f), vec3(1.0f, 1.0f, 0.0f), vec2(0.0f, 16.0f), vec3(0.0f, 1.0f, 0.0f))
    };


    GLuint vertexArrayObject;   // VAO
    glGenVertexArrays(1, &vertexArrayObject);
    // Upload Vertex Buffer to the GPU, keep a reference to it (vertexBufferObject)
    GLuint vertexBufferObject;  // VBO
    glGenBuffers(1, &vertexBufferObject);

    // bind the grid array
    glBindVertexArray(vertexArrayObject);
    glBindBuffer(GL_ARRAY_BUFFER, vertexBufferObject);  // bind the buffer object
    // put the vertex array to the buffer.
    glBufferData(GL_ARRAY_BUFFER, sizeof(texturedVertexArray), texturedVertexArray, GL_STATIC_DRAW);
    // set the attribute pointer (for vertex)
    glVertexAttribPointer(0,                   // attribute 0 matches aPos in Vertex Shader
                          3,                   // size
                          GL_FLOAT,            // type
                          GL_FALSE,            // normalized?
                          sizeof(TexturedColoredVertex), // stride - each vertex contain 2 vec3 (position, color)
                          (void*) 0             // array buffer offset
                          );
    glEnableVertexAttribArray(0);

    // since we have 2 attributes, position and color, we need another pointer for color
    glVertexAttribPointer(1,                            // attribute 1 matches aColor in Vertex Shader
                          3,
                          GL_FLOAT,
                          GL_FALSE,
                          sizeof(TexturedColoredVertex),
                          (void*)sizeof(glm::vec3)      // color is offseted a vec3 (comes after position)
                          );
    glEnableVertexAttribArray(1);
    
    glVertexAttribPointer(2,                            // attribute 2 matches aUV in Vertex Shader
                          2,
                          GL_FLOAT,
                          GL_FALSE,
                          sizeof(TexturedColoredVertex),
                          (void*)(2*sizeof(glm::vec3))      // uv is offseted by 2 vec3 (comes after position and color)
                          );
    glEnableVertexAttribArray(2);
    
    glVertexAttribPointer(3,                            // attribute 2 matches aNormal in Vertex Shader
                             3,
                             GL_FLOAT,
                             GL_FALSE,
                             sizeof(TexturedColoredVertex),
                             (void*)(2*sizeof(glm::vec3) + sizeof(vec2))      // uv is offseted by 2 vec3 (comes after position and color)
                             );
    glEnableVertexAttribArray(3);
    // why do we have to unbind tho?
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    return vertexArrayObject;
}

GLuint loadTexture(const char *filename)
{
  // Step1 Create and bind textures
  GLuint textureId = 0;
  glGenTextures(1, &textureId);
  assert(textureId != 0);

  glBindTexture(GL_TEXTURE_2D, textureId);
    

  // Step2 Set filter parameters
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

  // Step3 Load Textures with dimension data
  int width, height, nrChannels;
    stbi_set_flip_vertically_on_load(true);
  unsigned char *data = stbi_load(filename, &width, &height, &nrChannels, 0);
  if (!data)
  {
    std::cerr << "Error::Texture could not load texture file:" << filename << std::endl;
    return 0;
  }
    
 
  // Step4 Upload the texture to the PU
  GLenum format = 0;
  if (nrChannels == 1)
      format = GL_RED;
  else if (nrChannels == 3)
      format = GL_RGB;
  else if (nrChannels == 4)
      format = GL_RGBA;
  glTexImage2D(GL_TEXTURE_2D, 0, format, width, height,
               0, format, GL_UNSIGNED_BYTE, data);
  glGenerateMipmap(GL_TEXTURE_2D);

  // Step5 Free resources
  stbi_image_free(data);
  glBindTexture(GL_TEXTURE_2D, 0);
  return textureId;
}

void setProjectionMatrix(int shaderProgram, mat4 projectionMatrix)
{
    glUseProgram(shaderProgram);
    GLuint projectionMatrixLocation = glGetUniformLocation(shaderProgram, "projectionMatrix");
    glUniformMatrix4fv(projectionMatrixLocation, 1, GL_FALSE, &projectionMatrix[0][0]);
}

void setViewMatrix(int shaderProgram, mat4 viewMatrix)
{
    glUseProgram(shaderProgram);
    GLuint viewMatrixLocation = glGetUniformLocation(shaderProgram, "viewMatrix");
    glUniformMatrix4fv(viewMatrixLocation, 1, GL_FALSE, &viewMatrix[0][0]);
}

void setWorldMatrix(int shaderProgram, mat4 worldMatrix)
{
    glUseProgram(shaderProgram);
    GLuint worldMatrixLocation = glGetUniformLocation(shaderProgram, "worldMatrix");
    glUniformMatrix4fv(worldMatrixLocation, 1, GL_FALSE, &worldMatrix[0][0]);
}

void SetUniform1int(GLuint shader_id, const char* uniform_name, int uniform_value)
{
  glUseProgram(shader_id);
  glUniform1i(glGetUniformLocation(shader_id, uniform_name), uniform_value);
}

void SetUniform1float(GLuint shader_id, const char* uniform_name, float uniform_value)
{
  glUseProgram(shader_id);
  glUniform1f(glGetUniformLocation(shader_id, uniform_name), uniform_value);
}

// shader variable setters
void SetUniformMat4(GLuint shader_id, const char* uniform_name, mat4 uniform_value)
{
  glUseProgram(shader_id);
  glUniformMatrix4fv(glGetUniformLocation(shader_id, uniform_name), 1, GL_FALSE, &uniform_value[0][0]);
}

void SetUniformVec3(GLuint shader_id, const char* uniform_name, vec3 uniform_value)
{
  glUseProgram(shader_id);
  glUniform3fv(glGetUniformLocation(shader_id, uniform_name), 1, value_ptr(uniform_value));
}


#endif /* functions_h */
