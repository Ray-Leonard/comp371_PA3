#version 330 core
// (For vertex position) attribute position; vector size and name
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aColor;
// out is for outputting the attribute, in is to get input
out vec3 vertexColor;

// uniform matrix for transformation, scaling and rotating
uniform mat4 worldMatrix = mat4(1.0);
// uniform view matrix
uniform mat4 viewMatrix = mat4(1.0);    // identity mtx by default
uniform mat4 projectionMatrix = mat4(1.0);

void main()
{
   vertexColor = aColor;
  // mat4 modelViewProjection = projectionMatrix * viewMatrix * worldMatrix;
   gl_Position = projectionMatrix * viewMatrix * worldMatrix * vec4(aPos.x, aPos.y, aPos.z, 1.0);
    
}
