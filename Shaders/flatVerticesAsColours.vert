#version 410

layout (location = 0) in vec3 vertex_position;
//layout (location = 1) in vec3 vertex_normal;		// not used as no lighting equations
uniform mat4 projection_matrix, view_matrix, model_matrix;

out vec3 colour;			// varying is deprecated

void main() {

	colour = vertex_position;
	gl_Position = projection_matrix * view_matrix * model_matrix * vec4 (vertex_position, 1.0);   // try vertex_normal instead - no interpolation!!!
}
