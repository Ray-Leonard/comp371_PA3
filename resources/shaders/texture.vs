#version 330 core
// (For vertex position) attribute position; vector size and name
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aColor;
layout (location = 2) in vec2 aUV;
layout (location = 3) in vec3 aNormal;

// out is for outputting the attribute, in is to get input
out vec3 fragment_normal;
out vec3 fragment_position;
out vec4 fragment_position_light_space;
out vec2 vertexUV;
out vec3 vertexColor;

// uniform matrix for transformation, scaling and rotating
uniform mat4 worldMatrix = mat4(1.0);
// uniform view matrix
uniform mat4 viewMatrix = mat4(1.0);    // identity mtx by default
uniform mat4 projectionMatrix = mat4(1.0);
uniform mat4 light_view_proj_matrix;


void main()
{
   gl_Position = projectionMatrix * viewMatrix * worldMatrix * vec4(aPos.x, aPos.y, aPos.z, 1.0);
   vertexColor = aColor;
   vertexUV = aUV;
   fragment_normal = mat3(worldMatrix) * aNormal;
   fragment_position = vec3(worldMatrix * vec4(aPos, 1.0));
   fragment_position_light_space = light_view_proj_matrix * vec4(fragment_position, 1.0);
}
