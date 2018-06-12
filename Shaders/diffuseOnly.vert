#version 410

layout (location = 0) in vec3 vertex_position;
layout (location = 1) in vec3 vertex_normal;

uniform mat4 projection_matrix, view_matrix, model_matrix;

out vec3 position_eye, normal_eye;

void main () {

// could work out view_model_matrix?

	position_eye = vec3 (view_matrix * model_matrix * vec4 (vertex_position, 1.0));
	normal_eye = vec3 (view_matrix * model_matrix * vec4 (vertex_normal, 0.0));
	gl_Position = projection_matrix * vec4 (position_eye, 1.0);
}
