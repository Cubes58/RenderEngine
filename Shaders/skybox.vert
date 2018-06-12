#version 410

layout (location = 0) in vec3 vertex_position;

out vec3 texture_coordinates;

uniform mat4 projection_matrix;
uniform mat4 view_matrix;

void main() {
	texture_coordinates = vertex_position;
	gl_Position = projection_matrix * view_matrix * vec4(vertex_position, 1.0);
	
	//vec4 pos = projection_matrix * view_matrix * vec4(vertex_position, 1.0f);
	//gl_Position = pos.xyww;
	//texture_coordinates = vertex_position;
}