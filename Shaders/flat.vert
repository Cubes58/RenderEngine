#version 410

// tell the shader where the vertices are in the vao
layout(location = 0) in vec3 vertex_position;

uniform mat4 projection_matrix, view_matrix, model_matrix;

void main() {
	gl_Position = projection_matrix * view_matrix * model_matrix * vec4 (vertex_position, 1.0);
}
