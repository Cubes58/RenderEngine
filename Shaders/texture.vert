#version 410

layout (location = 0) in vec3 vertex_position;
layout (location = 1) in vec3 vertex_normal;
layout (location = 2) in vec2 vertex_textureUV; 				// per-vertex texture co-ords

uniform mat4 model_matrix, view_matrix, projection_matrix;

out vec2 texture_coordinates;

void main() 
{

	texture_coordinates = vertex_textureUV;

	gl_Position = projection_matrix * view_matrix * model_matrix * vec4 (vertex_position, 1.0);

}